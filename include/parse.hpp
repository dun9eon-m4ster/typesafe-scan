#pragma once

#include "types.hpp"
#include <charconv>
#include <expected>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace stdx::details {

template <typename T, conversion_specifier spec>
std::expected<T, scan_error> parse_helper(std::string_view string) {
    if constexpr (!is_compatable_v<T, spec>)
        return std::unexpected<scan_error>{"type and conversion specifier are not compatable"};

    T converted;
    auto result = std::from_chars(string.data(), string.data() + string.size(), converted);
    if (result.ec != std::error_code{})
        return std::unexpected<scan_error>{"from chars error: " + std::make_error_code(result.ec).message()};

    return converted;
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    auto specifier = cs_from_string(fmt);

    switch (specifier) {
    case conversion_specifier::empty: {
        return parse_helper<T, conversion_specifier::empty>(input);
    }    
    case conversion_specifier::d: {
        return parse_helper<T, conversion_specifier::d>(input);
    }
    case conversion_specifier::u: {
        return parse_helper<T, conversion_specifier::u>(input);
    }
    case conversion_specifier::f: {
        return parse_helper<T, conversion_specifier::f>(input);
    }
    case conversion_specifier::unknown:
        return std::unexpected<scan_error>("unknown specifier");
    default:
        return std::unexpected<scan_error>("invalid specifier");
    }

    return std::unexpected<scan_error>("not handled");
}

template <typename T>
    requires(is_string_v<T>)
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    auto specifier = cs_from_string(fmt);
    if (specifier != conversion_specifier::s && specifier != conversion_specifier::empty)
        return std::unexpected<scan_error>("Invalid string specifier");

    if (input.size() < 2)
        return std::unexpected<scan_error>("Invalid string format");
    if (input[0] != '\"' || input[input.size() - 1] != '\"')
        return std::unexpected<scan_error>("Missing quotes in string");

    return T{input.substr(1, input.size() - 2)};
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и текущей { есть текст,
        // проверяем его наличие во входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    // Проверяем оставшийся текст после последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

}  // namespace stdx::details