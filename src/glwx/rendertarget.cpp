#include "rendertarget.hpp"

using namespace glw;

namespace glwx {
RenderTarget::RenderTarget(size_t width, size_t height)
    : width_(width)
    , height_(height)
{
}

std::optional<RenderTarget::Attachment> RenderTarget::getNextAttachment(
    glw::ImageFormat format) const
{
    using A = Attachment;
    const auto fmtHasDepth = hasDepth(format);
    const auto fmtHasStencil = hasStencil(format);
    const auto depthStencilAttached = attachments_.count(A::DepthStencil) > 0;
    const auto depthAttached = depthStencilAttached || attachments_.count(A::Depth) > 0;
    const auto stencilAttached = depthStencilAttached || attachments_.count(A::Stencil) > 0;
    if (fmtHasDepth && fmtHasStencil) {
        if (depthAttached || stencilAttached)
            return std::nullopt;
        return A::DepthStencil;
    } else if (fmtHasDepth) {
        if (depthAttached)
            return std::nullopt;
        return A::Depth;
    } else if (fmtHasStencil) {
        if (stencilAttached)
            return std::nullopt;
        return A::Stencil;
    } else {
        const auto num = static_cast<GLenum>(A::Color7) - static_cast<GLenum>(A::Color0) + 1;
        for (size_t i = 0; i < num; ++i) {
            const auto att = static_cast<A>(static_cast<GLenum>(A::Color0) + i);
            if (!attachments_.count(att))
                return att;
        }
        return std::nullopt;
    }
}

void RenderTarget::attach(Attachment attachment, Texture&& tex)
{
    framebuffer_.texture(attachment, tex);
    textures_.insert_or_assign(attachment, std::move(tex));
    attachments_.insert(attachment);
}

void RenderTarget::attach(Attachment attachment, Renderbuffer&& rbuf)
{
    framebuffer_.renderbuffer(attachment, rbuf);
    renderBuffers_.insert_or_assign(attachment, std::move(rbuf));
    attachments_.insert(attachment);
}

Texture& RenderTarget::emplaceTexture(Attachment attachment, ImageFormat format)
{
    auto& tex = textures_.emplace(attachment, Texture::Target::Texture2D).first->second;
    tex.storage(format, width_, height_);
    tex.setFilter(Texture::MinFilter::Linear, Texture::MagFilter::Linear);
    framebuffer_.texture(attachment, tex);
    attachments_.insert(attachment);
    return tex;
}

RenderTarget::Attachment RenderTarget::emplaceTexture(glw::ImageFormat format)
{
    const auto attachment = getNextAttachment(format);
    assert(attachment);
    emplaceTexture(*attachment, format);
    return *attachment;
}

Renderbuffer& RenderTarget::emplaceRenderbuffer(Attachment attachment, ImageFormat format)
{
    auto& rbuf = renderBuffers_.emplace(attachment, Renderbuffer {}).first->second;
    rbuf.storage(format, width_, height_);
    framebuffer_.renderbuffer(attachment, rbuf);
    attachments_.insert(attachment);
    return rbuf;
}

RenderTarget::Attachment RenderTarget::emplaceRenderbuffer(glw::ImageFormat format)
{
    const auto attachment = getNextAttachment(format);
    assert(attachment);
    emplaceRenderbuffer(*attachment, format);
    return *attachment;
}

glw::Texture& RenderTarget::getTexture(Attachment attachment)
{
    return textures_.at(attachment);
}

glw::Renderbuffer& RenderTarget::getRenderbuffer(Attachment attachment)
{
    return renderBuffers_.at(attachment);
}

void RenderTarget::bind() const
{
    framebuffer_.bind();
    State::instance().setViewport(0, 0, width_, height_);
}

glw::Framebuffer& RenderTarget::getFramebuffer()
{
    return framebuffer_;
}

size_t RenderTarget::getWidth() const
{
    return width_;
}

size_t RenderTarget::getHeight() const
{
    return height_;
}

RenderTarget makeRenderTarget(size_t width, size_t height,
    const std::vector<glw::ImageFormat>& textureAttachments,
    const std::vector<glw::ImageFormat>& renderbufferAttachments)
{
    RenderTarget rt(width, height);
    for (auto fmt : textureAttachments)
        rt.emplaceTexture(fmt);
    for (auto fmt : renderbufferAttachments)
        rt.emplaceRenderbuffer(fmt);
    assert(rt.getFramebuffer().isComplete());
    glw::Framebuffer::unbind();
    return rt;
}
}
