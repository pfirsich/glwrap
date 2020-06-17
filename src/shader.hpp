#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "log.hpp"
#include "state.hpp"
#include "uniforminfo.hpp"

namespace glw {
struct ShaderResult {
    bool success;
    std::string log;

    ShaderResult(bool success, const std::string& log);
    explicit operator bool() const;
};

class Shader {
public:
    enum class Type : GLenum {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
    };

    // Consider saving Type, because according to the OpenGL wiki, you should
    // avoid attaching multiple shaders of the same type and I might want to check for that.
    Shader(Type type);

    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other);

    Shader& operator=(Shader&& other);

    void free();

    void setSource(const char* source, int length) const;

    void setSource(std::string_view source) const;

    // Different name, so I have to do less work to have the correct overload get chosen
    void setSources(size_t count, const char** sources, const int* lengths) const;

    template <typename Iterator>
    void setSources(Iterator begin, Iterator end) const
    {
        // We take a detour to a vector<string_view> so we can consume elmeents of type
        // const char*, string and string_view.
        // It's not cool, but it does what I want it to.
        // You can use the other functions, if you hate it.
        std::vector<std::string_view> sources(begin, end);
        std::vector<const char*> ptrs;
        std::vector<int> lengths;
        for (const auto& source : sources) {
            ptrs.push_back(source.data());
            lengths.push_back(source.size());
        }
        setSources(sources.size(), ptrs.data(), lengths.data());
    }

    template <typename Container>
    void setSources(const Container& sources) const
    {
        setSources(sources.cbegin(), sources.cend());
    }

    void setSources(std::initializer_list<std::string_view> sources) const;

    ShaderResult compile() const;

    GLuint getShaderObject() const;

private:
    GLuint shader_ = 0;
};

class ShaderProgram {
public:
    using UniformLocation = GLint;
    using AttributeLocation = GLint;
    static constexpr GLint invalidLocation = -1;

    ShaderProgram();

    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    ShaderProgram(ShaderProgram&& other);
    ShaderProgram& operator=(ShaderProgram&& other);

    void free();

    void attach(const Shader& shader);
    void detach(const Shader& shader);
    void detach();

    ShaderResult link();

    AttributeLocation getAttributeLocation(const std::string& name) const;
    UniformLocation getUniformLocation(const std::string& name) const;

    const std::unordered_map<std::string, UniformInfo>& getUniformInfo() const;

    void bind() const;
    static void unbind();

    template <typename... Args>
    void setUniform(const std::string& name, Args&&... args) const
    {
        const auto loc = getUniformLocation(name);
        if (loc != -1)
            setUniform(loc, std::forward<Args>(args)...);
    }

    void setUniform(UniformLocation loc, int value) const;
    void setUniform(UniformLocation loc, const int* vals, size_t count = 1);
    void setUniform(UniformLocation loc, float value) const;
    void setUniform(UniformLocation loc, const float* vals, size_t count = 1) const;
    void setUniform(UniformLocation loc, const glm::vec2& val) const;
    void setUniform(UniformLocation loc, const glm::vec2* vals, size_t count = 1) const;
    void setUniform(UniformLocation loc, const glm::vec3& val) const;
    void setUniform(UniformLocation loc, const glm::vec3* vals, size_t count = 1) const;
    void setUniform(UniformLocation loc, const glm::vec4& val) const;
    void setUniform(UniformLocation loc, const glm::vec4* vals, size_t count = 1) const;
    void setUniform(UniformLocation loc, const glm::mat2& val) const;
    void setUniform(UniformLocation loc, const glm::mat2* vals, size_t count = 1) const;
    void setUniform(UniformLocation loc, const glm::mat3& val) const;
    void setUniform(UniformLocation loc, const glm::mat3* vals, size_t count = 1) const;
    void setUniform(UniformLocation loc, const glm::mat4& val) const;
    void setUniform(UniformLocation loc, const glm::mat4* vals, size_t count = 1) const;

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
    std::vector<GLuint> attachedShaders_;
    mutable std::unordered_map<std::string, UniformLocation> attribLocations_;
    mutable std::unordered_map<std::string, UniformLocation> uniformLocations_;
    std::unordered_map<std::string, UniformInfo> uniformInfo_;
};

}
