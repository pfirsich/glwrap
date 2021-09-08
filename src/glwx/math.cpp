#include "glwx/math.hpp"

namespace glwx {
glm::mat4 matAbs(const glm::mat4& mat)
{
    return glm::mat4(glm::abs(mat[0]), glm::abs(mat[1]), glm::abs(mat[2]), glm::abs(mat[3]));
}

// If you are dealing with arbitrary, potentially non-affine transformations (the last row of the
// matrix is not (0, 0, 0, 1), e.g. projections), you need to divide by w after the matrix
// multiplication to get the cartesian coordinate the homogeneous coordinate blongs to
glm::vec4 transform(const glm::mat4& matrix, const glm::vec4& vec)
{
    auto res = matrix * vec;
    if (std::fabs(res.w) > std::numeric_limits<float>::epsilon())
        res = res / res.w;
    return res;
}

// A directional vector!
glm::vec3 transformVector(const glm::mat4& matrix, const glm::vec3& vec)
{
    return glm::vec3(transform(matrix, glm::vec4(vec, 0.0f)));
}

glm::vec3 transformPoint(const glm::mat4& matrix, const glm::vec3& point)
{
    return glm::vec3(transform(matrix, glm::vec4(point, 1.0f)));
}

uint32_t colorComponentToInt(float c)
{
    return static_cast<uint32_t>(std::floor(255.0f * c + 0.5f));
}

uint32_t colorToInt(const glm::vec4& col)
{
    return colorComponentToInt(col.r) << 24 | colorComponentToInt(col.g) << 16
        | colorComponentToInt(col.b) << 8 | colorComponentToInt(col.a);
}

// formulas from here: https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_sRGB.txt
float srgbToLinear(float v)
{
    assert(v >= 0.0f && v <= 1.0f);
    if (v <= 0.04045f)
        return v / 12.92f;
    return std::pow((v + 0.055f) / 1.055f, 2.4f);
}

glm::vec3 srgbToLinear(const glm::vec3& v)
{
    return glm::vec3(srgbToLinear(v.r), srgbToLinear(v.g), srgbToLinear(v.b));
}

glm::vec4 srgbToLinear(const glm::vec4& v)
{
    return glm::vec4(srgbToLinear(glm::vec3(v)), v.a);
}

float linearToSrgb(float v)
{
    if (std::isnan(v))
        return 0.0f;
    else if (v > 1.0f)
        return 1.0f;
    else if (v < 0.0f)
        return 0.0f;
    else if (v <= 0.0031308f)
        return v * 12.92f;
    return 1.055f * std::pow(v, 0.41666f) - 0.055f;
}

glm::vec3 linearToSrgb(const glm::vec3& v)
{
    return glm::vec3(linearToSrgb(v.r), linearToSrgb(v.g), linearToSrgb(v.b));
}

glm::vec4 linearToSrgb(const glm::vec4& v)
{
    return glm::vec4(linearToSrgb(glm::vec3(v)), v.a);
}
}
