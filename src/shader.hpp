#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "state.hpp"
#include "uniforminfo.hpp"

namespace glw {
class ShaderProgram {
public:
    enum class Type : GLenum {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
    };

    enum class State {
        Empty,
        Unlinked,
        Linked,
        CompileFailed,
        LinkFailed,
    };

    using UniformLocation = GLint;
    using AttributeLocation = GLint;
    static constexpr GLint invalidLocation = -1;

    ShaderProgram() = default;

    ~ShaderProgram()
    {
        if (glIsProgram(program_))
            glDeleteProgram(program_);
    }

    bool compile(Type type, const std::string& source);
    bool compile(Type type, const std::filesystem::path& path);

    bool compile(const std::string& vert, const std::string& frag)
    {
        return compile(Type::Vertex, vert) && compile(Type::Fragment, frag);
    }

    bool compile(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath)
    {
        return compile(Type::Vertex, vertPath) && compile(Type::Fragment, fragPath);
    }

    bool compileAndLink(const std::string& vert, const std::string& frag)
    {
        return compile(Type::Vertex, vert) && compile(Type::Fragment, frag) && link();
    }

    bool compileAndLink(
        const std::filesystem::path& fragPath, const std::filesystem::path& vertPath)
    {
        return compile(Type::Vertex, vertPath) && compile(Type::Fragment, fragPath) && link();
    }

    bool link();

    AttributeLocation getAttributeLocation(const std::string& name) const;
    UniformLocation getUniformLocation(const std::string& name) const;

    const std::unordered_map<std::string, UniformInfo>& getUniformInfo()
    {
        return uniformInfo_;
    }

    void bind() const
    {
        glw::State::instance().bindShader(program_);
    }

    static void unbind()
    {
        glw::State::instance().unbindShader();
    }

    GLuint getProgramObject() const
    {
        return program_;
    }

    State getState() const
    {
        return state_;
    }

    template <typename... Args>
    void setUniform(const std::string& name, Args&&... args) const
    {
        const auto loc = getUniformLocation(name);
        if (loc != -1)
            setUniform(loc, std::forward<Args>(args)...);
    }

    void setUniform(UniformLocation loc, int value) const
    {
        bind();
        glUniform1i(loc, value);
    }

    void setUniform(UniformLocation loc, const int* vals, size_t count = 1)
    {
        bind();
        glUniform1iv(loc, count, vals);
    }

    void setUniform(UniformLocation loc, float value) const
    {
        bind();
        glUniform1f(loc, value);
    }

    void setUniform(UniformLocation loc, const float* vals, size_t count = 1) const
    {
        bind();
        glUniform1fv(loc, count, vals);
    }

    void setUniform(UniformLocation loc, const glm::vec2& val) const
    {
        bind();
        glUniform2fv(loc, 1, glm::value_ptr(val));
    }

    void setUniform(UniformLocation loc, const glm::vec2* vals, size_t count = 1) const
    {
        bind();
        glUniform2fv(loc, count, glm::value_ptr(*vals));
    }

    void setUniform(UniformLocation loc, const glm::vec3& val) const
    {
        bind();
        glUniform3fv(loc, 1, glm::value_ptr(val));
    }

    void setUniform(UniformLocation loc, const glm::vec3* vals, size_t count = 1) const
    {
        bind();
        glUniform3fv(loc, count, glm::value_ptr(*vals));
    }

    void setUniform(UniformLocation loc, const glm::vec4& val) const
    {
        bind();
        glUniform4fv(loc, 1, glm::value_ptr(val));
    }

    void setUniform(UniformLocation loc, const glm::vec4* vals, size_t count = 1) const
    {
        bind();
        glUniform4fv(loc, count, glm::value_ptr(*vals));
    }

    void setUniform(UniformLocation loc, const glm::mat2& val) const
    {
        bind();
        glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(val));
    }

    void setUniform(UniformLocation loc, const glm::mat2* vals, size_t count = 1) const
    {
        bind();
        glUniformMatrix2fv(loc, count, GL_FALSE, glm::value_ptr(*vals));
    }

    void setUniform(UniformLocation loc, const glm::mat3& val) const
    {
        bind();
        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
    }

    void setUniform(UniformLocation loc, const glm::mat3* vals, size_t count = 1) const
    {
        bind();
        glUniformMatrix3fv(loc, count, GL_FALSE, glm::value_ptr(*vals));
    }

    void setUniform(UniformLocation loc, const glm::mat4& val) const
    {
        bind();
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
    }

    void setUniform(UniformLocation loc, const glm::mat4* vals, size_t count = 1) const
    {
        bind();
        glUniformMatrix4fv(loc, count, GL_FALSE, glm::value_ptr(*vals));
    }

    /*void setUniform(UniformLocation loc, const Texture& tex) const
    {
        bind();
        int unit = tex.getUnit();
        assert(unit >= 0);
        glUniform1i(loc, unit);
    }

    void setUniform(UniformLocation loc, const Texture& tex, int unit) const
    {
        bind();
        tex.bind(unit);
        glUniform1i(loc, unit);
    }*/

private:
    void retrieveUniformInfo();

    GLuint program_ = 0;
    std::vector<GLuint> shaders_;
    State state_ = State::Empty;
    mutable std::unordered_map<std::string, UniformLocation> attribLocations_;
    mutable std::unordered_map<std::string, UniformLocation> uniformLocations_;
    std::unordered_map<std::string, UniformInfo> uniformInfo_;
};
}
