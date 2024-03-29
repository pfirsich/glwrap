#pragma once

#include "glad/glad.h"

#include "glw/state.hpp"

namespace glw {
class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray& other) = delete;
    VertexArray& operator=(const VertexArray& other) = delete;

    VertexArray(VertexArray&& other);
    VertexArray& operator=(VertexArray&& other);

    void free();

    static void unbind();

    void bind() const;

    GLuint getVertexArray() const;

private:
    GLuint vao_;
};
}
