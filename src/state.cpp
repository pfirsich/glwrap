#include "state.hpp"

namespace glw {
State& State::instance()
{
    static State inst;
    return inst;
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
}
