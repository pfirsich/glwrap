#include <array>
#include <iostream>

#include <glw.hpp>
#include <glwx.hpp>

#include "glwx/vertexaccessor.hpp"

int main(int, char**)
{
    using namespace std::string_literals;

    const auto window = glwx::makeWindow("Mesh Example", 1024, 768).value();
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

    const auto prog = glwx::makeShaderProgram(std::string_view(vert), std::string_view(frag)).value();

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
    vertFmt.add(0, 2, glw::AttributeType::F32);
    vertFmt.add(1, 2, glw::AttributeType::F32);

    glwx::Mesh quad;
    quad.addVertexBuffer(vertFmt, glw::Buffer::UsageHint::StaticDraw).data(vertices);
    quad.addIndexBuffer(glw::IndexType::U8, glw::Buffer::UsageHint::StaticDraw).data(indices);
    quad.primitive.indexRange = glwx::Primitive::Range { 0, indices.size() };

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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        prog.bind();
        texture.bind(0);
        prog.setUniform("tex", 0);
        quad.draw();

        window.swap();
    }

    return 0;
}
