#include "glw/texture.hpp"

#include "glw/log.hpp"

namespace glw {
Texture::Texture(Target target)
    : target_(target)
{
    glGenTextures(1, &texture_);
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindTexture.xhtml
    // "When a texture is first bound, it assumes the specified target"...
    bind(0);
}

Texture::~Texture()
{
    free();
}

Texture::Texture(Texture&& other)
    : target_(other.target_)
    , texture_(other.texture_)
    , width_(other.width_)
    , height_(other.height_)
    , imageFormat_(other.imageFormat_)
{
    other.reset();
}

Texture& Texture::operator=(Texture&& other)
{
    free();
    target_ = other.target_;
    texture_ = other.texture_;
    width_ = other.width_;
    height_ = other.height_;
    imageFormat_ = other.imageFormat_;
    other.reset();
    return *this;
}

void Texture::free() const
{
    // silently ignores 0s
    glDeleteTextures(1, &texture_);
}

void Texture::bind(unsigned int unit) const
{
    State::instance().bindTexture(unit, static_cast<GLenum>(target_), texture_);
}

void Texture::bind(unsigned int unit, Target target) const
{
    State::instance().bindTexture(unit, static_cast<GLenum>(target), texture_);
}

void Texture::image(Target target, size_t level, ImageFormat imageFormat, size_t width,
    size_t height, DataFormat dataFormat, DataType dataType, const void* data)
{
    imageFormat_ = imageFormat;
    width_ = width;
    height_ = height;
    bind(0);
    glTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level),
        static_cast<GLenum>(imageFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height),
        0, static_cast<GLenum>(dataFormat), static_cast<GLenum>(dataType), data);
}

void Texture::image(ImageFormat imageFormat, size_t width, size_t height, DataFormat dataFormat,
    DataType dataType, const void* data)
{
    image(target_, 0, imageFormat, width, height, dataFormat, dataType, data);
}

void Texture::image(size_t level, ImageFormat imageFormat, size_t width, size_t height,
    DataFormat dataFormat, DataType dataType, const void* data)
{
    image(target_, level, imageFormat, width, height, dataFormat, dataType, data);
}

void Texture::subImage(Target target, size_t level, size_t x, size_t y, size_t width, size_t height,
    DataFormat dataFormat, DataType dataType, const void* data) const
{
    bind(0);
    glTexSubImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(x),
        static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height),
        static_cast<GLenum>(dataFormat), static_cast<GLenum>(dataType), data);
}

void Texture::subImage(DataFormat dataFormat, DataType dataType, const void* data) const
{
    subImage(target_, 0, 0, 0, width_, height_, dataFormat, dataType, data);
}

void Texture::subImage(size_t level, DataFormat dataFormat, DataType dataType, const void* data)
{
    subImage(target_, level, 0, 0, width_, height_, dataFormat, dataType, data);
}

size_t Texture::getMaxNumMipLevels() const
{
    return 1 + static_cast<size_t>(std::floor(std::log2(std::max(width_, height_))));
}

void Texture::storage(
    Target target, size_t levels, ImageFormat imageFormat, size_t width, size_t height)
{
    assert(width > 0 && height > 0);
    imageFormat_ = imageFormat;
    width_ = width;
    height_ = height;
    if (levels == 0)
        levels = getMaxNumMipLevels();

    bind(0);
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
    assert(target == Target::Texture2D || target == Target::TextureRectangle);
    const auto format = static_cast<GLenum>(imageFormat);
    const auto dataFormat = static_cast<GLenum>(getStorageFormat(imageFormat));
    for (size_t level = 0; level < levels; ++level) {
        glTexImage2D(static_cast<GLenum>(target), static_cast<GLint>(level), format,
            static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, dataFormat, GL_FLOAT,
            nullptr);
        width = std::max(static_cast<size_t>(1), width / 2);
        height = std::max(static_cast<size_t>(1), height / 2);
    }
    // glTexStorage would do this too
    glTexParameteri(
        static_cast<GLenum>(target), GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(levels - 1));
}

void Texture::storage(size_t levels, ImageFormat imageFormat, size_t width, size_t height)
{
    storage(target_, levels, imageFormat, width, height);
}

void Texture::generateMipmaps() const
{
    bind(0);
    glTexParameteri(static_cast<GLenum>(target_), GL_TEXTURE_MAX_LEVEL,
        static_cast<GLint>(getMaxNumMipLevels() - 1));
    glGenerateMipmap(static_cast<GLenum>(target_));
}

void Texture::setWrapS(WrapMode wrap)
{
    setParameter(GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrap));
}

