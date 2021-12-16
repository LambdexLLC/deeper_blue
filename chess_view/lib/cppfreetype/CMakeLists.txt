#
#	A lightweight C++ wrapper for Freetype(2)
#


cmake_minimum_required(VERSION 3.18)

project(cppfreetype
	VERSION 0.0.1
	LANGUAGES CXX)

add_library(${PROJECT_NAME}-lib INTERFACE)
target_include_directories(${PROJECT_NAME}-lib INTERFACE "include")
target_compile_features(${PROJECT_NAME}-lib INTERFACE cxx_std_20)


include ("CMakeUtils.cmake")

# Add the library directory
add_subdirectory("lib")
target_link_libraries(${PROJECT_NAME}-lib INTERFACE jclib freetype)

add_library(${PROJECT_NAME} ALIAS ${PROJECT_NAME}-lib)
