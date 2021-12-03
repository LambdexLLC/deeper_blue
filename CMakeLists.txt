cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(deeper_blue VERSION 0.0.1)

add_executable(${PROJECT_NAME}-exe "main.cpp")
target_compile_features(${PROJECT_NAME}-exe PUBLIC cxx_std_20)
