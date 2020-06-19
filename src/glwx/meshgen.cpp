#include "meshgen.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "vertexaccessor.hpp"

namespace glwx {

glwx::Mesh makeQuadMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc,
    const Region& position, const Region& texCoords)
{
    static constexpr std::array<uint8_t, 6> indices { 0, 1, 2, 2, 3, 0 };

    glwx::Mesh quad;

    auto& vbuf = quad.addVertexBuffer(vfmt, glw::Buffer::UsageHint::StaticDraw);
    vbuf.resize(4);

    assert(vfmt.get(loc.position));
    auto posAcc = glwx::VertexAccessor<glm::vec2>(vbuf, loc.position);
    posAcc[0] = position.min;
    posAcc[1] = glm::vec2(position.max.x, position.min.x);
    posAcc[2] = position.max;
    posAcc[3] = glm::vec2(position.min.x, position.max.x);

    if (loc.texCoords) {
        assert(vfmt.get(*loc.texCoords));
        auto tcAcc = glwx::VertexAccessor<glm::vec2>(vbuf, *loc.texCoords);
        tcAcc[0] = texCoords.min;
        tcAcc[1] = glm::vec2(texCoords.max.x, texCoords.min.x);
        tcAcc[2] = texCoords.max;
        tcAcc[3] = glm::vec2(texCoords.min.x, texCoords.max.x);
    }

    vbuf.update();

    quad.addIndexBuffer(glwx::IndexBuffer::ElementType::U8, glw::Buffer::UsageHint::StaticDraw)
        .data(indices);

    return quad;
}

glwx::Mesh makeBoxMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc, float width,
    float height, float depth)
{
    // clang-format off
    static constexpr std::array vertices = {
        -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, // +z
         1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, // -z
         1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, // +x
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, // -x
        -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, // +y
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, // -y
    };

    static constexpr std::array<uint8_t, 6> indices = { 0, 1, 2, 0, 2, 3 };

    static constexpr std::array<glm::vec3, 6> normals = {
        glm::vec3( 0.0f,  0.0f,  1.0f),
        glm::vec3( 0.0f,  0.0f, -1.0f),
        glm::vec3( 1.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3( 0.0f,  1.0f,  0.0f),
        glm::vec3( 0.0f, -1.0f,  0.0f),
    };

    static constexpr std::array<glm::vec2, 4> texCoords = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
    };
    // clang-format on

    glm::vec3 halfSize = glm::vec3(width * 0.5f, height * 0.5f, depth * 0.5f);

    glwx::Mesh box;

    auto& vbuf = box.addVertexBuffer(vfmt, glw::Buffer::UsageHint::StaticDraw);
    const auto vertexCount = vertices.size() / 3; // x, y, z
    vbuf.resize(vertexCount);

    assert(vfmt.get(loc.position));
    auto posAcc = glwx::VertexAccessor<glm::vec3>(vbuf, loc.position);
    for (size_t i = 0; i < vertexCount; ++i)
        posAcc[i] = glm::make_vec3(vertices.data() + i * 3) * halfSize;

    if (loc.normal) {
        assert(vfmt.get(*loc.normal));
        auto normalAcc = glwx::VertexAccessor<glm::vec3>(vbuf, *loc.normal);
        for (size_t i = 0; i < vertexCount; ++i) {
            const auto side = i / 4;
            normalAcc[i] = normals[side];
        }
    }

    if (loc.texCoords) {
        assert(vfmt.get(*loc.texCoords));
        auto tcAcc = glwx::VertexAccessor<glm::vec2>(vbuf, *loc.texCoords);
        for (size_t side = 0; side < 6; ++side)
            for (size_t corner = 0; corner < 4; ++corner)
                tcAcc[side * 4 + corner] = texCoords[corner];
    }

    vbuf.update();

    auto& ibuf = box.addIndexBuffer(
        glwx::IndexBuffer::ElementType::U8, glw::Buffer::UsageHint::StaticDraw);
    ibuf.resize(6 * 2 * 3); // 6 sides, 2 triangles per side, 3 indices per triangle
    auto& ibufData = ibuf.getData();
    for (size_t side = 0; side < 6; ++side)
        for (size_t vertex = 0; vertex < 6; ++vertex)
            ibufData[side * 6 + vertex] = 4 * side + indices[vertex];

    return box;
}
}
