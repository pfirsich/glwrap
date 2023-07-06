#include "glw/state.hpp"

namespace glw {
namespace {
    void setEnabled(GLenum capability, bool enabled)
    {
        if (enabled) {
            glEnable(capability);
        } else {
            glDisable(capability);
        }
    }
}

State& State::instance()
{
    static State inst;
    return inst;
}

void State::resetStatistics()
{
    statistics_ = Statistics {};
}
State::Statistics& State::getStatistics()
{
    return statistics_;
}

std::tuple<int, int, size_t, size_t> State::getViewport() const
{
    return viewport_;
}

void State::setViewport(int x, int y, size_t width, size_t height)
{
    glViewport(x, y, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    viewport_ = std::tuple(x, y, width, height);
}

void State::setViewport(const std::tuple<int, int, size_t, size_t>& viewport)
{
    const auto [x, y, width, height] = viewport;
    setViewport(x, y, width, height);
}

void State::setViewport(size_t width, size_t height)
{
    setViewport(0, 0, width, height);
}

DepthFunc State::getDepthFunc() const
{
    return depthFunc_;
}

void State::setDepthFunc(DepthFunc func)
{
    if (depthFunc_ == func)
        return;
    glDepthFunc(static_cast<GLenum>(func));
    depthFunc_ = func;
}

bool State::getDepthWrite() const
{
    return depthWrite_;
}

void State::setDepthWrite(bool write)
{
    if (depthWrite_ == write)
        return;
    glDepthMask(write ? GL_TRUE : GL_FALSE);
    depthWrite_ = write;
}

bool State::getCullFaceEnabled() const
{
    return cullFaceEnabled_;
}

void State::setCullFaceEnabled(bool enabled)
{
    if (cullFaceEnabled_ == enabled)
        return;
    setEnabled(GL_CULL_FACE, enabled);
    cullFaceEnabled_ = enabled;
}

FrontFaceMode State::getFrontFaceMode() const
{
    return frontFaceMode_;
}

void State::setFrontFaceMode(FrontFaceMode mode)
{
    if (frontFaceMode_ == mode)
        return;
    glFrontFace(static_cast<GLenum>(mode));
    frontFaceMode_ = mode;
}

FaceCullMode State::getFaceCullMode() const
{
    return faceCullMode_;
}

void State::setFaceCullMode(FaceCullMode mode)
{
    if (faceCullMode_ == mode)
        return;
    glCullFace(static_cast<GLenum>(mode));
    faceCullMode_ = mode;
}

bool State::getBlendEnabled() const
{
    return blendEnabled_;
}

void State::setBlendEnabled(bool enabled)
{
    if (blendEnabled_ == enabled)
        return;
    setEnabled(GL_BLEND, enabled);
    blendEnabled_ = enabled;
}

std::tuple<float, float, float, float> State::getBlendColor() const
{
    return blendColor_;
}

void State::setBlendColor(float r, float g, float b, float a)
{
    const auto color = std::tuple(r, g, b, a);
    if (blendColor_ != color)
        return;
    glBlendColor(r, g, b, a);
    blendColor_ = color;
}

void State::setBlendColor(const std::tuple<float, float, float, float>& color)
{
    setBlendColor(std::get<0>(color), std::get<1>(color), std::get<2>(color), std::get<3>(color));
}

BlendFuncSeparate State::getBlendFunc() const
{
    return blendFunc_;
}

void State::setBlendFunc(BlendFunc srcRgb, BlendFunc srcAlpha, BlendFunc dstRgb, BlendFunc dstAlpha)
{
    if (blendFunc_.srcRgb != srcRgb || blendFunc_.srcAlpha != srcAlpha
        || blendFunc_.dstRgb != dstRgb || blendFunc_.dstAlpha != dstAlpha)
        return;
    glBlendFuncSeparate(static_cast<GLenum>(srcRgb), static_cast<GLenum>(dstRgb),
        static_cast<GLenum>(srcAlpha), static_cast<GLenum>(dstAlpha));
    blendFunc_ = BlendFuncSeparate {
        .srcRgb = srcRgb, .srcAlpha = srcAlpha, .dstRgb = dstRgb, .dstAlpha = dstAlpha
    };
}

void State::setBlendFunc(const BlendFuncSeparate& func)
{
    setBlendFunc(func.srcRgb, func.srcAlpha, func.dstRgb, func.dstAlpha);
}

void State::setBlendFunc(BlendFunc src, BlendFunc dst)
{
    setBlendFunc(src, src, dst, dst);
}

BlendEquationRgba State::getBlendEquation() const
{
    return blendEquation_;
}

void State::setBlendEquation(BlendEquation rgb, BlendEquation a)
{
    if (blendEquation_.rgb != rgb || blendEquation_.a != a)
        return;
    glBlendEquationSeparate(static_cast<GLenum>(rgb), static_cast<GLenum>(a));
    blendEquation_ = BlendEquationRgba { .rgb = rgb, .a = a };
}

void State::setBlendEquation(const BlendEquationRgba& eq)
{
    setBlendEquation(eq.rgb, eq.a);
}

void State::setBlendEquation(BlendEquation eq)
{
    setBlendEquation(eq, eq);
}

GLuint State::getCurrentVao() const
{
    return vao_;
}

void State::bindVao(GLuint vao)
{
    glBindVertexArray(vao);
    vao_ = vao;
    statistics_.vertexArrayBinds++;
}

void State::unbindVao()
{
    bindVao(0);
}

GLuint State::getCurrentShader() const
{
    return shaderProgram_;
}

void State::bindShader(GLuint prog)
{
    if (shaderProgram_ == prog)
        return;
    glUseProgram(prog);
    shaderProgram_ = prog;
    statistics_.shaderBinds++;
}

void State::unbindShader()
{
    bindShader(0);
}

GLuint State::getCurrentBuffer(GLenum target) const
{
    return buffers_[getBufferIndex(target)];
}

void State::bindBuffer(GLenum target, GLuint buffer)
{
    auto& currentBuffer = buffers_[getBufferIndex(target)];
    if (currentBuffer == buffer)
        return;
    glBindBuffer(target, buffer);
    currentBuffer = buffer;
}

void State::unbindBuffer(GLenum target)
{
    bindBuffer(target, 0);
}

GLuint State::getCurrentTexture(unsigned int unit, GLenum target) const
{
    return textures_[unit][getTextureIndex(target)];
}

std::optional<unsigned int> State::getTextureUnit(GLenum target, GLuint texture) const
{
    for (size_t unit = 0; unit < textures_.size(); ++unit) {
        if (getCurrentTexture(static_cast<unsigned int>(unit), target) == texture)
            return static_cast<unsigned int>(unit);
    }
    return std::nullopt;
}

void State::bindTexture(unsigned int unit, GLenum target, GLuint texture)
{
    auto& currentTexture = textures_[unit][getTextureIndex(target)];
    if (currentTexture == texture)
        return;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, texture);
    currentTexture = texture;
    statistics_.textureBinds++;
}

void State::unbindTexture(unsigned int unit, GLenum target)
{
    bindTexture(unit, target, 0);
}

size_t State::getBufferIndex(GLenum target)
{
    return getTargetIndex(bufferBindings, target);
}

size_t State::getTextureIndex(GLenum target)
{
    return getTargetIndex(textureBindings, target);
}

GLuint State::getCurrentFramebuffer(GLenum target) const
{
    switch (target) {
    case GL_FRAMEBUFFER:
        assert(readFramebuffer_ == drawFramebuffer_);
        return readFramebuffer_;
    case GL_READ_FRAMEBUFFER:
        return readFramebuffer_;
    case GL_DRAW_FRAMEBUFFER:
        return drawFramebuffer_;
    }
    std::abort();
}

void State::bindFramebuffer(GLenum target, GLuint fbo)
{
    switch (target) {
    case GL_FRAMEBUFFER:
        if (readFramebuffer_ == fbo && drawFramebuffer_ == fbo)
            return;
        break;
    case GL_READ_FRAMEBUFFER:
        if (readFramebuffer_ == fbo)
            return;
        break;
    case GL_DRAW_FRAMEBUFFER:
        if (drawFramebuffer_ == fbo)
            return;
        break;
    }

    glBindFramebuffer(target, fbo);
    if (target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER)
        readFramebuffer_ = fbo;
    if (target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER)
        drawFramebuffer_ = fbo;
    statistics_.frameBufferBinds++;
}

void State::unbindFramebuffer(GLenum target)
{
    bindFramebuffer(target, 0);
}

GLuint State::getCurrentRenderbuffer() const
{
    return renderbuffer_;
}

void State::bindRenderbuffer(GLuint rbo)
{
    if (renderbuffer_ == rbo)
        return;
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
}

void State::unbindRenderbuffer()
{
    bindRenderbuffer(0);
}
}
