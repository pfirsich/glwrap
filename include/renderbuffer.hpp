#pragma once

#include "glad/glad.h"

#include "imageformat.hpp"
#include "state.hpp"

namespace glw {
class Renderbuffer {
public:
    Renderbuffer();
    ~Renderbuffer();

    Renderbuffer(const Renderbuffer&) = delete;
    Renderbuffer& operator=(const Renderbuffer&) = delete;

    Renderbuffer(Renderbuffer&& other);
    Renderbuffer& operator=(Renderbuffer&& other);

    static void unbind();
    void bind() const;

    void storage(ImageFormat format, size_t width, size_t height);

    GLuint getRenderbuffer() const;

private:
    void reset();
    void free();

    GLuint rbo_ = 0;
};
}
