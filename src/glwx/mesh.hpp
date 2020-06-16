#pragma once

#include <vector>

#include "../vertexarray.hpp"
#include "buffers.hpp"

namespace glw {
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

    Mesh(DrawMode mode = DrawMode::Triangles)
        : mode_(mode)
    {
    }

    // Be careful with these functions, because if the Buffer is destroyed, the
    // binding in the VAO (this mesh) will be removed.
    // Prefer to use the functions below, that emplace buffers into the mesh instead.
    void attachVertexBuffer(const VertexFormat& format, const Buffer& buffer) const
    {
        // TODO: Assert that all locations used in format are not already in use.
        vao_.bind();
        buffer.bind(Buffer::Target::Array);
        format.set();
        vao_.unbind();
        buffer.unbind(Buffer::Target::Array);
    }

    void attachIndexBuffer(const Buffer& buffer) const
    {
        vao_.bind();
        buffer.bind(Buffer::Target::ElementArray);
        vao_.unbind();
        buffer.unbind(Buffer::Target::ElementArray);
    }

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

    size_t getVertexCount() const
    {
        assert(!vertexBuffers_.empty());
        return getMinField<size_t>(
            vertexBuffers_, [](const glwx::VertexBuffer& buf) { return buf.getCount(); });
    }

    // For index meshes this range is a range in the index buffer, for non-indexed meshes
    // it's a range of vertex indices.
    void draw(size_t offset, size_t count) const
    {
        vao_.bind();
        const auto mode = static_cast<GLenum>(mode_);
        if (indexBuffer_) {
            glDrawElements(mode, count, static_cast<GLenum>(indexBuffer_->getElementType()),
                reinterpret_cast<const void*>(indexBuffer_->getElementSize() * offset));
        } else {
            glDrawArrays(mode, offset, count);
        }
        vao_.unbind();
    }

    void draw() const
    {
        if (indexBuffer_) {
            draw(0, indexBuffer_->getCount());
        } else {
            draw(0, getVertexCount());
        }
    }

    void draw(size_t offset, size_t count, size_t instanceCount) const
    {
        vao_.bind();
        const auto mode = static_cast<GLenum>(mode_);
        if (indexBuffer_) {
            glDrawElementsInstanced(mode, count,
                static_cast<GLenum>(indexBuffer_->getElementType()),
                reinterpret_cast<const void*>(indexBuffer_->getElementSize() * offset),
                instanceCount);
        } else {
            glDrawArraysInstanced(mode, offset, count, instanceCount);
        }
        vao_.unbind();
    }

    void draw(size_t instanceCount) const
    {
        if (indexBuffer_) {
            draw(0, indexBuffer_->getCount(), instanceCount);
        } else {
            draw(0, getVertexCount(), instanceCount);
        }
    }

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
    VertexArray vao_;
    DrawMode mode_;
};
}
