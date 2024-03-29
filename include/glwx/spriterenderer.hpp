#pragma once

#include <vector>

#include "glw/shader.hpp"
#include "glw/texture.hpp"
#include "glwx/buffers.hpp"
#include "glwx/primitive.hpp"
#include "glwx/transform2d.hpp"

namespace glwx {

struct TextureRegion {
    // These are all in [0, 1] (uv coordinates)
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    glm::vec2 size = glm::vec2(1.0f, 1.0f);
};

class SpriteBatch {
public:
    using IndexType = uint16_t;

    struct AttributeLocations {
        static constexpr size_t Position = 0;
        static constexpr size_t TexCoord = 1;
        static constexpr size_t Color = 2;
    };

    SpriteBatch(size_t vertexCount = 0, size_t indexCount = 0);

    IndexType addVertex(
        const glm::vec2& position, const glm::vec2& texCoord, const glm::vec4& color);

    void addIndex(IndexType index);

    size_t getNumVertices() const;
    size_t getNumIndices() const;

    void clear();

    void flush();

private:
    struct Vertex {
        glm::vec2 position;
        glm::vec2 texCoord;
        glm::vec4 color;
    };

    static const glw::VertexFormat& getVertexFormat();

    std::vector<Vertex> vertices_;
    std::vector<IndexType> indices_;
    Primitive primitive_;
    DefaultBuffer vertexBuffer_;
    DefaultBuffer indexBuffer_;
};

class SpriteRenderer {
public:
    static constexpr std::string_view defaultVertexShader = R"(
        #version 330 core

        uniform mat4 projectionMatrix;
        uniform float depth = 0.0;

        layout (location = 0) in vec2 attrPosition;
        layout (location = 1) in vec2 attrTexCoords;
        layout (location = 2) in vec4 attrColor;

        out vec2 texCoords;
        out vec4 color;

        void main() {
            texCoords = attrTexCoords;
            color = attrColor;
            gl_Position = projectionMatrix * vec4(attrPosition, depth, 1.0);
        }
    )";

    static constexpr std::string_view defaultFragmentShader = R"(
        #version 330 core

        uniform sampler2D base;

        in vec2 texCoords;
        in vec4 color;

        out vec4 fragColor;

        void main() {
            fragColor = texture(base, texCoords) * color;
        }
    )";

    static glw::ShaderProgram& getDefaultShaderProgram();

    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    void setCurrentTexture(const glw::Texture* texture);

    void draw(const glw::Texture& texture, const Transform2D& transform,
        const TextureRegion& region = TextureRegion {});

    void draw(const glw::Texture& texture, const std::vector<glm::vec2>& points,
        const Transform2D& transform = Transform2D {},
        const TextureRegion& region = TextureRegion {});

    void drawLine(const glw::Texture& texture, const std::vector<glm::vec2>& points, float width,
        bool closed = false, const Transform2D& transform = {});

    void drawCircle(const glw::Texture& texture, const glm::vec2& position, float radius,
        size_t segments = 32, const Transform2D& transform = {});

    SpriteBatch::IndexType getVertexOffset() const;
    SpriteBatch::IndexType addVertex(const glm::vec2& position, const glm::vec2& texCoord);
    void addIndex(SpriteBatch::IndexType index);

    void flush();

private:
    const glw::Texture* currentTexture_ = nullptr;
    SpriteBatch batch_;
};

}
