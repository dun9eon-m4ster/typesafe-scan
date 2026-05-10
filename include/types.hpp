#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <tuple>
#include <type_traits>

namespace stdx::details {

enum class conversion_specifier { unknown, empty, d, s, u, f };

constexpr conversion_specifier cs_from_string(std::string_view string) {
    if (string == "")
        return conversion_specifier::empty;
    if (string == "d")
        return conversion_specifier::d;
    if (string == "s")
        return conversion_specifier::s;
    if (string == "u")
        return conversion_specifier::u;
    if (string == "f")
        return conversion_specifier::f;

    return conversion_specifier::unknown;
}

template <typename T, conversion_specifier spec>
struct is_compatable {
    static constexpr bool value = false;
};

template <typename T, conversion_specifier spec>
static constexpr bool is_compatable_v = is_compatable<T, spec>::value;

template <typename T>
struct is_compatable<T, conversion_specifier::d> {
    static constexpr bool value = std::numeric_limits<T>::is_integer;
};

template <typename T>
struct is_compatable<T, conversion_specifier::u> {
    static constexpr bool value = std::numeric_limits<T>::is_integer && !std::is_signed_v<T>;
};

template <typename T>
struct is_compatable<T, conversion_specifier::f> {
    static constexpr bool value = std::is_floating_point_v<T>;
};

template <typename T>
struct is_compatable<T, conversion_specifier::s> {
    static constexpr bool value = std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>;
};

template <typename T>
struct is_compatable<T, conversion_specifier::empty> {
    static constexpr bool value = true;
};

template <typename... Ts>
struct is_supported {
    static constexpr bool value =
        ((std::is_same_v<Ts, int8_t> || std::is_same_v<Ts, int16_t> || std::is_same_v<Ts, int32_t> ||
          std::is_same_v<Ts, int64_t> || std::is_same_v<Ts, uint8_t> || std::is_same_v<Ts, uint16_t> ||
          std::is_same_v<Ts, uint32_t> || std::is_same_v<Ts, uint64_t> || std::is_same_v<Ts, float> ||
          std::is_same_v<Ts, double> || std::is_same_v<Ts, std::string> || std::is_same_v<Ts, std::string_view>) &&
         ...);
};

template <typename... Ts>
static constexpr bool is_supported_v = is_supported<Ts...>::value;

template <typename T>
struct is_string {
    static constexpr bool value = std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>;
};

template <typename T>
static constexpr bool is_string_v = is_string<T>::value;

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
