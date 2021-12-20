#pragma once

/*
	Provides a generic buffer backed uniform block implementation.
*/

#include <jclib/gl/gl.hpp>

namespace lbx::chess_view
{
	/**
	 * @brief Basic generalization of a "specific" buffer backed uniform block.
	*/
	class UniformBlockBuffer
	{
	public:

		/**
		 * @brief Gets the ID of the buffer for this block buffer.
		 * @return Vertex buffer object ID.
		*/
		gl::vbo_id buffer() const noexcept
		{
			return this->vbo_;
		};

		/**
		 * @brief Gets the location of this uniform block within a program.
		 * @param _program Program to get location from.
		 * @return Uniform block location.
		*/
		jc::optional<gl::uniform_block_location> get_location(const gl::program_id& _program) const
		{
			return gl::get_uniform_block_index(_program, this->name_.c_str());
		};

		/**
		 * @brief Checks if the program given has this uniform block.
		 * @param _program Program to check, must not be null.
		 * @return True if program has this block, false otherwise.
		*/
		bool has_uniform_block(const gl::program_id& _program)
		{
			return this->get_location(_program).has_value();
		};

		/**
		 * @brief Configures the program to use this uniform block.
		 * @param _program Program to configure.
		*/
		void configure(const gl::program_id& _program)
		{
			const auto _location = this->get_location(_program);
			auto& _buffer = this->vbo_;

			// TODO : add to jcgl
			glUniformBlockBinding(_program.get(), _location.value().get(), 0);
			_buffer.bind(gl::vbo_target::uniform);

			const auto _size = static_cast<size_t>(gl::get(_buffer.id(), gl::vbo_parameter::size));

			// TODO : add to jcgl
			glBindBufferRange(GL_UNIFORM_BUFFER, 0, _buffer.get(), 0, _size);
		};

		/**
		 * @brief TODO: Comment This
		 * @param _name
		 * @param _index
		*/
		UniformBlockBuffer(const std::string& _name, GLuint _index) :
			name_{ _name }, index_{ _index },
			vbo_{ gl::new_vbo() }
		{};

	private:

		/**
		 * @brief The name for this uniform block.
		*/
		std::string name_{};

		/**
		 * @brief Buffer where our uniform block data is stored.
		*/
		gl::unique_vbo vbo_{};

		/**
		 * @brief Index to use for the buffer binding.
		*/
		GLuint index_ = 0;
	};
};
