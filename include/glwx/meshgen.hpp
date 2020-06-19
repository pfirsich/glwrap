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

Mesh makeQuadMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc,
    const Region& position, const Region& texCoords = Region {});

Mesh makeBoxMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc, float width,
    float height, float depth);

Mesh makeSphereMesh(const glw::VertexFormat& vfmt, const AttributeLocations& loc, float radius,
    size_t slices, size_t stacks, bool cubeProjectionTexCoords = false);
}
