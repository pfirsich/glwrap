#include "shader.hpp"

#include "log.hpp"

namespace glw {

ShaderResult::ShaderResult(bool success, const std::string& log)
    : success(success)
    , log(log)
{
}

ShaderResult::operator bool() const
{
    return success;
}

Shader::Shader(Type type)
    : shader_(glCreateShader(static_cast<GLenum>(type)))
{
}

Shader::~Shader()
{
    free();
}

Shader::Shader(Shader&& other)
    : shader_(other.shader_)
{
    other.shader_ = 0;
}

Shader& Shader::operator=(Shader&& other)
{
    free();
    shader_ = other.shader_;
    other.shader_ = 0;
    return *this;
}

void Shader::free()
{
    glDeleteShader(shader_);
    shader_ = 0;
}

void Shader::setSource(const char* source, int length) const
{
    glShaderSource(shader_, 1, &source, &length);
}

void Shader::setSource(std::string_view source) const
{
    setSource(source.data(), source.size());
}

void Shader::setSources(size_t count, const char** sources, const int* lengths) const
{
    glShaderSource(shader_, count, sources, lengths);
}

void Shader::setSources(std::initializer_list<std::string_view> sources) const
{
    setSources(sources.begin(), sources.end());
}

ShaderResult Shader::compile() const
{
    glCompileShader(shader_);

    // get the log in any case
    GLint logLength = 0;
    glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &logLength);
    std::string log;
    if (logLength > 0) {
        log.resize(logLength);
        glGetShaderInfoLog(shader_, logLength, nullptr, log.data());
    }

    GLint compileStatus;
    glGetShaderiv(shader_, GL_COMPILE_STATUS, &compileStatus);
    return ShaderResult(compileStatus == GL_TRUE, log);
}

GLuint Shader::getShaderObject() const
{
    return shader_;
}

ShaderProgram::ShaderProgram()
    : program_(glCreateProgram())
{
}

ShaderProgram::~ShaderProgram()
{
    free();
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
    : program_(other.program_)
    , attachedShaders_(std::move(other.attachedShaders_))
    , attribLocations_(std::move(other.attribLocations_))
    , uniformLocations_(std::move(other.uniformLocations_))
    , uniformInfo_(std::move(other.uniformInfo_))
{
    other.program_ = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
{
    free();
    program_ = other.program_;
    other.program_ = 0;
    attachedShaders_ = std::move(other.attachedShaders_);
    attribLocations_ = std::move(other.attribLocations_);
    uniformLocations_ = std::move(other.uniformLocations_);
    uniformInfo_ = std::move(other.uniformInfo_);
    return *this;
}

void ShaderProgram::free()
{
    detach();

    if (program_)
        glDeleteProgram(program_);
}

void ShaderProgram::attach(const Shader& shader)
{
    glAttachShader(program_, shader.getShaderObject());
    attachedShaders_.push_back(shader.getShaderObject());
}

void ShaderProgram::detach(const Shader& shader)
{
    glDetachShader(program_, shader.getShaderObject());
    attachedShaders_.erase(
        std::remove(attachedShaders_.begin(), attachedShaders_.end(), shader.getShaderObject()),
        attachedShaders_.end());
}

void ShaderProgram::detach()
{
    for (const auto shader : attachedShaders_)
        glDetachShader(program_, shader);
    attachedShaders_.clear();
}

ShaderResult ShaderProgram::link()
{
    glLinkProgram(program_);

    GLint logLength = 0;
    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &logLength);
    std::string log;
    if (logLength > 0) {
        log.resize(logLength);
        glGetProgramInfoLog(program_, logLength, nullptr, log.data());
    }

    GLint linkStatus;
    glGetProgramiv(program_, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_TRUE)
        retrieveUniformInfo();
    return ShaderResult(linkStatus == GL_TRUE, log);
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

const std::unordered_map<std::string, UniformInfo>& ShaderProgram::getUniformInfo() const
{
    return uniformInfo_;
}

void ShaderProgram::bind() const
{
    glw::State::instance().bindShader(program_);
}

void ShaderProgram::unbind()
{
    glw::State::instance().unbindShader();
}

void ShaderProgram::setUniform(UniformLocation loc, int value) const
{
    bind();
    glUniform1i(loc, value);
}

void ShaderProgram::setUniform(UniformLocation loc, const int* vals, size_t count)
{
    bind();
    glUniform1iv(loc, count, vals);
}

void ShaderProgram::setUniform(UniformLocation loc, float value) const
{
    bind();
    glUniform1f(loc, value);
}

void ShaderProgram::setUniform(UniformLocation loc, const float* vals, size_t count) const
{
    bind();
    glUniform1fv(loc, count, vals);
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::vec2& val) const
{
    bind();
    glUniform2fv(loc, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::vec2* vals, size_t count) const
{
    bind();
    glUniform2fv(loc, count, glm::value_ptr(*vals));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::vec3& val) const
{
    bind();
    glUniform3fv(loc, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::vec3* vals, size_t count) const
{
    bind();
    glUniform3fv(loc, count, glm::value_ptr(*vals));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::vec4& val) const
{
    bind();
    glUniform4fv(loc, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::vec4* vals, size_t count) const
{
    bind();
    glUniform4fv(loc, count, glm::value_ptr(*vals));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::mat2& val) const
{
    bind();
    glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::mat2* vals, size_t count) const
{
    bind();
    glUniformMatrix2fv(loc, count, GL_FALSE, glm::value_ptr(*vals));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::mat3& val) const
{
    bind();
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::mat3* vals, size_t count) const
{
    bind();
    glUniformMatrix3fv(loc, count, GL_FALSE, glm::value_ptr(*vals));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::mat4& val) const
{
    bind();
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::setUniform(UniformLocation loc, const glm::mat4* vals, size_t count) const
{
    bind();
    glUniformMatrix4fv(loc, count, GL_FALSE, glm::value_ptr(*vals));
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
