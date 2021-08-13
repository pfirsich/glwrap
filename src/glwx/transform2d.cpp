#include "transform2d.hpp"

namespace glwx {

Transform2D::Transform2D(float a, float b, float c, float d, float x, float y)
    : a(a)
    , b(b)
    , c(c)
    , d(d)
    , x(x)
    , y(y)
{
}

// T * R * S * O
Transform2D::Transform2D(
    const glm::vec2& position, float rotation, const glm::vec2& scale, const glm::vec2& offset)
{
    translate(offset);
    this->scale(scale);
    rotate(rotation);
    translate(position);
}

void Transform2D::reset()
{
    a = 1.0f;
    b = 0.0f;
    c = 0.0f;
    d = 1.0f;
    x = 0.0f;
    y = 0.0f;
}

void Transform2D::translate(const glm::vec2& v)
{
    x += v.x;
    y += v.y;
}

void Transform2D::scale(const glm::vec2& v)
{
    a *= v.x;
    b *= v.x;
    x *= v.x;
    c *= v.y;
    d *= v.y;
    y *= v.y;
}

void Transform2D::rotate(float angle)
{
    const auto ca = std::cos(angle);
    const auto sa = std::sin(angle);
    apply(Transform2D(ca, -sa, sa, ca));
}

void Transform2D::apply(const Transform2D& other)
{
    const auto na = other.a * a + other.b * c;
    const auto nb = other.a * b + other.b * d;
    const auto nc = other.c * a + other.d * c;
    const auto nd = other.c * b + other.d * d;
    a = na;
    b = nb;
    c = nc;
    d = nd;
    const auto nx = other.a * x + other.b * y + other.x;
    const auto ny = other.c * x + other.d * y + other.y;
    x = nx;
    y = ny;
}

glm::mat3 Transform2D::getMatrix() const
{
    return glm::mat3(a, c, 0.0f, b, d, 0.0f, x, y, 1.0f);
}

glm::vec2 Transform2D::transformDirection(const glm::vec2& dir) const
{
    return glm::vec2(a * dir.x + b * dir.y, c * dir.x + d * dir.y);
}

glm::vec2 Transform2D::transformPoint(const glm::vec2& point) const
{
    return glm::vec2(a * point.x + b * point.y + x, c * point.x + d * point.y + y);
}

}
