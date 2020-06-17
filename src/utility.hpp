#pragma once

#include <tuple>
#include <utility>
#include <vector>

namespace glw {
template <typename T>
std::pair<const uint8_t*, size_t> toPtrRange(const T* ptr, size_t count)
{
    return std::make_pair(reinterpret_cast<const uint8_t*>(ptr), sizeof(T) * count);
}

// sizeof will result in a substitution failure for T = void, so
// I can overload instead of specializing the template.
std::pair<const uint8_t*, size_t> toPtrRange(const void* data, size_t size);

template <typename Container>
std::pair<const uint8_t*, size_t> toPtrRange(const Container& container)
{
    return toPtrRange(container.data(), container.size());
}

template <typename T, size_t N>
std::pair<const uint8_t*, size_t> toPtrRange(T (&array)[N])
{
    return toPtrRange(&array[0], N);
}

template <typename... Args>
std::vector<uint8_t> makeByteVector(Args&&... args)
{
    const auto [data, size] = toPtrRange(std::forward<Args>(args)...);
    return std::vector<uint8_t>(data, data + size);
}

std::vector<uint8_t> makeByteVector();

template <typename T>
std::tuple<size_t, const uint8_t*, size_t> toOffsetPtrRange(
    size_t offset, const T* ptr, size_t count)
{
    return std::make_tuple(
        sizeof(T) * offset, reinterpret_cast<const uint8_t*>(ptr), sizeof(T) * count);
}

std::tuple<size_t, const uint8_t*, size_t> toOffsetPtrRange(
    size_t offset, const void* data, size_t size);

template <typename Container>
std::tuple<size_t, const uint8_t*, size_t> toOffsetPtrRange(
    size_t offset, const Container& container)
{
    return toOffsetPtrRange(offset, container.data(), container.size());
}

template <typename T, size_t N>
std::tuple<size_t, const uint8_t*, size_t> toOffsetPtrRange(size_t offset, T (&array)[N])
{
    return toOffsetPtrRange(offset, &array[0], N);
}
}
