#include "glwx/indexaccessor.hpp"

#include <cassert>

#include "glw/log.hpp"

namespace glwx {
size_t IndexAccessor::Proxy::operator=(size_t index)
{
    assert(elementSize == 1 || elementSize == 2 || elementSize == 4);
    switch (elementSize) {
    case 1:
        assert(index <= std::numeric_limits<uint8_t>::max());
        *reinterpret_cast<uint8_t*>(data) = static_cast<uint8_t>(index);
        break;
    case 2:
        assert(index <= std::numeric_limits<uint16_t>::max());
        *reinterpret_cast<uint16_t*>(data) = static_cast<uint16_t>(index);
        break;
    case 4:
        assert(index <= std::numeric_limits<uint32_t>::max());
        *reinterpret_cast<uint32_t*>(data) = static_cast<uint32_t>(index);
        break;
    }
    return index;
}

IndexAccessor::Proxy::operator size_t() const
{
    return get();
}

size_t IndexAccessor::Proxy::get() const
{
    assert(elementSize == 1 || elementSize == 2 || elementSize == 4);
    switch (elementSize) {
    case 1:
        return *reinterpret_cast<const uint8_t*>(data);
    case 2:
        return *reinterpret_cast<const uint16_t*>(data);
    case 4:
        return *reinterpret_cast<const uint32_t*>(data);
    }
    return 0;
}

IndexAccessor::IndexAccessor(IndexBuffer& buffer)
    : buffer_(buffer)
{
}

IndexAccessor::Proxy IndexAccessor::operator[](size_t index)
{
    return Proxy { buffer_.getData().data() + index * buffer_.getElementSize(),
        buffer_.getElementSize() };
}
}
