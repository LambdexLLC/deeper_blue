cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

project(lbx-chess-controller-exe)

add_executable(${PROJECT_NAME} "source/main.cpp")
target_include_directories(${PROJECT_NAME} PRIVATE "source")
target_link_libraries(${PROJECT_NAME} PUBLIC httplib jclib nlohmann_json)

