#pragma once

#include <filesystem>
#include <optional>

#include <fmt/core.h>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

template <>
struct fmt::formatter<std::filesystem::path> {
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const std::filesystem::path& path, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "{}", path.c_str());
    }
};

template <typename T>
struct fmt::formatter<std::optional<T>> {
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const std::optional<T>& opt, FormatContext& ctx)
    {
        if (opt)
            return format_to(ctx.out(), "{}", *opt);
        return format_to(ctx.out(), "nullopt");
    }
};

template <glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>> {
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const glm::vec<L, T, Q>& v, FormatContext& ctx)
    {
        return format_to(ctx.out(), glm::to_string(v));
    }
};

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<C, R, T, Q>> {
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const glm::mat<C, R, T, Q>& m, FormatContext& ctx)
    {
        return format_to(ctx.out(), glm::to_string(m));
    }
};

template <typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> {
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const glm::qua<T, Q>& q, FormatContext& ctx)
    {
        return format_to(ctx.out(), "quat({}, ({}, {}, {}))", q.w, q.x, q.y, q.z);
    }
};
