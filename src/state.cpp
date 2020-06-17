#include "state.hpp"

namespace glw {
State& State::instance()
{
    static State inst;
    return inst;
}

std::tuple<int, int, size_t, size_t> State::getViewport() const
{
    return currentViewport_;
}

void State::setViewport(int x, int y, size_t width, size_t height)
{
    glViewport(x, y, width, height);
    currentViewport_ = std::tuple(x, y, width, height);
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

GLuint State::getCurrentVao() const
{
    return currentVao_;
}

void State::bindVao(GLuint vao)
{
    glBindVertexArray(vao);
    currentVao_ = vao;
}

void State::unbindVao()
{
    bindVao(0);
}

GLuint State::getCurrentShader() const
{
    return currentShaderProgram_;
}

void State::bindShader(GLuint prog)
{
    if (currentShaderProgram_ == prog)
        return;
    glUseProgram(prog);
    currentShaderProgram_ = prog;
}

void State::unbindShader()
{
    bindShader(0);
}

GLuint State::getCurrentBuffer(GLenum target) const
{
    return currentBuffers_[getBufferIndex(target)];
}

void State::bindBuffer(GLenum target, GLuint buffer)
{
    auto& currentBuffer = currentBuffers_[getBufferIndex(target)];
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
    return currentTextures_[unit][getTextureIndex(target)];
}

std::optional<unsigned int> State::getTextureUnit(GLenum target, GLuint texture) const
{
    for (size_t unit = 0; unit < currentTextures_.size(); ++unit) {
        if (getCurrentTexture(unit, target) == texture)
            return unit;
    }
    return std::nullopt;
}

void State::bindTexture(unsigned int unit, GLenum target, GLuint texture)
{
    auto& currentTexture = currentTextures_[unit][getTextureIndex(target)];
    if (currentTexture == texture)
        return;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target, texture);
    currentTexture = texture;
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
        assert(currentReadFramebuffer_ == currentDrawFramebuffer_);
        return currentReadFramebuffer_;
    case GL_READ_FRAMEBUFFER:
        return currentReadFramebuffer_;
    case GL_DRAW_FRAMEBUFFER:
        return currentDrawFramebuffer_;
    default:
        assert(false && "Invalid Framebuffer target");
    }
}

void State::bindFramebuffer(GLenum target, GLuint fbo)
{
    switch (target) {
    case GL_FRAMEBUFFER:
        if (currentReadFramebuffer_ == fbo && currentDrawFramebuffer_ == fbo)
            return;
        break;
    case GL_READ_FRAMEBUFFER:
        if (currentReadFramebuffer_ == fbo)
            return;
        break;
    case GL_DRAW_FRAMEBUFFER:
        if (currentDrawFramebuffer_ == fbo)
            return;
        break;
    }

    glBindFramebuffer(target, fbo);
    if (target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER)
        currentReadFramebuffer_ = fbo;
    if (target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER)
        currentDrawFramebuffer_ = fbo;
}

void State::unbindFramebuffer(GLenum target)
{
    bindFramebuffer(target, 0);
}

GLuint State::getCurrentRenderbuffer() const
{
    return currentRenderbuffer_;
}

void State::bindRenderbuffer(GLuint rbo)
{
    if (currentRenderbuffer_ == rbo)
        return;
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
}

void State::unbindRenderbuffer()
{
    bindRenderbuffer(0);
}
}
