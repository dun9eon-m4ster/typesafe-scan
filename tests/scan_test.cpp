#include <gtest/gtest.h>
#include <print>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
    auto result = stdx::scan<std::string_view, double, uint>("\"number\" 3.14 1000000", "{s} {f} {d}");

    if (!result.has_value())
        std::println("Error: {}", result.error().message);
    else
        std::apply(
            [](auto &&...args) {
                int i = 0;
                (std::println("parse_result {}: {}", i++, args), ...);
            },
            result->values());
    EXPECT_TRUE(result);
}

TEST(ScanTest, ArgumentCountMatchTest) {
    {
        auto result = stdx::scan<int, int, int>("1 2 3 4 5", "{d} {d} {d} {d} {d}");
        EXPECT_FALSE(result);
    }

    {
        auto result = stdx::scan<int, int, int>("1 2", "{d} {d}");
        EXPECT_FALSE(result);
    }

    {
        auto result = stdx::scan<int, int, int>("1 2 3", "{d} {d} {d}");
        EXPECT_TRUE(result);
    }
}

TEST(ScanTest, InputAndFormatSpacing) {
    {
        auto result = stdx::scan<int, int, int>("1 2  3", "{d} {d} {d}");
        EXPECT_FALSE(result);
    }

    {
        auto result = stdx::scan<int, int, int>("1 2 3", "{d} {d}  {d}");
        EXPECT_FALSE(result);
    }

    {
        auto result = stdx::scan<int, int, int>("1 2 3", "{d} {d} {d}");
        EXPECT_TRUE(result);
    }
}

TEST(ScanTest, CorrectSpecifier) {
    {
        auto result = stdx::scan<int, std::string, double>("1 \"str\" 3.14", "{d} {d} {f}");
        EXPECT_FALSE(result);
    }

    {
        auto result = stdx::scan<int, std::string, double>("1 \"str\" 3.14", "{d} {s} {f}");
        EXPECT_TRUE(result);
    }

    {
        auto result = stdx::scan<int, std::string, double>("1 \"str\" 3.14", "{d} {} {f}");
        EXPECT_TRUE(result);
    }

    {
        auto result = stdx::scan<int, std::string, double>("1 \"str\" 3.14", "{} {} {}");
        EXPECT_TRUE(result);
    }

    {
        auto result = stdx::scan<int, double, uint>("1 2 3", "{} {d} {u}");
        EXPECT_FALSE(result);
    }

    {
        auto result = stdx::scan<uint, uint, uint>("1 2 3", "{} {d} {u}");
        EXPECT_TRUE(result);
    }
}

TEST(ScanTest, ResultAndParamsTypeMatch) {

    auto result = stdx::scan<int, uint, double, std::string_view>("-42 42 3.14 \"str\"", "{} {} {} {}");
    EXPECT_TRUE(result);

    using ResultType = std::tuple<int, uint, double, std::string_view>;

    EXPECT_TRUE((std::is_same_v<int, std::tuple_element_t<0, ResultType>>));
    EXPECT_TRUE((std::is_same_v<uint, std::tuple_element_t<1, ResultType>>));
    EXPECT_TRUE((std::is_same_v<double, std::tuple_element_t<2, ResultType>>));
    EXPECT_TRUE((std::is_same_v<std::string_view, std::tuple_element_t<3, ResultType>>));
}

TEST(ScanTest, ExpectedResultValues) {

    auto result = stdx::scan<int, uint, double, float, std::string_view, std::string>(
        "-42 42 3.14 9.86 \"string_view\" \"string\"", "{} {} {} {} {} {}");
    EXPECT_TRUE(result);

    EXPECT_EQ(std::get<0>(result->values()), -42);
    EXPECT_EQ(std::get<1>(result->values()), 42);
    EXPECT_TRUE(std::abs(std::get<2>(result->values()) - 3.14) < 1e+6);
    EXPECT_TRUE(std::abs(std::get<3>(result->values()) - 9.86) < 1e+6);
    EXPECT_EQ(std::get<4>(result->values()), std::string_view("string_view"));
    EXPECT_EQ(std::get<5>(result->values()), std::string_view("string"));
}

TEST(ScanTest, ConstTypeTest) {
    auto result = stdx::scan<const int>("10", "{}");
    EXPECT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values()), 10);
}