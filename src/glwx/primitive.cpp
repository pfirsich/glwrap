#include "glwx/primitive.hpp"

using namespace glw;

namespace glwx {
Primitive::Primitive(DrawMode mode)
    : mode(mode)
{
}

void Primitive::addVertexBuffer(const Buffer& buffer, const VertexFormat& vfmt)
{
    // Assert that all locations used in vfmt are not already in use
    vertexArray.bind();
    buffer.bind(Buffer::Target::Array);
    vfmt.set();
    vertexArray.unbind();
    buffer.unbind(Buffer::Target::Array);
    vertexRange = Range { 0, buffer.getSize() / vfmt.getStride() };
}

void Primitive::addVertexBuffer(const VertexBuffer& buffer)
{
    addVertexBuffer(buffer, buffer.getVertexFormat());
    // If we add multiple vertex buffers, don't exceed any one of them
    if (vertexRange.count > 0)
        vertexRange = Range { 0, std::min(vertexRange.count, buffer.getCount()) };
    else
        vertexRange = Range { 0, buffer.getCount() };
}

void Primitive::setIndexBuffer(const Buffer& buffer, IndexType indexType)
{
    vertexArray.bind();
    buffer.bind(Buffer::Target::ElementArray);
    vertexArray.unbind();
    buffer.unbind(Buffer::Target::ElementArray);
    indexType_ = indexType;
    indexRange = Range { 0, buffer.getSize() / glw::getIndexTypeSize(indexType) };
}

void Primitive::setIndexBuffer(const IndexBuffer& buffer)
{
    setIndexBuffer(buffer, buffer.getIndexType());
    indexRange = Range { 0, buffer.getCount() };
}

void Primitive::draw(size_t offset, size_t count) const
{
    vertexArray.bind();
    const auto m = static_cast<GLenum>(mode);
    if (indexType_) {
        glDrawElements(m, static_cast<GLsizei>(count), static_cast<GLenum>(*indexType_),
            reinterpret_cast<const void*>(glw::getIndexTypeSize(*indexType_) * offset));
    } else {
        glDrawArrays(m, static_cast<GLsizei>(offset), static_cast<GLsizei>(count));
    }
    vertexArray.unbind();
    State::instance().getStatistics().drawCalls++;
}

void Primitive::draw() const
{
    if (indexType_) {
        draw(indexRange.offset, indexRange.count);
    } else {
        draw(vertexRange.offset, vertexRange.count);
    }
}

void Primitive::draw(size_t offset, size_t count, size_t instanceCount) const
{
    vertexArray.bind();
    const auto m = static_cast<GLenum>(mode);
    if (indexType_) {
        glDrawElementsInstanced(m, static_cast<GLsizei>(count), static_cast<GLenum>(*indexType_),
            reinterpret_cast<const void*>(glw::getIndexTypeSize(*indexType_) * offset),
            static_cast<GLsizei>(instanceCount));
    } else {
        glDrawArraysInstanced(m, static_cast<GLint>(offset), static_cast<GLsizei>(count),
            static_cast<GLsizei>(instanceCount));
    }
    vertexArray.unbind();
    State::instance().getStatistics().drawCalls++;
}

void Primitive::draw(size_t instanceCount) const
{
    if (indexType_) {
        draw(indexRange.offset, indexRange.count, instanceCount);
    } else {
        draw(vertexRange.offset, vertexRange.count, instanceCount);
    }
}
}
