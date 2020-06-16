#pragma once

#include "glad/glad.h"

#include "utility.hpp"

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

    template <typename... Args>
    void data(Target target, UsageHint usage, Args&&... args) const
    {
        bind(target);
        const auto [data, size] = toPtrRange(std::forward<Args>(args)...);
        glBufferData(static_cast<GLenum>(target), size, data, static_cast<GLenum>(usage));
    }

    template <typename... Args>
    void subData(Target target, size_t offset, Args&&... args) const
    {
        bind(target);
        const auto [byteOffset, data, size] = toOffsetPtrRange(offset, std::forward<Args>(args)...);
        glBufferSubData(static_cast<GLenum>(target), byteOffset, size, data);
    }

    template <typename... Args>
    void subData(Target target, Args&&... args) const
    {
        bind(target);
        const auto [data, size] = toPtrRange(std::forward<Args>(args)...);
        glBufferSubData(static_cast<GLenum>(target), 0, size, data);
    }

    GLuint getVbo() const
    {
        return vbo_;
    }

private:
    GLuint vbo_;
};

}
