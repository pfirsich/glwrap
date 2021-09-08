#pragma once

#include <cassert>
#include <vector>

#include "glw/buffer.hpp"
#include "glw/utility.hpp"
#include "glw/vertexformat.hpp"

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
        assert(target != Target::Undefined);
        Buffer::data(target, usage, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void data(Target target, Args&&... args)
    {
        assert(usage != UsageHint::Undefined);
        Buffer::data(target, usage, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void data(Args&&... args)
    {
        assert(target != Target::Undefined);
        assert(usage != UsageHint::Undefined);
        Buffer::data(target, usage, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void subData(Args&&... args) const
    {
        assert(target != Target::Undefined);
        Buffer::subData(target, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void update(Args&&... args)
    {
        assert(target != Target::Undefined);
        assert(usage != UsageHint::Undefined);
        Buffer::update(target, usage, std::forward<Args>(args)...);
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
    template <typename... Args>
    IndexBuffer(glw::IndexType type, Buffer::UsageHint usage, Args&&... args)
        : BufferData(Buffer::Target::ElementArray, usage, std::forward<Args>(args)...)
        , indexType_(type)
    {
    }

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    IndexBuffer(IndexBuffer&&) = default;
    IndexBuffer& operator=(IndexBuffer&&) = default;

    void resize(size_t indexCount);

    glw::IndexType getIndexType() const;
    size_t getElementSize() const;

    size_t getCount() const;

private:
    glw::IndexType indexType_;
};

}
