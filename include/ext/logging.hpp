// Copyright - 2016-2020 - Jan Christoph Uhde <Jan@UhdeJC.com>
// Please see LICENSE.md for license or visit https://github.com/extcpp/basics

// Logging:
//
// There are topics and levels. Topics provide names and contain some extra
// information. Levels range from 0 (fatal) to 100 (trace) and are used when
// writing a log line.
//
// Furthermore topics contain an internal activation level. When a level is
// passed into the macro below then this `macro_level_` is checked  against the
// activation level. If the `topic::activation_level` is greater or equal than
// the `macro_level_`, then a log message is emitted.
//
// Logtopics, a mutex and a topic_map are automatically created using compiler
// specific functionality in order to avoid the static initialisation fiasco.
//
//
// Usage
//  EXT_LOG("cafe", info) << "hi there";
//  EXT_LOG("babe", network, error) << "your network is broken";
//  EXT_LOG("2bad", fatal) << "your app will terminate";

#ifndef EXT_LOGGING_HEADER
#define EXT_LOGGING_HEADER
#include <ext/logging/functionality.hpp>
#include <ext/macros/compiler.hpp>
#include <iostream>
#include <type_traits>

// helper
#define eXT_LOG_SELECT5TH_PARAMETER(_1, _2, _3, _4, NAME, ...) NAME

// __VA_ARGS__ do not expand correctly when using VisualStudio compiler :(
// https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly
#define eXT_LOG_EXPAND(x) x

// variable logging - if() ...
#define eXT_LOG_INTERNAL(id_, topic_, macro_level_, cond_)                                 \
    if (ext::logging::_detail::variable_level_is_active((macro_level_), (topic_)) && cond_) \
    ext::logging::_detail::logger(id_, (topic_), (macro_level_), __FILE__, __LINE__, __FUNCTION__)

#define eXT_LOG_INTERNAL_ADD_PREFIX(id_, topic_, macro_level_, cond_) \
    eXT_LOG_INTERNAL(id_, (ext::logging::topic::topic_), (ext::logging::level::macro_level_), cond_)

#define EXT_LOGVARIABLE4(id, topic_, macro_level_, cond_) eXT_LOG_INTERNAL_ADD_PREFIX(id, topic_, macro_level_, cond_)
#define EXT_LOGVARIABLE3(id, topic_, macro_level_) eXT_LOG_INTERNAL_ADD_PREFIX(id, topic_, macro_level_, true)
#define EXT_LOGVARIABLE2(id, macro_level_) eXT_LOG_INTERNAL_ADD_PREFIX(id, no_topic, macro_level_, true)
#define EXT_LOGVARIABLE1(id) eXT_LOG_INTERNAL_ADD_PREFIX(id, no_topic, EXT_LOGGING_DEFAULT_LEVEL, true)

// 1st __VA_ARGS__ shifts the args into the correct position
// macro can not be empty because of the leading `,` (fixed with __VA_OPT__ in c++20)
#ifdef EXT_COMPILER_VC
    #define EXT_LOGVARIABLE(...)                                                                                    \
        eXT_LOG_SELECT5TH_PARAMETER(                                                                               \
            eXT_LOG_EXPAND(__VA_ARGS__), EXT_LOGVARIABLE4, EXT_LOGVARIABLE3, EXT_LOGVARIABLE2, EXT_LOGVARIABLE1, ) \
        (eXT_LOG_EXPAND(__VA_ARGS__))
#else
    #define EXT_LOGVARIABLE(...)                                                                   \
        eXT_LOG_SELECT5TH_PARAMETER(                                                              \
            __VA_ARGS__, EXT_LOGVARIABLE4, EXT_LOGVARIABLE3, EXT_LOGVARIABLE2, EXT_LOGVARIABLE1, ) \
        (__VA_ARGS__)
#endif // EXT_COMPILER_VC

#define EXT_DEV_VARIABLE eXT_LOG_INTERNAL_ADD_PREFIX("$$$$", dev, EXT_LOGGING_DEFAULT_LEVEL, true)
#define EXT_DEV_IF(cond_) eXT_LOG_INTERNAL_ADD_PREFIX("$$$$", dev, EXT_LOGGING_DEFAULT_LEVEL, cond_)
#define EXT_LOG EXT_LOGVARIABLE


// variable logging - if constexpr() ...
#define eXT_LOG_INTERNAL_CONST(id_, topic_, macro_level_, cond_)                          \
    if constexpr (ext::logging::_detail::constexpr_level_is_active(macro_level_) && cond_) \
    ext::logging::_detail::logger(id_, (topic_), (macro_level_), __FILE__, __LINE__, __FUNCTION__)

#define eXT_LOG_INTERNAL_ADD_PREFIX_CONST(id_, topic_, macro_level_, cond_) \
    eXT_LOG_INTERNAL_CONST(id_, (ext::logging::topic::topic_), (ext::logging::level::macro_level_), cond_)

#define EXT_LOGCONST4(id, topic_, macro_level_, cond_) eXT_LOG_INTERNAL_ADD_PREFIX_CONST(id, topic_, macro_level_, cond_)
#define EXT_LOGCONST3(id, topic_, macro_level_) eXT_LOG_INTERNAL_ADD_PREFIX_CONST(id, topic_, macro_level_, true)
#define EXT_LOGCONST2(id, macro_level_) eXT_LOG_INTERNAL_ADD_PREFIX_CONST(id, no_topic, macro_level_, true)
#define EXT_LOGCONST1(id) eXT_LOG_INTERNAL_ADD_PREFIX_CONST(id, no_topic, EXT_LOGGING_DEFAULT_LEVEL, true)

#ifdef EXT_COMPILER_VC
    #define EXT_LOGCONST(...)                                                                           \
        eXT_LOG_SELECT5TH_PARAMETER(                                                                   \
            eXT_LOG_EXPAND(__VA_ARGS__), EXT_LOGCONST4, EXT_LOGCONST3, EXT_LOGCONST2, EXT_LOGCONST1, ) \
        (eXT_LOG_EXPAND(__VA_ARGS__))
#else
    #define EXT_LOGCONST(...)                                                                                   \
        eXT_LOG_SELECT5TH_PARAMETER(__VA_ARGS__, EXT_LOGCONST4, EXT_LOGCONST3, EXT_LOGCONST2, EXT_LOGCONST1, ) \
        (__VA_ARGS__)
#endif // EXT_COMPILER_VC

#define EXT_DEV eXT_LOG_INTERNAL_ADD_PREFIX_CONST("@@@@", dev, EXT_LOGGING_DEFAULT_LEVEL, true)
#define EXT_DEV_IF_CONST(cond_) eXT_LOG_INTERNAL_ADD_PREFIX_CONST("@@@@", dev, EXT_LOGGING_DEFAULT_LEVEL, cond_)
#define EXT_LOG_CONST EXT_LOGCONST


#endif // EXT_LOGGING_HEADER
