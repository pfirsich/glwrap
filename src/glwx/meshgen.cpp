#include "glwx/meshgen.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "glw/log.hpp"
#include "glwx/indexaccessor.hpp"
#include "glwx/vertexaccessor.hpp"

namespace glwx {

Mesh makeQuadMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc,
    const Region& position, const Region& texCoords)
{
    static constexpr std::array<uint8_t, 6> indices { 0, 1, 2, 2, 3, 0 };

    Mesh quad;

    auto& vbuf = quad.addVertexBuffer(vfmt, glw::Buffer::UsageHint::StaticDraw);
    vbuf.resize(4);

    assert(vfmt.get(loc.position));
    auto posAcc = VertexAccessor<glm::vec2>(vbuf, loc.position);
    posAcc[0] = position.min;
    posAcc[1] = glm::vec2(position.max.x, position.min.x);
    posAcc[2] = position.max;
    posAcc[3] = glm::vec2(position.min.x, position.max.x);

    if (loc.texCoords) {
        assert(vfmt.get(*loc.texCoords));
        auto tcAcc = VertexAccessor<glm::vec2>(vbuf, *loc.texCoords);
        tcAcc[0] = texCoords.min;
        tcAcc[1] = glm::vec2(texCoords.max.x, texCoords.min.x);
        tcAcc[2] = texCoords.max;
        tcAcc[3] = glm::vec2(texCoords.min.x, texCoords.max.x);
    }

    vbuf.update();

    quad.addIndexBuffer(glw::IndexType::U8, glw::Buffer::UsageHint::StaticDraw).data(indices);

    quad.primitive.indexRange = glwx::Primitive::Range { 0, indices.size() };

    return quad;
}

Mesh makeBoxMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc, float width,
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

    Mesh box;

    auto& vbuf = box.addVertexBuffer(vfmt, glw::Buffer::UsageHint::StaticDraw);
    const auto vertexCount = vertices.size() / 3; // x, y, z
    vbuf.resize(vertexCount);

    assert(vfmt.get(loc.position));
    auto posAcc = VertexAccessor<glm::vec3>(vbuf, loc.position);
    const glm::vec3 halfSize = glm::vec3(width * 0.5f, height * 0.5f, depth * 0.5f);
    for (size_t i = 0; i < vertexCount; ++i)
        posAcc[i] = glm::make_vec3(vertices.data() + i * 3) * halfSize;

    if (loc.normal) {
        assert(vfmt.get(*loc.normal));
        auto normalAcc = VertexAccessor<glm::vec3>(vbuf, *loc.normal);
        for (size_t i = 0; i < vertexCount; ++i) {
            const auto side = i / 4;
            normalAcc[i] = normals[side];
        }
    }

    if (loc.texCoords) {
        assert(vfmt.get(*loc.texCoords));
        auto tcAcc = VertexAccessor<glm::vec2>(vbuf, *loc.texCoords);
        for (size_t side = 0; side < 6; ++side)
            for (size_t corner = 0; corner < 4; ++corner)
                tcAcc[side * 4 + corner] = texCoords[corner];
    }

    vbuf.update();

    auto& ibuf = box.addIndexBuffer(glw::IndexType::U8, glw::Buffer::UsageHint::StaticDraw);
    // 6 sides, 2 triangles per side, 3 indices per triangle
    const auto indexCount = 6 * 2 * 3;
    ibuf.resize(indexCount);
    auto indexAcc = IndexAccessor(ibuf);
    for (size_t side = 0; side < 6; ++side)
        for (size_t vertex = 0; vertex < 6; ++vertex)
            indexAcc[side * 6 + vertex] = 4 * side + indices[vertex];

    ibuf.update();

    box.primitive.indexRange = glwx::Primitive::Range { 0, indexCount };

    return box;
}

