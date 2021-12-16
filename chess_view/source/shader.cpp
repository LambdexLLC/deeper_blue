#include "shader.hpp"

#include <string>
#include <fstream>
#include <iostream>

namespace lbx::chess_view
{
	inline std::string read_text_file(const fs::path& _path)
	{
		std::cout << _path.generic_string() << '\n';
		std::ifstream _file{ _path };
		std::string _data(fs::file_size(_path), '\0');
		_file.read(_data.data(), _data.size());
		return _data;
	};


	/**
	 * @brief Attempts to create a simple vertex + fragment shader program from source strings
	 * @param _vertexSource Vertex shader source code
	 * @param _fragmentSource Fragment shader source code
	 * @return The created program, or null on failure
	*/
	gl::unique_program create_simple_shader_program(std::string_view _vertexSource, std::string_view _fragmentSource)
	{
		// Compile vertex shader
		auto _vertex = gl::new_shader(gl::shader_type::vertex);
		if (!gl::compile(_vertex, _vertexSource))
		{
			// Failed to compile
			std::cout << gl::get_info_log(_vertex) << '\n';
			return gl::unique_program{};
		};

		// Compile fragment shader
		auto _fragment = gl::new_shader(gl::shader_type::fragment);
		if (!gl::compile(_fragment, _fragmentSource))
		{
			// Failed to compile
			std::cout << gl::get_info_log(_fragment) << '\n';
			return gl::unique_program{};
		};

		// Create the program and link it
		auto _program = gl::new_program();
		{
			const auto _shaderIDs = std::array
			{
				_vertex.id(), _fragment.id()
			};
			if (!gl::link(_program, _shaderIDs))
			{
				// Failed to link
				std::cout << gl::get_info_log(_program) << '\n';
				return gl::unique_program{};
			};
		};

		return _program;
	};

	/**
	 * @brief Attempts to load a simple vertex + fragment shader program from disk
	 * @param _vertexPath Path to the vertex shader source
	 * @param _fragmentPath Path to the fragment shader source
	 * @return The created program, or null on failure
	*/
	gl::unique_program load_simple_shader_program(const fs::path& _vertexPath, const fs::path& _fragmentPath)
	{
		const auto _vertexSource = read_text_file(_vertexPath);
		const auto _fragmentSource = read_text_file(_fragmentPath);
		return create_simple_shader_program(_vertexSource, _fragmentSource);
	};
};