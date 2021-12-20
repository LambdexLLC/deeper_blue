#pragma once

#include "uniform_block.hpp"

#include "gfx/window.hpp"

namespace lbx::chess_view
{
	/**
	 * @brief Buffer backed uniform block that streams the UI View uniform data to shaders.
	*/
	class UniformBlock_UIView
	{
	private:

		void rebuffer_data()
		{
			const auto _matrix = this->projection_.matrix();
			gl::buffer_subdata(this->buffer_.buffer(), std::span{ &_matrix[0][0], 16 });
		};
		void reallocate_data()
		{
			gl::resize_buffer<float>(this->buffer_.buffer(), 16);
		};

	public:

		void configure(const gl::program_id& _program)
		{
			auto& _buffer = this->buffer_;
			if (_buffer.has_uniform_block(_program))
			{
				_buffer.configure(_program);
			};
		};

		void update()
		{
			this->rebuffer_data();
		};





		UniformBlock_UIView(Window& _window) :
			buffer_{ "UIView", 0 }, projection_{ _window.size_buffer() }
		{
			this->reallocate_data();
			this->rebuffer_data();
		};

	private:
		UniformBlockBuffer buffer_;
		WindowOrthoProjection projection_;
	};

};