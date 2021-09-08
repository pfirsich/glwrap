#include "glw/framebuffer.hpp"

namespace glw {
Framebuffer::Framebuffer()
{
    glGenFramebuffers(1, &fbo_);
}

Framebuffer::~Framebuffer()
{
    free();
}

Framebuffer::Framebuffer(Framebuffer&& other)
    : fbo_(other.fbo_)
{
    other.reset();
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other)
{
    free();
    fbo_ = other.fbo_;
    other.reset();
    return *this;
}

void Framebuffer::unbind(Target target)
{
    State::instance().unbindFramebuffer(static_cast<GLenum>(target));
}

void Framebuffer::bind(Target target) const
{
    State::instance().bindFramebuffer(static_cast<GLenum>(target), fbo_);
}

void Framebuffer::texture(
    Target target, Attachment attachment, const Texture& tex, size_t level) const
{
    bind(target);
    glFramebufferTexture(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        tex.getTexture(), static_cast<GLint>(level));
}

void Framebuffer::texture(Attachment attachment, const Texture& tex, size_t level) const
{
    texture(Target::Both, attachment, tex, level);
}

void Framebuffer::texture2D(Target target, Attachment attachment, Texture::Target texTarget,
    const Texture& tex, size_t level) const
{
    bind(target);
    glFramebufferTexture2D(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        static_cast<GLenum>(texTarget), tex.getTexture(), static_cast<GLint>(level));
}

void Framebuffer::texture2D(
    Attachment attachment, Texture::Target texTarget, const Texture& tex, size_t level) const
{
    texture2D(Target::Both, attachment, texTarget, tex, level);
}

void Framebuffer::renderbuffer(Target target, Attachment attachment, const Renderbuffer& rbuf) const
{
    bind(target);
    glFramebufferRenderbuffer(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        GL_RENDERBUFFER, rbuf.getRenderbuffer());
}

void Framebuffer::renderbuffer(Attachment attachment, const Renderbuffer& rbuf) const
{
    renderbuffer(Target::Both, attachment, rbuf);
}

void Framebuffer::detach(Target target, Attachment attachment)
{
    bind(target);
    glFramebufferRenderbuffer(
        static_cast<GLenum>(target), static_cast<GLenum>(attachment), GL_RENDERBUFFER, 0);
}

void Framebuffer::detach(Attachment attachment)
{
    detach(Target::Both, attachment);
}

Framebuffer::Status Framebuffer::getStatus(Target target) const
{
    bind(target);
    return static_cast<Status>(glCheckFramebufferStatus(static_cast<GLenum>(target)));
}

bool Framebuffer::isComplete() const
{
    return getStatus() == Status::Complete;
}

GLuint Framebuffer::getFramebuffer() const
{
    return fbo_;
}

void Framebuffer::reset()
{
    fbo_ = 0;
}

void Framebuffer::free() const
{
    glDeleteFramebuffers(1, &fbo_);
}
}
