#include "shader.hpp"

#include <fstream>
#include <memory>
#include <sstream>
#include <vector>

#include "log.hpp"
#include "utility.hpp"

namespace glw {
bool ShaderProgram::compile(ShaderProgram::Type type, const std::string& source)
{
    if (state_ != State::Empty && state_ != State::Unlinked) {
        LOG_ERROR("To compile a shader, the ShaderProgram has to be in state Empty or Unlinked");
        return false;
    }

    const GLuint shader = glCreateShader(static_cast<GLenum>(type));
    const char* cStr = source.c_str();
    glShaderSource(shader, 1, &cStr, nullptr);
    glCompileShader(shader);

    // get the log in any case
    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        std::string shaderLog(logLength, '\0');
        glGetShaderInfoLog(shader, logLength, nullptr, shaderLog.data());
        LOG_WARNING("Shader compile log: {}", shaderLog);
    }

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        state_ = State::CompileFailed;
        LOG_ERROR("Shader compile failed");
        return false;
    }

    state_ = State::Unlinked;
    shaders_.push_back(shader);
    return true;
}

bool ShaderProgram::compile(ShaderProgram::Type type, const std::filesystem::path& filename)
{
    const auto fileData = readFile(filename);
    if (!fileData) {
        LOG_ERROR("Could not read file '{}'", filename);
    }
    return compile(type, *fileData);
}

bool ShaderProgram::link()
{
    if (state_ != State::Unlinked)
        return false;

    program_ = glCreateProgram();
    for (const auto shader : shaders_) {
        glAttachShader(program_, shader);
    }
    glLinkProgram(program_);

    for (const auto shader : shaders_) {
        glDetachShader(program_, shader);
        glDeleteShader(shader);
    }
    shaders_.clear();

    GLint logLength = 0;
    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        std::string programLog(logLength, '\0');
        glGetProgramInfoLog(program_, logLength, nullptr, programLog.data());
        LOG_WARNING("Shader link log: {}", programLog);
    }

    GLint linkStatus;
    glGetProgramiv(program_, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        state_ = State::LinkFailed;
        LOG_ERROR("Program link failed");
        return false;
    }

    state_ = State::Linked;
    retrieveUniformInfo();
    return true;
}

ShaderProgram::AttributeLocation ShaderProgram::getAttributeLocation(const std::string& name) const
{
    const auto it = attribLocations_.find(name);
    if (it == attribLocations_.end()) {
        const auto loc = glGetAttribLocation(program_, name.c_str());
        attribLocations_.emplace(name, loc);
        return loc;
    } else {
        return it->second;
    }
}

ShaderProgram::UniformLocation ShaderProgram::getUniformLocation(const std::string& name) const
{
    const auto it = uniformLocations_.find(name);
    if (it == uniformLocations_.end()) {
        const auto loc = glGetUniformLocation(program_, name.c_str());
        uniformLocations_.emplace(name, loc);
        return loc;
    } else {
        return it->second;
    }
}

void ShaderProgram::retrieveUniformInfo()
{
    GLint maxUniformNameLength = 0;
    glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
    std::string name(maxUniformNameLength, '\0');
    GLint activeUniformCount = 0;
    glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &activeUniformCount);
    for (int i = 0; i < activeUniformCount; ++i) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveUniform(program_, i, maxUniformNameLength, &length, &size, &type, name.data());
        if (length > 0) {
            uniformInfo_.emplace(
                name, UniformInfo { name, i, size, static_cast<UniformInfo::Type>(type) });
        }
    }
}
}
