#pragma once

#include <stack>
#include <vector>

#include <glm/glm.hpp>

namespace glwx {

// It seems glm does not have mat3 transform functions, so I do it myself.
// This is different structurally from Transform (3D) in a way that I deem more useful for 2D
// applications (mostly games).
class Transform2D {
public:
    Transform2D() = default;

    Transform2D(float a, float b, float c, float d, float x = 0.0f, float y = 0.0f);

    // T * R * S * O
    Transform2D(const glm::vec2& position, float rotation = 0.0f,
        const glm::vec2& scale = glm::vec2(1.0f, 1.0f),
        const glm::vec2& offset = glm::vec2(0.0f, 0.0f));

    void reset();
    void translate(const glm::vec2& v);
    void scale(const glm::vec2& v);
    void rotate(float angle);
    void apply(const Transform2D& other);

    glm::vec2 transformDirection(const glm::vec2& dir) const;
    glm::vec2 transformPoint(const glm::vec2& point) const;

    glm::mat3 getMatrix() const;

private:
    /*
     * a  b  x
     * c  d  y
     * 0  0  1
     */
    float a = 1.0f, b = 0.0f, c = 0.0f, d = 1.0f;
    float x = 0.0f, y = 0.0f;
};

class Transform2DStack {
    Transform2DStack()
    {
        stack_.push(Transform2D {});
    }

    void push()
    {
        stack_.push(stack_.top());
    }

    void pop()
    {
        assert(stack_.size() > 1);
        stack_.pop();
    }

    operator Transform2D&()
    {
        return stack_.top();
    }

private:
    std::stack<Transform2D, std::vector<Transform2D>> stack_;
};

}
