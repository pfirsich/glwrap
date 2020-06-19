#include "debug.hpp"

namespace glwx {
namespace debug {
    namespace detail {
        std::function<void(Source, Type, GLuint, Severity, std::string_view)> callback;

#if !_MSC_VER
#define __stdcall
#endif

        void __stdcall debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei length, const GLchar* message, const void* /*userParam*/)
        {
            callback(static_cast<Source>(source), static_cast<Type>(type), id,
                static_cast<Severity>(severity), std::string_view(message, length));
        }

        bool init()
        {
            if (GLAD_GL_KHR_debug) {
                glDebugMessageCallback(debugCallback, nullptr);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
                glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
                return true;
            }
            return false;
        }
    }

    Group::Group(Source source, GLuint id, std::string_view message)
    {
        assert(source == Source::Application || source == Source::ThirdParty);
        glPushDebugGroup(static_cast<GLenum>(source), id, message.size(), message.data());
    }

    Group::~Group()
    {
        glPopDebugGroup();
    }

    bool init()
    {
        detail::callback = [](glwx::debug::Source source, glwx::debug::Type type, GLuint id,
                               glwx::debug::Severity severity, std::string_view message) {
            LOG_DEBUG("OpenGL [{}] [{}] [{}] [{}]: {}", source, type, id, severity, message);
        };
        return detail::init();
    }

    void control(std::optional<Source> source, std::optional<Type> type,
        std::optional<Severity> severity, bool enabled)
    {
        const auto source_ = source ? static_cast<GLenum>(*source) : GL_DONT_CARE;
        const auto type_ = type ? static_cast<GLenum>(*type) : GL_DONT_CARE;
        const auto severity_ = severity ? static_cast<GLenum>(*severity) : GL_DONT_CARE;
        glDebugMessageControl(source_, type_, severity_, 0, nullptr, enabled);
    }
    void control(Source source, Type type, std::vector<GLuint> ids, bool enabled)
    {
        glDebugMessageControl(static_cast<GLenum>(source), static_cast<GLenum>(type), GL_DONT_CARE,
            ids.size(), ids.data(), enabled);
    }

    void insert(Source source, Type type, GLuint id, Severity severity, std::string_view message)
    {
        assert(source == Source::Application || source == Source::ThirdParty);
        glDebugMessageInsert(static_cast<GLenum>(source), static_cast<GLenum>(type), id,
            static_cast<GLenum>(severity), message.size(), message.data());
    }

    void setLabel(const glw::Buffer& buffer, std::string_view label)
    {
        glObjectLabel(GL_BUFFER, buffer.getBuffer(), label.size(), label.data());
    }

    void setLabel(const glw::Shader& shader, std::string_view label)
    {
        glObjectLabel(GL_SHADER, shader.getShader(), label.size(), label.data());
    }

    void setLabel(const glw::ShaderProgram& program, std::string_view label)
    {
        glObjectLabel(GL_PROGRAM, program.getProgram(), label.size(), label.data());
    }

    void setLabel(const glw::VertexArray& vertexArray, std::string_view label)
    {
        glObjectLabel(GL_VERTEX_ARRAY, vertexArray.getVertexArray(), label.size(), label.data());
    }

    void setLabel(const glw::Texture& texture, std::string_view label)
    {
        glObjectLabel(GL_TEXTURE, texture.getTexture(), label.size(), label.data());
    }

    void setLabel(const glw::Renderbuffer& renderbuffer, std::string_view label)
    {
        glObjectLabel(GL_RENDERBUFFER, renderbuffer.getRenderbuffer(), label.size(), label.data());
    }

    void setLabel(const glw::Framebuffer& framebuffer, std::string_view label)
    {
        glObjectLabel(GL_FRAMEBUFFER, framebuffer.getFramebuffer(), label.size(), label.data());
    }

    std::string_view toString(Source source)
    {
        switch (source) {
        case Source::Api:
            return "Api";
        case Source::WindowSystem:
            return "WindowSystem";
        case Source::ShaderCompiler:
            return "ShaderCompiler";
        case Source::ThirdParty:
            return "ThirdParty";
        case Source::Application:
            return "Application";
        case Source::Other:
            return "Other";
        }
    }

    std::string_view toString(Type type)
    {
        switch (type) {
        case Type::Error:
            return "Error";
        case Type::DeprecatedBehavior:
            return "DeprecatedBehavior";
        case Type::UndefinedBehavior:
            return "UndefinedBehavior";
        case Type::Portability:
            return "Portability";
        case Type::Performance:
            return "Performance";
        case Type::Marker:
            return "Marker";
        case Type::PushGroup:
            return "PushGroup";
        case Type::PopGroup:
            return "PopGroup";
        case Type::Other:
            return "Other";
        }
    }

    std::string_view toString(Severity severity)
    {
        switch (severity) {
        case Severity::High:
            return "High";
        case Severity::Medium:
            return "Medium";
        case Severity::Low:
            return "Low";
        case Severity::Notification:
            return "Notification";
        }
    }

    std::ostream& operator<<(std::ostream& os, Source source)
    {
        return os << toString(source);
    }

    std::ostream& operator<<(std::ostream& os, Type type)
    {
        return os << toString(type);
    }

    std::ostream& operator<<(std::ostream& os, Severity severity)
    {
        return os << toString(severity);
    }
}
}
