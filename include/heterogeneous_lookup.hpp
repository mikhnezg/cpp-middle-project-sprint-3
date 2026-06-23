#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = void;

    bool operator()(std::string_view a, std::string_view b) const noexcept { return a < b; }
    bool operator()(const std::string &a, std::string_view b) const noexcept { return std::string_view(a) < b; }
    bool operator()(std::string_view a, const std::string &b) const noexcept { return a < std::string_view(b); }
    bool operator()(const std::string &a, const std::string &b) const noexcept { return a < b; }
};

struct TransparentStringEqual {
    using is_transparent = void;

    bool operator()(std::string_view a, std::string_view b) const noexcept { return a == b; }
    bool operator()(const std::string &a, std::string_view b) const noexcept { return std::string_view(a) == b; }
    bool operator()(std::string_view a, const std::string &b) const noexcept { return a == std::string_view(b); }
    bool operator()(const std::string &a, const std::string &b) const noexcept { return a == b; }
};

struct TransparentStringHash {
    using is_transparent = void;

    std::size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
    std::size_t operator()(const std::string &s) const noexcept { return std::hash<std::string_view>{}(s); }
};

}  // namespace bookdb
