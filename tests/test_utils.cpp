#include <gtest/gtest.h>

#include "utils.h"

TEST(UtilsTest, FormatUptime) {
    EXPECT_EQ(utils::formatUptime(0.5), "0h 30m");
    EXPECT_EQ(utils::formatUptime(1.0), "1h 0m");
    EXPECT_EQ(utils::formatUptime(25.5), "1d 1h 30m");
    EXPECT_EQ(utils::formatUptime(48.25), "2d 0h 15m");
}

TEST(UtilsTest, ParseUint64) {
    uint64_t result;
    EXPECT_TRUE(utils::parseUint64("123", result));
    EXPECT_EQ(result, 123);
    
    EXPECT_TRUE(utils::parseUint64("0", result));
    EXPECT_EQ(result, 0);
    
    EXPECT_TRUE(utils::parseUint64("18446744073709551615", result)); // max uint64
    EXPECT_EQ(result, 18446744073709551615ULL);
    
    EXPECT_FALSE(utils::parseUint64("-123", result));
    EXPECT_FALSE(utils::parseUint64("abc", result));
    EXPECT_FALSE(utils::parseUint64("123abc", result));
}

TEST(UtilsTest, ParseDouble) {
    double result;
    EXPECT_TRUE(utils::parseDouble("123.45", result));
    EXPECT_DOUBLE_EQ(result, 123.45);
    
    EXPECT_TRUE(utils::parseDouble("0", result));
    EXPECT_DOUBLE_EQ(result, 0.0);
    
    EXPECT_TRUE(utils::parseDouble("-123.45", result));
    EXPECT_DOUBLE_EQ(result, -123.45);
    
    EXPECT_FALSE(utils::parseDouble("abc", result));
}

TEST(UtilsTest, Split) {
    auto parts = utils::split("a b c", ' ');
    ASSERT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
    
    parts = utils::split("", ' ');
    EXPECT_TRUE(parts.empty());
    
    parts = utils::split("single", ' ');
    ASSERT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], "single");
}

TEST(UtilsTest, Trim) {
    EXPECT_EQ(utils::trim("  hello  "), "hello");
    EXPECT_EQ(utils::trim("\t\n  world  \t\n"), "world");
    EXPECT_EQ(utils::trim("no trimming"), "no trimming");
    EXPECT_EQ(utils::trim(""), "");
    EXPECT_EQ(utils::trim("   "), "");
}

TEST(UtilsTest, ParseArguments) {
    bool continuous_mode;
    int interval;
    
    // Тест с пустыми аргументами (однократный запуск)
    const char* argv1[] = {"program"};
    EXPECT_TRUE(utils::parseArguments(1, const_cast<char**>(argv1), continuous_mode, interval));
    EXPECT_FALSE(continuous_mode);
    EXPECT_EQ(interval, 2);
    
    // Тест с continuous режимом
    const char* argv2[] = {"program", "-c"};
    EXPECT_TRUE(utils::parseArguments(2, const_cast<char**>(argv2), continuous_mode, interval));
    EXPECT_TRUE(continuous_mode);
    EXPECT_EQ(interval, 2);
    
    // Тест с continuous режимом и интервалом
    const char* argv3[] = {"program", "--continuous", "5"};
    EXPECT_TRUE(utils::parseArguments(3, const_cast<char**>(argv3), continuous_mode, interval));
    EXPECT_TRUE(continuous_mode);
    EXPECT_EQ(interval, 5);
    
    // Тест с отрицательным интервалом (должен вернуть false)
    const char* argv4[] = {"program", "-c", "-1"};
    EXPECT_FALSE(utils::parseArguments(3, const_cast<char**>(argv4), continuous_mode, interval));
}