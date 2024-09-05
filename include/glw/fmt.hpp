#pragma once

#include <fmt/format.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace detail {
// avoid including <algorithm> (which is HUGE)
template <typename Out, typename In>
auto copy_n(Out out, In in, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        *out++ = *in++;
    }
    return out;
}
}

template <glm::length_t L, typename T>
struct fmt::formatter<glm::vec<L, T>> : fmt::formatter<T> {
    auto format(const glm::vec<L, T>& v, format_context& ctx) const
    {
        auto out = ::detail::copy_n(ctx.out(), "vec", 3);
        *out++ = '0' + L;
        *out++ = '(';
        for (glm::length_t i = 0; i < L; ++i) {
            if (i > 0) {
                out = ::detail::copy_n(out, ", ", 2);
            }
            out = formatter<T>::format(v[i], ctx);
        }
        *out++ = ')';
        return out;
    }
};

template <typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> : fmt::formatter<T> {
    auto format(const glm::qua<T, Q>& q, format_context& ctx) const
    {
        auto out = ::detail::copy_n(ctx.out(), "quat(", 5);
        out = formatter<T>::format(q.w, ctx);
        out = ::detail::copy_n(out, ", {", 3);
        out = formatter<T>::format(q.x, ctx);
        out = ::detail::copy_n(out, ", ", 2);
        out = formatter<T>::format(q.y, ctx);
        out = ::detail::copy_n(out, ", ", 2);
        out = formatter<T>::format(q.z, ctx);
        out = ::detail::copy_n(out, "})", 2);
        return out;
    }
};

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<C, R, T, Q>> : fmt::formatter<T> {
    auto format(const glm::mat<C, R, T, Q>& m, format_context& ctx) const
    {
        auto out = ::detail::copy_n(ctx.out(), "mat", 3);
        *out++ = '0' + C;
        if (C != R) {
            *out++ = 'x';
            *out++ = '0' + R;
        }
        *out++ = '(';
        for (size_t c = 0; c < C; ++c) {
            if (c > 0) {
                out = ::detail::copy_n(out, ", ", 2);
            }
            *out++ = '(';
            for (size_t r = 0; r < R; ++r) {
                if (r > 0) {
                    out = ::detail::copy_n(out, ", ", 2);
                }
                out = formatter<T>::format(m[c][r], ctx);
            }
            *out++ = ')';
        }
        *out++ = ')';
        return out;
    }
};