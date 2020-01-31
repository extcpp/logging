# Copyright - 2016-2020 - Jan Christoph Uhde <Jan@UhdeJC.com>

cmake_minimum_required(VERSION 3.8)
project(ext-logging VERSION 0.0.1 LANGUAGES CXX)
message(STATUS "extINFO -- entering ext-logging")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

## OPTIONS
option(EXTLOG_EXAMPLES "build examples" OFF)
option(EXTLOG_WARNINGS "enable warnings" ON)
option(EXTLOG_CHECKED "user assert" ON)
option(EXTLOG_TESTS "build tests" OFF)

## general setup and includes
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON) # XCode / VS folders

set(LIBEXT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external_libs/ext-basics)
include(ext_cmake_setup)
include(ext_cmake_install)

find_package(Threads REQUIRED)
find_package(Filesystem REQUIRED)
if(NOT TARGET ext::basics)
    add_subdirectory(external_libs/ext-basics EXCLUDE_FROM_ALL)
endif()

set(FORCE_COLORED_OUTPUT ON)
ext_colored_compiler_ouput()

# verbose windows linking
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /VERBOSE:LIB")

if(LINUX)
    set(EXT_OUTDIR "")
elseif(MSVC)
    #TODO - move settings below into corresponding targets
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${EXT_OUTDIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${EXT_OUTDIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXT_OUTDIR}")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

# include build dir to find version.hpp
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# required by folder structure for XCode and VisualStudio (includes)
# sources are always required
include(src_files.cmake)
include(include_files.cmake)

### library setup
set(ext_common_compile_definitions
    $<IF:$<BOOL:${ext_is_big_endian}>,EXT_BIG_ENDIAN,EXT_LITTLE_ENDIAN>
    $<$<BOOL:${EXT_CXX_COMPILER_IS_GCC}>:EXT_GCC>
    $<$<BOOL:${EXT_CXX_COMPILER_IS_CLANG}>:EXT_CLANG>
    $<$<BOOL:${ext_l1_cache_line_size}>:EXT_KNOWN_L1_CACHE_LINE_SIZE=${ext_l1_cache_line_size}>
)


add_library(ext-logging ${ext-logging-source} ${ext-logging-header})
target_include_directories(ext-logging PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)
target_compile_features(ext-logging PUBLIC cxx_std_17)
target_compile_options(ext-logging PRIVATE ${ext_stone-warnings})
target_compile_definitions(ext-logging PUBLIC ${ext_common_compile_definitions})
target_compile_definitions(ext-logging PUBLIC EXT_SHARED_LIB)
target_compile_definitions(ext-logging PRIVATE EXT_IN_LIB=1)
target_link_libraries(ext-logging PUBLIC
    ext::basics
    Threads::Threads
    std::filesystem
)

# set up folder structure for XCode and VisualStudio
set_target_properties (ext-logging PROPERTIES FOLDER logging)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ext-logging-header} ${ext-logging-source})

add_library(ext::logging ALIAS ext-logging)

## testing
if(EXTLOG_TESTS)
    ext_log("ext-logging tests enabled")
    include(CTest)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    ext_add_test_subdirectory("google" tests)
else()
    ext_log("ext-logging tests disabled")
endif()

## add projects using this lib
if(EXTLOG_EXAMPLES)
    ext_log("ext-logging examples enabled")
    add_subdirectory(examples)
else()
    ext_log("ext-logging examples disabled")
endif()

## installation
if(COMMAND ext_install)
    set_target_properties(ext-logging PROPERTIES EXPORT_NAME logging)

    ext_install(ext-logging include/ext)

    install(TARGETS ext-logging
        EXPORT ext-logging-targets
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
	install(
        EXPORT ext-logging-targets
		FILE ext-logging-config.cmake
        NAMESPACE ext::
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
endif()

add_custom_target(
    update_logging_version ALL
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND}
                -D "EXT_GIT_VERSION_OUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/ext_version.hpp"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ext_script_git_version.cmake"
)
