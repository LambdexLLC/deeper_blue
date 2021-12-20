#pragma once

#include "gfx/gl.hpp"

#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;

namespace lbx::chess_view
{
	/**
	 * @brief Attempts to create a simple vertex + fragment shader program from source strings
	 * @param _vertexSource Vertex shader source code
	 * @param _fragmentSource Fragment shader source code
	 * @return The created program, or null on failure
	*/
	gl::unique_program create_simple_shader_program(std::string_view _vertexSource, std::string_view _fragmentSource);

	/**
	 * @brief Attempts to load a simple vertex + fragment shader program from disk
	 * @param _vertexPath Path to the vertex shader source
	 * @param _fragmentPath Path to the fragment shader source
	 * @return The created program, or null on failure
	*/
	gl::unique_program load_simple_shader_program(const fs::path& _vertexPath, const fs::path& _fragmentPath);
};