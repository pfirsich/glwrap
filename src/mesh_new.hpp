#include <optional>
#include <utility>
#include <vector>

#include "buffers.hpp"
#include "utility.hpp"
#include "vertexarray.hpp"
#include "vertexformat.hpp"

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

    template <typename ElementType, typename... Args>
    BufferData<ElementType>& addVertexBuffer(const VertexFormat& format, Args&&... args)
    {
        // TODO: Assert that all locations used in format are not already in use.
        vao_.bind();
        auto dataPtr = std::make_unique<BufferData<ElementType>>(
            Buffer::Target::Array, std::forward<Args>(args)...);
        auto& data = *dataPtr;
        auto& buf = vertexBuffers_.emplace_back(
            VertexBuffer { format, std::unique_ptr<BufferDataBase>(std::move(dataPtr)) });
        buf.buffer->bind();
        buf.format.set();
        VertexArray::unbind();
        return data;
    }

    // TODO: Add vertex buffer accessors!

    template <typename ElementType, typename... Args>
    BufferData<ElementType>& addIndexBuffer(Args&&... args)
    {
        static_assert(isIndexType<ElementType>);
        assert(!indexBuffer_);
        vao_.bind();
        auto dataPtr = std::make_unique<BufferData<ElementType>>(
            Buffer::Target::ElementArray, std::forward<Args>(args)...);
        auto& data = *dataPtr;
        indexBuffer_ = std::move(dataPtr);
        indexBuffer_->bind();
        VertexArray::unbind();
        return data;
    }

    void draw(size_t instanceCount = 0) const
    {
        vao_.bind();

        const auto mode = static_cast<GLenum>(mode_);
        if (indexBuffer_) {
            const auto count = indexBuffer_->getSize();
            const auto indexType = getIndexType(indexBuffer_->getElementSize());
            if (instanceCount > 0) {
                glDrawElementsInstanced(mode, count, indexType, nullptr, instanceCount);
            } else {
                glDrawElements(mode, count, indexType, nullptr);
            }
        } else {
            assert(!vertexBuffers_.empty());
            const auto count = getMinField<size_t>(
                vertexBuffers_, [](const VertexBuffer& buf) { return buf.buffer->getSize(); });

            if (instanceCount > 0) {
                glDrawArraysInstanced(mode, 0, count, instanceCount);
            } else {
                glDrawArrays(mode, 0, count);
            }
        }
    }

private:
    struct VertexBuffer {
        VertexFormat format;
        std::unique_ptr<BufferDataBase> buffer;
    };

    std::vector<VertexBuffer> vertexBuffers_;
    std::unique_ptr<BufferDataBase> indexBuffer_;
    VertexArray vao_;
    DrawMode mode_;
};
}
