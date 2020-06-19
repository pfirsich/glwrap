#pragma once

#include <glm/glm.hpp>

#include "mesh.hpp"

namespace glwx {
struct AttributeLocations {
    size_t position = 0;
    std::optional<size_t> texCoords = std::nullopt;
    std::optional<size_t> normal = std::nullopt;
};

struct Region {
    glm::vec2 min { 0.0f, 0.0f };
    glm::vec2 max { 1.0f, 1.0f };
};

glwx::Mesh makeQuadMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc,
    const Region& position, const Region& texCoords = Region {});

glwx::Mesh makeBoxMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc, float width,
    float height, float depth);

glwx::Mesh makeSphereMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc,
    float radius, int slices, int stacks, bool cubeProjectionTexCoords);
}
