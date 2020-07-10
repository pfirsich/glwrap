#include "vertexaccessor.hpp"

#include <cassert>

namespace glwx {
namespace detail {
    template <uint32_t Bits>
    constexpr uint32_t ones()
    {
        return (1 << Bits) - 1;
    }

    template <uint32_t Offset, uint32_t NumBits>
    constexpr uint32_t getBits(uint32_t val)
    {
        return (val >> Offset) & ones<NumBits>();
    }

    template <uint32_t Offset, uint32_t NumBits>
    constexpr uint32_t zeroRange(uint32_t val)
    {
        return val & (0xffffffff ^ (ones<NumBits>() << Offset));
    }

    template <typename IntType>
    float convertNormalizedInt(IntType v, int64_t minVal = std::numeric_limits<IntType>::min(),
        int64_t maxVal = std::numeric_limits<IntType>::max())
    {
        if constexpr (std::is_signed_v<IntType>) {
            return static_cast<float>(v - minVal) / static_cast<float>(maxVal - minVal) * 2.0f
                - 1.0f;
        } else {
            return static_cast<float>(v) / static_cast<float>(maxVal);
        }
    }

    template <typename IntType>
    float convertInt(bool normalized, const uint8_t* data, size_t component)
    {
        const auto intVal = *reinterpret_cast<const IntType*>(data + sizeof(IntType) * component);
        if (normalized)
            return convertNormalizedInt(intVal);
        else
            return static_cast<float>(intVal);
    }

    template <uint32_t Bits>
    constexpr int fromTwosComplement(uint32_t val)
    {
        assert(val <= ones<Bits>());
        if (val >= ones<Bits - 1>()) { // negative
            // Invert the operation from toTwosComplement: subtract 1 and invert
            return ones<Bits>() ^ (val - 1);
        } else { // positive
            return static_cast<int>(val);
        }
    }

    float convertI2101010(const uint8_t* data, size_t component)
    {
        auto& val = *reinterpret_cast<const uint32_t*>(data);
        switch (component) {
        case 0:
            return convertNormalizedInt<int>(
                fromTwosComplement<10>(getBits<0, 10>(val)), -512, 511);
        case 1:
            return convertNormalizedInt<int>(
                fromTwosComplement<10>(getBits<10, 10>(val)), -512, 511);
        case 2:
            return convertNormalizedInt<int>(
                fromTwosComplement<10>(getBits<20, 10>(val)), -512, 511);
        case 3:
            return convertNormalizedInt<int>(fromTwosComplement<2>(getBits<30, 2>(val)), -2, 1);
        }
        return 0.0f;
    }

    float convertUi2101010(const uint8_t* data, size_t component)
    {
        const uint32_t val = *reinterpret_cast<const uint32_t*>(data);
        switch (component) {
        case 0: // x
            return convertNormalizedInt(getBits<0, 10>(val), 0, 1023);
        case 1: // y
            return convertNormalizedInt(getBits<10, 10>(val), 0, 1023);
        case 2: // z
            return convertNormalizedInt(getBits<20, 10>(val), 0, 1023);
        case 3: // w
            return convertNormalizedInt(getBits<30, 2>(val), 0, 3);
        }
        return 0.0f; // Will never happen
    }

    float convert(
        glw::AttributeType dataType, bool normalized, const uint8_t* data, size_t component)
    {
        switch (dataType) {
        case glw::AttributeType::I8:
            return convertInt<int8_t>(normalized, data, component);
        case glw::AttributeType::U8:
            return convertInt<uint8_t>(normalized, data, component);
        case glw::AttributeType::I16:
            return convertInt<int16_t>(normalized, data, component);
        case glw::AttributeType::U16:
            return convertInt<uint16_t>(normalized, data, component);
        case glw::AttributeType::I32:
            return convertInt<int32_t>(normalized, data, component);
        case glw::AttributeType::U32:
            return convertInt<uint32_t>(normalized, data, component);
        case glw::AttributeType::F16:
            assert(false && "F16 unimplemented");
        case glw::AttributeType::F32:
            return *reinterpret_cast<const float*>(data + sizeof(float) * component);
        case glw::AttributeType::F64:
            return *reinterpret_cast<const double*>(data + sizeof(double) * component);
        case glw::AttributeType::IW2Z10Y10X10:
            // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
            assert(normalized);
            return convertI2101010(data, component);
        case glw::AttributeType::UiW2Z10Y10X10:
            assert(normalized);
            return convertUi2101010(data, component);
        case glw::AttributeType::UiZ10FY11FX11F:
            assert(false && "UiZ10Y11X11 unimplemented");
        }
    }

