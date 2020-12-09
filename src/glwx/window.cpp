#include "window.hpp"

#include <cassert>

#include "glad/glad.h"

#include "../log.hpp"

namespace glwx {
Sdl& Sdl::instance()
{
    static Sdl sdl;
    return sdl;
}

Sdl::Sdl()
{
    result_ = SDL_Init(0);
}

Sdl::~Sdl()
{
    if (result_ == 0)
        SDL_Quit();
}

int Sdl::getResult() const
{
    return result_;
}

glw::LogLevel convertSdlLogPriority(SDL_LogPriority priority)
{
    switch (priority) {
    case SDL_LOG_PRIORITY_VERBOSE:
    case SDL_LOG_PRIORITY_DEBUG:
        return glw::LogLevel::Debug;
    case SDL_LOG_PRIORITY_INFO:
        return glw::LogLevel::Info;
    case SDL_LOG_PRIORITY_WARN:
        return glw::LogLevel::Warning;
    case SDL_LOG_PRIORITY_ERROR:
        return glw::LogLevel::Error;
    case SDL_LOG_PRIORITY_CRITICAL:
        return glw::LogLevel::Critical;
    default:
        return glw::LogLevel::Warning;
    }
}

std::string_view sdlLogCategoryToString(int category)
{
    switch (category) {
    case SDL_LOG_CATEGORY_APPLICATION:
        return "Application";
    case SDL_LOG_CATEGORY_ERROR:
        return "Error";
    case SDL_LOG_CATEGORY_ASSERT:
        return "Assert";
    case SDL_LOG_CATEGORY_SYSTEM:
        return "System";
    case SDL_LOG_CATEGORY_AUDIO:
        return "Audio";
    case SDL_LOG_CATEGORY_VIDEO:
        return "Video";
    case SDL_LOG_CATEGORY_RENDER:
        return "Render";
    case SDL_LOG_CATEGORY_INPUT:
        return "Input";
    case SDL_LOG_CATEGORY_TEST:
        return "Test";
    case SDL_LOG_CATEGORY_CUSTOM:
        return "Custom";
    default:
        return "Unknown";
    }
}

void sdlLogFunction(void* /*userdata*/, int category, SDL_LogPriority priority, const char* message)
{
    LOG_LEVEL(convertSdlLogPriority(priority), "SDL Log [{}] {}", sdlLogCategoryToString(category),
        message);
}

bool initGladOnce()
{
    static bool initialized = false;
    static bool result = false;
    if (!initialized) {
        result = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    }
    return result;
}

bool initSubSystem(uint32_t flags)
{
    if (SDL_WasInit(flags) == 0) {
        if (SDL_InitSubSystem(flags) < 0) {
            LOG_CRITICAL("Could not initialize SDL Subsystem {}: {}", flags, SDL_GetError());
            return false;
        }
    }
    return true;
}

Window::~Window()
{
    if (glContext_)
        SDL_GL_DeleteContext(glContext_);
    if (window_)
        SDL_DestroyWindow(window_);
}

Window::Window(Window&& other)
    : window_(other.window_)
    , glContext_(other.glContext_)
{
    other.window_ = nullptr;
    other.glContext_ = nullptr;
}

Window& Window::operator=(Window&& other)
{
    window_ = other.window_;
    glContext_ = other.glContext_;
    other.window_ = nullptr;
    other.glContext_ = nullptr;
    return *this;
}

bool Window::init(
    const std::string& title, size_t width, size_t height, const Properties& properties)
{
    SDL_LogSetOutputFunction(sdlLogFunction, nullptr);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    if (Sdl::instance().getResult() < 0) {
        LOG_CRITICAL("Could not initialize SDL: {}", SDL_GetError());
        return false;
    }

    if (!initSubSystem(SDL_INIT_VIDEO))
        return false;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, properties.contextMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, properties.contextMinor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, properties.redSize);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, properties.greenSize);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, properties.blueSize);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, properties.alphaSize);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, properties.stencil ? 8 : 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, properties.depth);

    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, properties.srgb ? 1 : 0);

#ifndef NDEBUG
    int contextFlags = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
    contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
#endif

    if (properties.msaaSamples > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, properties.msaaSamples);
    }

    Uint32 flags = SDL_WINDOW_OPENGL;
    flags |= (properties.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    flags |= (properties.fullscreen && properties.fullscreenDesktop ? SDL_WINDOW_FULLSCREEN_DESKTOP
                                                                    : 0);
    flags |= (properties.hidden ? SDL_WINDOW_HIDDEN : SDL_WINDOW_SHOWN);
    flags |= (properties.borderless ? SDL_WINDOW_BORDERLESS : 0);
    flags |= (properties.resizable ? SDL_WINDOW_RESIZABLE : 0);
    flags |= (properties.maximized ? SDL_WINDOW_MAXIMIZED : 0);
    flags |= (properties.minimized ? SDL_WINDOW_MINIMIZED : 0);
    window_ = SDL_CreateWindow(
        title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    if (!window_) {
        LOG_CRITICAL("SDL_CreateWindow failed! - '%s'\n", SDL_GetError());
        return false;
    }

    glContext_ = SDL_GL_CreateContext(window_);
    if (!glContext_) {
        LOG_CRITICAL("SDL_GL_CreateContext failed! - '%s'\n", SDL_GetError());
        return false;
    }

    if (!initGladOnce()) {
        LOG_CRITICAL("Could not initialize GLAD");
        return false;
    }

    return true;
}

void Window::swap() const
{
    SDL_GL_SwapWindow(window_);
}

glm::ivec2 Window::getSize() const
{
    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    return glm::ivec2(w, h);
}

bool Window::setSwapInterval(int interval) const
{
    if (SDL_GL_SetSwapInterval(interval) < 0) {
        LOG_ERROR("SDL_GL_SetSwapInterval failed: {}", SDL_GetError());
        return false;
    }
    return true;
}

void Window::maximize() const
{
    SDL_MaximizeWindow(window_);
}

float getTime()
{
    if (!initSubSystem(SDL_INIT_TIMER))
        return 0.0f;
    static const auto start = SDL_GetPerformanceCounter();
    const auto now = SDL_GetPerformanceCounter();
    return static_cast<float>(now - start) / SDL_GetPerformanceFrequency();
}

std::optional<Window> makeWindow(
    const std::string& title, size_t width, size_t height, const Window::Properties& properties)
{
    Window window;
    if (!window.init(title, width, height, properties))
        return std::nullopt;
    return window;
}
}
