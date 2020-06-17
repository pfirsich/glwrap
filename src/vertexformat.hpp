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
            I2_10_10_10 = GL_INT_2_10_10_10_REV,
            U2_10_10_10 = GL_UNSIGNED_INT_2_10_10_10_REV,
            U10_11_11 = GL_UNSIGNED_INT_10F_11F_11F_REV,
        };

        size_t offset;
        int location;
        int size;
        Type dataType;
        bool normalized = false;
        unsigned int divisor = 0;

        static size_t getDataTypeSize(Type type);

        size_t getAlignedSize() const;
        size_t getTotalSize() const;
    };

    VertexFormat() = default;
    ~VertexFormat() = default;
    VertexFormat(const VertexFormat& other) = default;
    VertexFormat& operator=(const VertexFormat& other) = default;

    // Returns an index into the attributes vector for a given attribute location if present
    std::optional<size_t> get(int location) const;

    VertexFormat& add(int location, int size, Attribute::Type dataType, bool normalized = false,
        unsigned int divisor = 0);

    void set() const;

    size_t getStride() const;

private:
    std::vector<Attribute> attributes_;
    size_t stride_ = 0;
};
}
