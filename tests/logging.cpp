// Copyright - 2016-2020 - Jan Christoph Uhde <Jan@UhdeJC.com>
// Please see LICENSE.md for license or visit https://github.com/extcpp/basics
#include <cstring>
#include <filesystem>
#include <sstream>

#include <gtest/gtest.h>

#include <ext/macros/platform.hpp>
#include <ext/util/except.hpp>
#include <ext/util/files.hpp>

#define EXT_LOGGING_DEFAULT_LEVEL warn
#include <ext/logging.hpp>


using namespace std::literals;

struct LoggingTest : public ::testing::Test {
    LoggingTest() : _log{} {
        using namespace ext::logging;
        configuration::stream = &_log;
#ifdef EXT_LOGGING_ENABLE_VIM_GDB
        configuration::gdb = false;
        configuration::vim = false;
#endif
        configuration::prefix_newline = false;
        configuration::append_newline = true;
        configuration::filename = true;
        configuration::function = true;
        set_level_all(level::EXT_LOGGING_DEFAULT_LEVEL);
    };


    void compare(std::string const& expected = "") {
        ASSERT_EQ(expected, _log.str());
    }

    std::string path() {
        auto path = std::filesystem::current_path().parent_path().parent_path().parent_path() / "logging" / "tests" /
                    "logging.cpp";
        return path.string();
    };

    std::string line() {
        return std::to_string(_line);
    }

    std::stringstream _log;
    std::size_t _line;
};
using LoggingDeathTest = LoggingTest;

TEST_F(LoggingTest, logging_dev) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_DEV << "development");
    compare("[@@@@] warning (development) logging.cpp:" + line() + " in TestBody(): 'development'\n");
}

TEST_F(LoggingTest, logging_dev_if_static) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_DEV_IF_CONST(true) << "development");
    compare("[@@@@] warning (development) logging.cpp:" + line() + " in TestBody(): 'development'\n");
}

TEST_F(LoggingTest, logging_dev_variable) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_DEV_VARIABLE << "development");
    compare("[$$$$] warning (development) logging.cpp:" + line() + " in TestBody(): 'development'\n");
}

TEST_F(LoggingTest, logging_dev_if) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_DEV_IF(true) << "development");
    compare("[$$$$] warning (development) logging.cpp:" + line() + " in TestBody(): 'development'\n");
}

TEST_F(LoggingTest, logging_default) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("badcafe") << "2 Cafe " << "NOHA");
    compare("[badcafe] warning logging.cpp:" + line() + " in TestBody(): '2 Cafe NOHA'\n");
}

TEST_F(LoggingTest, logging_no_filename) {
    ext::logging::configuration::filename = false;
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("badcafe") << "2 Cafe " << "NOHA");
    compare("[badcafe] warning in TestBody(): '2 Cafe NOHA'\n");
}

TEST_F(LoggingTest, logging_no_function) {
    ext::logging::configuration::function = false;
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("badcafe") << "2 Cafe " << "NOHA");
    compare("[badcafe] warning logging.cpp:" + line() + ": '2 Cafe NOHA'\n");
}

TEST_F(LoggingTest, logging_no_file_no_function) {
    ext::logging::configuration::filename = false;
    ext::logging::configuration::function = false;
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("badcafe") << "2 Cafe " << "NOHA");
    compare("[badcafe] warning: '2 Cafe NOHA'\n");
}

TEST_F(LoggingTest, logging_id_level) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("log-id", error) << "Where is Pierre?");
    compare("[log-id] error logging.cpp:" + line() + " in TestBody(): 'Where is Pierre?'\n");
}

TEST_F(LoggingTest, logging_id_topic_level) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("log-id", network, error) << "Where is Pierre?");
    compare("[log-id] error (network) logging.cpp:" + line() + " in TestBody(): 'Where is Pierre?'\n");
}

TEST_F(LoggingTest, logging_id_topic_level_condition_true) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("log-id", network, error, true) << "Where is Pierre?");
    compare("[log-id] error (network) logging.cpp:" + line() + " in TestBody(): 'Where is Pierre?'\n");
}

TEST_F(LoggingTest, logging_static_id_topic_level_condition_true) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG_CONST("log-id", network, error, true) << "Where is Pierre?");
    compare("[log-id] error (network) logging.cpp:" + line() + " in TestBody(): 'Where is Pierre?'\n");
}

TEST_F(LoggingTest, logging_id_topic_level_condition_false) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("log-id", network, error, false) << "Where is Pierre?");
    compare("");
}

TEST_F(LoggingTest, logging_level_too_low_1) {
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("id", info) << "will not be logged");
    compare("");
}

TEST_F(LoggingTest, logging_level_too_low_2) {
    ext::logging::topic::network.activation_level = ext::logging::level::error;
    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("music", network, warn) << "will not be logged");
    compare("");
}

TEST_F(LoggingTest, logging_gdb_vim) {
    using namespace ext::logging;
#ifdef EXT_LOGGING_ENABLE_VIM_GDB
    configuration::gdb = true;
    configuration::vim = true;
#endif

    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("babe") << "cafe?");

    compare(
#ifdef EXT_LOGGING_ENABLE_VIM_GDB
        "# vim "s + path() + " +" + line() + "\n"
        "# break logging.cpp:" + line() + "\n"
#endif
        "[babe] warning logging.cpp:"s +
        line() + " in TestBody(): 'cafe?'\n");
}

TEST_F(LoggingTest, logging_prepend_newline) {
    using namespace ext::logging;
    configuration::prefix_newline = true;
    configuration::append_newline = false;

    _line = __LINE__ + 1;
    ASSERT_NO_THROW(EXT_LOG("babe") << "2cafe?");
    compare("\n[babe] warning logging.cpp:" + line() + " in TestBody(): '2cafe?'");
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

    EXPECT_TRUE(_detail::variable_level_is_active(level::error));
    EXPECT_TRUE(_detail::variable_level_is_active(level::warn));
    EXPECT_FALSE(_detail::variable_level_is_active(level::info));
    EXPECT_FALSE(_detail::variable_level_is_active(level::trace));

    // fix compiled in
    EXPECT_TRUE(_detail::constexpr_level_is_active(level::error));
    EXPECT_TRUE(_detail::constexpr_level_is_active(level::warn));
    EXPECT_FALSE(_detail::constexpr_level_is_active(level::info));
    EXPECT_FALSE(_detail::constexpr_level_is_active(level::trace));

    set_level_all(level::error);

    EXPECT_TRUE(_detail::variable_level_is_active(level::error));
    EXPECT_FALSE(_detail::variable_level_is_active(level::warn));
    EXPECT_FALSE(_detail::variable_level_is_active(level::info));
    EXPECT_FALSE(_detail::variable_level_is_active(level::trace));

    // fix compiled in
    EXPECT_TRUE(_detail::constexpr_level_is_active(level::error));
    EXPECT_TRUE(_detail::constexpr_level_is_active(level::warn));
    EXPECT_FALSE(_detail::constexpr_level_is_active(level::info));
    EXPECT_FALSE(_detail::constexpr_level_is_active(level::trace));
}

TEST_F(LoggingTest, change_single_level) {
    using namespace ext::logging;
    EXPECT_FALSE(_detail::variable_level_is_active(level::info, topic::network));
    topic::network.activation_level = level::info;
    EXPECT_TRUE(_detail::variable_level_is_active(level::info, topic::network));
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
