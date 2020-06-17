#pragma once

#include <array>
#include <cassert>
#include <optional>

#include "glad/glad.h"

namespace glw {
class State {
public:
    // GL_MAX_TEXTURE_IMAGE_UNITS is 16 in GL 3.3
    static constexpr size_t maxTextureUnits = 16;

    static State& instance();

    ~State() = default;

    GLuint getCurrentVao() const;
    void bindVao(GLuint vao);
    void unbindVao();

    GLuint getCurrentShader() const;
    void bindShader(GLuint prog);
    void unbindShader();

    GLuint getCurrentBuffer(GLenum target) const;
    void bindBuffer(GLenum target, GLuint buffer);
    void unbindBuffer(GLenum target);

    GLuint getCurrentTexture(unsigned int unit, GLenum target) const;
    std::optional<unsigned int> getTextureUnit(GLenum target, GLuint texture) const;
    void bindTexture(unsigned int unit, GLenum target, GLuint texture);
    void unbindTexture(unsigned int unit, GLenum target);

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

    template <typename Container>
    static size_t getTargetIndex(const Container& container, GLenum value)
    {
        for (size_t i = 0; i < container.size(); ++i)
            if (container[i] == value)
                return i;
        assert(false && "Unknown target");
    }

    static size_t getBufferIndex(GLenum target);
    static size_t getTextureIndex(GLenum target);

    State() = default;
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    GLuint currentVao_ = 0;
    GLuint currentShaderProgram_ = 0;
    std::array<GLuint, bufferBindings.size()> currentBuffers_;
    std::array<std::array<GLuint, textureBindings.size()>, maxTextureUnits> currentTextures_ {};
};
}
