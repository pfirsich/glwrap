#include "texture.hpp"

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
    , wrapS_(other.wrapS_)
    , wrapT_(other.wrapT_)
    , minFilter_(other.minFilter_)
    , magFilter_(other.magFilter_)
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
    wrapS_ = other.wrapS_;
    wrapT_ = other.wrapT_;
    minFilter_ = other.minFilter_;
    magFilter_ = other.magFilter_;
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

void Texture::bind(unsigned unit, Target target) const
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
    glTexImage2D(static_cast<GLenum>(target), level, static_cast<GLenum>(imageFormat), width,
        height, 0, static_cast<GLenum>(dataFormat), static_cast<GLenum>(dataType), data);
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
    glTexSubImage2D(static_cast<GLenum>(target), level, x, y, width, height,
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

void Texture::storage(
    Target target, size_t levels, ImageFormat imageFormat, size_t width, size_t height)
{
    imageFormat_ = imageFormat;
    width_ = width;
    height_ = height;
    bind(0);
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
    assert(target == Target::Texture2D || target == Target::TextureRectangle);
    for (size_t level = 0; level < levels; ++level) {
        image(level, imageFormat, width, height, getStorageFormat(imageFormat), DataType::F32,
            nullptr);
        width = std::max(1ul, width / 2);
        height = std::max(1ul, height / 2);
    }
    // glTexStorage would do this too
    glTexParameteri(static_cast<GLenum>(target), GL_TEXTURE_MAX_LEVEL, levels - 1);
}

void Texture::storage(ImageFormat imageFormat, size_t width, size_t height)
{
    storage(target_, 1, imageFormat, width, height);
}

void Texture::generateMipmaps() const
{
    bind(0);
    glGenerateMipmap(static_cast<GLenum>(target_));
}

void Texture::setWrap(WrapMode wrapS, WrapMode wrapT)
{
    setParameter(GL_TEXTURE_WRAP_S, static_cast<GLenum>(wrapS_ = wrapS));
    setParameter(GL_TEXTURE_WRAP_T, static_cast<GLenum>(wrapT_ = wrapT));
}

void Texture::setWrap(WrapMode wrap)
{
    setWrap(wrap, wrap);
}

std::pair<Texture::WrapMode, Texture::WrapMode> Texture::getWrap() const
{
    return std::make_pair(wrapS_, wrapT_);
}

void Texture::setMinFilter(MinFilter filter)
{
    setParameter(GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(minFilter_ = filter));
}

Texture::MinFilter Texture::getMinFilter() const
{
    return minFilter_;
}

void Texture::setMagFilter(MagFilter filter)
{
    setParameter(GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(magFilter_ = filter));
}

Texture::MagFilter Texture::getMagFilter() const
{
    return magFilter_;
}

void Texture::setFilter(MinFilter minFilter, MagFilter magFilter)
{
    setMinFilter(minFilter);
    setMagFilter(magFilter);
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

void Texture::reset()
{
    target_ = Target::Invalid;
    texture_ = 0;
    width_ = 0;
    height_ = 0;
    imageFormat_ = ImageFormat::Invalid;
    wrapS_ = WrapMode::Invalid;
    wrapT_ = WrapMode::Invalid;
    minFilter_ = MinFilter::Invalid;
    magFilter_ = MagFilter::Invalid;
}
}
