#pragma once

#include <algorithm>
#include <cassert>

#include "glad/glad.h"

#include "imageformat.hpp"
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

    Texture(Target target);
    ~Texture();

    Texture(const Texture& other) = delete;
    Texture& operator=(Texture& other) = delete;

    Texture(Texture&& other);
    Texture& operator=(Texture&& other);

    void free() const;

    void bind(unsigned int unit) const;

    void bind(unsigned unit, Target target) const;

    void image(Target target, size_t level, ImageFormat imageFormat, size_t width, size_t height,
        DataFormat dataFormat, DataType dataType, const void* data);

    // common use-case variants
    void image(ImageFormat imageFormat, size_t width, size_t height, DataFormat dataFormat,
        DataType dataType, const void* data);
    void image(size_t level, ImageFormat imageFormat, size_t width, size_t height,
        DataFormat dataFormat, DataType dataType, const void* data);

    void subImage(Target target, size_t level, size_t x, size_t y, size_t width, size_t height,
        DataFormat dataFormat, DataType dataType, const void* data) const;
    // common use-case variants
    void subImage(DataFormat dataFormat, DataType dataType, const void* data) const;
    void subImage(size_t level, DataFormat dataFormat, DataType dataType, const void* data);

    void storage(
        Target target, size_t levels, ImageFormat imageFormat, size_t width, size_t height);

    void storage(ImageFormat imageFormat, size_t width, size_t height);

    void generateMipmaps() const;

    void setWrap(WrapMode wrapS, WrapMode wrapT);
    void setWrap(WrapMode wrap);
    std::pair<WrapMode, WrapMode> getWrap() const;

    void setMinFilter(MinFilter filter);
    MinFilter getMinFilter() const;
    void setMagFilter(MagFilter filter);
    MagFilter getMagFilter() const;
    void setFilter(MinFilter minFilter, MagFilter magFilter);

    Target getTarget() const;
    GLuint getTexture() const;
    size_t getWidth() const;
    size_t getHeight() const;
    ImageFormat getImageFormat() const;

private:
    static DataFormat getStorageFormat(ImageFormat format);

    void setParameter(GLenum param, GLint val);
    void reset();

    Target target_;
    GLuint texture_;
    size_t width_ = 0;
    size_t height_ = 0;
    ImageFormat imageFormat_ = ImageFormat::Invalid;
    WrapMode wrapS_ = WrapMode::Repeat;
    WrapMode wrapT_ = WrapMode::Repeat;
    MinFilter minFilter_ = MinFilter::NearestMipmapLinear;
    MagFilter magFilter_ = MagFilter::Linear;
};

}
