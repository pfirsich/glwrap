#include "transform.hpp"

#include <glm/gtx/matrix_query.hpp>
#include <glm/gtx/transform.hpp>

namespace glwx {
Transform::Transform(const glm::vec3& pos)
    : position_(pos)
{
}

Transform::Transform(const glm::vec3& pos, const glm::quat& ori)
    : position_(pos)
    , orientation_(ori)
{
}

Transform::Transform(const glm::vec3& pos, const glm::quat& ori, const glm::vec3& scale)
    : position_(pos)
    , orientation_(ori)
    , scale_(scale)
{
}

Transform::Transform(const glm::mat4& matrix)
{
    setMatrix(matrix);
}

const glm::vec3& Transform::getPosition() const
{
    return position_;
}

void Transform::setPosition(const glm::vec3& position)
{
    position_ = position;
    dirty_ = true;
}

void Transform::move(const glm::vec3& v)
{
    setPosition(position_ + v);
}

void Transform::moveLocal(const glm::vec3& v)
{
    move(orientation_ * v);
}

const glm::vec3& Transform::getScale() const
{
    return scale_;
}

void Transform::setScale(const glm::vec3& scale)
{
    scale_ = scale;
    dirty_ = true;
}

void Transform::scale(const glm::vec3& v)
{
    setScale(scale_ * v);
}

const glm::quat& Transform::getOrientation() const
{
    return orientation_;
}

void Transform::setOrientation(const glm::quat& orientation)
{
    // It is a requirement for some methods of this class that quaterion_ is normalized,
    // so we ensure it here
    orientation_ = glm::normalize(orientation);
    dirty_ = true;
}

void Transform::rotate(const glm::quat& v)
{
    setOrientation(v * orientation_);
}

void Transform::rotateLocal(const glm::quat& v)
{
    setOrientation(orientation_ * v);
}

void Transform::setMatrix(const glm::mat4& matrix)
{
    // glm::decompose is somehow incorrect
    assert(matrix[0][3] == 0.0f && matrix[1][3] == 0.0f && matrix[2][3] == 0.0f
        && matrix[3][3] == 1.0f);
    const auto mat3 = glm::mat3(matrix);
    assert(glm::determinant(mat3) != 0.0f);
    position_ = glm::vec3(matrix[3]);
    scale_ = glm::vec3(glm::length(mat3[0]), glm::length(mat3[1]), glm::length(mat3[2]));
    auto rot = glm::mat3(mat3[0] / scale_.x, mat3[1] / scale_.y, mat3[2] / scale_.z);
    if (glm::determinant(rot) < 0.0f) {
        scale_ *= -1.0f;
        rot *= -1.0f;
    }
    // All these epsilons have been tweaked to work with all the glTF sample models
    assert(std::fabs(glm::determinant(rot) - 1.0f) < 1e-6f);
    assert(glm::isIdentity(glm::transpose(rot) * rot, 1e-4f));
    orientation_ = glm::normalize(glm::quat_cast(rot));
    dirty_ = true;
    assert(glm::isNull(matrix - getMatrix(), 1e-4f));
}

const glm::mat4& Transform::getMatrix() const
{
    if (dirty_)
        matrix_ = glm::translate(position_) * glm::mat4_cast(orientation_) * glm::scale(scale_);
    return matrix_;
}

glm::vec3 Transform::localToWorld(const glm::vec3& vec) const
{
    return orientation_ * vec;
}

glm::vec3 Transform::getForward() const
{
    return localToWorld(glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Transform::getRight() const
{
    return localToWorld(glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Transform::getUp() const
{
    return localToWorld(glm::vec3(0.0f, 1.0f, 0.0f));
}

void Transform::lookAtPos(const glm::vec3& pos, const glm::vec3& at, const glm::vec3& up)
{
    position_ = pos;
    orientation_ = glm::normalize(glm::conjugate(glm::quat_cast(glm::lookAt(pos, at, up))));
    dirty_ = true;
}

void Transform::lookAt(const glm::vec3& at, const glm::vec3& up)
{
    lookAtPos(position_, at, up);
}
}
