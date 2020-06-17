#include "utility.hpp"

namespace glw {
std::pair<const uint8_t*, size_t> toPtrRange(const void* data, size_t size)
{
    return std::make_pair(reinterpret_cast<const uint8_t*>(data), size);
}

std::vector<uint8_t> makeByteVector()
{
    return std::vector<uint8_t>();
}

std::tuple<size_t, const uint8_t*, size_t> toOffsetPtrRange(
    size_t offset, const void* data, size_t size)
{
    return std::make_tuple(offset, reinterpret_cast<const uint8_t*>(data), size);
}
}
