#pragma once

#include "buffers.hpp"
#include "enums.hpp"
#include "vertexarray.hpp"

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

    void addVertexBuffer(
        const glw::Buffer& buffer, const glw::VertexFormat& vfmt, size_t offset, size_t count);
    void addVertexBuffer(const glw::Buffer& buffer, const glw::VertexFormat& vfmt, size_t count);
    void addVertexBuffer(const VertexBuffer& buffer); // sets count too

    void setIndexBuffer(
        const glw::Buffer& buffer, glw::IndexType indexType, size_t offset, size_t count);
    void setIndexBuffer(const glw::Buffer& buffer, glw::IndexType indexType, size_t count);
    void setIndexBuffer(const IndexBuffer& buffer); // sets count too

    // For index meshes this range is a range in the index buffer, for non-indexed meshes
    // it's a range of vertex indices.
    void draw(size_t offset, size_t count) const;

    void draw() const;

    void draw(size_t offset, size_t count, size_t instanceCount) const;

    void draw(size_t instanceCount) const;

private:
    void addVertexBuffer(const glw::Buffer& buffer, const glw::VertexFormat& vfmt);
    void setIndexBuffer(const glw::Buffer& buffer);

    std::optional<glw::IndexType> indexType_;
};
}
