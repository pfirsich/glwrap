#pragma once

#include <filesystem>
#include <optional>

#include <glm/glm.hpp>

#include "../texture.hpp"

namespace glwx {
glw::Texture makeTexture2D(const uint8_t* buffer, size_t width, size_t height, size_t channels);
glw::Texture makeTexture2D(const glm::vec4& color, size_t width = 1, size_t height = 1);
glw::Texture makeTexture2D(size_t width, size_t height, size_t checkerSize,
    const glm::vec4& col1 = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
    const glm::vec4& col2 = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
std::optional<glw::Texture> makeTexture2D(const uint8_t* encodedBuffer, size_t size);
std::optional<glw::Texture> makeTexture2D(const std::filesystem::path& path);
}
