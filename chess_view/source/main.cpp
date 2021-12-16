#include "text/text.hpp"
#include "text/artist.hpp"

#include "image.hpp"

#include "gl.hpp"
#include "gfx.hpp"
#include "shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <lambdex/chess/chess.hpp>

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

	void resync_square_size()
	{
		auto& _width = this->square_width_;
		auto& _height = this->square_height_;

		const auto _zLevel = -0.1f;

		auto& _baseVerts = base_verts_;
		_baseVerts = std::array
		{
			BaseVertex{ 0.0f, 0.0f,		 _zLevel,		0.0f, 1.0f },
			BaseVertex{ 0.0f, _height,	 _zLevel,	0.0f, 0.0f },
			BaseVertex{ _width, _height, _zLevel,	1.0f, 0.0f },

			BaseVertex{ 0.0f, 0.0f,		 _zLevel,		0.0f, 1.0f },
			BaseVertex{ _width, _height, _zLevel,	1.0f, 0.0f },
			BaseVertex{ _width, 0.0f,	 _zLevel,		1.0f, 1.0f },
		};

		if (this->vbo_)
		{
			gl::buffer_data(this->vbo_, _baseVerts);
		};
	};

	void create_instances(const lbx::chess::PieceBoard& _board)
	{
		auto& _instances = this->instances_;
		_instances.resize(64, Instance{});
		
		lbx::chess::Position _pos{ 0 };
		for (auto it = _instances.begin(); it != _instances.end(); ++it, ++_pos)
		{
			const auto _posPair = lbx::chess::PositionPair{ _pos };
			const auto _piece = _board[_pos];

			it->y = static_cast<float>(_posPair.rank());
			it->x = static_cast<float>(_posPair.file());

			using Piece = lbx::chess::Piece;
			switch (_piece)
			{
			case Piece::king_white:
				it->alpha = 1.0f;
				it->piece = 0.0f;
				break;
			case Piece::king_black:
				it->alpha = 1.0f;
				it->piece = 6.0f;
				break;

			case Piece::queen_white:
				it->alpha = 1.0f;
				it->piece = 1.0f;
				break;
			case Piece::queen_black:
				it->alpha = 1.0f;
				it->piece = 7.0f;
				break;

			case Piece::bishop_white:
				it->alpha = 1.0f;
				it->piece = 2.0f;
				break;
			case Piece::bishop_black:
				it->alpha = 1.0f;
				it->piece = 8.0f;
				break;

			case Piece::knight_white:
				it->alpha = 1.0f;
				it->piece = 3.0f;
				break;
			case Piece::knight_black:
				it->alpha = 1.0f;
				it->piece = 9.0f;
				break;

			case Piece::rook_white:
				it->alpha = 1.0f;
				it->piece = 4.0f;
				break;
			case Piece::rook_black:
				it->alpha = 1.0f;
				it->piece = 10.0f;
				break;

			case Piece::pawn_white:
				it->alpha = 1.0f;
				it->piece = 5.0f;
				break;
			case Piece::pawn_black:
				it->alpha = 1.0f;
				it->piece = 11.0f;
				break;


			default: [[fallthrough]];
			case Piece::empty:
				it->alpha = 0.0f;
				it->piece = 0.0f;
				break;
			};
			
			switch (lbx::chess::get_square_color(_pos.get()))
			{
			case lbx::chess::Color::black:
				it->br = this->board_color_0.r;
				it->bg = this->board_color_0.g;
				it->bb = this->board_color_0.b;
				break;
			case lbx::chess::Color::white:
				it->br = this->board_color_1.r;
				it->bg = this->board_color_1.g;
				it->bb = this->board_color_1.b;
				break;
			};
		};
	};

