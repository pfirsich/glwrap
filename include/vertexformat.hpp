#pragma once

#include <algorithm>
#include <optional>
#include <vector>

#include "glad/glad.h"

namespace glw {
enum class AttributeType : GLenum {
    I8 = GL_BYTE,
    U8 = GL_UNSIGNED_BYTE,
    I16 = GL_SHORT,
    U16 = GL_UNSIGNED_SHORT,
    I32 = GL_INT,
    U32 = GL_UNSIGNED_INT,
    F16 = GL_HALF_FLOAT,
    F32 = GL_FLOAT,
    F64 = GL_DOUBLE,
    // Fixed = GL_FIXED,
    IW2Z10Y10X10 = GL_INT_2_10_10_10_REV,
    UiW2Z10Y10X10 = GL_UNSIGNED_INT_2_10_10_10_REV,
    UiZ10FY11FX11F = GL_UNSIGNED_INT_10F_11F_11F_REV,
};

// Not really supposed to be here, but when you need it, you most likely need the rest of this file
enum class IndexType {
    U8 = GL_UNSIGNED_BYTE,
    U16 = GL_UNSIGNED_SHORT,
    U32 = GL_UNSIGNED_INT,
};

IndexType getIndexType(size_t vertexCount);
size_t getIndexTypeSize(IndexType type);

class VertexFormat {
public:
    struct Attribute {
        size_t offset;
        size_t location;
        size_t components;
        AttributeType dataType;
        bool normalized = false;
        size_t divisor = 0;

        size_t getAlignedSize() const;
    };

    VertexFormat() = default;
    ~VertexFormat() = default;
    VertexFormat(const VertexFormat& other) = default;
    VertexFormat& operator=(const VertexFormat& other) = default;

    // add might obviously invalidate the pointers!
    const Attribute* get(size_t location) const;

    // this sets the stride to max(stride, offset + attribute.getAlignedSize)
    VertexFormat& add(size_t offset, size_t location, size_t components, AttributeType dataType,
        bool normalized = false, size_t divisor = 0);

    // calls add(getStride(), ...)
    VertexFormat& add(size_t location, size_t components, AttributeType dataType,
        bool normalized = false, size_t divisor = 0);

    void set() const;

    size_t getStride() const;
    void setStride(size_t stride);

private:
    std::vector<Attribute> attributes_;
    size_t stride_ = 0;
};
}
