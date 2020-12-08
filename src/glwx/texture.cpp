#include "texture.hpp"

#define STBI_WINDOWS_UTF8 // Windows, I hate you
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../log.hpp"
#include "utility.hpp"

using namespace glw;

namespace glwx {
constexpr std::array<glw::ImageFormat, 4> channelsToFormat {
    glw::ImageFormat::Red,
    glw::ImageFormat::Rg,
    glw::ImageFormat::Rgb,
    glw::ImageFormat::Rgba,
};

Texture makeTexture2D(
    const uint8_t* buffer, size_t width, size_t height, size_t channels, bool mipmaps)
{
    assert(channels >= 1 && channels <= 4);
    const auto format = channelsToFormat[channels - 1];
    // This works because the underlying values are the same
    const auto dataFormat = static_cast<Texture::DataFormat>(format);
    Texture texture(Texture::Target::Texture2D);
    texture.storage(mipmaps ? 0 : 1, format, width, height);
    texture.subImage(dataFormat, Texture::DataType::U8, buffer);
    if (mipmaps) {
        texture.generateMipmaps();
        texture.setFilter(Texture::MinFilter::LinearMipmapNearest, Texture::MagFilter::Linear);
    } else {
        texture.setFilter(Texture::MinFilter::Linear, Texture::MagFilter::Linear);
    }
    return texture;
}

Texture makeTexture2D(const glm::vec4& color, size_t width, size_t height, bool mipmaps)
{
    const auto c = colorToInt(color);
    std::vector<uint32_t> buffer(width * height, c);
    return makeTexture2D(
        reinterpret_cast<const uint8_t*>(buffer.data()), width, height, 4, mipmaps);
}

glw::Texture makeTexture2D(size_t width, size_t height, size_t checkerSize, const glm::vec4& col1,
    const glm::vec4& col2, bool mipmaps)
{
    const auto c = colorToInt(col2);
    std::vector<uint32_t> buffer(width * height, colorToInt(col1));
    for (size_t i = 0; i < width * height; ++i) {
        if ((i / checkerSize + i / width / checkerSize) % 2 == 0)
            buffer[i] = c;
    }
    return makeTexture2D(
        reinterpret_cast<const uint8_t*>(buffer.data()), width, height, 4, mipmaps);
}

auto stbiImagePtr(uint8_t* buffer)
{
    static auto deleter = [](uint8_t* buffer) { stbi_image_free(buffer); };
    return std::unique_ptr<uint8_t, decltype(deleter)>(buffer, deleter);
}

std::optional<glw::Texture> makeTexture2D(const uint8_t* encodedBuffer, size_t size, bool mipmaps)
{
    int width = 0, height = 0, channels = 0;
    const auto image
        = stbiImagePtr(stbi_load_from_memory(encodedBuffer, size, &width, &height, &channels, 0));
    if (!image) {
        LOG_ERROR("Could not load encoded image from memory: {}", stbi_failure_reason());
        return std::nullopt;
    }
    return makeTexture2D(image.get(), width, height, channels, mipmaps);
}

std::optional<glw::Texture> makeTexture2D(const std::filesystem::path& path, bool mipmaps)
{
    int width = 0, height = 0, channels = 0;
    const auto image
        = stbiImagePtr(stbi_load(path.u8string().c_str(), &width, &height, &channels, 0));
    if (!image) {
        LOG_ERROR("Could not load image from file: {}", stbi_failure_reason());
        return std::nullopt;
    }
    return makeTexture2D(image.get(), width, height, channels, mipmaps);
}

std::optional<glw::Texture> makeCubeTexture(const std::filesystem::path& posX,
    const std::filesystem::path& negX, const std::filesystem::path& posY,
    const std::filesystem::path& negY, const std::filesystem::path& posZ,
    const std::filesystem::path& negZ)
{
    Texture texture(Texture::Target::TextureCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.getTexture());
    std::array<std::filesystem::path, 6> files { posX, negX, posY, negY, posZ, negZ };
    for (size_t i = 0; i < 6; ++i) {
        int w = 0, h = 0, c = 0;
        const auto image = stbiImagePtr(stbi_load(files[i].u8string().c_str(), &w, &h, &c, 0));
        if (!image) {
            LOG_ERROR("Could not load image from file: {}", stbi_failure_reason());
            return std::nullopt;
        }
        const auto target = static_cast<Texture::Target>(
            static_cast<GLenum>(Texture::Target::TextureCubeMapPosX) + i);
        const auto format = channelsToFormat[c - 1];
        const auto dataFormat = static_cast<Texture::DataFormat>(format);
        assert(c >= 1 && c <= 4);
        texture.image(target, 0, format, w, h, dataFormat, Texture::DataType::U8, image.get());
    }
    texture.setFilter(glw::Texture::MinFilter::Linear, glw::Texture::MagFilter::Linear);
    texture.setWrap(glw::Texture::WrapMode::ClampToEdge);
    return texture;
}
}