public:

	void set_square_size(float _width, float _height)
	{
		this->square_height_ = _height;
		this->square_width_ = _width;
		this->resync_square_size();
	};

	bool init(lbx::chess_view::Window& _window)
	{
		this->projection_ = lbx::chess_view::WindowOrthoProjection{ _window.size_buffer() };

		// Load up the image
		{
			auto _image = lbx::chess_view::load_png_file(SOURCE_ROOT "/assets/textures/chess_pieces.png");
			const auto _subtexHeight = _image.height() / 12;

			auto& _texture = this->piece_texture_;
			_texture = gl::new_texture(gl::texture_target::array2D);
			gl::set_storage_3D(_texture, gl::internal_format::rgba8, _image.width(), _subtexHeight, 12);
			
			for (int n = 0; n != 12; ++n)
			{
				gl::set_subimage_3D
				(
					_texture,
					gl::format::rgba, gl::pixel_typecode::gl_unsigned_byte,
					_image.data() + (_image.width() * _image.encoding().bytes_per_pixel() * _subtexHeight * n),	 // data source
					_image.width(), _subtexHeight, 1,		// size of the image (w, h, d)
					0, 0, n									// offset to place image in (dw, dh, dd)
				);
			};
		};
		
		auto& _vao = this->vao_;
		auto& _vbo = this->vbo_;
		auto& _ibo = this->ibo_;

		_vao = gl::new_vao();
		gl::bind(_vao);

		_vbo = gl::new_vbo();
		gl::bind(_vbo, gl::vbo_target::array);
		gl::buffer_data(_vbo, this->base_verts_);

		_ibo = gl::new_vbo();
		gl::bind(_ibo, gl::vbo_target::array);
		gl::buffer_data(_ibo, this->instances_);

		return true;
	};

	void configure_attributes(gl::program_id _program)
	{
		this->program_ = _program;

		gl::bind(this->vao_);

		gl::vertex_binding_index _baseBindingIdx{ 1 };
		gl::bind_vertex_buffer(_baseBindingIdx, this->vbo_, 0, sizeof(BaseVertex));
		gl::set_vertex_divisor(_baseBindingIdx, 0);

		gl::vertex_binding_index _instanceBindingIdx{ 2 };
		gl::bind_vertex_buffer(_instanceBindingIdx, this->ibo_, 0, sizeof(Instance));
		gl::set_vertex_divisor(_instanceBindingIdx, 1);


		gl::vertex_attribute_index _basePos = gl::get_program_resource_location(_program, gl::resource_type::program_input, "base_pos").value();
		gl::enable_attribute_array(_basePos);
		gl::set_attribute_binding(_basePos, _baseBindingIdx);

		gl::vertex_attribute_index _baseUVs = gl::get_program_resource_location(_program, gl::resource_type::program_input, "base_uvs").value();
		gl::enable_attribute_array(_baseUVs);
		gl::set_attribute_binding(_baseUVs, _baseBindingIdx);
		
		gl::set_attribute_format(_basePos, gl::typecode::gl_float, 3, false, 0);
		gl::set_attribute_format(_baseUVs, gl::typecode::gl_float, 2, false, sizeof(float) * 3);
		
		
		gl::vertex_attribute_index _instancePos = gl::get_program_resource_location(_program, gl::resource_type::program_input, "instance_pos").value();
		gl::enable_attribute_array(_instancePos);
		gl::set_attribute_binding(_instancePos, _instanceBindingIdx);

		gl::vertex_attribute_index _instancePiece = gl::get_program_resource_location(_program, gl::resource_type::program_input, "instance_piece").value();
		gl::enable_attribute_array(_instancePiece);
		gl::set_attribute_binding(_instancePiece, _instanceBindingIdx);

		gl::vertex_attribute_index _instanceAlpha = gl::get_program_resource_location(_program, gl::resource_type::program_input, "instance_alpha").value();
		gl::enable_attribute_array(_instanceAlpha);
		gl::set_attribute_binding(_instanceAlpha, _instanceBindingIdx);

		gl::vertex_attribute_index _instanceBgCol = gl::get_program_resource_location(_program, gl::resource_type::program_input, "instance_background_color").value();
		gl::enable_attribute_array(_instanceBgCol);
		gl::set_attribute_binding(_instanceBgCol, _instanceBindingIdx);

		gl::set_attribute_format(_instancePos, gl::typecode::gl_float, 2, false, 0);
		gl::set_attribute_format(_instancePiece, gl::typecode::gl_float, 1, false, sizeof(float) * 2);
		gl::set_attribute_format(_instanceAlpha, gl::typecode::gl_float, 1, false, sizeof(float) * 3);
		gl::set_attribute_format(_instanceBgCol, gl::typecode::gl_unsigned_byte, 3, true, sizeof(float) * 4);


		this->square_size_uni_ = gl::get_program_resource_location(_program, gl::resource_type::uniform, "square_size").value();
		this->projection_uni_ = gl::get_program_resource_location(_program, gl::resource_type::uniform, "projection").value();
		this->model_uni_ = gl::get_program_resource_location(_program, gl::resource_type::uniform, "model").value();
	};

	void draw(lbx::chess_view::GraphicsState& _state) final
	{
		gl::bind(this->program_);
		
		gl::bind(this->vao_);
		gl::bind(this->piece_texture_, gl::texture_target::array2D);
		
		gl::set_uniform(this->program_, this->square_size_uni_, this->square_width_, this->square_height_);
		gl::set_uniform(this->program_, this->projection_uni_, this->projection_.matrix());
		gl::set_uniform(this->program_, this->model_uni_, this->model_matrix_);

		glDrawArraysInstanced(GL_TRIANGLES, 0, this->base_verts_.size(), this->instances_.size());
	};

	void set_board(const lbx::chess::PieceBoard& _board)
	{
		this->create_instances(_board);
		gl::buffer_data(this->ibo_, this->instances_);
	};

	BoardArtist(const lbx::chess::PieceBoard& _board, lbx::chess_view::Window& _window)
	{
		int _fbWidth, _fbHeight;
		glfwGetFramebufferSize(_window, &_fbWidth, &_fbHeight);
		const auto _squareHeight = static_cast<float>(_fbHeight) / 8.0f;

		this->set_square_size(_squareHeight, _squareHeight);
		this->create_instances(_board);

		const auto _goodInit = this->init(_window);
		JCLIB_ASSERT(_goodInit);
	};



	gl::program_id program_{};
	gl::unique_texture piece_texture_{};

	gl::unique_vao vao_;
	gl::unique_vbo vbo_;
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

	gl::uniform_location projection_uni_{};
	gl::uniform_location model_uni_{};
	
	lbx::chess_view::WindowOrthoProjection projection_{};
	glm::mat4 model_matrix_{ 1.0f };
};

