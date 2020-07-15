#include <limits>

#include <glm/glm.hpp>

namespace glwx {
struct Aabb {
    glm::vec3 min = glm::vec3(HUGE_VALF);
    glm::vec3 max = glm::vec3(-HUGE_VALF);

    void fit(const glm::vec3& point);
    void fit(const Aabb& other);

    bool empty() const;

    bool valid() const;

    glm::vec3 center() const;
    glm::vec3 extent() const;
    glm::vec3 size() const;

    bool contains(const glm::vec3& point) const;
    bool overlaps(const Aabb& other) const;

    Aabb transformed(const glm::mat4& mat) const;
};
}
