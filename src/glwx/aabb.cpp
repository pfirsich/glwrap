#include "aabb.hpp"

#include <glwx/math.hpp>

namespace glwx {
void Aabb::fit(const glm::vec3& point)
{
    min = glm::min(min, point);
    max = glm::max(max, point);
}

void Aabb::fit(const Aabb& other)
{
    fit(other.min);
    fit(other.max);
}

bool Aabb::empty() const
{
    return glm::length(max - min) > std::numeric_limits<float>::epsilon();
}

bool Aabb::valid() const
{
    return glm::all(glm::lessThanEqual(min, max));
}

glm::vec3 Aabb::center() const
{
    return (min + max) * 0.5f;
}

glm::vec3 Aabb::extent() const
{
    return (max - min) * 0.5f;
}

glm::vec3 Aabb::size() const
{
    return max - min;
}

bool Aabb::contains(const glm::vec3& point) const
{
    return glm::all(glm::greaterThanEqual(point, min)) && glm::all(glm::lessThanEqual(point, max));
}

bool Aabb::overlaps(const Aabb& other) const
{
    return glm::all(glm::lessThanEqual(min, other.max))
        && glm::all(glm::greaterThanEqual(max, other.min));
}

Aabb Aabb::transformed(const glm::mat4& mat) const
{
    // https://zeux.io/2010/10/17/aabb-from-obb-with-component-wise-abs/
    // for later understanding: each of the ± are independent signs!
    // that's why you need the abs
    const auto cen = glwx::transformPoint(mat, center());
    const auto ext = glwx::transformVector(glwx::matAbs(mat), extent());
    return Aabb { cen - ext, cen + ext };
}
}
