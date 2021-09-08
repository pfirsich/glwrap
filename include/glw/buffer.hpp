#pragma once

#include "glad/glad.h"

#include "glw/state.hpp"
#include "glw/utility.hpp"

namespace glw {
// I could have this thing store a Target to use as a default target for method
// overloads that don't take a Target parameter, but I want this to be a thin
// zero-overhead abstraction above the GL buffer.
class Buffer {
public:
    enum class Target : GLenum {
        Undefined = 0,
        Array = GL_ARRAY_BUFFER,
        // AtomicCounter = GL_ATOMIC_COUNTER_BUFFER, // 4.2
        CopyRead = GL_COPY_READ_BUFFER,
        CopyWrite = GL_COPY_WRITE_BUFFER,
        // DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER, // 4.3
        // DrawIndirect = GL_DRAW_INDIRECT_BUFFER, // 4.0
        ElementArray = GL_ELEMENT_ARRAY_BUFFER,
        PixelPack = GL_PIXEL_PACK_BUFFER,
        PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
        // Query = GL_QUERY_BUFFER, // 4.4
        // ShaderStorage = GL_SHADER_STORAGE_BUFFER, // 4.3
        Texture = GL_TEXTURE_BUFFER,
        TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
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

    static void unbind(Target target);

    Buffer();
    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer& operator=(const Buffer& other) = delete;

    Buffer(Buffer&& other);
    Buffer& operator=(Buffer&& other);

    void free();

    // http://hacksoflife.blogspot.de/2015/06/glmapbuffer-no-longer-cool.html
    // => Don't implement map()?

    void bind(Target target) const;

    template <typename... Args>
    void data(Target target, UsageHint usage, Args&&... args)
    {
        bind(target);
        const auto [data, size] = toPtrRange(std::forward<Args>(args)...);
        glBufferData(static_cast<GLenum>(target), size, data, static_cast<GLenum>(usage));
        size_ = size;
        unbind(target);
    }

    template <typename... Args>
    void subData(Target target, size_t offset, Args&&... args) const
    {
        bind(target);
        const auto [byteOffset, data, size] = toOffsetPtrRange(offset, std::forward<Args>(args)...);
        glBufferSubData(static_cast<GLenum>(target), byteOffset, size, data);
        unbind(target);
    }

    template <typename... Args>
    void subData(Target target, Args&&... args) const
    {
        bind(target);
        const auto [data, size] = toPtrRange(std::forward<Args>(args)...);
        glBufferSubData(static_cast<GLenum>(target), 0, size, data);
        unbind(target);
    }

    template <typename... Args>
    void update(Target target, UsageHint usage, Args&&... args)
    {
        const auto [ptr, size] = toPtrRange(std::forward<Args>(args)...);
        if (size_ < size) {
            data(target, usage, ptr, size);
        } else {
            subData(target, ptr, size);
        }
    }

    GLuint getBuffer() const;

    size_t getSize() const;

private:
    GLuint buffer_ = 0;
    // I know this is not really in the spirit of these "thin" OpenGL wrappers, but it is necessary
    // to keep track of the data store size in a couple of places and it would be *way* dirtier
    // to keep track of it in other (multiple!) places.
    size_t size_ = 0;
};

}
