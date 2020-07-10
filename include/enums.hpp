#pragma once

#include <glad/glad.h>

namespace glw {
enum class DrawMode : GLenum {
    Points = GL_POINTS,
    Lines = GL_LINES,
    LineLoop = GL_LINE_LOOP,
    LineStrip = GL_LINE_STRIP,
    Triangles = GL_TRIANGLES,
    TriangleFan = GL_TRIANGLE_FAN,
    TriangleStrip = GL_TRIANGLE_STRIP,
};
}
