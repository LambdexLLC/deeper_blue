# Add dependencies

ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/glfw" glfw "https://github.com/glfw/glfw")
ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/lua" lua::lua "https://github.com/JonathanCline/lua")
ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/jcopengl" jclib::gl "https://github.com/JonathanCline/jcopengl")
ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/glm" glm "https://github.com/g-truc/glm")

add_subdirectory("glad")


add_library(lodepng STATIC "lodepng/lodepng.h" "lodepng/lodepng.cpp")
target_include_directories(lodepng PUBLIC "lodepng")