Mesh makeSphereMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc, float radius,
    size_t slices, size_t stacks, bool cubeProjectionTexCoords)
{
    assert(slices > 3 && stacks > 2);

    Mesh sphere;

    auto& vbuf = sphere.addVertexBuffer(vfmt, glw::Buffer::UsageHint::StaticDraw);
    const auto vertexCount = slices * stacks;
    vbuf.resize(vertexCount);

    assert(vfmt.get(loc.position));
    auto posAcc = VertexAccessor<glm::vec3>(vbuf, loc.position);

    size_t index = 0;
    for (size_t stack = 0; stack < stacks; ++stack) {
        const float stackAngle = glm::pi<float>() / (stacks - 1) * stack;
        const float xzRadius = glm::sin(stackAngle) * radius;
        const float y = glm::cos(stackAngle) * radius;
        for (size_t slice = 0; slice < slices; ++slice) {
            const float sliceAngle = 2.0f * glm::pi<float>() / (slices - 1) * slice;
            posAcc[index++]
                = glm::vec3(glm::cos(sliceAngle) * xzRadius, y, glm::sin(sliceAngle) * xzRadius);
        }
    }

    if (loc.normal) {
        assert(vfmt.get(*loc.normal));
        auto normalAcc = VertexAccessor<glm::vec3>(vbuf, *loc.normal);
        for (size_t i = 0; i < vertexCount; ++i)
            normalAcc[i] = glm::normalize(posAcc[i].get());
    }

    if (loc.texCoords) {
        assert(vfmt.get(*loc.texCoords));
        auto tcAcc = VertexAccessor<glm::vec2>(vbuf, *loc.texCoords);

        if (cubeProjectionTexCoords) {
            for (size_t i = 0; i < vertexCount; ++i) {
                // http://www.gamedev.net/topic/443878-texture-lookup-in-cube-map/
                const glm::vec3 dir = glm::normalize(posAcc[i].get());
                const glm::vec3 absDir = glm::abs(dir);

                int majorDirIndex = 0;
                if (absDir.x >= absDir.y && absDir.x >= absDir.z)
                    majorDirIndex = 0;
                if (absDir.y >= absDir.x && absDir.y >= absDir.z)
                    majorDirIndex = 1;
                if (absDir.z >= absDir.x && absDir.z >= absDir.y)
                    majorDirIndex = 2;
                const float majorDirSign = dir[majorDirIndex] > 0.0f ? 1.0f : -1.0f;

                glm::vec3 vec; // Yes, vec. Which vec? I don't know
                switch (majorDirIndex) {
                case 0:
                    vec = glm::vec3(-majorDirSign * dir.z, -dir.y, dir.x);
                    break;
                case 1:
                    vec = glm::vec3(dir.x, majorDirSign * dir.z, dir.y);
                    break;
                case 2:
                    vec = glm::vec3(majorDirSign * dir.x, -dir.y, dir.z);
                    break;
                default:
                    assert(false);
                    break;
                }

                const auto u = (vec.x / glm::abs(vec.z) + 1.0f) / 2.0f;
                const auto v = (vec.y / glm::abs(vec.z) + 1.0f) / 2.0f;
                tcAcc[i++] = glm::vec2(u, v);
            }
        } else {
            size_t index = 0;
            for (size_t stack = 0; stack < stacks; ++stack) {
                const float u = 0.5f * stack / (stacks - 1);
                for (size_t slice = 0; slice < slices; ++slice) {
                    const float v = 2.0f * slice / (slices - 1);
                    tcAcc[index++] = glm::vec2(u, v);
                }
            }
        }
    }

    vbuf.update();

    auto& ibuf
        = sphere.addIndexBuffer(glw::getIndexType(vertexCount), glw::Buffer::UsageHint::StaticDraw);
    const size_t triangleCount = 2 * (slices - 1) * (stacks - 1);
    const size_t indexCount = triangleCount * 3;
    ibuf.resize(indexCount);

    index = 0;
    auto ibufAcc = IndexAccessor(ibuf);
    for (size_t stack = 0; stack < stacks - 1; ++stack) {
        size_t firstStackVertex = stack * slices;
        for (size_t slice = 0; slice < slices - 1; ++slice) {
            size_t firstFaceVertex = firstStackVertex + slice;
            size_t nextVertex = firstFaceVertex + 1;

            ibufAcc[index++] = nextVertex + slices;
            ibufAcc[index++] = firstFaceVertex + slices;
            ibufAcc[index++] = firstFaceVertex;

            ibufAcc[index++] = nextVertex;
            ibufAcc[index++] = nextVertex + slices;
            ibufAcc[index++] = firstFaceVertex;
        }
    }

    ibuf.update();

    sphere.primitive.indexRange = glwx::Primitive::Range { 0, indexCount };

    return sphere;
}
}
