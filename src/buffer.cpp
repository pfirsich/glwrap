#include "buffer.hpp"

namespace glw {
void Buffer::unbind(Target target)
{
    State::instance().unbindBuffer(static_cast<GLenum>(target));
}

Buffer::Buffer()
{
    glGenBuffers(1, &vbo_);
}

Buffer::~Buffer()
{
    free();
}

Buffer::Buffer(Buffer&& other)
    : vbo_(other.vbo_)
    , size_(other.size_)
{
    other.vbo_ = 0;
    other.size_ = 0;
}

Buffer& Buffer::operator=(Buffer&& other)
{
    free();
    vbo_ = other.vbo_;
    size_ = other.size_;
    other.vbo_ = 0;
    other.size_ = 0;
    return *this;
}

void Buffer::free()
{
    if (vbo_)
        glDeleteBuffers(1, &vbo_);
    vbo_ = 0;
}

// http://hacksoflife.blogspot.de/2015/06/glmapbuffer-no-longer-cool.html
// => Don't implement map()?

void Buffer::bind(Target target) const
{
    State::instance().bindBuffer(static_cast<GLenum>(target), vbo_);
}

GLuint Buffer::getVbo() const
{
    return vbo_;
}

size_t Buffer::getSize() const
{
    return size_;
}
}
