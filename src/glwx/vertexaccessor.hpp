#pragma once

#include <glm/glm.hpp>

#include "buffers.hpp"

namespace glwx {

namespace detail {
    struct UiRgb10A2 {
        unsigned int w : 2;
        unsigned int z : 10;
        unsigned int y : 10;
        unsigned int x : 10;
    };

    struct IRgba10A2 {
        int w : 2;
        int z : 10;
        int y : 10;
        int x : 10;
    };
}

glm::vec4 i2101010ToVec(uint32_t val);
uint32_t vecToI2101010(const glm::vec4& val);

glm::vec4 ui2101010ToVec(uint32_t val);
uint32_t vecToUi2101010(const glm::vec4& val);

namespace detail {
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

    float convertI2101010(const uint8_t* data, size_t component);
    float convertUi2101010(const uint8_t* data, size_t component);

    float convert(glw::VertexFormat::Attribute::Type dataType, bool normalized, const uint8_t* data,
        size_t component);

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

    void assignI2101010(uint8_t* data, size_t component, float v);
    void assignUi2101010(uint8_t* data, size_t component, float v);

    void assign(glw::VertexFormat::Attribute::Type dataType, bool normalized, uint8_t* data,
        size_t component, float v);
}

template <typename T>
class VertexAccessor {
    static constexpr auto numComponents = T().length();
    static_assert(numComponents == 2 || numComponents == 3 || numComponents == 4);

public:
    struct Proxy {
        T operator=(const T& v)
        {
            for (size_t i = 0; i < numComponents; ++i)
                detail::assign(dataType, normalized, data, i, v[i]);
            return v;
        }

        operator T() const
        {
            T v {};
            for (size_t i = 0; i < numComponents; ++i)
                v[i] = detail::convert(dataType, normalized, data, i);
            return v;
        }

        uint8_t* data;
        glw::VertexFormat::Attribute::Type dataType;
        bool normalized;
    };

    VertexAccessor(VertexBuffer& buffer, int location)
        : buffer_(buffer)
        , attribute_(*buffer.getVertexFormat().get(location))
    {
        assert(numComponents == attribute_.size);
    }

    Proxy operator[](size_t index)
    {
        return Proxy { getElementPointer(index), attribute_.dataType, attribute_.normalized };
    }

private:
    uint8_t* getElementPointer(size_t index)
    {
        const auto& format = buffer_.getVertexFormat();
        const auto vertex = buffer_.getData().data() + index * format.getStride();
        return vertex + attribute_.offset;
    }

    VertexBuffer& buffer_;
    const glw::VertexFormat::Attribute& attribute_;
};
}
