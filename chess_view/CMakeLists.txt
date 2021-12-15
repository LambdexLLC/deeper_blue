
cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(deeper_blue-viewer VERSION 0.0.1)



# Libraries to be privately linked
set(private_link 
    nlohmann_json
    httplib
    jclib
    fmt
    lbx::chess-lib
	glfw
	glad
    lua::lua
    lodepng
    jclib::gl
    glm
    freetype)

# Directories to be privately included
set(private_include 
    "source")

    
add_executable(${PROJECT_NAME}-exe "source/main.cpp")

target_compile_features(${PROJECT_NAME}-exe PUBLIC cxx_std_20)
target_include_directories(${PROJECT_NAME}-exe PRIVATE ${private_include})
target_link_libraries(${PROJECT_NAME}-exe PRIVATE ${private_link})

target_compile_definitions(${PROJECT_NAME}-exe PRIVATE SOURCE_ROOT="${CMAKE_CURRENT_LIST_DIR}")



# Trickle down!

ADD_CMAKE_SUBDIRS_HERE()