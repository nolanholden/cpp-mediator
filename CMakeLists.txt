cmake_minimum_required(VERSION 2.8)

include(gtest.cmake)

SET(COVERAGE OFF CACHE BOOL "Coverage")

add_executable(tests tests/mediator_unittests.cc)

find_package (Threads)
target_link_libraries(tests gtest ${CMAKE_THREAD_LIBS_INIT})

target_compile_options(tests PRIVATE -std=c++14 -g -Wall -Werror -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef)

if (COVERAGE)
    target_compile_options(tests PRIVATE --coverage)
    target_link_libraries(tests PRIVATE --coverage)
endif()

