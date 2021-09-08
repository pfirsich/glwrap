#pragma once

#include <functional>
#include <iostream>
#include <string_view>

#include "glad/glad.h"
#include <fmt/format.h>

#include "glw/buffer.hpp"
#include "glw/framebuffer.hpp"
#include "glw/renderbuffer.hpp"
#include "glw/shader.hpp"
#include "glw/texture.hpp"
#include "glw/vertexarray.hpp"

namespace glwx {
namespace debug {
    enum class Source : GLenum {
        Api = GL_DEBUG_SOURCE_API,
        WindowSystem = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
        ShaderCompiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
        ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY,
        Application = GL_DEBUG_SOURCE_APPLICATION,
        Other = GL_DEBUG_SOURCE_OTHER,
    };

    enum class Type : GLenum {
        Error = GL_DEBUG_TYPE_ERROR,
        DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
        UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
        Portability = GL_DEBUG_TYPE_PORTABILITY,
        Performance = GL_DEBUG_TYPE_PERFORMANCE,
        Marker = GL_DEBUG_TYPE_MARKER,
        PushGroup = GL_DEBUG_TYPE_PUSH_GROUP,
        PopGroup = GL_DEBUG_TYPE_POP_GROUP,
        Other = GL_DEBUG_TYPE_OTHER,
    };

    enum class Severity : GLenum {
        High = GL_DEBUG_SEVERITY_HIGH,
        Medium = GL_DEBUG_SEVERITY_MEDIUM,
        Low = GL_DEBUG_SEVERITY_LOW,
        Notification = GL_DEBUG_SEVERITY_NOTIFICATION,
    };

    namespace detail {
        extern std::function<void(Source, Type, GLuint, Severity, std::string_view)> callback;
        bool init();
    }

    struct Group {
        Group(Source source, GLuint id, std::string_view message);
        ~Group();
    };

    // This will set a callback that calls a glw log macro, which provides a way to
    // specify custom handlers anyway.
    bool init();

    template <typename Func>
    bool init(Func&& func)
    {
        detail::callback = func;
        return detail::init();
    }

    void control(std::optional<Source> source, std::optional<Type> type,
        std::optional<Severity> severity, bool enabled);
    void control(Source source, Type type, std::vector<GLuint> ids, bool enabled);

    void insert(Source source, Type type, GLuint id, Severity severity, std::string_view message);

    void setLabel(const glw::Buffer& buffer, std::string_view label);
    void setLabel(const glw::Shader& shader, std::string_view label);
    void setLabel(const glw::ShaderProgram& program, std::string_view label);
    void setLabel(const glw::VertexArray& vertexArray, std::string_view label);
    void setLabel(const glw::Texture& texture, std::string_view label);
    void setLabel(const glw::Renderbuffer& renderbuffer, std::string_view label);
    void setLabel(const glw::Framebuffer& framebuffer, std::string_view label);

    std::string_view toString(Source source);
    std::string_view toString(Type type);
    std::string_view toString(Severity severity);

    std::ostream& operator<<(std::ostream& os, Source source);
    std::ostream& operator<<(std::ostream& os, Type type);
    std::ostream& operator<<(std::ostream& os, Severity severity);
}
}

template <>
struct fmt::formatter<glwx::debug::Source> {
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(glwx::debug::Source source, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}", glwx::debug::toString(source));
    }
};

template <>
struct fmt::formatter<glwx::debug::Type> {
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(glwx::debug::Type type, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}", glwx::debug::toString(type));
    }
};

template <>
struct fmt::formatter<glwx::debug::Severity> {
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(glwx::debug::Severity severity, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}", glwx::debug::toString(severity));
    }
};
