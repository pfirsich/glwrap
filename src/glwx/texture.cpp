#include "texture.hpp"

#include "utility.hpp"

using namespace glw;

namespace glwx {
glw::ImageFormat getFormat(size_t channels)
{
    assert(channels >= 1 && channels <= 4);
    switch (channels) {
    case 1:
        return glw::ImageFormat::Red;
    case 2:
        return glw::ImageFormat::Rg;
    case 3:
        return glw::ImageFormat::Rgb;
    case 4:
        return glw::ImageFormat::Rgba;
    default:
        assert(false && "Invalid number of channels");
    }
}

Texture makeTexture2D(const uint8_t* buffer, size_t width, size_t height, size_t channels)
{
    const auto format = getFormat(channels);
    // This works because the underlying values are the same
    const auto dataFormat = static_cast<Texture::DataFormat>(format);
    Texture texture(Texture::Target::Texture2D);
    texture.storage(format, width, height);
    texture.setFilter(Texture::MinFilter::Linear, Texture::MagFilter::Linear);
    texture.subImage(dataFormat, Texture::DataType::U8, buffer);
    return texture;
}

Texture makeTexture2D(const glm::vec4& color, size_t width, size_t height)
{
    const auto c = colorToInt(color);
    std::vector<uint32_t> buffer(width * height, c);
    return makeTexture2D(reinterpret_cast<const uint8_t*>(buffer.data()), width, height, 4);
}

glw::Texture makeTexture2D(
    size_t width, size_t height, size_t checkerSize, const glm::vec4& col1, const glm::vec4& col2)
{
    const auto c = colorToInt(col2);
    std::vector<uint32_t> buffer(width * height, colorToInt(col1));
    for (size_t i = 0; i < width * height; ++i) {
        if ((i / checkerSize + i / width / checkerSize) % 2 == 0)
            buffer[i] = c;
    }
    return makeTexture2D(reinterpret_cast<const uint8_t*>(buffer.data()), width, height, 4);
}
}
