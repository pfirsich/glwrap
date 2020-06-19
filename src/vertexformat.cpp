#include "vertexformat.hpp"

#include <cassert>

namespace glw {
size_t VertexFormat::Attribute::getAlignedSize() const
{
    // https://www.opengl.org/wiki/Vertex_Specification_Best_Practices#Attribute_sizes
    // => align each attribute to at least 4 bytes
    switch (dataType) {
    case Type::I8:
    case Type::U8:
        // since components is in [1, 4], we can just return 4
        return 4;
    case Type::I16:
    case Type::U16:
    case Type::F16:
        // No clever formulas, we just handle every case explicitly
        if (components == 1 || components == 2)
            return 4;
        else if (components == 3 || components == 4)
            return 8;
        assert(false && "Invalid component size");
        return 2 * components;
    // Everything below is already sufficiently aligned
    case Type::I32:
    case Type::U32:
    case Type::F32:
        return 4 * components;
    case Type::IW2Z10Y10X10:
    case Type::UiW2Z10Y10X10:
    case Type::UiZ10FY11FX11F:
        return 4;
    case Type::F64:
        return 8 * components;
    }
}

const VertexFormat::Attribute* VertexFormat::get(size_t location) const
{
    const auto it = std::find_if(attributes_.begin(), attributes_.end(),
        [location](const auto& attr) { return attr.location == location; });
    if (it == attributes_.end())
        return nullptr;
    return &(*it);
}

VertexFormat& VertexFormat::add(size_t location, int components, Attribute::Type dataType,
    bool normalized, unsigned int divisor)
{
    assert(components >= 1 && components <= 4);
    assert(!get(location));
    attributes_.push_back(
        Attribute { stride_, location, components, dataType, normalized, divisor });
    stride_ += attributes_.back().getAlignedSize();
    return *this;
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

size_t VertexFormat::getStride() const
{
    return stride_;
}
}
