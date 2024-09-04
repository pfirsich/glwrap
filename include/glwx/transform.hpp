#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace glwx {
class Transform {
public:
    Transform() = default;
    Transform(const Transform&) = default;
    Transform(Transform&&) = default;
    Transform(const glm::vec3& pos);
    Transform(const glm::vec3& pos, const glm::quat& ori);
    Transform(const glm::vec3& pos, const glm::quat& ori, const glm::vec3& scale);
    Transform(const glm::mat4& matrix);

    Transform& operator=(const Transform&) = default;
    Transform& operator=(Transform&&) = default;

    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);
    void move(const glm::vec3& v);
    void moveLocal(const glm::vec3& v);

    const glm::vec3& getScale() const;
    void setScale(float scale);
    void setScale(const glm::vec3& scale);
    void scale(float scale);
    void scale(const glm::vec3& v);

    const glm::quat& getOrientation() const;
    void setOrientation(const glm::quat& orientation);
    void rotate(const glm::quat& v);
    void rotateLocal(const glm::quat& v);

    void setMatrix(const glm::mat4& matrix);
    const glm::mat4& getMatrix() const;

    // for direction vectors
    glm::vec3 localToWorld(const glm::vec3& vec) const;

    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;

    // This works like gluLookAt, so you may pass `up` in world space.
    // This might not work sometimes, but it makes things a lot easier most of the time.
    void lookAtPos(const glm::vec3& pos, const glm::vec3& at,
        const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

    void lookAt(const glm::vec3& at, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

private:
    glm::vec3 position_ { 0.0f, 0.0f, 0.0f };
    glm::quat orientation_ { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 scale_ { 1.0f, 1.0f, 1.0f };
    mutable glm::mat4 matrix_ { 1.0f };
    bool dirty_ { true };
};
}
