#pragma once

#include <cassert>

#include "glad/glad.h"

namespace glw {
class State {
public:
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

    static size_t getBufferIndex(GLenum target)
    {
        for (size_t i = 0; i < bufferBindings.size(); ++i)
            if (bufferBindings[i] == target)
                return i;
        assert(false && "Unknown buffer target");
    }

    State() = default;
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    GLuint currentVao_ = 0;
    GLuint currentShaderProgram_ = 0;
    std::array<GLuint, bufferBindings.size()> currentBuffers_;
};
}
