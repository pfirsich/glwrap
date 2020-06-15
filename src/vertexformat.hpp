#pragma once

#include <algorithm>
#include <optional>
#include <vector>

#include <glad/glad.h>

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

        static size_t getDataTypeSize(Type type)
        {
            switch (type) {
            case Type::I8:
            case Type::U8:
                return 1;
            case Type::I16:
            case Type::U16:
            case Type::F16:
                return 2;
            case Type::I32:
            case Type::U32:
            case Type::F32:
            case Type::I2_10_10_10:
            case Type::U2_10_10_10:
            case Type::U10_11_11:
                return 4;
            case Type::F64:
                return 8;
            }
        }

        size_t getAlignedSize() const
        {
            // https://www.opengl.org/wiki/Vertex_Specification_Best_Practices#Attribute_sizes
            // => align each attribute to at least 4 bytes
            switch (dataType) {
            case Type::I8:
            case Type::U8:
                // since size in [1, 4], we can just return 4
                return 4;
            case Type::I16:
            case Type::U16:
            case Type::F16:
                if (size == 1)
                    return 2;
                else if (size == 3)
                    return 4;
                else
                    // size = 2 and 4 are 4 byte aligned
                    return size;
            default:
                // everything else is at least 4-byte aligned
                return size;
            }
        }

        size_t getTotalSize() const
        {
            return getDataTypeSize(dataType) * getAlignedSize();
        }
    };

    VertexFormat() = default;
    ~VertexFormat() = default;
    VertexFormat(const VertexFormat& other) = default;
    VertexFormat& operator=(const VertexFormat& other) = default;

    // Returns an index into the attributes vector for a given attribute location if present
    std::optional<size_t> get(int location) const
    {
        const auto it = std::find_if(attributes_.begin(), attributes_.end(),
            [location](const auto& attr) { return attr.location == location; });
        if (it == attributes_.end())
            return std::nullopt;
        return std::distance(attributes_.begin(), it);
    }

    VertexFormat& add(int location, int size, Attribute::Type dataType, bool normalized = false,
        unsigned int divisor = 0)
    {
        assert(size >= 1 && size <= 4);
        assert(!get(location));
        attributes_.push_back(Attribute { stride_, location, size, dataType, normalized, divisor });
        stride_ += attributes_.back().getTotalSize();
        return *this;
    }

    void set() const
    {
        for (const auto& attr : attributes_) {
            glEnableVertexAttribArray(attr.location);
            glVertexAttribPointer(attr.location, attr.size, static_cast<GLenum>(attr.dataType),
                attr.normalized ? GL_TRUE : GL_FALSE, stride_,
                reinterpret_cast<const GLvoid*>(attr.offset));
            if (attr.divisor > 0)
                glVertexAttribDivisor(attr.location, attr.divisor);
        }
    }

    size_t getStride() const
    {
        return stride_;
    }

private:
    std::vector<Attribute> attributes_;
    size_t stride_ = 0;
};
}