gl::unique_program load_board_shader_program()
{
	const fs::path _root = SOURCE_ROOT "/assets/shaders/board/";
	return lbx::chess_view::load_simple_shader_program(_root / "vertex.glsl", _root / "fragment.glsl");
};
gl::unique_program load_text_shader_program()
{
	const fs::path _root = SOURCE_ROOT "/assets/shaders/text/";
	return lbx::chess_view::load_simple_shader_program(_root / "vertex.glsl", _root / "fragment.glsl");
};

void APIENTRY test_debug_callback
(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
)
{
	std::cout << std::string_view{ message, (size_t)length } << '\n';

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		break;
	case GL_DEBUG_SEVERITY_LOW:
		break;
	case GL_DEBUG_SEVERITY_MEDIUM: [[fallthrough]];
	case GL_DEBUG_SEVERITY_HIGH:
		__debugbreak();
		break;
	default:
		JCLIB_ABORT();
		break;
	};
};


int main()
{
	lbx::chess_view::GraphicsState _state{};
	if (!lbx::chess_view::init_graphics(_state)) { return -1; };


	gl::enable_debug_output_synchronous();
	gl::set_debug_callback(test_debug_callback, nullptr);



	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	auto _shader = load_board_shader_program();
	JCLIB_ASSERT(_shader);
	auto _textShader = load_text_shader_program();
	JCLIB_ASSERT(_textShader);


	lbx::chess_view::WindowOrthoProjection _projection{ _state.window_.size_buffer() };
	_projection.invoke(_state.window_);


	{
		using namespace lbx::chess;
		auto _artist = jc::make_unique<BoardArtist>(make_standard_board(), _state.window_);
		_artist->configure_attributes(_shader);
		_state.insert_artist(std::move(_artist));
	};




	auto _fontSize = lbx::text::FontSize_Pixels{ 0, 64 };
	auto _arialFont = lbx::text::load_font_face_file("C:/Windows/Fonts/ariblk.ttf", _fontSize).value();

	lbx::text::TextArtist _arialTextArtist{ _arialFont };
	JCLIB_ASSERT(_arialTextArtist.init());
	_arialTextArtist.configure_attributes(_textShader);
	_arialTextArtist.position_ = glm::vec3{ 0.0f, 0.0f, 0.0f };

	auto _block = _arialTextArtist.add_text("your mom is cool", 400.0f, 400.0f);

	while (_state.keep_running())
	{
		{
			const auto _mat = _projection.matrix();
			const auto _uni = gl::get_program_resource_location(_textShader, gl::resource_type::uniform, "projection").value();
			gl::set_uniform(_textShader, _uni, _mat);
		};

		_state.clear();
		_state.draw();

		_arialTextArtist.draw();

		_state.swap_buffers();
		_state.pull_events();

		if (glfwGetKey(_state.window_, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			static bool _once = false;
			if (!_once)
			{
				_arialTextArtist.append_text(_block, " NOT!");
				_once = true;
			};
		};

	};

	return 0;
};
