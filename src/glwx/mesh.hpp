#pragma once

#include <memory>
#include <vector>

#include "../vertexarray.hpp"
#include "buffers.hpp"

namespace glwx {
class Mesh {
public:
    enum class DrawMode : GLenum {
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        LineStrip = GL_LINE_STRIP,
        Triangles = GL_TRIANGLES,
        TriangleFan = GL_TRIANGLE_FAN,
        TriangleStrip = GL_TRIANGLE_STRIP,
    };

    Mesh(DrawMode mode = DrawMode::Triangles);

    // Be careful with these functions, because if the Buffer is destroyed, the
    // binding in the VAO (this mesh) will be removed.
    // Prefer to use the functions below, that emplace buffers into the mesh instead.
    void attachVertexBuffer(const glw::VertexFormat& format, const glw::Buffer& buffer) const;

    void attachIndexBuffer(const glw::Buffer& buffer) const;

    template <typename... Args>
    glwx::VertexBuffer& addVertexBuffer(Args&&... args)
    {
        auto& buffer = vertexBuffers_.emplace_back(std::forward<Args>(args)...);
        attachVertexBuffer(buffer.getVertexFormat(), buffer);
        return buffer;
    }

    // TODO: Add vertex buffer accessors!

    template <typename... Args>
    glwx::IndexBuffer& addIndexBuffer(Args&&... args)
    {
        assert(!indexBuffer_);
        indexBuffer_ = std::make_unique<glwx::IndexBuffer>(std::forward<Args>(args)...);
        attachIndexBuffer(*indexBuffer_);
        return *indexBuffer_;
    }

    size_t getVertexCount() const;

    // I think this function is kinda sketchy, because if you clear a buffer, it will not update it.
    // You have to do that manually. But I don't think that's a very common use case and it will
    // probably help prevent more bugs than it creates.
    // We don't call it automatically in draw() or something, because I'm not sure how to detect if
    // the buffers are dirty.
    void updateBuffers();

    // For index meshes this range is a range in the index buffer, for non-indexed meshes
    // it's a range of vertex indices.
    void draw(size_t offset, size_t count) const;

    void draw() const;

    void draw(size_t offset, size_t count, size_t instanceCount) const;

    void draw(size_t instanceCount) const;

private:
    template <typename FieldType, typename Container, typename Func>
    static FieldType getMinField(const Container& container, Func&& func)
    {
        assert(!container.empty());
        auto it = container.begin();
        FieldType min = func(*it);
        ++it;
        for (; it != container.end(); ++it) {
            const auto field = func(*it);
            if (field < min)
                min = field;
        }
        return min;
    }

    std::vector<glwx::VertexBuffer> vertexBuffers_;
    std::unique_ptr<glwx::IndexBuffer> indexBuffer_;
    glw::VertexArray vao_;
    DrawMode mode_;
};
}
