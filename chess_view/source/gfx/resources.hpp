#pragma once

/*
	Provides access to graphics resources used everywhere
*/

#include "shader.hpp"

namespace lbx::chess_view
{
	/**
	 * @brief Storage for common graphics resources
	*/
	struct GFXResources
	{
		/**
		 * @brief Loads in resources using application config variables.
		 * @return True on good load, false otherwise.
		*/
		bool load();


		/**
		 * @brief The text shader program.
		*/
		gl::unique_program text_shader{};

		/**
		 * @brief The chess board shader program.
		*/
		gl::unique_program board_shader{};
	};


};