    template <typename IntType>
    IntType convertToNormalizedInt(float v, int64_t minVal = std::numeric_limits<IntType>::min(),
        int64_t maxVal = std::numeric_limits<IntType>::max())
    {
        if constexpr (std::is_signed_v<IntType>) {
            return static_cast<IntType>(
                static_cast<int64_t>((v * 0.5f + 0.5f) * static_cast<float>(maxVal - minVal))
                + minVal);
        } else {
            return static_cast<IntType>(v * static_cast<float>(maxVal));
        }
    }

    template <typename IntType>
    void assignInt(bool normalized, uint8_t* data, size_t component, float v)
    {
        auto& intVal = *reinterpret_cast<IntType*>(data + sizeof(IntType) * component);
        if (normalized)
            intVal = convertToNormalizedInt<IntType>(v);
        else
            intVal = v;
    }

    template <uint32_t Bits>
    constexpr uint32_t toTwosComplement(int val)
    {
        if (val >= 0) {
            assert(static_cast<uint32_t>(val) <= ones<Bits - 1>());
            return val;
        } else {
            assert(val >= -(1 << (Bits - 1)));
            const uint32_t abs = -val;
            // invert all the bits and add 1
            // we don't have to mask off a potential overflow, because it should only overflow for
            // 0, which is handled in the above case
            return (ones<Bits>() ^ abs) + 1;
        }
    }

    void assignI2101010(uint8_t* data, size_t component, float v)
    {
        auto& val = *reinterpret_cast<uint32_t*>(data);
        switch (component) {
        case 0: // x
            val = zeroRange<0, 10>(val)
                | (toTwosComplement<10>(convertToNormalizedInt<int>(v, -512, 511)) << 0);
            break;
        case 1: // y
            val = zeroRange<10, 10>(val)
                | (toTwosComplement<10>(convertToNormalizedInt<int>(v, -512, 511)) << 10);
            break;
        case 2: // z
            val = zeroRange<20, 10>(val)
                | (toTwosComplement<10>(convertToNormalizedInt<int>(v, -512, 511)) << 20);
            break;
        case 3: // w
            val = zeroRange<30, 2>(val)
                | (toTwosComplement<2>(convertToNormalizedInt<int>(v, -2, 1)) << 30);
            break;
        }
    }

    void assignUi2101010(uint8_t* data, size_t component, float v)
    {
        auto& val = *reinterpret_cast<uint32_t*>(data);
        switch (component) {
        case 0: // x
            val = zeroRange<0, 10>(val) | (convertToNormalizedInt<uint32_t>(v, 0, 1023) << 0);
            break;
        case 1: // y
            val = zeroRange<10, 10>(val) | (convertToNormalizedInt<uint32_t>(v, 0, 1023) << 10);
            break;
        case 2: // z
            val = zeroRange<20, 10>(val) | (convertToNormalizedInt<uint32_t>(v, 0, 1023) << 20);
            break;
        case 3: // w
            val = zeroRange<30, 2>(val) | (convertToNormalizedInt<uint32_t>(v, 0, 3) << 30);
            break;
        }
    }

    void assign(
        glw::AttributeType dataType, bool normalized, uint8_t* data, size_t component, float v)
    {
        switch (dataType) {
        case glw::AttributeType::I8:
            assignInt<int8_t>(normalized, data, component, v);
            break;
        case glw::AttributeType::U8:
            assignInt<uint8_t>(normalized, data, component, v);
            break;
        case glw::AttributeType::I16:
            assignInt<int16_t>(normalized, data, component, v);
            break;
        case glw::AttributeType::U16:
            assignInt<uint16_t>(normalized, data, component, v);
            break;
        case glw::AttributeType::I32:
            assignInt<int32_t>(normalized, data, component, v);
            break;
        case glw::AttributeType::U32:
            assignInt<uint32_t>(normalized, data, component, v);
            break;
        case glw::AttributeType::F16:
            assert(false && "F16 unimplemented");
            break;
        case glw::AttributeType::F32:
            *reinterpret_cast<float*>(data + sizeof(float) * component) = v;
            break;
        case glw::AttributeType::F64:
            *reinterpret_cast<double*>(data + sizeof(double) * component) = v;
            break;
        case glw::AttributeType::IW2Z10Y10X10:
            assert(normalized); // see above
            assignI2101010(data, component, v);
            break;
        case glw::AttributeType::UiW2Z10Y10X10:
            assignUi2101010(data, component, v);
            break;
        case glw::AttributeType::UiZ10FY11FX11F:
            assert(false && "UiZ10Y11X11 unimplemented");
            break;
        }
    }
}
}
