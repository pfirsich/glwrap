#pragma once

#include <algorithm>
#include <cassert>

#include "glad/glad.h"

#include "state.hpp"

namespace glw {

class Texture {
public:
    enum class WrapMode : GLenum {
        Invalid = 0,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        Repeat = GL_REPEAT,
    };

    // Excellent tutorial on texture filtering:
    // https://paroj.github.io/gltut/Texturing/Tutorial%2015.html
    enum class MinFilter : GLenum {
        Invalid = 0,
        // I added the common names for these techniques, though they are not very good names
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR, // "Bilinear"
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
        LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST, // "Bilinear"
        NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR, // "Trilinear", but not "Bilinear" - you
                                                        // probably don't want to use this
        LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR, // "Trilinear"
    };

    enum class MagFilter : GLenum {
        Invalid = 0,
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
    };

    enum class Target : GLenum {
        Invalid = 0,
        Texture1D = GL_TEXTURE_1D,
        Texture2D = GL_TEXTURE_2D,
        Texture3D = GL_TEXTURE_3D,
        Texture1DArray = GL_TEXTURE_1D_ARRAY,
        Texture2DArray = GL_TEXTURE_2D_ARRAY,
        TextureRectangle = GL_TEXTURE_RECTANGLE,
        TextureCupeMap = GL_TEXTURE_CUBE_MAP,
        TextureBuffer = GL_TEXTURE_BUFFER,
        Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
        Texture2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
        TextureCuteMapPosX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        TextureCuteMapNegX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        TextureCuteMapPosY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        TextureCuteMapNegY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        TextureCuteMapPosZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        TextureCuteMapNegZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };

    enum class InternalFormat : GLenum {
        Invalid = 0,

        DepthComponent = GL_DEPTH_COMPONENT,
        DepthComponent16 = GL_DEPTH_COMPONENT16,
        DepthComponent24 = GL_DEPTH_COMPONENT24,
        DepthComponent32 = GL_DEPTH_COMPONENT32,
        DepthComponent32F = GL_DEPTH_COMPONENT32F,
        DepthStencil = GL_DEPTH_STENCIL,
        Red = GL_RED,
        Rg = GL_RG,
        Rgb = GL_RGB,
        Rgba = GL_RGBA,

        R8 = GL_R8,
        R8Snorm = GL_R8_SNORM,
        R16 = GL_R16,
        R16Snorm = GL_R16_SNORM,
        Rg8 = GL_RG8,
        Rg8Snorm = GL_RG8_SNORM,
        Rg16 = GL_RG16,
        Rg16Snorm = GL_RG16_SNORM,
        R3G3B2 = GL_R3_G3_B2,
        Rgb4 = GL_RGB4,
        Rgb5 = GL_RGB5,
        Rgb8 = GL_RGB8,
        Rgb8Snorm = GL_RGB8_SNORM,
        Rgb10 = GL_RGB10,
        Rgb12 = GL_RGB12,
        Rgb16Snorm = GL_RGB16_SNORM,
        Rgba2 = GL_RGBA2,
        Rgba4 = GL_RGBA4,
        Rgb5_a1 = GL_RGB5_A1,
        Rgba8 = GL_RGBA8,
        Rgba8Snorm = GL_RGBA8_SNORM,
        Rgb10A2 = GL_RGB10_A2,
        Rgb10A2Ui = GL_RGB10_A2UI,
        Rgba12 = GL_RGBA12,
        Rgba16 = GL_RGBA16,
        Srgb8 = GL_SRGB8,
        Srgb8Alpha8 = GL_SRGB8_ALPHA8,
        R16f = GL_R16F,
        Rg16f = GL_RG16F,
        Rgb16f = GL_RGB16F,
        Rgba16f = GL_RGBA16F,
        R32f = GL_R32F,
        Rg32f = GL_RG32F,
        Rgb32f = GL_RGB32F,
        Rgba32f = GL_RGBA32F,
        R11fG11fB10f = GL_R11F_G11F_B10F,
        Rgb9E5 = GL_RGB9_E5,
        R8I = GL_R8I,
        R8Ui = GL_R8UI,
        R16I = GL_R16I,
        R16Ui = GL_R16UI,
        R32I = GL_R32I,
        R32Ui = GL_R32UI,
        Rg8I = GL_RG8I,
        Rg8Ui = GL_RG8UI,
        Rg16I = GL_RG16I,
        Rg16Ui = GL_RG16UI,
        Rg32I = GL_RG32I,
        Rg32Ui = GL_RG32UI,
        Rgb8I = GL_RGB8I,
        Rgb8Ui = GL_RGB8UI,
        Rgb16I = GL_RGB16I,
        Rgb16Ui = GL_RGB16UI,
        Rgb32I = GL_RGB32I,
        Rgb32Ui = GL_RGB32UI,
        Rgba8I = GL_RGBA8I,
        Rgba8Ui = GL_RGBA8UI,
        Rgba16I = GL_RGBA16I,
        Rgba16Ui = GL_RGBA16UI,
        Rgba32I = GL_RGBA32I,
        Rgba32Ui = GL_RGBA32UI,

