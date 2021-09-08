#pragma once

#include "glw/enums.hpp"
#include "glw/vertexarray.hpp"
#include "glwx/buffers.hpp"

namespace glwx {
class Primitive {
public:
    struct Range {
        size_t offset = 0;
        size_t count = 0;
    };

    Range vertexRange;
    // This is unused if no index buffer was added
    Range indexRange;
    glw::VertexArray vertexArray;
    glw::DrawMode mode;

    explicit Primitive(glw::DrawMode mode = glw::DrawMode::Triangles);

    // Make sure to set vertexRange before drawing!
    // These don't have an extra count parameter, because you might have multiple vertex buffers for
    // a single primitive
    void addVertexBuffer(const glw::Buffer& buffer, const glw::VertexFormat& vfmt);
    void addVertexBuffer(const VertexBuffer& buffer); // sets count too

    // Make sure to set indexRange before drawing!
    void setIndexBuffer(const glw::Buffer& buffer, glw::IndexType indexType);
    void setIndexBuffer(const IndexBuffer& buffer); // sets count too

    // For index meshes this range is a range in the index buffer, for non-indexed meshes
    // it's a range of vertex indices.
    void draw(size_t offset, size_t count) const;

    void draw() const;

    void draw(size_t offset, size_t count, size_t instanceCount) const;

    void draw(size_t instanceCount) const;

private:
    std::optional<glw::IndexType> indexType_;
};
}
