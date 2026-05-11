#pragma once

#include <concepts>
#include <cstdint>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <tuple>
#include <type_traits>

namespace stdx::details {

template <typename T>
concept numeric = std::integral<T> || std::floating_point<T>;

template <typename T>
concept stringlike = std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>;

template <typename... Ts>
concept is_supported = ((std::is_same_v<Ts, int8_t> || std::is_same_v<Ts, int16_t> || std::is_same_v<Ts, int32_t> ||
                         std::is_same_v<Ts, int64_t> || std::is_same_v<Ts, uint8_t> || std::is_same_v<Ts, uint16_t> ||
                         std::is_same_v<Ts, uint32_t> || std::is_same_v<Ts, uint64_t> || std::is_same_v<Ts, float> ||
                         std::is_same_v<Ts, double> || stringlike<Ts>) &&
                        ...);

template <typename T>
constexpr bool is_compatible(std::string_view spec) {
    if (spec == "d")
        return std::integral<T>;

    if (spec == "u")
        return std::unsigned_integral<T>;

    if (spec == "f")
        return std::floating_point<T>;

    if (spec == "s")
        return stringlike<T>;

    if (spec == "")
        return true;

    return false;
}

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {
    std::tuple<Ts...> &values() { return m_values; }

private:
    std::tuple<Ts...> m_values;
};

}  // namespace stdx::details
