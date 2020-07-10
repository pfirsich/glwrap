#pragma once

#include <istream>
#include <optional>
#include <streambuf>

#include <glm/glm.hpp>

namespace glwx {
// http://tuttlem.github.io/2014/08/18/getting-istream-to-work-off-a-byte-array.html
class membuf : public std::basic_streambuf<char> {
public:
    membuf(const uint8_t* data, size_t size)
    {
        char* p = (char*)data; // :/
        setg(p, p, p + size);
    }
};

class memstream : public std::istream {
private:
    membuf mBuffer;

public:
    memstream(const uint8_t* data, size_t size)
        : std::istream(&mBuffer)
        , mBuffer(data, size)
    {
        rdbuf(&mBuffer);
    }
};

std::string toHexStream(const uint8_t* data, size_t size);

std::optional<std::string> readFile(const std::string& filename);

uint32_t colorToInt(const glm::vec4& col);
}
