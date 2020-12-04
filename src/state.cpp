#include "state.hpp"

namespace glw {
State& State::instance()
{
    static State inst;
    return inst;
}

std::tuple<int, int, size_t, size_t> State::getViewport() const
{
    return viewport_;
}

void State::setViewport(int x, int y, size_t width, size_t height)
{
    glViewport(x, y, width, height);
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
    glDepthFunc(static_cast<GLenum>(func));
    depthFunc_ = func;
}

GLuint State::getCurrentVao() const
{
    return vao_;
}

void State::bindVao(GLuint vao)
{
    glBindVertexArray(vao);
    vao_ = vao;
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
        if (getCurrentTexture(unit, target) == texture)
            return unit;
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
