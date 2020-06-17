#include "vertexarray.hpp"

namespace glw {
VertexArray::VertexArray()
{
    glGenVertexArrays(1, &vao_);
}

VertexArray::~VertexArray()
{
    free();
}

VertexArray::VertexArray(VertexArray&& other)
    : vao_(other.vao_)
{
    other.vao_ = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other)
{
    free();
    vao_ = other.vao_;
    other.vao_ = 0;
    return *this;
}

void VertexArray::free()
{
    if (vao_)
        glDeleteVertexArrays(1, &vao_);
    vao_ = 0;
}

void VertexArray::unbind()
{
    State::instance().unbindVao();
}

void VertexArray::bind() const
{
    State::instance().bindVao(vao_);
}
}
