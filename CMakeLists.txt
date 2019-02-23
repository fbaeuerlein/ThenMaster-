cmake_minimum_required(VERSION 3.13)

#compileAsC99()
add_compile_options(-std=c++14 -g)

add_subdirectory("googletest")
add_subdirectory("test")

