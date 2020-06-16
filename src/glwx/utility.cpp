#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "utility.hpp"

namespace glw {

// http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::optional<std::string> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::in);
    if (!file)
        return std::nullopt;

    file.seekg(0, std::ios::end);
    std::string contents(file.tellg(), '\0');
    file.seekg(0, std::ios::beg);
    file.read(contents.data(), contents.size());
    file.close();
    return contents;
}

std::string toHexStream(const uint8_t* buffer, size_t size)
{
    std::stringstream ss;
    for (size_t i = 0; i < size; ++i) {
        ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(buffer[i]);
        if (i < size - 1)
            ss << " ";
    }
    return ss.str();
}

uint32_t colorToInt(const glm::vec4& col)
{
    return static_cast<int>(col.r * 255) << 0 | static_cast<int>(col.g * 255) << 8
        | static_cast<int>(col.b * 255) << 16 | static_cast<int>(col.a * 255) << 24;
}
}
