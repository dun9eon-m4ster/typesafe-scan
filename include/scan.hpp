#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <cstddef>
#include <expected>
#include <format>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace stdx {

template <std::size_t I = 0, typename... Ts>
std::expected<void, details::scan_error> parse_into(std::tuple<Ts...> &tuple,
                                                    const std::vector<std::string_view> &formats,
                                                    const std::vector<std::string_view> &values) {
    if constexpr (I == sizeof...(Ts)) {
        return {};
    } else {
        using T = std::tuple_element_t<I, std::tuple<Ts...>>;

        auto parsed = details::parse_value_with_format<T>(values[I], formats[I]);

        if (!parsed) {
            auto error_msg = std::format("{}  {}  {}", std::string("Error occurred on type number: "),
                                         std::to_string(I + 1), std::string(parsed.error().message));
            return std::unexpected<details::scan_error>(error_msg);
        }

        std::get<I>(tuple) = std::move(*parsed);

        return parse_into<I + 1>(tuple, formats, values);
    }
}

// замените болванку функции scan на рабочую версию
template <typename... Ts>
    requires(details::is_supported_v<std::remove_cv_t<Ts>...>)
std::expected<details::scan_result<std::remove_cv_t<Ts>...>, details::scan_error> scan(std::string_view input,
                                                                                       std::string_view format) {
    auto src = details::parse_sources<std::remove_cv_t<Ts>...>(input, format);
    if (!src.has_value())
        return std::unexpected<details::scan_error>("parse sources error");

    const auto &value = src.value();
    if (value.first.size() != value.second.size())
        return std::unexpected<details::scan_error>("format & input size diff");
    size_t size = value.first.size();
    if (size != sizeof...(Ts))
        return std::unexpected<details::scan_error>("params count doesn't match with args count");

    details::scan_result<std::remove_cv_t<Ts>...> result;

    auto pre_result = parse_into(result.values(), value.first, value.second);
    if (!pre_result)
        return std::unexpected(pre_result.error());

    return result;
}

}  // namespace stdx
