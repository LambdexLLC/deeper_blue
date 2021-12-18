#pragma once

#include <jclib/gl/gl.hpp>

#include <glm/matrix.hpp>

namespace gl = jc::gl;

namespace jc::gl
{
	// Uniform specialization to allow glm matrix upload
	template <>
	struct uniform_ftor<glm::mat4>
	{
		static void set(const program_id& _program, const uniform_location& _location, const glm::mat4& _mat, bool _transpose = false)
		{
			glProgramUniformMatrix4fv(_program.get(), _location.get(), 1, _transpose, &_mat[0][0]);
		};
	};
}
