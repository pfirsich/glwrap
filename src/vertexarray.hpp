#pragma once

#include "glad/glad.h"

#include "state.hpp"

namespace glw {
class VertexArray {
public:
    VertexArray()
    {
        glGenVertexArrays(1, &vao_);
    }

    ~VertexArray()
    {
        free();
    }

    VertexArray(const VertexArray& other) = delete;
    VertexArray& operator=(const VertexArray& other) = delete;

    VertexArray(VertexArray&& other)
        : vao_(other.vao_)
    {
        other.vao_ = 0;
    }

    VertexArray& operator=(VertexArray&& other)
    {
        free();
        vao_ = other.vao_;
        other.vao_ = 0;
        return *this;
    }

    void free()
    {
        if (vao_)
            glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }

    static void unbind()
    {
        State::instance().unbindVao();
    }

    void bind() const
    {
        State::instance().bindVao(vao_);
    }

private:
    GLuint vao_;
};
}
