#include <array>
#include <iostream>

#include <SDL2/SDL.h>

#include "glw.hpp"
#include "glwx.hpp"

SDL_Window* window;
SDL_GLContext glContext;

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

void sdlLogFunction(void* /*userdata*/, int category, SDL_LogPriority priority, const char* message)
{
    LOG_LEVEL(convertSdlLogPriority(priority), "SDL log (category: {}): {}", category, message);
}

bool initSdl(const char* title, size_t width, size_t height, bool fullscreen)
{
    SDL_LogSetOutputFunction(sdlLogFunction, nullptr);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL Video" << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

#ifndef NDEBUG
    int contextFlags = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &contextFlags);
    contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);
#endif

    const auto msaaSamples = 0;
    if (msaaSamples > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaaSamples);
    }

    Uint32 flags = SDL_WINDOW_OPENGL;
    flags |= (fullscreen ? SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    flags |= SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow(
        title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        return false;
    }

    const auto vsync = false;
    if (vsync) {
        if (SDL_GL_SetSwapInterval(1) < 0) {
            std::cerr << "SDL_GL_SetSwapInterval failed: " << SDL_GetError() << std::endl;
        }
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    return true;
}

int main(int, char**)
{
    using namespace std::string_literals;

    if (!initSdl("GLW Test", 1024, 768, false)) {
        return 1;
    }

    const auto prog = glwx::makeShaderProgram(R"(
        #version 330 core

        layout (location = 0) in vec2 attrPosition;
        layout (location = 1) in vec2 attrTexCoords;

        out vec2 texCoords;

        void main() {
            texCoords = attrTexCoords;
            gl_Position = vec4(attrPosition, 0.0, 1.0);
        }
    )"s,
        R"(
        #version 330 core

        uniform sampler2D tex;

        in vec2 texCoords;

        out vec4 fragColor;

        void main() {
            fragColor = texture2D(tex, texCoords);
        }
    )"s);
    if (!prog) {
        return 1;
    }

    // clang-format off
    const std::array<float, 16> vertices {
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f,
    };

    const std::array<uint8_t, 6> indices {
        0, 1, 2,
        2, 3, 0,
    };
    // clang-format on

    glw::VertexFormat vertFmt;
    vertFmt.add(0, 2, glw::VertexFormat::Attribute::Type::F32);
    vertFmt.add(1, 2, glw::VertexFormat::Attribute::Type::F32);

    glwx::Mesh quad;
    quad.addVertexBuffer(vertFmt, glw::Buffer::UsageHint::StaticDraw).data(vertices);
    quad.addIndexBuffer(glwx::IndexBuffer::ElementType::U8, glw::Buffer::UsageHint::StaticDraw)
        .data(indices);

    // const auto texture = glwx::makeTexture2D(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    const auto texture = glwx::makeTexture2D(512, 512, 32);

    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texture.bind(0);
        prog->bind();
        prog->setUniform("tex", 0);
        quad.draw();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
