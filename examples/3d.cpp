#include <iostream>

#include <glw.hpp>
#include <glwx.hpp>

int main(int, char**)
{
    using namespace std::string_literals;

    const auto window = glwx::makeWindow("3D Example", 1024, 768).value();
    glw::State::instance().setViewport(window.getSize().x, window.getSize().y);

#ifndef NDEBUG
    glwx::debug::init();
#endif

    const auto vert = R"(
        #version 330 core

        uniform mat4 modelMatrix;
        uniform mat4 viewMatrix;
        uniform mat4 projectionMatrix;
        uniform mat3 normalMatrix;

        layout (location = 0) in vec3 attrPosition;
        layout (location = 1) in vec2 attrTexCoords;
        layout (location = 2) in vec3 attrNormal;

        out vec2 texCoords;
        out vec3 normal; // view space

        void main() {
            texCoords = attrTexCoords;
            normal = normalMatrix * attrNormal;
            gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(attrPosition, 1.0);
        }
    )"s;

    const auto frag = R"(
        #version 330 core

        uniform sampler2D albedo;
        uniform vec3 lightDir; // view space

        in vec2 texCoords;
        in vec3 normal;

        out vec4 fragColor;

        void main() {
            vec4 base = texture2D(albedo, texCoords);
            float nDotL = max(dot(lightDir, normal), 0.0);
            fragColor = vec4(base.rgb * nDotL, 1.0);
            // fragColor = vec4(normal, 1.0);
        }
    )"s;

    const auto prog = glwx::makeShaderProgram(vert, frag).value();

    glw::VertexFormat vertFmt;
    vertFmt.add(0, 3, glw::AttributeType::F32);
    vertFmt.add(1, 2, glw::AttributeType::U16, true);
    vertFmt.add(2, 4, glw::AttributeType::IW2Z10Y10X10, true);

    // auto mesh = glwx::makeBoxMesh(vertFmt, { 0, 1, 2 }, 1.0f, 1.0f, 1.0f);
    auto mesh = glwx::makeSphereMesh(vertFmt, { 0, 1, 2 }, 0.5f, 32, 32);

    const auto texture = glwx::makeTexture2D("assets/wood.jpg").value();

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix
        = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const auto aspect = static_cast<float>(window.getSize().x) / window.getSize().y;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f);

    glEnable(GL_DEPTH_TEST);

    SDL_Event event;
    bool running = true;
    float time = glwx::getTime();
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

        const auto now = glwx::getTime();
        const auto dt = time - now;
        time = now;

        modelMatrix = glm::rotate(modelMatrix, dt, glm::vec3(0.0f, 1.0f, 0.0f));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texture.bind(0);

        prog.bind();
        prog.setUniform("albedo", 0);
        prog.setUniform("modelMatrix", modelMatrix);
        prog.setUniform("viewMatrix", viewMatrix);
        prog.setUniform("projectionMatrix", projectionMatrix);
        const auto modelViewMatrix = viewMatrix * modelMatrix;
        const auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelViewMatrix)));
        prog.setUniform("normalMatrix", normalMatrix);
        prog.setUniform("lightDir", glm::vec3(0.0f, 0.0f, 1.0f));

        mesh.draw();

        window.swap();
    }

    return 0;
}