void Texture::setWrapT(WrapMode wrap)
{
    setParameter(GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrap));
}

void Texture::setWrapR(WrapMode wrap)
{
    setParameter(GL_TEXTURE_WRAP_R, static_cast<GLenum>(wrap));
}

void Texture::setWrap(WrapMode wrapS, WrapMode wrapT)
{
    setWrapS(wrapS);
    setWrapT(wrapT);
}

void Texture::setWrap(WrapMode wrap)
{
    setWrap(wrap, wrap);
    setWrapR(wrap);
}

void Texture::setMinFilter(MinFilter filter)
{
    setParameter(GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(filter));
}

void Texture::setMagFilter(MagFilter filter)
{
    setParameter(GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(filter));
}

void Texture::setFilter(MinFilter minFilter, MagFilter magFilter)
{
    setMinFilter(minFilter);
    setMagFilter(magFilter);
}

void Texture::setBaseLevel(size_t level)
{
    setParameter(GL_TEXTURE_BASE_LEVEL, static_cast<unsigned int>(level));
}

void Texture::setBorderColor(const glm::vec4& color)
{
    bind(0);
    glTexParameterfv(static_cast<GLenum>(target_), GL_TEXTURE_BORDER_COLOR, &color.x);
}

void Texture::setBorderColor(const glm::ivec4& color)
{
    bind(0);
    glTexParameteriv(static_cast<GLenum>(target_), GL_TEXTURE_BORDER_COLOR, &color.x);
}

void Texture::setCompareFunc(DepthFunc func)
{
    setParameter(GL_TEXTURE_COMPARE_FUNC, static_cast<GLenum>(func));
}

void Texture::setCompareMode(CompareMode mode)
{
    setParameter(GL_TEXTURE_COMPARE_MODE, static_cast<GLenum>(mode));
}

void Texture::setLodBias(float bias)
{
    setParameter(GL_TEXTURE_LOD_BIAS, bias);
}

void Texture::setMinLod(int min)
{
    setParameter(GL_TEXTURE_MIN_LOD, min);
}

void Texture::setMaxLod(int max)
{
    setParameter(GL_TEXTURE_MAX_LOD, max);
}

void Texture::setMaxLevel(size_t level)
{
    setParameter(GL_TEXTURE_MAX_LEVEL, static_cast<unsigned int>(level));
}

void Texture::setSwizzleR(Swizzle swizzle)
{
    setParameter(GL_TEXTURE_SWIZZLE_R, static_cast<GLenum>(swizzle));
}

void Texture::setSwizzleG(Swizzle swizzle)
{
    setParameter(GL_TEXTURE_SWIZZLE_G, static_cast<GLenum>(swizzle));
}

void Texture::setSwizzleB(Swizzle swizzle)
{
    setParameter(GL_TEXTURE_SWIZZLE_B, static_cast<GLenum>(swizzle));
}

void Texture::setSwizzleA(Swizzle swizzle)
{
    setParameter(GL_TEXTURE_SWIZZLE_A, static_cast<GLenum>(swizzle));
}

void Texture::setSwizzle(Swizzle r, Swizzle g, Swizzle b, Swizzle a)
{
    bind(0);
    const std::array<GLint, 4> swizzle = { static_cast<GLint>(r), static_cast<GLint>(g),
        static_cast<GLint>(b), static_cast<GLint>(a) };
    glTexParameteriv(static_cast<GLenum>(target_), GL_TEXTURE_SWIZZLE_RGBA, swizzle.data());
}

Texture::Target Texture::getTarget() const
{
    return target_;
}

GLuint Texture::getTexture() const
{
    return texture_;
}

size_t Texture::getWidth() const
{
    return width_;
}

size_t Texture::getHeight() const
{
    return height_;
}

ImageFormat Texture::getImageFormat() const
{
    return imageFormat_;
}

Texture::DataFormat Texture::getStorageFormat(ImageFormat format)
{
    if (hasDepth(format))
        return DataFormat::Depth;
    return DataFormat::Rgba;
}

void Texture::setParameter(GLenum param, GLint val)
{
    bind(0);
    glTexParameteri(static_cast<GLenum>(target_), param, val);
}

void Texture::setParameter(GLenum param, GLenum val)
{
    bind(0);
    glTexParameteri(static_cast<GLenum>(target_), param, val);
}

void Texture::setParameter(GLenum param, float val)
{
    bind(0);
    glTexParameterf(static_cast<GLenum>(target_), param, val);
}

void Texture::reset()
{
    target_ = Target::Invalid;
    texture_ = 0;
    width_ = 0;
    height_ = 0;
    imageFormat_ = ImageFormat::Invalid;
}
}
