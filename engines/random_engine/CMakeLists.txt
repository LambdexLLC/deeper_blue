# The name for this chess engine
set(engine_name "random")

# Optional description for the engine
set(engine_description "beats your mom every time")






###################################################################
###																###
###			Below section is for internal usage only			###
###																###
###################################################################

cmake_minimum_required(VERSION 3.18)

set(__project_name "lbx-chess-engine_${engine_name}")
set(__target_name "${__project_name}-shared")

project(${__project_name}
	LANGUAGES CXX
	VERSION 0.1.0
	DESCRIPTION "${engine_description}")

add_library(${__target_name} SHARED "source/engine.cpp")

target_include_directories(${__target_name} PUBLIC "include" PRIVATE "source")
target_compile_features(${__target_name} PUBLIC cxx_std_20)
target_link_libraries(${__target_name} PUBLIC lbx::chess-lib)
