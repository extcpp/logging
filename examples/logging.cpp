// Copyright - 2020 - Jan Christoph Uhde <Jan@UhdeJC.com>
// Please see LICENSE.md for license or visit https://github.com/extcpp/basics
#include <iostream>
#include <map>
#include <vector>

#ifndef _WIN32
#include <ext/logging.hpp>

struct test {
	test() {
		EXT_LOG("test class", error) << "hello from ctor";
	}
};

static test someclass{};

int main(/*int argc, const char *argv[]*/) {
    namespace el = ext::logging;

    using namespace ext::logging;
    el::configuration::function = false;
    el::configuration::append_newline = true;
    el::set_level_all(ext::logging::level::info);

    EXT_DEV << "foo";
    EXT_DEV_IF(true) << "bar";
    EXT_DEV_IF_CONST(false) << "foo";
    volatile bool v = false;
    (void) v;
    //EXT_DEV_IF_CONST(v) << "foo"; // if constexpr condition is not a constant expression
    EXT_DEV_VARIABLE << "bar";

	// clang-format off
    EXT_LOG("hi mic", warn) << "1" << "2" << "3";
	// clang-format on

    EXT_LOG("cafe") << "default level and topic";
    EXT_LOG("0000", trace) << "where is the byte gone";
    EXT_LOG("1111", network, debug) << "ohlala";
    EXT_LOG("2222", info) << "Hi there!";

    el::set_level_all(ext::logging::level::error);
    EXT_LOG("2222", info) << "Hi there!";
    EXT_LOG_CONST("3333", warn) << "something is wrong";
    EXT_LOG_CONST("3333", warn) << "something is wrong";

    EXT_LOG_CONST("4444", network, error, false) << "your network is broken";
    EXT_LOG_CONST("5555", network, error, true) << "your network is broken";
    EXT_LOG("music", network, warn) << "Green Day";
    EXT_LOG("6666", fatal) << "your app will terminate";

#else
int main(/*int argc, const char *argv[]*/) {
#endif // _WIN32
    return 0;
}
