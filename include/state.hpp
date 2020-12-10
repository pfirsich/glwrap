#pragma once

#include <array>
#include <cassert>
#include <cstdlib>
#include <optional>
#include <tuple>

#include "glad/glad.h"

namespace glw {
enum class DepthFunc : GLenum {
    Never = GL_NEVER,
    Less = GL_LESS,
    Equal = GL_EQUAL,
    Lequal = GL_LEQUAL,
    Greater = GL_GREATER,
    Notequal = GL_NOTEQUAL,
    Gequal = GL_GEQUAL,
    Always = GL_ALWAYS,
};

class State {
public:
    struct Statistics {
        size_t vertexArrayBinds = 0;
        size_t shaderBinds = 0;
        size_t textureBinds = 0;
        size_t frameBufferBinds = 0;
    };

    // GL_MAX_TEXTURE_IMAGE_UNITS is 16 in GL 3.3
    static constexpr size_t maxTextureUnits = 16;

    static State& instance();

    ~State() = default;

    void resetStatistics();
    Statistics getStatistics() const;

    std::tuple<int, int, size_t, size_t> getViewport() const;
    void setViewport(int x, int y, size_t width, size_t height);
    void setViewport(const std::tuple<int, int, size_t, size_t>& viewport);
    void setViewport(size_t width, size_t height);

    DepthFunc getDepthFunc() const;
    void setDepthFunc(DepthFunc func);

    GLuint getCurrentVao() const;
    void bindVao(GLuint vao);
    void unbindVao();

    GLuint getCurrentShader() const;
    void bindShader(GLuint prog);
    void unbindShader();

    GLuint getCurrentBuffer(GLenum target) const;
    void bindBuffer(GLenum target, GLuint buffer);
    void unbindBuffer(GLenum target);

    GLuint getCurrentTexture(unsigned int unit, GLenum target) const;
    std::optional<unsigned int> getTextureUnit(GLenum target, GLuint texture) const;
    void bindTexture(unsigned int unit, GLenum target, GLuint texture);
    void unbindTexture(unsigned int unit, GLenum target);

    GLuint getCurrentFramebuffer(GLenum target) const;
    void bindFramebuffer(GLenum target, GLuint fbo);
    void unbindFramebuffer(GLenum target);

    GLuint getCurrentRenderbuffer() const;
    void bindRenderbuffer(GLuint rbo);
    void unbindRenderbuffer();

private:
    // I think specifying size explicitly is dangerous and I can't only specify type
    static constexpr std::array bufferBindings = {
        static_cast<GLenum>(GL_ARRAY_BUFFER),
        //  GL_ATOMIC_COUNTER_BUFFER, // 4.2
        static_cast<GLenum>(GL_COPY_READ_BUFFER),
        static_cast<GLenum>(GL_COPY_WRITE_BUFFER),
        //  GL_DISPATCH_INDIRECT_BUFFER, // 4.3
        //  GL_DRAW_INDIRECT_BUFFER, // 4.0
        static_cast<GLenum>(GL_ELEMENT_ARRAY_BUFFER),
        static_cast<GLenum>(GL_PIXEL_PACK_BUFFER),
        static_cast<GLenum>(GL_PIXEL_UNPACK_BUFFER),
        //  GL_QUERY_BUFFER, // 4.4
        //  GL_SHADER_STORAGE_BUFFER, // 4.3
        static_cast<GLenum>(GL_TEXTURE_BUFFER),
        static_cast<GLenum>(GL_TRANSFORM_FEEDBACK_BUFFER),
        static_cast<GLenum>(GL_UNIFORM_BUFFER),
    };

    static constexpr std::array textureBindings = {
        static_cast<GLenum>(GL_TEXTURE_1D),
        static_cast<GLenum>(GL_TEXTURE_2D),
        static_cast<GLenum>(GL_TEXTURE_3D),
        static_cast<GLenum>(GL_TEXTURE_1D_ARRAY),
        static_cast<GLenum>(GL_TEXTURE_2D_ARRAY),
        static_cast<GLenum>(GL_TEXTURE_RECTANGLE),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP),
        static_cast<GLenum>(GL_TEXTURE_BUFFER),
        static_cast<GLenum>(GL_TEXTURE_2D_MULTISAMPLE),
        static_cast<GLenum>(GL_TEXTURE_2D_MULTISAMPLE_ARRAY),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_X),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Y),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Z),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z),
    };

    template <typename Container>
    static size_t getTargetIndex(const Container& container, GLenum value)
    {
        for (size_t i = 0; i < container.size(); ++i)
            if (container[i] == value)
                return i;
        std::abort();
    }

    static size_t getBufferIndex(GLenum target);
    static size_t getTextureIndex(GLenum target);

    State() = default;
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    Statistics statistics_;
    std::tuple<int, int, size_t, size_t> viewport_ = { 0, 0, 0, 0 };
    DepthFunc depthFunc_ = DepthFunc::Less;
    GLuint vao_ = 0;
    GLuint shaderProgram_ = 0;
    std::array<GLuint, bufferBindings.size()> buffers_;
    std::array<std::array<GLuint, textureBindings.size()>, maxTextureUnits> textures_ {};
    GLuint readFramebuffer_ = 0;
    GLuint drawFramebuffer_ = 0;
    GLuint renderbuffer_ = 0;
};
}
