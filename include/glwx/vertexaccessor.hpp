#pragma once

#include <glm/glm.hpp>

#include "buffers.hpp"
#include "log.hpp"

namespace glwx {
namespace detail {
    float convert(
        glw::AttributeType dataType, bool normalized, const uint8_t* data, size_t component);

    void assign(
        glw::AttributeType dataType, bool normalized, uint8_t* data, size_t component, float v);
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
            return get();
        }

        T get() const
        {
            T v {};
            for (size_t i = 0; i < numComponents; ++i)
                v[i] = detail::convert(dataType, normalized, data, i);
            return v;
        }

        uint8_t* data;
        glw::AttributeType dataType;
        bool normalized;
    };

    VertexAccessor(VertexBuffer& buffer, int location)
        : buffer_(buffer)
        , attribute_(*buffer.getVertexFormat().get(location))
    {
        assert(numComponents <= attribute_.components);
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
