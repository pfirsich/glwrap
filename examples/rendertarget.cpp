#include <iostream>

#include "glwx/debug.hpp"
#include "glwx/mesh.hpp"
#include "glwx/meshgen.hpp"
#include "glwx/rendertarget.hpp"
#include "glwx/shader.hpp"
#include "glwx/texture.hpp"
#include "glwx/window.hpp"

int main(int, char**)
{
    using namespace std::string_literals;

    const auto window = glwx::makeWindow("RendeTarget Example", 1024, 768).value();
    glw::State::instance().setViewport(window.getSize().x, window.getSize().y);

#ifndef NDEBUG
    glwx::debug::init();
#endif

    const auto vert = R"(
        #version 330 core

        layout (location = 0) in vec2 attrPosition;
        layout (location = 1) in vec2 attrTexCoords;

        out vec2 texCoords;

        void main() {
            texCoords = attrTexCoords;
            gl_Position = vec4(attrPosition, 0.0, 1.0);
        }
    )"s;

    const auto frag = R"(
        #version 330 core

        uniform sampler2D tex;

        in vec2 texCoords;

        out vec4 fragColor;

        void main() {
            fragColor = texture2D(tex, texCoords);
        }
    )"s;

    const auto prog
        = glwx::makeShaderProgram(std::string_view(vert), std::string_view(frag)).value();

    auto renderTarget = glwx::makeRenderTarget(
        512, 512, { glw::ImageFormat::Rgba }, { glw::ImageFormat::Depth24 });

    glw::VertexFormat vertFmt;
    vertFmt.add(0, 2, glw::AttributeType::F32);
    vertFmt.add(1, 2, glw::AttributeType::U16, true);

    auto quad = glwx::makeQuadMesh(vertFmt, { 0, 1 }, { { -0.8f, -0.8f }, { 0.8f, 0.8f } });

    const auto texture = glwx::makeTexture2D("assets/wood.jpg").value();

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

        const auto winViewport = glw::State::instance().getViewport();

        renderTarget.bind();

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texture.bind(0);
        prog.bind();
        prog.setUniform("tex", 0);
        quad.draw();

        glw::Framebuffer::unbind();
        glw::State::instance().setViewport(winViewport);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderTarget.getTexture(glw::Framebuffer::Attachment::Color0).bind(0);
        quad.draw();

        window.swap();
    }

    return 0;
}
