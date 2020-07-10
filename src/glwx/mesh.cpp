#include "mesh.hpp"

using namespace glw;

namespace glwx {
Mesh::Mesh(DrawMode mode)
    : mode_(mode)
{
}

void Mesh::attachVertexBuffer(const glw::VertexFormat& format, const glw::Buffer& buffer) const
{
    // TODO: Assert that all locations used in format are not already in use.
    vao_.bind();
    buffer.bind(Buffer::Target::Array);
    format.set();
    vao_.unbind();
    buffer.unbind(Buffer::Target::Array);
}

void Mesh::attachIndexBuffer(const glw::Buffer& buffer) const
{
    vao_.bind();
    buffer.bind(Buffer::Target::ElementArray);
    vao_.unbind();
    buffer.unbind(Buffer::Target::ElementArray);
}

size_t Mesh::getVertexCount() const
{
    assert(!vertexBuffers_.empty());
    return getMinField<size_t>(
        vertexBuffers_, [](const glwx::VertexBuffer& buf) { return buf.getCount(); });
}

void Mesh::updateBuffers()
{
    for (auto& buffer : vertexBuffers_)
        if (!buffer.getData().empty())
            buffer.update();
    if (!indexBuffer_->getData().empty())
        indexBuffer_->update();
}

void Mesh::draw(size_t offset, size_t count) const
{
    vao_.bind();
    const auto mode = static_cast<GLenum>(mode_);
    if (indexBuffer_) {
        glDrawElements(mode, count, static_cast<GLenum>(indexBuffer_->getIndexType()),
            reinterpret_cast<const void*>(indexBuffer_->getElementSize() * offset));
    } else {
        glDrawArrays(mode, offset, count);
    }
    vao_.unbind();
}

void Mesh::draw() const
{
    if (indexBuffer_) {
        draw(0, indexBuffer_->getCount());
    } else {
        draw(0, getVertexCount());
    }
}

void Mesh::draw(size_t offset, size_t count, size_t instanceCount) const
{
    vao_.bind();
    const auto mode = static_cast<GLenum>(mode_);
    if (indexBuffer_) {
        glDrawElementsInstanced(mode, count, static_cast<GLenum>(indexBuffer_->getIndexType()),
            reinterpret_cast<const void*>(indexBuffer_->getElementSize() * offset), instanceCount);
    } else {
        glDrawArraysInstanced(mode, offset, count, instanceCount);
    }
    vao_.unbind();
}

void Mesh::draw(size_t instanceCount) const
{
    if (indexBuffer_) {
        draw(0, indexBuffer_->getCount(), instanceCount);
    } else {
        draw(0, getVertexCount(), instanceCount);
    }
}
}
