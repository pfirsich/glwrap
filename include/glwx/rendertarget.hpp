#pragma once

#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

#include "glw/framebuffer.hpp"

namespace glwx {
// This is essentially a framebuffer that also contains(owns) all it's attachments
class RenderTarget {
public:
    using Attachment = glw::Framebuffer::Attachment;

    RenderTarget(size_t width, size_t height);

    std::optional<Attachment> getNextAttachment(glw::ImageFormat format) const;

    void attach(Attachment attachment, glw::Texture&& tex);
    void attach(Attachment attachment, glw::Renderbuffer&& rbuf);

    glw::Texture& emplaceTexture(Attachment attachment, glw::ImageFormat format);
    Attachment emplaceTexture(glw::ImageFormat format);

    glw::Renderbuffer& emplaceRenderbuffer(Attachment attachment, glw::ImageFormat format);
    Attachment emplaceRenderbuffer(glw::ImageFormat format);

    glw::Texture& getTexture(Attachment attachment);
    glw::Renderbuffer& getRenderbuffer(Attachment attachment);

    void bind() const;

    glw::Framebuffer& getFramebuffer();

    size_t getWidth() const;
    size_t getHeight() const;

private:
    std::unordered_map<Attachment, glw::Texture> textures_;
    std::unordered_map<Attachment, glw::Renderbuffer> renderBuffers_;
    std::set<Attachment> attachments_;
    glw::Framebuffer framebuffer_;
    size_t width_;
    size_t height_;
};

RenderTarget makeRenderTarget(size_t width, size_t height,
    const std::vector<glw::ImageFormat>& textureAttachments,
    const std::vector<glw::ImageFormat>& renderbufferAttachments);
}