        CompressedRed = GL_COMPRESSED_RED,
        CompressedRg = GL_COMPRESSED_RG,
        CompressedRgb = GL_COMPRESSED_RGB,
        CompressedRgba = GL_COMPRESSED_RGBA,
        CompressedSrgb = GL_COMPRESSED_SRGB,
        CompressedSrgbAlpha = GL_COMPRESSED_SRGB_ALPHA,
        CompressedRedRgtc1 = GL_COMPRESSED_RED_RGTC1,
        CompressedSignedRedRgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
        CompressedRgRgtc2 = GL_COMPRESSED_RG_RGTC2,
        CompressedSignedRgRgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
        // CompressedRgbaBptcUnorm = GL_COMPRESSED_RGBA_BPTC_UNORM, // 4.2
        // CompressedSrgbAlphaBptcUnorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, // 4.2
        // CompressedRgbBptcSignedFloat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, // 4.2
        // CompressedRgbBptcUnsignedFloat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, // 4.2
    };

    enum class DataFormat : GLenum {
        Red = GL_RED,
        Rg = GL_RG,
        Rgb = GL_RGB,
        Bgr = GL_BGR,
        Rgba = GL_RGBA,
        Bgra = GL_BGRA,
        RedInteger = GL_RED_INTEGER,
        RgInteger = GL_RG_INTEGER,
        RgbInteger = GL_RGB_INTEGER,
        BgrInteger = GL_BGR_INTEGER,
        RgbaInteger = GL_RGBA_INTEGER,
        BgraInteger = GL_BGRA_INTEGER,
        StencilIndex = GL_STENCIL_INDEX,
        DepthComponent = GL_DEPTH_COMPONENT,
        DepthStencil = GL_DEPTH_STENCIL,
    };

    enum class DataType : GLenum {
        U8 = GL_UNSIGNED_BYTE,
        S8 = GL_BYTE,
        U16 = GL_UNSIGNED_SHORT,
        S16 = GL_SHORT,
        U32 = GL_UNSIGNED_INT,
        I32 = GL_INT,
        F16 = GL_HALF_FLOAT,
        F32 = GL_FLOAT,
        R3G3B2 = GL_UNSIGNED_BYTE_3_3_2,
        B2G3R3 = GL_UNSIGNED_BYTE_2_3_3_REV,
        R5G6B5 = GL_UNSIGNED_SHORT_5_6_5,
        B5G6R5 = GL_UNSIGNED_SHORT_5_6_5_REV,
        R4G4B4A4 = GL_UNSIGNED_SHORT_4_4_4_4,
        A4B4G4R4 = GL_UNSIGNED_SHORT_4_4_4_4_REV,
        R5G5B5A1 = GL_UNSIGNED_SHORT_5_5_5_1,
        A1B5G5R5 = GL_UNSIGNED_SHORT_1_5_5_5_REV,
        R8G8B8A8 = GL_UNSIGNED_INT_8_8_8_8,
        A8B8G8R8 = GL_UNSIGNED_INT_8_8_8_8_REV,
        R10G10B10A2 = GL_UNSIGNED_INT_10_10_10_2,
        A2B10G10R10 = GL_UNSIGNED_INT_2_10_10_10_REV,
    };

    Texture(Target target)
        : target_(target)
    {
        glGenTextures(1, &texture_);
        // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindTexture.xhtml
        // "When a texture is first bound, it assumes the specified target"...
        bind(0);
    }

    ~Texture()
    {
        free();
    }

    Texture(const Texture& other) = delete;
    Texture& operator=(Texture& other) = delete;

    Texture(Texture&& other)
        : target_(other.target_)
        , texture_(other.texture_)
    {
        other.reset();
    }

    Texture& operator=(Texture&& other)
    {
        free();
        target_ = other.target_;
        texture_ = other.texture_;
        other.reset();
        return *this;
    }

    void free() const
    {
        // silently ignores 0s
        glDeleteTextures(1, &texture_);
    }

    void bind(unsigned int unit) const
    {
        State::instance().bindTexture(unit, static_cast<GLenum>(target_), texture_);
    }

    void bind(unsigned unit, Target target) const
    {
        State::instance().bindTexture(unit, static_cast<GLenum>(target), texture_);
    }

    void image(Target target, size_t level, InternalFormat internalFormat, size_t width,
        size_t height, DataFormat dataFormat, DataType dataType, const void* data)
    {
        internalFormat_ = internalFormat;
        width_ = width;
        height_ = height;
        bind(0);
        glTexImage2D(static_cast<GLenum>(target), level, static_cast<GLenum>(internalFormat), width,
            height, 0, static_cast<GLenum>(dataFormat), static_cast<GLenum>(dataType), data);
    }

