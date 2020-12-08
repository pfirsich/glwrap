#pragma once

#include <filesystem>
#include <optional>

#include <glm/glm.hpp>

#include "../texture.hpp"

namespace glwx {
glw::Texture makeTexture2D(
    const uint8_t* buffer, size_t width, size_t height, size_t channels, bool mipmaps = true);
glw::Texture makeTexture2D(
    const glm::vec4& color, size_t width = 1, size_t height = 1, bool mipmaps = false);
glw::Texture makeTexture2D(size_t width, size_t height, size_t checkerSize,
    const glm::vec4& col1 = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
    const glm::vec4& col2 = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), bool mipmaps = true);
std::optional<glw::Texture> makeTexture2D(
    const uint8_t* encodedBuffer, size_t size, bool mipmaps = true);
std::optional<glw::Texture> makeTexture2D(const std::filesystem::path& path, bool mipmaps = true);
std::optional<glw::Texture> makeCubeTexture(const std::filesystem::path& posX,
    const std::filesystem::path& negX, const std::filesystem::path& posY,
    const std::filesystem::path& negY, const std::filesystem::path& posZ,
    const std::filesystem::path& negZ);
}
