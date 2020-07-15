#pragma once

#include <filesystem>
#include <optional>

#include "../shader.hpp"

namespace glwx {
std::optional<glw::Shader> makeShader(glw::Shader::Type type, std::string_view source);
std::optional<glw::Shader> makeShader(glw::Shader::Type type, const std::filesystem::path& path);

std::optional<glw::ShaderProgram> makeShaderProgram(
    const glw::Shader& vert, const glw::Shader& frag);

template <typename V, typename F>
std::optional<glw::ShaderProgram> makeShaderProgram(const V& vertSource, const F& fragSource)
{
    const auto vert = makeShader(glw::Shader::Type::Vertex, vertSource);
    if (!vert) {
        LOG_ERROR("Could not compile vertex shader");
        return std::nullopt;
    }

    const auto frag = makeShader(glw::Shader::Type::Fragment, fragSource);
    if (!frag) {
        LOG_ERROR("Could not compile fragment shader");
        return std::nullopt;
    }

    return makeShaderProgram(*vert, *frag);
}

// I wanted to add an overload that takes a single const T&, which would use that T&
// for both shaders, but include VERTEX_SHADER and FRAGMENT_SHADER defines.
// Sadly those defines would have to be at the given as the first source string, which
// would result in the version directive potentially being after that define, which would
// yield an invalid shader.
}
