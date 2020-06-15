#pragma once

#include <cassert>
#include <vector>

#include "glad/glad.h"

namespace glw {
// I could have this thing store a Target to use as a default target for method
// overloads that don't take a Target parameter, but I want this to be a thin
// zero-overhead abstraction above the GL buffer.
class Buffer {
public:
    enum class Target : GLenum {
        Undefined = 0,
        Array = GL_ARRAY_BUFFER,
        ElementArray = GL_ELEMENT_ARRAY_BUFFER,
        Uniform = GL_UNIFORM_BUFFER,
    };

    enum class UsageHint : GLenum {
        Undefined = 0,
        StreamDraw = GL_STREAM_DRAW,
        StreamRead = GL_STREAM_READ,
        StreamCopy = GL_STREAM_COPY,
        StaticDraw = GL_STATIC_DRAW,
        StaticRead = GL_STATIC_READ,
        StaticCopy = GL_STATIC_COPY,
        DynamicDraw = GL_DYNAMIC_DRAW,
        DynamicRead = GL_DYNAMIC_READ,
        DynamicCopy = GL_DYNAMIC_COPY,
    };

    static void unbind(Target target)
    {
        glBindBuffer(static_cast<GLenum>(target), 0);
    }

    Buffer()
        : vbo_(0)
    {
        glGenBuffers(1, &vbo_);
    }

    ~Buffer()
    {
        free();
    }

    Buffer(const Buffer& other) = delete;
    Buffer& operator=(const Buffer& other) = delete;

    Buffer(Buffer&& other)
        : vbo_(other.vbo_)
    {
        other.vbo_ = 0;
    }

    Buffer& operator=(Buffer&& other)
    {
        free();
        vbo_ = other.vbo_;
        other.vbo_ = 0;
        return *this;
    }

    void free()
    {
        if (vbo_)
            glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }

    // http://hacksoflife.blogspot.de/2015/06/glmapbuffer-no-longer-cool.html
    // => Don't implement map()?

    void bind(Target target) const
    {
        glBindBuffer(static_cast<GLenum>(target), vbo_);
    }

    void data(Target target, size_t size, const void* data, UsageHint usage) const
    {
        glBufferData(static_cast<GLenum>(target), size, data, static_cast<GLenum>(usage));
    }

    void subData(Target target, size_t offset, size_t size, const void* data) const
    {
        glBufferSubData(static_cast<GLenum>(target), offset, size, data);
    }

    GLuint getVbo() const
    {
        return vbo_;
    }

private:
    GLuint vbo_;
};

// Most of the time a single buffer will have the same usage and target, this
// class saves those too.
// This is a bad name for this class
struct DefaultBuffer : public Buffer {
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

    void data(size_t size, const void* data, UsageHint usage) const
    {
        assert(target);
        Buffer::data(target, size, data, usage);
    }

    void data(Target target, size_t size, const void* data) const
    {
        assert(usage);
        Buffer::data(target, size, data, usage);
    }

    void data(size_t size, const void* data)
    {
        assert(target);
        assert(usage);
        Buffer::data(target, size, data, usage);
    }

    void subData(size_t offset, size_t size, const void* data) const
    {
        assert(target);
        Buffer::subData(target, offset, size, data);
    }
};

// I dislike this design heavily, because I make BufferData::bind a virtual function call,
// but the BufferData is only bound, when the VAO is initialized, so it should be fine?
// I need the common base class, so I can put different BufferData (different template args) into a
// single Mesh.
class BufferDataBase {
public:
    virtual ~BufferDataBase()
    {
    }

    virtual void bind() const = 0;
    virtual void unbind() const = 0;
    virtual size_t getSize() const = 0;
    // I only need this for index buffers. This is horrible!
    virtual size_t getElementSize() const = 0;
};

// This class saves a GL Buffer and it's local data.
template <typename Element = uint8_t>
class BufferData : public BufferDataBase {
public:
    using ElementType = Element;

    template <typename... Args>
    BufferData(Buffer::Target target, Buffer::UsageHint usage, Args&&... args)
        : data_(std::forward<Args>(args)...)
        , buffer_(target, usage)
    {
    }

    ~BufferData() = default;

    void bind() const override
    {
        buffer_.bind();
    }

    void unbind() const override
    {
        buffer_.unbind();
    }

    void setData()
    {
        buffer_.bind();
        buffer_.data(sizeof(Element) * data_.size(), data_.data());
        lastUploadedSize_ = data_.size();
        buffer_.unbind();
    }

    void setSubData(size_t index, size_t len) const
    {
        // If I understand the reference pages correctly, it should be fine to call glBufferSubData
        // with size = 0, if you have not allocated a data store with glBufferData for that buffer
        // object yet (it's super sketch though).
        buffer_.bind();
        buffer_.subData(index * sizeof(Element), len * sizeof(Element), data_.data() + index);
        buffer_.unbind();
    }

    void setSubData()
    {
        setSubData(0, data_.size());
    }

    void update()
    {
        if (lastUploadedSize_ < data_.size())
            setData();
        else
            setSubData();
    }

    std::vector<Element>& getData()
    {
        return data_;
    }

    const std::vector<Element>& getData() const
    {
        return data_;
    }

    size_t getSize() const override
    {
        return data_.size();
    }

    size_t getElementSize() const override
    {
        return sizeof(Element);
    }

private:
    std::vector<Element> data_;
    DefaultBuffer buffer_;
    size_t lastUploadedSize_ = 0;
};

template <typename T>
constexpr bool isIndexType = std::is_same<T, uint8_t>::value || std::is_same<T, uint16_t>::value
    || std::is_same<T, uint32_t>::value;

inline GLenum getIndexType(size_t indexSize)
{
    assert(indexSize == 1 || indexSize == 2 || indexSize == 4);
    if (indexSize == 1) {
        return GL_UNSIGNED_BYTE;
    } else if (indexSize == 2) {
        return GL_UNSIGNED_SHORT;
    } else if (indexSize == 4) {
        return GL_UNSIGNED_INT;
    }
    return 0;
}
}
