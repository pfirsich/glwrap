#include "glw/buffer.hpp"

namespace glw {
void Buffer::unbind(Target target)
{
    State::instance().unbindBuffer(static_cast<GLenum>(target));
}

Buffer::Buffer()
{
    glGenBuffers(1, &buffer_);
}

Buffer::~Buffer()
{
    free();
}

Buffer::Buffer(Buffer&& other)
    : buffer_(other.buffer_)
    , size_(other.size_)
{
    other.buffer_ = 0;
    other.size_ = 0;
}

Buffer& Buffer::operator=(Buffer&& other)
{
    free();
    buffer_ = other.buffer_;
    size_ = other.size_;
    other.buffer_ = 0;
    other.size_ = 0;
    return *this;
}

void Buffer::free()
{
    if (buffer_)
        glDeleteBuffers(1, &buffer_);
    buffer_ = 0;
}

// http://hacksoflife.blogspot.de/2015/06/glmapbuffer-no-longer-cool.html
// => Don't implement map()?

void Buffer::bind(Target target) const
{
    State::instance().bindBuffer(static_cast<GLenum>(target), buffer_);
}

GLuint Buffer::getBuffer() const
{
    return buffer_;
}

size_t Buffer::getSize() const
{
    return size_;
}
}
