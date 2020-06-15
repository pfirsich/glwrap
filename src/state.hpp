#pragma once

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
        glUseProgram(prog);
        currentShaderProgram_ = prog;
    }

    void unbindShader()
    {
        bindShader(0);
    }

private:
    State() = default;
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    GLuint currentVao_ = 0;
    GLuint currentShaderProgram_ = 0;
};
}
