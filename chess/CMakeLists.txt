cmake_minimum_required(VERSION 3.16)

project(lbx-chess-lib VERSION 0.0.1 LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC "source/chess.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "source")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC jclib lambdex::utility::lib)

add_subdirectory("lib")
add_subdirectory("source")
add_subdirectory("tests")

add_library(lbx::chess-lib ALIAS lbx-chess-lib)
