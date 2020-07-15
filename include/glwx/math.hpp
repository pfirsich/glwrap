#include <cmath>
#include <limits>

#include <glm/glm.hpp>

namespace glwx {
glm::mat4 matAbs(const glm::mat4& mat);

// If you are dealing with arbitrary, potentially non-affine transformations (the last row of the
// matrix is not (0, 0, 0, 1), e.g. projections), you need to divide by w after the matrix
// multiplication to get the cartesian coordinate the homogeneous coordinate blongs to
glm::vec4 transform(const glm::mat4& matrix, const glm::vec4& vec);

// A directional vector!
glm::vec3 transformVector(const glm::mat4& matrix, const glm::vec3& vec);

glm::vec3 transformPoint(const glm::mat4& matrix, const glm::vec3& point);

uint32_t colorComponentToInt(float c);
uint32_t colorToInt(const glm::vec4& col);

float srgbToLinear(float v);
glm::vec3 srgbToLinear(const glm::vec3& v);
glm::vec4 srgbToLinear(const glm::vec4& v);

float linearToSrgb(float v);
glm::vec3 linearToSrgb(const glm::vec3& v);
glm::vec4 linearToSrgb(const glm::vec4& v);
}
