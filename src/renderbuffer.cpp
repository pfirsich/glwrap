#include "renderbuffer.hpp"

namespace glw {
Renderbuffer::Renderbuffer()
{
    glGenRenderbuffers(1, &rbo_);
}

Renderbuffer::~Renderbuffer()
{
    free();
}

Renderbuffer::Renderbuffer(Renderbuffer&& other)
    : rbo_(other.rbo_)
{
    other.reset();
}

Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other)
{
    free();
    rbo_ = other.rbo_;
    other.reset();
    return *this;
}

void Renderbuffer::unbind()
{
    State::instance().unbindRenderbuffer();
}

void Renderbuffer::bind() const
{
    State::instance().bindRenderbuffer(rbo_);
}

void Renderbuffer::storage(ImageFormat format, size_t width, size_t height)
{
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(format), width, height);
}

GLuint Renderbuffer::getRenderbuffer() const
{
    return rbo_;
}

void Renderbuffer::reset()
{
    rbo_ = 0;
}

void Renderbuffer::free()
{
    glDeleteRenderbuffers(1, &rbo_);
}
}
