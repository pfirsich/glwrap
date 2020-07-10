#include "primitive.hpp"

using namespace glw;

namespace glwx {
Primitive::Primitive(DrawMode mode)
    : mode(mode)
{
}

void Primitive::addVertexBuffer(
    const glw::Buffer& buffer, const glw::VertexFormat& vfmt, size_t offset, size_t count)
{
    addVertexBuffer(buffer, vfmt);
    vertexRange = Range { offset, count };
}

void Primitive::addVertexBuffer(
    const glw::Buffer& buffer, const glw::VertexFormat& vfmt, size_t count)
{
    addVertexBuffer(buffer, vfmt, 0, count);
}

void Primitive::addVertexBuffer(const VertexBuffer& buffer)
{
    addVertexBuffer(buffer, buffer.getVertexFormat(), buffer.getCount());
}

void Primitive::setIndexBuffer(
    const Buffer& buffer, IndexType indexType, size_t offset, size_t count)
{
    setIndexBuffer(buffer);
    indexType_ = indexType;
    indexRange = Range { offset, count };
}

void Primitive::setIndexBuffer(const Buffer& buffer, IndexType indexType, size_t count)
{
    setIndexBuffer(buffer, indexType, 0, count);
}

void Primitive::setIndexBuffer(const IndexBuffer& buffer)
{
    setIndexBuffer(buffer, buffer.getIndexType(), buffer.getCount());
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

void Primitive::addVertexBuffer(const Buffer& buffer, const VertexFormat& vfmt)
{
    // Assert that all locations used in vfmt are not already in use
    vertexArray.bind();
    buffer.bind(Buffer::Target::Array);
    vfmt.set();
    vertexArray.unbind();
    buffer.unbind(Buffer::Target::Array);
}

void Primitive::setIndexBuffer(const Buffer& buffer)
{
    vertexArray.bind();
    buffer.bind(Buffer::Target::ElementArray);
    vertexArray.unbind();
    buffer.unbind(Buffer::Target::ElementArray);
}
}
