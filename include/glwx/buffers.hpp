#pragma once

#include <cassert>
#include <vector>

#include "../buffer.hpp"
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

    DefaultBuffer(Target target);
    DefaultBuffer(UsageHint usage);
    DefaultBuffer(Target target, UsageHint usage);

    void bind() const;

    void unbind() const;

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

    void setData();
    void setSubData(size_t index, size_t len) const;
    void setSubData() const;
    void update();

    std::vector<uint8_t>& getData();
    const std::vector<uint8_t>& getData() const;

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

    void resize(size_t vertexCount);

    const glw::VertexFormat& getVertexFormat() const;

    size_t getCount() const;

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

    static ElementType getElementType(size_t vertexCount);

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

    void resize(size_t indexCount);

    ElementType getElementType() const;

    size_t getElementSize() const;

    size_t getCount() const;

private:
    ElementType elementType_;
};

}
