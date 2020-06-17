#include "buffers.hpp"

namespace glwx {

DefaultBuffer::DefaultBuffer(Target target)
    : target(target)
{
}

DefaultBuffer::DefaultBuffer(UsageHint usage)
    : usage(usage)
{
}

DefaultBuffer::DefaultBuffer(Target target, UsageHint usage)
    : target(target)
    , usage(usage)
{
}

void DefaultBuffer::bind() const
{
    assert(target);
    Buffer::bind(target);
}

void DefaultBuffer::unbind() const
{
    assert(target);
    Buffer::unbind(target);
}

void BufferData::setData()
{
    data(data_.data(), data_.size());
}

void BufferData::setSubData(size_t index, size_t len) const
{
    // If I understand the reference pages correctly, it should be fine to call glBufferSubData
    // with size = 0, if you have not allocated a data store with glBufferData for that buffer
    // object yet (it's super sketch though).
    subData(index, data_.data() + index, len);
}

void BufferData::setSubData() const
{
    subData(data_.data(), data_.size());
}

void BufferData::update()
{
    if (getSize() < data_.size())
        setData();
    else
        setSubData();
}

std::vector<uint8_t>& BufferData::getData()
{
    return data_;
}

const std::vector<uint8_t>& BufferData::getData() const
{
    return data_;
}

const glw::VertexFormat& VertexBuffer::getVertexFormat() const
{
    return vertexFormat_;
}

size_t VertexBuffer::getCount() const
{
    assert(getSize() % vertexFormat_.getStride() == 0);
    return getSize() / vertexFormat_.getStride();
}

IndexBuffer::ElementType IndexBuffer::getElementType() const
{
    return elementType_;
}

size_t IndexBuffer::getElementSize() const
{
    switch (elementType_) {
    case ElementType::U8:
        return 1;
    case ElementType::U16:
        return 2;
    case ElementType::U32:
        return 4;
    }
}

size_t IndexBuffer::getCount() const
{
    assert(getSize() % getElementSize() == 0);
    return getSize() / getElementSize();
}
}
