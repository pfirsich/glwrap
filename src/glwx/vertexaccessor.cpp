#include "vertexaccessor.hpp"

namespace glwx {
glm::vec4 ui2101010ToVec(uint32_t val)
{
    // clang-format off
    return glm::vec4(
        ((val >> 0) & 1023) / 1023.0f, // x
        ((val >> 10) & 1023) / 1023.0f, // y
        ((val >> 20) & 1023) / 1023.0f, // z
        ((val >> 30) & 0b11) / 3.0f // w
    );
    // clang-format on
}

uint32_t vecToUi2101010(const glm::vec4& val)
{
    // clang-format off
    return static_cast<uint32_t>(val.x * 1023) << 0
         | static_cast<uint32_t>(val.y * 1023) << 10
         | static_cast<uint32_t>(val.z * 1023) << 20
         | static_cast<uint32_t>(val.w * 3) << 30;
    // clang-format on
}

namespace detail {

    float convertI2101010(const uint8_t* data, size_t component)
    {
        auto& s = *reinterpret_cast<const IRgba10A2*>(data);
        switch (component) {
        case 0:
            return convertNormalizedInt<int>(s.x, 0, 1023);
        case 1:
            return convertNormalizedInt<int>(s.y, 0, 1023);
        case 2:
            return convertNormalizedInt<int>(s.z, 0, 1023);
        case 3:
            return convertNormalizedInt<int>(s.w, 0, 3);
        }
        return 0.0f;
    }

    float convertUi2101010(const uint8_t* data, size_t component)
    {
        const uint32_t val = *reinterpret_cast<const uint32_t*>(data);
        switch (component) {
        case 0: // x
            return convertNormalizedInt((val >> 0) & 1023, 0, 1023);
        case 1: // y
            return convertNormalizedInt((val >> 10) & 1023, 0, 1023);
        case 2: // z
            return convertNormalizedInt((val >> 20) & 1023, 0, 1023);
        case 3: // w
            return convertNormalizedInt((val >> 30) & 0b11, 0, 0b11);
        }
        return 0.0f; // Will never happen
    }

    float convert(glw::VertexFormat::Attribute::Type dataType, bool normalized, const uint8_t* data,
        size_t component)
    {
        switch (dataType) {
        case glw::VertexFormat::Attribute::Type::I8:
            return convertInt<int8_t>(normalized, data, component);
        case glw::VertexFormat::Attribute::Type::U8:
            return convertInt<uint8_t>(normalized, data, component);
        case glw::VertexFormat::Attribute::Type::I16:
            return convertInt<int16_t>(normalized, data, component);
        case glw::VertexFormat::Attribute::Type::U16:
            return convertInt<uint16_t>(normalized, data, component);
        case glw::VertexFormat::Attribute::Type::I32:
            return convertInt<int32_t>(normalized, data, component);
        case glw::VertexFormat::Attribute::Type::U32:
            return convertInt<uint32_t>(normalized, data, component);
        case glw::VertexFormat::Attribute::Type::F16:
            assert(false && "F16 unimplemented");
        case glw::VertexFormat::Attribute::Type::F32:
            return *reinterpret_cast<const float*>(data + sizeof(float) * component);
        case glw::VertexFormat::Attribute::Type::F64:
            return *reinterpret_cast<const double*>(data + sizeof(double) * component);
        case glw::VertexFormat::Attribute::Type::IW2Z10Y10X10:
            // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
            assert(normalized);
            return convertI2101010(data, component);
        case glw::VertexFormat::Attribute::Type::UiW2Z10Y10X10:
            assert(normalized);
            return convertUi2101010(data, component);
        case glw::VertexFormat::Attribute::Type::UiZ10FY11FX11F:
            assert(false && "UiZ10Y11X11 unimplemented");
        }
    }

    void assignI2101010(uint8_t* data, size_t component, float v)
    {
        auto& val = *reinterpret_cast<IRgba10A2*>(data);
        switch (component) {
        case 0: // x
            val.x = convertToNormalizedInt<int>(v, 0, 1023);
            break;
        case 1: // y
            val.y = convertToNormalizedInt<int>(v, 0, 1023);
            break;
        case 2: // z
            val.z = convertToNormalizedInt<int>(v, 0, 1023);
            break;
        case 3: // w
            val.w = convertToNormalizedInt<int>(v, 0, 3);
            break;
        }
    }

    void assignUi2101010(uint8_t* data, size_t component, float v)
    {
        auto& val = *reinterpret_cast<uint32_t*>(data);
        switch (component) {
        case 0: // x
            val = (val & (0xffffffff ^ (1023 << 0)))
                | (convertToNormalizedInt<uint32_t>(v, 0, 1023) << 0);
            break;
        case 1: // y
            val = (val & (0xffffffff ^ (1023 << 10)))
                | (convertToNormalizedInt<uint32_t>(v, 0, 1023) << 10);
            break;
        case 2: // z
            val = (val & (0xffffffff ^ (1023 << 20)))
                | (convertToNormalizedInt<uint32_t>(v, 0, 1023) << 20);
            break;
        case 3: // w
            val = (val & (0xffffffff ^ (0b11 << 30)))
                | (convertToNormalizedInt<uint32_t>(v, 0, 3) << 30);
            break;
        }
    }

    void assign(glw::VertexFormat::Attribute::Type dataType, bool normalized, uint8_t* data,
        size_t component, float v)
    {
        switch (dataType) {
        case glw::VertexFormat::Attribute::Type::I8:
            assignInt<int8_t>(normalized, data, component, v);
            break;
        case glw::VertexFormat::Attribute::Type::U8:
            assignInt<uint8_t>(normalized, data, component, v);
            break;
        case glw::VertexFormat::Attribute::Type::I16:
            assignInt<int16_t>(normalized, data, component, v);
            break;
        case glw::VertexFormat::Attribute::Type::U16:
            assignInt<uint16_t>(normalized, data, component, v);
            break;
        case glw::VertexFormat::Attribute::Type::I32:
            assignInt<int32_t>(normalized, data, component, v);
            break;
        case glw::VertexFormat::Attribute::Type::U32:
            assignInt<uint32_t>(normalized, data, component, v);
            break;
        case glw::VertexFormat::Attribute::Type::F16:
            assert(false && "F16 unimplemented");
            break;
        case glw::VertexFormat::Attribute::Type::F32:
            *reinterpret_cast<float*>(data + sizeof(float) * component) = v;
            break;
        case glw::VertexFormat::Attribute::Type::F64:
            *reinterpret_cast<double*>(data + sizeof(double) * component) = v;
            break;
        case glw::VertexFormat::Attribute::Type::IW2Z10Y10X10:
            assert(normalized); // see above
            assignI2101010(data, component, v);
            break;
        case glw::VertexFormat::Attribute::Type::UiW2Z10Y10X10:
            assignUi2101010(data, component, v);
            break;
        case glw::VertexFormat::Attribute::Type::UiZ10FY11FX11F:
            assert(false && "UiZ10Y11X11 unimplemented");
            break;
        }
    }
}
}
