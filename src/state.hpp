#pragma once

#include <cassert>
#include <optional>

#include "glad/glad.h"

namespace glw {
class State {
public:
    // GL_MAX_TEXTURE_IMAGE_UNITS is 16 in GL 3.3
    static constexpr size_t maxTextureUnits = 16;

    static State& instance()
    {
        static State inst;
        return inst;
    }

    ~State() = default;

    GLuint getCurrentVao() const
    {
        return currentVao_;
    }

    void bindVao(GLuint vao)
    {
        glBindVertexArray(vao);
        currentVao_ = vao;
    }

    void unbindVao()
    {
        bindVao(0);
    }

    GLuint getCurrentShader() const
    {
        return currentShaderProgram_;
    }

    void bindShader(GLuint prog)
    {
        if (currentShaderProgram_ == prog)
            return;
        glUseProgram(prog);
        currentShaderProgram_ = prog;
    }

    void unbindShader()
    {
        bindShader(0);
    }

    void bindBuffer(GLenum target, GLuint buffer)
    {
        auto& currentBuffer = currentBuffers_[getBufferIndex(target)];
        if (currentBuffer == buffer)
            return;
        glBindBuffer(target, buffer);
        currentBuffer = buffer;
    }

    void unbindBuffer(GLenum target)
    {
        bindBuffer(target, 0);
    }

    void bindTexture(unsigned int unit, GLenum target, GLuint texture)
    {
        auto& currentTexture = currentTextures_[unit][getTextureIndex(target)];
        if (currentTexture == texture)
            return;
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(target, texture);
        currentTexture = texture;
    }

    void unbindTexture(unsigned int unit, GLenum target)
    {
        bindTexture(unit, target, 0);
    }

    GLuint getCurrentTexture(unsigned int unit, GLenum target) const
    {
        return currentTextures_[unit][getTextureIndex(target)];
    }

    std::optional<unsigned int> getTextureUnit(GLenum target, GLuint texture) const
    {
        for (size_t unit = 0; unit < currentTextures_.size(); ++unit) {
            if (getCurrentTexture(unit, target) == texture)
                return unit;
        }
        return std::nullopt;
    }

private:
    // I think specifying size explicitly is dangerous and I can't only specify type
    static constexpr std::array bufferBindings = {
        static_cast<GLenum>(GL_ARRAY_BUFFER),
        //  GL_ATOMIC_COUNTER_BUFFER, // 4.2
        static_cast<GLenum>(GL_COPY_READ_BUFFER),
        static_cast<GLenum>(GL_COPY_WRITE_BUFFER),
        //  GL_DISPATCH_INDIRECT_BUFFER, // 4.3
        //  GL_DRAW_INDIRECT_BUFFER, // 4.0
        static_cast<GLenum>(GL_ELEMENT_ARRAY_BUFFER),
        static_cast<GLenum>(GL_PIXEL_PACK_BUFFER),
        static_cast<GLenum>(GL_PIXEL_UNPACK_BUFFER),
        //  GL_QUERY_BUFFER, // 4.4
        //  GL_SHADER_STORAGE_BUFFER, // 4.3
        static_cast<GLenum>(GL_TEXTURE_BUFFER),
        static_cast<GLenum>(GL_TRANSFORM_FEEDBACK_BUFFER),
        static_cast<GLenum>(GL_UNIFORM_BUFFER),
    };

    static constexpr std::array textureBindings = {
        static_cast<GLenum>(GL_TEXTURE_1D),
        static_cast<GLenum>(GL_TEXTURE_2D),
        static_cast<GLenum>(GL_TEXTURE_3D),
        static_cast<GLenum>(GL_TEXTURE_1D_ARRAY),
        static_cast<GLenum>(GL_TEXTURE_2D_ARRAY),
        static_cast<GLenum>(GL_TEXTURE_RECTANGLE),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP),
        static_cast<GLenum>(GL_TEXTURE_BUFFER),
        static_cast<GLenum>(GL_TEXTURE_2D_MULTISAMPLE),
        static_cast<GLenum>(GL_TEXTURE_2D_MULTISAMPLE_ARRAY),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_X),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Y),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_Z),
        static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z),
    };

    static size_t getBufferIndex(GLenum target)
    {
        for (size_t i = 0; i < bufferBindings.size(); ++i)
            if (bufferBindings[i] == target)
                return i;
        assert(false && "Unknown buffer target");
    }

    static size_t getTextureIndex(GLenum target)
    {
        for (size_t i = 0; i < textureBindings.size(); ++i)
            if (textureBindings[i] == target)
                return i;
        assert(false && "Unknown texture target");
    }

    struct BoundTexture {
        GLenum target = 0;
        GLuint texture = 0;
    };

    State() = default;
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    GLuint currentVao_ = 0;
    GLuint currentShaderProgram_ = 0;
    std::array<GLuint, bufferBindings.size()> currentBuffers_;
    std::array<std::array<GLuint, textureBindings.size()>, maxTextureUnits> currentTextures_ {};
};
}
