// Copyright - 2016-2020 - Jan Christoph Uhde <Jan@UhdeJC.com>
// Please see LICENSE.md for license or visit https://github.com/extcpp/basics
#include <cstring>
#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>

#include <ext/macros/platform.hpp>
#include <ext/util/except.hpp>
#include <ext/util/files.hpp>

#define EXT_LOGGING_DEFAULT_LEVEL warn
#include <ext/logging.hpp>


using namespace std::literals;

struct LoggingTest : public ::testing::Test {
    LoggingTest() : _log{"test.log", std::ios::trunc} {
        using namespace ext::logging;
        configuration::stream = &_log;
        configuration::gdb = false;
        configuration::vim = false;
        configuration::prefix_newline = false;
        configuration::append_newline = true;
        set_level_all(level::EXT_LOGGING_DEFAULT_LEVEL);
    };


    void compare(std::string const& expected = ""){
        _log.close();
        std::ifstream file{"test.log"};
        std::istreambuf_iterator<char> eos;
        std::string logged{std::istreambuf_iterator<char>(file), eos};
        ASSERT_EQ(expected, logged);
    }

    std::string path() {
        auto path = std::filesystem::current_path().parent_path()
                                              .parent_path()
                                              .parent_path()
                                              / "logging" / "tests" / "logging.cpp";
        return path.string();
    };

    std::string line() {
        return std::to_string(_line);
    }

    std::ofstream _log;
    std::size_t _line;

};
using LoggingDeathTest = LoggingTest;

TEST_F(LoggingTest, logging_no_crash_gdb_vim) {
    using namespace ext::logging;
    configuration::gdb = true;
    configuration::vim = true;

    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("babe") << "cafe?");

    compare(
        "# vim "s + path() + " +" + line() +"\n"
        "# break logging.cpp:" + line() + "\n"
        "[babe] warning in TestBody(): 'cafe?'\n"
    );
}

TEST_F(LoggingTest, logging_no_crash) {
    using namespace ext::logging;
    configuration::prefix_newline = true;
    configuration::append_newline = false;

    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("babe") << "2cafe?");
    compare("\n[babe] warning logging.cpp:" + line() + " in TestBody(): '2cafe?'");
}

TEST_F(LoggingTest, logging_level_too_low) {
    ext::logging::topic::network.activation_level = ext::logging::level::error;
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("music", network, warn) << "Green" << "Day");
    compare("");
}

TEST_F(LoggingTest, logging_level_ok) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("music", network, error) << "Green" << " Day");
    compare("[music] error (network) logging.cpp:" + line() +" in TestBody(): 'Green Day'\n");
}

// does not die
#ifndef EXT_COMPILER_VC
TEST_F(LoggingDeathTest, fatal) {
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    using namespace ext::logging;
    ASSERT_DEATH_IF_SUPPORTED(EXT_LOG("work", network, fatal) << "What?!?! No Cafe!?!?!? :(", "");
}
#endif // EXT_COMPILER_VC

TEST_F(LoggingTest, change_all_levels) {
    using namespace ext::logging;

    EXPECT_TRUE(_detail::level_is_active(level::error));
    EXPECT_TRUE(_detail::level_is_active(level::warn));
    EXPECT_FALSE(_detail::level_is_active(level::info));
    EXPECT_FALSE(_detail::level_is_active(level::trace));

    // fix compiled in
    EXPECT_TRUE(_detail::default_level_is_active(level::error));
    EXPECT_TRUE(_detail::default_level_is_active(level::warn));
    EXPECT_FALSE(_detail::default_level_is_active(level::info));
    EXPECT_FALSE(_detail::default_level_is_active(level::trace));

    set_level_all(level::error);

    EXPECT_TRUE(_detail::level_is_active(level::error));
    EXPECT_FALSE(_detail::level_is_active(level::warn));
    EXPECT_FALSE(_detail::level_is_active(level::info));
    EXPECT_FALSE(_detail::level_is_active(level::trace));

    // fix compiled in
    EXPECT_TRUE(_detail::default_level_is_active(level::error));
    EXPECT_TRUE(_detail::default_level_is_active(level::warn));
    EXPECT_FALSE(_detail::default_level_is_active(level::info));
    EXPECT_FALSE(_detail::default_level_is_active(level::trace));
}

TEST_F(LoggingTest, change_single_level) {
    using namespace ext::logging;
    EXPECT_FALSE(_detail::level_is_active(level::info, topic::network));
    topic::network.activation_level = level::info;
    EXPECT_TRUE(_detail::level_is_active(level::info, topic::network));
}

TEST_F(LoggingTest, levels_to_string) {
    using namespace ext::logging;
    EXPECT_STREQ(_detail::level_to_str(level::fatal).c_str(), "fatal");
    EXPECT_STREQ(_detail::level_to_str(level::error).c_str(), "error");
    EXPECT_STREQ(_detail::level_to_str(level::warn).c_str(), "warning");
    EXPECT_STREQ(_detail::level_to_str(level::info).c_str(), "info");
    EXPECT_STREQ(_detail::level_to_str(level::debug).c_str(), "debug");
    EXPECT_STREQ(_detail::level_to_str(level::trace).c_str(), "trace");
    EXPECT_STREQ(_detail::level_to_str(static_cast<level>(1337)).c_str(), "unknown");
}
