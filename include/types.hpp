#pragma once

#include <concepts>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <tuple>
#include <type_traits>

namespace stdx::details {

template <typename T>
struct is_string {
    static constexpr bool value = std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>;
};

template <typename T>
static constexpr bool is_string_v = is_string<T>::value;

template <typename... Ts>
struct is_supported {
    static constexpr bool value =
        ((std::is_same_v<Ts, int8_t> || std::is_same_v<Ts, int16_t> || std::is_same_v<Ts, int32_t> ||
          std::is_same_v<Ts, int64_t> || std::is_same_v<Ts, uint8_t> || std::is_same_v<Ts, uint16_t> ||
          std::is_same_v<Ts, uint32_t> || std::is_same_v<Ts, uint64_t> || std::is_same_v<Ts, float> ||
          std::is_same_v<Ts, double> || is_string_v<Ts>) &&
         ...);
};

template <typename... Ts>
static constexpr bool is_supported_v = is_supported<Ts...>::value;

template <typename T>
constexpr bool is_compatible(std::string_view spec) {
    if (spec == "d")
        return std::numeric_limits<T>::is_integer;

    if (spec == "u")
        return std::numeric_limits<T>::is_integer && !std::is_signed_v<T>;

    if (spec == "f")
        return std::is_floating_point_v<T>;

    if (spec == "s")
        return is_string_v<T>;

    if (spec == "")
        return true;

    return false;
}

template <typename T>
concept numeric = std::integral<T> || std::floating_point<T>;

template <typename T>
concept stringlike = is_string_v<T>;

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
