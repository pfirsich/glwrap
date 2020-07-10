#include "primitive.hpp"

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
        glDrawElements(m, count, static_cast<GLenum>(*indexType_),
            reinterpret_cast<const void*>(glw::getIndexTypeSize(*indexType_) * offset));
    } else {
        glDrawArrays(m, offset, count);
    }
    vertexArray.unbind();
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
        glDrawElementsInstanced(m, count, static_cast<GLenum>(*indexType_),
            reinterpret_cast<const void*>(glw::getIndexTypeSize(*indexType_) * offset),
            instanceCount);
    } else {
        glDrawArraysInstanced(m, offset, count, instanceCount);
    }
    vertexArray.unbind();
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
