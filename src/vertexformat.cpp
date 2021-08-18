#include "vertexformat.hpp"

#include <cassert>
#include <limits>

namespace glw {
IndexType getIndexType(size_t vertexCount)
{
    assert(vertexCount <= std::numeric_limits<uint32_t>::max());
    if (vertexCount <= std::numeric_limits<uint8_t>::max())
        return IndexType::U8;
    else if (vertexCount <= std::numeric_limits<uint16_t>::max())
        return IndexType::U16;
    else
        return IndexType::U32;
}

size_t getIndexTypeSize(IndexType type)
{
    switch (type) {
    case IndexType::U8:
        return 1;
    case IndexType::U16:
        return 2;
    case IndexType::U32:
        return 4;
    }
    std::abort();
}

size_t VertexFormat::Attribute::getAlignedSize() const
{
    // https://www.opengl.org/wiki/Vertex_Specification_Best_Practices#Attribute_sizes
    // => align each attribute to at least 4 bytes
    switch (dataType) {
    case AttributeType::I8:
    case AttributeType::U8:
        // since components is in [1, 4], we can just return 4
        return 4;
    case AttributeType::I16:
    case AttributeType::U16:
    case AttributeType::F16:
        // No clever formulas, we just handle every case explicitly
        if (components == 1 || components == 2)
            return 4;
        else if (components == 3 || components == 4)
            return 8;
        assert(false && "Invalid component size");
        return 2 * components;
    // Everything below is already sufficiently aligned
    case AttributeType::I32:
    case AttributeType::U32:
    case AttributeType::F32:
        return 4 * components;
    case AttributeType::IW2Z10Y10X10:
    case AttributeType::UiW2Z10Y10X10:
    case AttributeType::UiZ10FY11FX11F:
        return 4;
    case AttributeType::F64:
        return 8 * components;
    }
    std::abort();
}

const VertexFormat::Attribute* VertexFormat::get(size_t location) const
{
    const auto it = std::find_if(attributes_.begin(), attributes_.end(),
        [location](const auto& attr) { return attr.location == location; });
    if (it == attributes_.end())
        return nullptr;
    return &(*it);
}

VertexFormat& VertexFormat::add(size_t offset, size_t location, size_t components,
    AttributeType dataType, bool normalized, size_t divisor)
{
    assert(components >= 1 && components <= 4);
    assert(!get(location));
    attributes_.push_back(
        Attribute { offset, location, components, dataType, normalized, divisor });
    stride_ = std::max(stride_, offset + attributes_.back().getAlignedSize());
    return *this;
}

VertexFormat& VertexFormat::add(
    size_t location, size_t components, AttributeType dataType, bool normalized, size_t divisor)
{
    return add(stride_, location, components, dataType, normalized, divisor);
}

void VertexFormat::set() const
{
    for (const auto& attr : attributes_) {
        glEnableVertexAttribArray(attr.location);
        glVertexAttribPointer(attr.location, attr.components, static_cast<GLenum>(attr.dataType),
            attr.normalized ? GL_TRUE : GL_FALSE, stride_,
            reinterpret_cast<const GLvoid*>(attr.offset));
        if (attr.divisor > 0)
            glVertexAttribDivisor(attr.location, attr.divisor);
    }
}

void VertexFormat::setStride(size_t stride)
{
    stride_ = stride;
}

size_t VertexFormat::getStride() const
{
    return stride_;
}
}
