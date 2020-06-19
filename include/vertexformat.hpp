#pragma once

#include <algorithm>
#include <optional>
#include <vector>

#include "glad/glad.h"

namespace glw {
class VertexFormat {
public:
    struct Attribute {
        enum class Type : GLenum {
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

        size_t offset;
        size_t location;
        int components;
        Type dataType;
        bool normalized = false;
        unsigned int divisor = 0;

        size_t getAlignedSize() const;
    };

    VertexFormat() = default;
    ~VertexFormat() = default;
    VertexFormat(const VertexFormat& other) = default;
    VertexFormat& operator=(const VertexFormat& other) = default;

    // add might obviously invalidate the pointers!
    const Attribute* get(size_t location) const;

    VertexFormat& add(size_t location, int components, Attribute::Type dataType,
        bool normalized = false, unsigned int divisor = 0);

    void set() const;

    size_t getStride() const;

private:
    std::vector<Attribute> attributes_;
    size_t stride_ = 0;
};
}
