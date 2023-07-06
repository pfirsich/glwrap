#pragma once

#include <array>
#include <cassert>
#include <cstdlib>
#include <optional>
#include <tuple>

#include "glad/glad.h"

namespace glw {
enum class DepthFunc : GLenum {
    Never = GL_NEVER,
    Less = GL_LESS,
    Equal = GL_EQUAL,
    Lequal = GL_LEQUAL,
    Greater = GL_GREATER,
    Notequal = GL_NOTEQUAL,
    Gequal = GL_GEQUAL,
    Always = GL_ALWAYS,
};

enum FrontFaceMode : GLenum {
    Cw = GL_CW,
    Ccw = GL_CCW,
};

enum FaceCullMode : GLenum {
    Front = GL_FRONT,
    Back = GL_BACK,
    FrontAndBack = GL_FRONT_AND_BACK,
};

enum BlendEquation : GLenum {
    Add = GL_FUNC_ADD,
    Subtract = GL_FUNC_SUBTRACT,
    ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
    Min = GL_MIN,
    Max = GL_MAX,
};

enum BlendFunc : GLenum {
    Zero = GL_ZERO,
    One = GL_ONE,
    SrcColor = GL_SRC_COLOR,
    OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
    DstColor = GL_DST_COLOR,
    OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
    SrcAlpha = GL_SRC_ALPHA,
    OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
    DstAlpha = GL_DST_ALPHA,
    OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
    ConstantColor = GL_CONSTANT_COLOR,
    OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
    ConstantAlpha = GL_CONSTANT_ALPHA,
    OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
    SrcAlphaSaturate = GL_SRC_ALPHA_SATURATE,
    Src1Color = GL_SRC1_COLOR,
    OneMinusSrc1Color = GL_ONE_MINUS_SRC1_COLOR,
    Src1Alpha = GL_SRC1_ALPHA,
    OneMinusSrc1Alpha = GL_ONE_MINUS_SRC1_ALPHA,
};

struct BlendFuncSeparate {
    BlendFunc srcRgb;
    BlendFunc srcAlpha;
    BlendFunc dstRgb;
    BlendFunc dstAlpha;
};

struct BlendEquationRgba {
    BlendEquation rgb;
    BlendEquation a;
};

class State {
public:
    struct Statistics {
        size_t vertexArrayBinds = 0;
        size_t shaderBinds = 0;
        size_t textureBinds = 0;
        size_t frameBufferBinds = 0;
        size_t drawCalls = 0;
    };

    // GL_MAX_TEXTURE_IMAGE_UNITS is 16 in GL 3.3
    static constexpr size_t maxTextureUnits = 16;

    static State& instance();

    ~State() = default;

    void resetStatistics();
    Statistics& getStatistics();

    std::tuple<int, int, size_t, size_t> getViewport() const;
    void setViewport(int x, int y, size_t width, size_t height);
    void setViewport(const std::tuple<int, int, size_t, size_t>& viewport);
    void setViewport(size_t width, size_t height);

    DepthFunc getDepthFunc() const;
    void setDepthFunc(DepthFunc func);

    bool getDepthWrite() const;
    void setDepthWrite(bool write);

    bool getCullFaceEnabled() const;
    void setCullFaceEnabled(bool enabled);

    FrontFaceMode getFrontFaceMode() const;
    void setFrontFaceMode(FrontFaceMode mode);

    FaceCullMode getFaceCullMode() const;
    void setFaceCullMode(FaceCullMode mode);

    bool getBlendEnabled() const;
    void setBlendEnabled(bool enabled);

    std::tuple<float, float, float, float> getBlendColor() const;
    void setBlendColor(float r, float g, float b, float a);
    void setBlendColor(const std::tuple<float, float, float, float>& color);

    BlendFuncSeparate getBlendFunc() const;
    void setBlendFunc(BlendFunc srcRgb, BlendFunc srcAlpha, BlendFunc dstRgb, BlendFunc dstAlpha);
    void setBlendFunc(const BlendFuncSeparate& func);
    void setBlendFunc(BlendFunc src, BlendFunc dst);

    BlendEquationRgba getBlendEquation() const;
    void setBlendEquation(BlendEquation rgb, BlendEquation a);
    void setBlendEquation(const BlendEquationRgba& eq);
    void setBlendEquation(BlendEquation eq);

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

    GLuint getCurrentFramebuffer(GLenum target) const;
    void bindFramebuffer(GLenum target, GLuint fbo);
    void unbindFramebuffer(GLenum target);

    GLuint getCurrentRenderbuffer() const;
    void bindRenderbuffer(GLuint rbo);
    void unbindRenderbuffer();

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
        std::abort();
    }

    static size_t getBufferIndex(GLenum target);
    static size_t getTextureIndex(GLenum target);

    State() = default;
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    Statistics statistics_;
    std::tuple<int, int, size_t, size_t> viewport_ = { 0, 0, 0, 0 };
    DepthFunc depthFunc_ = DepthFunc::Less;
    bool depthWrite_ = true;
    bool cullFaceEnabled_ = false;
    FrontFaceMode frontFaceMode_ = FrontFaceMode::Ccw;
    FaceCullMode faceCullMode_ = FaceCullMode::Back;
    bool blendEnabled_ = false;
    std::tuple<float, float, float, float> blendColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
    BlendFuncSeparate blendFunc_ = {
        .srcRgb = BlendFunc::One,
        .srcAlpha = BlendFunc::One,
        .dstRgb = BlendFunc::Zero,
        .dstAlpha = BlendFunc::Zero,
    };
    BlendEquationRgba blendEquation_ = { .rgb = BlendEquation::Add, .a = BlendEquation::Add };
    GLuint vao_ = 0;
    GLuint shaderProgram_ = 0;
    std::array<GLuint, bufferBindings.size()> buffers_;
    std::array<std::array<GLuint, textureBindings.size()>, maxTextureUnits> textures_ {};
    GLuint readFramebuffer_ = 0;
    GLuint drawFramebuffer_ = 0;
    GLuint renderbuffer_ = 0;
};
}
