#include <array>
#include <iostream>

#include "glw.hpp"
#include "glwx.hpp"

#include "glwx/vertexaccessor.hpp"

struct Region {
    glm::vec2 min { 0.0f, 0.0f };
    glm::vec2 max { 1.0f, 1.0f };
};

glwx::Mesh makeQuad(const glw::VertexFormat& vfmt, size_t positionLocation, const Region& position,
    std::optional<size_t> texCoordLocation = std::nullopt, const Region& texCoords = Region {})
{
    static constexpr std::array<uint8_t, 6> indices { 0, 1, 2, 2, 3, 0 };

    glwx::Mesh quad;
    quad.addIndexBuffer(glwx::IndexBuffer::ElementType::U8, glw::Buffer::UsageHint::StaticDraw)
        .data(indices);

    auto& vbuf = quad.addVertexBuffer(vfmt, glw::Buffer::UsageHint::StaticDraw);

    assert(vfmt.get(positionLocation));
    vbuf.getData().resize(vfmt.getStride() * 4);
    auto posAcc = glwx::VertexAccessor<glm::vec2>(vbuf, positionLocation);
    posAcc[0] = position.min;
    posAcc[1] = glm::vec2(position.max.x, position.min.x);
    posAcc[2] = position.max;
    posAcc[3] = glm::vec2(position.min.x, position.max.x);

    if (texCoordLocation) {
        assert(vfmt.get(*texCoordLocation));
        auto tcAcc = glwx::VertexAccessor<glm::vec2>(vbuf, *texCoordLocation);
        tcAcc[0] = texCoords.min;
        tcAcc[1] = glm::vec2(texCoords.max.x, texCoords.min.x);
        tcAcc[2] = texCoords.max;
        tcAcc[3] = glm::vec2(texCoords.min.x, texCoords.max.x);
    }

    vbuf.update();

    return quad;
}

int main(int, char**)
{
    using namespace std::string_literals;

    const auto window = glwx::makeWindow("GLW Test", 1024, 768).value();
    glw::State::instance().setViewport(window.getSize().x, window.getSize().y);

#ifndef NDEBUG
    glwx::debug::init([](glwx::debug::Source source, glwx::debug::Type type, GLuint id,
                          glwx::debug::Severity severity, std::string_view message) {
        std::cout << "OpengL Debug "
                  << "[Source: " << source << "] [Type: " << type << "] [Id: " << id
                  << "] [Severity: " << severity << "]: " << message << std::endl;
    });
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

    auto renderTarget = glwx::makeRenderTarget(
        512, 512, { glw::ImageFormat::Rgba }, { glw::ImageFormat::Depth24 });

    const auto prog = glwx::makeShaderProgram(vert, frag).value();

    // clang-format off
    /*const std::array<float, 16> vertices {
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f,
    };

    const std::array<uint8_t, 6> indices {
        0, 1, 2,
        2, 3, 0,
    };*/
    // clang-format on

    glw::VertexFormat vertFmt;
    vertFmt.add(0, 2, glw::VertexFormat::Attribute::Type::F32);
    vertFmt.add(1, 2, glw::VertexFormat::Attribute::Type::U16, true);

    /*glwx::Mesh quad;
    quad.addVertexBuffer(vertFmt, glw::Buffer::UsageHint::StaticDraw).data(vertices);
    quad.addIndexBuffer(glwx::IndexBuffer::ElementType::U8, glw::Buffer::UsageHint::StaticDraw)
        .data(indices);*/
    auto quad = makeQuad(vertFmt, 0, Region { { -0.8f, -0.8f }, { 0.8f, 0.8f } }, 1);

    // const auto texture = glwx::makeTexture2D(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    // const auto texture = glwx::makeTexture2D(512, 512, 32);
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
