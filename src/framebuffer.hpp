#pragma once

#include "glad/glad.h"

#include "renderbuffer.hpp"
#include "state.hpp"
#include "texture.hpp"

namespace glw {
class Framebuffer {
public:
    enum class Target : GLenum {
        Read = GL_READ_FRAMEBUFFER,
        Draw = GL_DRAW_FRAMEBUFFER,
        Both = GL_FRAMEBUFFER,
    };

    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCheckFramebufferStatus.xhtml
    enum class Status : GLenum {
        Complete = GL_FRAMEBUFFER_COMPLETE,
        Undefined = GL_FRAMEBUFFER_UNDEFINED,
        IncompleteAttachment = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
        IncompleteMissingAttachment = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
        IncompleteDrawBuffer = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
        IncompleteReadBuffer = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
        Unsupported = GL_FRAMEBUFFER_UNSUPPORTED,
        IncompleteMultisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
        IncompleteLayerTargets = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS,
    };

    enum class Attachment : GLenum {
        Color0 = GL_COLOR_ATTACHMENT0,
        Color1 = GL_COLOR_ATTACHMENT1,
        Color2 = GL_COLOR_ATTACHMENT2,
        Color3 = GL_COLOR_ATTACHMENT3,
        Color4 = GL_COLOR_ATTACHMENT4,
        Color5 = GL_COLOR_ATTACHMENT5,
        Color6 = GL_COLOR_ATTACHMENT6,
        Color7 = GL_COLOR_ATTACHMENT7,
        Depth = GL_DEPTH_ATTACHMENT,
        Stencil = GL_STENCIL_ATTACHMENT,
        DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
    };

    Framebuffer();
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other);
    Framebuffer& operator=(Framebuffer&& other);

    static void unbind(Target target = Target::Both);

    void bind(Target target = Target::Both) const;

    void texture(Target target, Attachment attachment, const Texture& tex, size_t level = 0) const;
    void texture(Attachment attachment, const Texture& tex, size_t level = 0) const;

    void texture2D(Target target, Attachment attachment, Texture::Target texTarget,
        const Texture& tex, size_t level = 0) const;
    void texture2D(Attachment attachment, Texture::Target texTarget, const Texture& tex,
        size_t level = 0) const;

    void renderbuffer(Target target, Attachment attachment, const Renderbuffer& rbuf) const;
    void renderbuffer(Attachment attachment, const Renderbuffer& rbuf) const;

    void detach(Target target, Attachment attachment);
    void detach(Attachment attachment);

    Status getStatus(Target target = Target::Draw) const;

    bool isComplete() const;

    GLuint getFramebuffer() const;

private:
    void reset();
    void free() const;

    GLuint fbo_ = 0;
};
}