    // common use-case variants
    void image(InternalFormat internalFormat, size_t width, size_t height, DataFormat dataFormat,
        DataType dataType, const void* data)
    {
        image(target_, 0, internalFormat, width, height, dataFormat, dataType, data);
    }

    void image(size_t level, InternalFormat internalFormat, size_t width, size_t height,
        DataFormat dataFormat, DataType dataType, const void* data)
    {
        image(target_, level, internalFormat, width, height, dataFormat, dataType, data);
    }

    void subImage(Target target, size_t level, size_t x, size_t y, size_t width, size_t height,
        DataFormat dataFormat, DataType dataType, const void* data) const
    {
        bind(0);
        glTexSubImage2D(static_cast<GLenum>(target), level, x, y, width, height,
            static_cast<GLenum>(dataFormat), static_cast<GLenum>(dataType), data);
    }

    // common use-case variants
    void subImage(DataFormat dataFormat, DataType dataType, const void* data) const
    {
        subImage(target_, 0, 0, 0, width_, height_, dataFormat, dataType, data);
    }

    void subImage(size_t level, DataFormat dataFormat, DataType dataType, const void* data)
    {
        subImage(target_, level, 0, 0, width_, height_, dataFormat, dataType, data);
    }

    void storage(
        Target target, size_t levels, InternalFormat internalFormat, size_t width, size_t height)
    {
        internalFormat_ = internalFormat;
        width_ = width;
        height_ = height;
        bind(0);
        // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
        assert(target == Target::Texture2D || target == Target::TextureRectangle);
        for (size_t level = 0; level < levels; ++level) {
            image(level, internalFormat, width, height, getStorageFormat(internalFormat),
                DataType::F32, nullptr);
            width = std::max(1ul, width / 2);
            height = std::max(1ul, height / 2);
        }
        // glTexStorage would do this too
        glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_MAX_LEVEL, levels - 1);
    }

    void storage(InternalFormat internalFormat, size_t width, size_t height)
    {
        storage(target_, 1, internalFormat, width, height);
    }

    void generateMipmaps() const
    {
        bind(0);
        glGenerateMipmap(static_cast<GLenum>(target_));
    }

    void setWrap(WrapMode wrapS, WrapMode wrapT)
    {
        setParameter(GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrapS_ = wrapS));
        setParameter(GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrapT_ = wrapT));
    }

    void setWrap(WrapMode wrap)
    {
        setWrap(wrap, wrap);
    }

    std::pair<WrapMode, WrapMode> getWrap() const
    {
        return std::make_pair(wrapS_, wrapT_);
    }

    void setMinFilter(MinFilter filter)
    {
        setParameter(GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter_ = filter));
    }

    MinFilter getMinFilter() const
    {
        return minFilter_;
    }

    void setMagFilter(MagFilter filter)
    {
        setParameter(GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter_ = filter));
    }

    MagFilter getMagFilter() const
    {
        return magFilter_;
    }

    void setFilter(MinFilter minFilter, MagFilter magFilter)
    {
        setMinFilter(minFilter);
        setMagFilter(magFilter);
    }

    Target getTarget() const
    {
        return target_;
    }

    GLuint getTexture() const
    {
        return texture_;
    }

    size_t getWidth() const
    {
        return width_;
    }

    size_t getHeight() const
    {
        return height_;
    }

    InternalFormat getInternalFormat() const
    {
        return internalFormat_;
    }

private:
    void setParameter(GLenum param, GLint val)
    {
        bind(0);
        glTexParameteri(static_cast<GLenum>(target_), param, val);
    }

    static DataFormat getStorageFormat(InternalFormat format)
    {
        switch (format) {
        case InternalFormat::DepthComponent:
        case InternalFormat::DepthComponent16:
        case InternalFormat::DepthComponent24:
        case InternalFormat::DepthComponent32:
        case InternalFormat::DepthComponent32F:
            return DataFormat::DepthComponent;
        default:
            return DataFormat::Rgba;
        }
    }

    void reset()
    {
        target_ = Target::Invalid;
        texture_ = 0;
        width_ = 0;
        height_ = 0;
        internalFormat_ = InternalFormat::Invalid;
        wrapS_ = WrapMode::Invalid;
        wrapT_ = WrapMode::Invalid;
        minFilter_ = MinFilter::Invalid;
        magFilter_ = MagFilter::Invalid;
    }

    Target target_;
    GLuint texture_;
    size_t width_ = 0;
    size_t height_ = 0;
    InternalFormat internalFormat_ = InternalFormat::Invalid;
    WrapMode wrapS_ = WrapMode::Repeat;
    WrapMode wrapT_ = WrapMode::Repeat;
    MinFilter minFilter_ = MinFilter::NearestMipmapLinear;
    MagFilter magFilter_ = MagFilter::Linear;
};

}
