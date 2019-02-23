cmake_minimum_required(VERSION 3.13)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/test")

include(unittest)

set(
    SOURCES
    main.cpp
)

set(HEADERS
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

make_test(
    NAME test_type
    LIBS gtest gtest_main pthread
    SRCS main.cpp
    VALGRIND
    COVERAGE
)
    


