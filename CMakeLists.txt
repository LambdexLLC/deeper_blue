#
#   Root cmake lists for the deeper_blue project
#

cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(deeper_blue VERSION 0.0.1)


include ("CMakeUtils.cmake")

foreach(i IN LISTS add_subdirs)
    add_subdirectory("${i}")
endforeach()


# Libraries to be privately linked
set(private_link 
    nlohmann_json
    httplib
    fmt)

# Directories to be privately included
set(private_include 
    "source")


# Enable tests
include (CTest)
enable_testing()



add_executable(${PROJECT_NAME}-exe "source/main.cpp")

target_compile_features(${PROJECT_NAME}-exe PUBLIC cxx_std_20)
target_include_directories(${PROJECT_NAME}-exe PRIVATE ${private_include})
target_link_libraries(${PROJECT_NAME}-exe PUBLIC jclib lbx::chess-lib PRIVATE ${private_link})

add_subdirectory("source")

target_compile_definitions(${PROJECT_NAME}-exe PRIVATE SOURCE_ROOT="${CMAKE_CURRENT_LIST_DIR}")



#	Make sure that the env folder is coppied to the build output to ensure the environment is
#	defined correctly

file(COPY "${CMAKE_CURRENT_LIST_DIR}/env" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

