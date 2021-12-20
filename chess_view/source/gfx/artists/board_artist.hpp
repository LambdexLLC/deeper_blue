#pragma once

#include "gfx/gl.hpp"
#include "gfx/gfx.hpp"
#include "gfx/window.hpp"
#include "gfx/image.hpp"

#include <jclib/gl/gl.hpp>

#include <jclib/ranges.h>

#include <lambdex/chess/board.hpp>

namespace lbx
{
	template <typename T>
	struct VertexBuffer
	{
	public:

		using value_type = T;
		using size_type = size_t;

		gl::vbo_id id() const
		{
			return this->vbo_.id();
		};
		operator gl::vbo_id() const
		{
			return this->id();
		};

		void resize(size_type _count, gl::vbo_usage _usage = gl::vbo_usage::static_draw)
		{
			gl::resize_buffer<value_type>(this->id(), _count, _usage);
		};

		template <std::ranges::contiguous_range RangeT>
		requires (jc::cx_convertible_to<jc::ranges::value_t<RangeT>, value_type>)
			void assign(const RangeT& _range, gl::vbo_usage _usage = gl::vbo_usage::static_draw)
		{
			gl::buffer_data(this->id(), _range, _usage);
		};

		size_type size_bytes() const
		{
			return static_cast<size_type>(gl::get<GLint>(this->id(), gl::vbo_parameter::size));
		};
		size_type size() const
		{
			return this->size_bytes() / sizeof(value_type);
		};

		void bind(gl::vbo_target _target)
		{
			this->id().bind(_target);
		};

		VertexBuffer() :
			vbo_{ gl::new_vbo() }
		{};
		VertexBuffer(gl::unique_vbo _vbo) :
			vbo_{ std::move(_vbo) }
		{};

		gl::unique_vbo vbo_;
	};







};



namespace lbx::chess_view
{

	struct BoardArtist : public lbx::chess_view::IArtist
	{
	private:

		/**
		 * @brief Type for the base vertex data
		*/
		struct BaseVertex
		{
			float x;
			float y;
			float z;

			float u;
			float v;
		};

		/**
		 * @brief Data for each instance
		*/
		struct Instance
		{
			float x;
			float y;

			float piece;
			float alpha;

			// Background color
			uint8_t br;
			uint8_t bg;
			uint8_t bb;

		};

		void resync_square_size();

		void create_instances(const chess::PieceBoard& _board);

	public:

		void set_square_size(float _width, float _height);

		void configure_attributes(gl::program_id _program);



		/**
		 * @brief Intializes the artist and acquires resources.
		 * @param _state Graphics state.
		 * @return True on good init, false otherwise.
		*/
		bool init(GraphicsState& _state) final;

		/**
		 * @brief Draws the artist's data onto the screen.
		 * @param _state Graphics state.
		*/
		void draw(GraphicsState& _state) final;

		/**
		 * @brief Sets the board that is being drawn.
		 * @param _board Chess board to draw.
		*/
		void set_board(const chess::PieceBoard& _board);

		// Sets the board for this to draw
		BoardArtist(const chess::PieceBoard& _board);

	private:

		gl::program_id program_{};
		gl::unique_texture piece_texture_{};

		gl::unique_vao vao_;
		VertexBuffer<BaseVertex> vbo_;
		gl::unique_vbo ibo_;

		float square_width_ = 0.1f;
		float square_height_ = 0.1f;

		struct SquareColor
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};

		SquareColor board_color_0{ 91, 53, 38 };
		SquareColor board_color_1{ 114, 95, 80 };

		std::array<BaseVertex, 6> base_verts_{};

		std::vector<Instance> instances_{};

		gl::uniform_location square_size_uni_{};

		gl::uniform_location model_uni_{};
		glm::mat4 model_matrix_{ 1.0f };
	};

};
