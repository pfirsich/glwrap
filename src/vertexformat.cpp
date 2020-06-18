#include "vertexformat.hpp"

#include <cassert>

namespace glw {
size_t VertexFormat::Attribute::getDataTypeSize(Type type)
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
    case Type::IW2Z10Y10X10:
    case Type::UiW2Z10Y10X10:
    case Type::UiZ10FY11FX11F:
        return 4;
    case Type::F64:
        return 8;
    }
}

size_t VertexFormat::Attribute::getAlignedSize() const
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

size_t VertexFormat::Attribute::getTotalSize() const
{
    return getDataTypeSize(dataType) * getAlignedSize();
}

const VertexFormat::Attribute* VertexFormat::get(size_t location) const
{
    const auto it = std::find_if(attributes_.begin(), attributes_.end(),
        [location](const auto& attr) { return attr.location == location; });
    if (it == attributes_.end())
        return nullptr;
    return &(*it);
}

VertexFormat& VertexFormat::add(
    size_t location, int size, Attribute::Type dataType, bool normalized, unsigned int divisor)
{
    assert(size >= 1 && size <= 4);
    assert(!get(location));
    attributes_.push_back(Attribute { stride_, location, size, dataType, normalized, divisor });
    stride_ += attributes_.back().getTotalSize();
    return *this;
}

void VertexFormat::set() const
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

size_t VertexFormat::getStride() const
{
    return stride_;
}
}
