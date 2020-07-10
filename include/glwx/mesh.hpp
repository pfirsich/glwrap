#pragma once

#include <memory>
#include <vector>

#include "../vertexarray.hpp"
#include "buffers.hpp"
#include "enums.hpp"
#include "primitive.hpp"

namespace glwx {
class Mesh {
public:
    Primitive primitive;

    explicit Mesh(glw::DrawMode mode = glw::DrawMode::Triangles);

    template <typename... Args>
    glwx::VertexBuffer& addVertexBuffer(Args&&... args)
    {
        auto& buffer = vertexBuffers_.emplace_back(std::forward<Args>(args)...);
        primitive.addVertexBuffer(buffer);
        return buffer;
    }

    // TODO: Add vertex buffer accessors!

    template <typename... Args>
    glwx::IndexBuffer& addIndexBuffer(Args&&... args)
    {
        assert(!indexBuffer_);
        indexBuffer_ = std::make_unique<glwx::IndexBuffer>(std::forward<Args>(args)...);
        primitive.setIndexBuffer(*indexBuffer_);
        return *indexBuffer_;
    }

    template <typename... Args>
    void draw(Args&&... args) const
    {
        primitive.draw(std::forward<Args>(args)...);
    }

private:
    std::vector<glwx::VertexBuffer> vertexBuffers_;
    std::unique_ptr<glwx::IndexBuffer> indexBuffer_;
};
}
