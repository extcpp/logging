# Copyright - 2016-2020 - Jan Christoph Uhde <Jan@UhdeJC.com>

cmake_minimum_required(VERSION 3.0.0)
project(ext-logging-examples)

## examples
set(examples
    logging
)

foreach(ex IN LISTS examples) # <- DO NOT EXPAND LIST
    set(cpp "${ex}.cpp")
    set(example "${ex}_example")
    add_executable(${example} ${cpp})
    target_link_libraries(${example}
        ${CMAKE_THREAD_LIBS_INIT}
        ext::logging
    )
    target_compile_options(${example} PRIVATE ${ext_stone-warnings})
    set_target_properties (${example} PROPERTIES FOLDER examples/${example})
endforeach()
