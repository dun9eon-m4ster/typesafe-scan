#include <gtest/gtest.h>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
    auto result = stdx::scan<std::string_view, double, uint>("\"number\" 3.14 1000000", "{s} {f} {d}");
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