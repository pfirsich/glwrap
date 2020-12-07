#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <utility>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

namespace glwx {
class Sdl {
public:
    static Sdl& instance();
    ~Sdl();

    int getResult() const;

private:
    Sdl();
    Sdl(const Sdl&) = delete;
    Sdl& operator=(const Sdl&) = delete;

    // SDL_Init returns 0 on success and < 0 on error
    int result_ = 1;
};

class Window {
public:
    struct Properties {
        int contextMajor = 3;
        int contextMinor = 3;
        int redSize = 8;
        int greenSize = 8;
        int blueSize = 8;
        int alphaSize = 8;
        bool fullscreen = false;
        bool fullscreenDesktop = false;
        bool hidden = false;
        bool borderless = false;
        bool resizable = true;
        bool maximized = false;
        bool minimized = false;
        int depth = 24;
        bool stencil = false;
        bool srgb = false;
        int msaaSamples = 0;
    };

    Window() = default;
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&& other);
    Window& operator=(Window&& other); // can only move-assign to default-constructed window

    bool init(const std::string& title, size_t width, size_t height, const Properties& properties);

    glm::ivec2 getSize() const;
    void swap() const;
    bool setSwapInterval(int interval) const;

private:
    SDL_Window* window_ = nullptr;
    SDL_GLContext glContext_ = nullptr;
};

float getTime();

std::optional<Window> makeWindow(const std::string& title, size_t width, size_t height,
    const Window::Properties& properties = Window::Properties());
}
