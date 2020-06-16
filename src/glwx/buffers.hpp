#pragma once

#include <cassert>
#include <vector>

#include "../buffers.hpp"
#include "../utility.hpp"
#include "../vertexformat.hpp"

namespace glwx {

// Most of the time a single buffer will have the same usage and target, this
// class saves those too.
// This is a bad name for this class
struct DefaultBuffer : public glw::Buffer {
    Target target = Target::Undefined;
    UsageHint usage = UsageHint::Undefined;

    DefaultBuffer() = default;
    ~DefaultBuffer() = default;
    DefaultBuffer(DefaultBuffer&& other) = default;
    DefaultBuffer& operator=(DefaultBuffer&& other) = default;

    DefaultBuffer(Target target)
        : target(target)
    {
    }

    DefaultBuffer(UsageHint usage)
        : usage(usage)
    {
    }

    DefaultBuffer(Target target, UsageHint usage)
        : target(target)
        , usage(usage)
    {
    }

    void bind() const
    {
        assert(target);
        Buffer::bind(target);
    }

    void unbind() const
    {
        assert(target);
        Buffer::unbind(target);
    }

    template <typename... Args>
    void data(UsageHint usage, Args&&... args)
    {
        assert(target);
        Buffer::data(target, usage, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void data(Target target, Args&&... args)
    {
        assert(usage);
        Buffer::data(target, usage, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void data(Args&&... args)
    {
        assert(target);
        assert(usage);
        Buffer::data(target, usage, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void subData(Args&&... args) const
    {
        assert(target);
        Buffer::subData(target, std::forward<Args>(args)...);
    }
};

// This class saves a GL Buffer and it's local data.
class BufferData : public DefaultBuffer {
public:
    template <typename... Args>
    BufferData(Buffer::Target target, Buffer::UsageHint usage, Args&&... args)
        : DefaultBuffer(target, usage)
        , data_(glw::makeByteVector(std::forward<Args>(args)...))
    {
    }

    ~BufferData() = default;

    BufferData(const BufferData&) = delete;
    BufferData& operator=(const BufferData&) = delete;

    BufferData(BufferData&&) = default;
    BufferData& operator=(BufferData&&) = default;

    void setData()
    {
        data(data_.data(), data_.size());
    }

    void setSubData(size_t index, size_t len) const
    {
        // If I understand the reference pages correctly, it should be fine to call glBufferSubData
        // with size = 0, if you have not allocated a data store with glBufferData for that buffer
        // object yet (it's super sketch though).
        subData(index, data_.data() + index, len);
    }

    void setSubData() const
    {
        subData(data_.data(), data_.size());
    }

    void update()
    {
        if (getSize() < data_.size())
            setData();
        else
            setSubData();
    }

    std::vector<uint8_t>& getData()
    {
        return data_;
    }

    const std::vector<uint8_t>& getData() const
    {
        return data_;
    }

private:
    std::vector<uint8_t> data_;
};

class VertexBuffer : public BufferData {
public:
    template <typename... Args>
    VertexBuffer(const glw::VertexFormat& vfmt, Buffer::UsageHint usage, Args&&... args)
        : BufferData(Buffer::Target::Array, usage, std::forward<Args>(args)...)
        , vertexFormat_(vfmt)
    {
    }

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&&) = default;
    VertexBuffer& operator=(VertexBuffer&&) = default;

    const glw::VertexFormat& getVertexFormat() const
    {
        return vertexFormat_;
    }

    size_t getCount() const
    {
        assert(getSize() % vertexFormat_.getStride() == 0);
        return getSize() / vertexFormat_.getStride();
    }

private:
    glw::VertexFormat vertexFormat_;
};

class IndexBuffer : public BufferData {
public:
    enum class ElementType : GLenum {
        U8 = GL_UNSIGNED_BYTE,
        U16 = GL_UNSIGNED_SHORT,
        U32 = GL_UNSIGNED_INT,
    };

    template <typename... Args>
    IndexBuffer(ElementType type, Buffer::UsageHint usage, Args&&... args)
        : BufferData(Buffer::Target::ElementArray, usage, std::forward<Args>(args)...)
        , elementType_(type)
    {
    }

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    IndexBuffer(IndexBuffer&&) = default;
    IndexBuffer& operator=(IndexBuffer&&) = default;

    ElementType getElementType() const
    {
        return elementType_;
    }

    size_t getElementSize() const
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

    size_t getCount() const
    {
        assert(getSize() % getElementSize() == 0);
        return getSize() / getElementSize();
    }

private:
    ElementType elementType_;
};

}
