#include "glwx/shader.hpp"

#include <fmt/std.h>

#include "glw/fmt.hpp"
#include "glwx/utility.hpp"

using namespace glw;

namespace glwx {

std::optional<Shader> makeShader(Shader::Type type, std::string_view source)
{
    Shader shader(type);
    shader.setSource(source);
    const auto res = shader.compile();
    if (!res.log.empty())
        LOG_WARNING("Shader log: {}", res.log);
    if (!res)
        return std::nullopt;
    return shader;
}

std::optional<Shader> makeShader(Shader::Type type, const std::filesystem::path& path)
{
    const auto fileData = readFile(path);
    if (!fileData) {
        LOG_ERROR("Could not read file '{}'", path);
        return std::nullopt;
    }
    return makeShader(type, std::string_view(*fileData));
}

std::optional<ShaderProgram> makeShaderProgram(const Shader& vert, const Shader& frag)
{
    ShaderProgram prog;
    prog.attach(vert);
    prog.attach(frag);
    const auto linkRes = prog.link();
    if (!linkRes.log.empty())
        LOG_WARNING("Link log: {}", linkRes.log);
    if (!linkRes)
        return std::nullopt;
    prog.detach(); // Unsure if I want this, but I think I do, so the Shaders can be deleted
    return prog;
}

}
