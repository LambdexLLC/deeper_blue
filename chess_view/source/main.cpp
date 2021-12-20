#include "application/config.hpp"

#include "text/text.hpp"
#include "text/artist.hpp"

#include "gfx/gfx.hpp"
#include "gfx/shader.hpp"

#include "board_artist.hpp"

#include <unordered_map>






int main(int _nargs, const char* _vargs[])
{
	using namespace lbx::chess_view;

	// Determine the path to this application's root directory
	const auto _executablePath = fs::path{ _vargs[0] };
	const auto _applicationRootPath = _executablePath.parent_path();
	
	// TEMPORARY USE SOURCE ROOT
	set_application_root(SOURCE_ROOT);

	// Load the config
	if (!load_config())
	{
		JCLIB_ASSERT(false);
		return -1;
	};


	GraphicsState _state{};
	if (!init_graphics(_state)) { return -1; };




	





	WindowOrthoProjection _projection{ _state.window_.size_buffer() };
	_projection.invoke(_state.window_);



	auto _viewBuffer = gl::new_vbo();
	
	{
		const auto _mat = _projection.matrix();
		gl::buffer_data(_viewBuffer, std::span{ &_mat[0][0], 16 });
	};

	{
		gl::program_id _program = _state.resources().board_shader;

		struct UniformBlock
		{
			gl::uniform_block_location location;
			std::string name;
		};

		const auto _location = glGetUniformBlockIndex(_program.get(), "View");
		std::cout << _location << '\n';

		glUniformBlockBinding(_program.get(), _location, 0);
		_viewBuffer.bind(gl::vbo_target::uniform);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, _viewBuffer.get(), 0, sizeof(float) * 16);
	};




	using namespace lbx;

	auto _fontSize = text::FontSize_Pixels{ 0, 64 };
	auto _arialFont = text::load_font_face_file("C:/Windows/Fonts/ariblk.ttf", _fontSize).value();


	// Draws text
	text::TextArtist _arialTextArtist{ _arialFont, _state.resources().text_shader };

	auto _block = _arialTextArtist.add_text("Your mom", 400.0f, 400.0f);
	_arialTextArtist.set_text(_block, "No, your mom.");



	{
		auto _artist = jc::make_unique<BoardArtist>(chess::make_standard_board(), _state.window_);
		_artist->configure_attributes(_state.resources().board_shader);
		_state.insert_artist(std::move(_artist));
	};


	while (_state.keep_running())
	{
		{
			const auto _mat = _projection.matrix();
			gl::buffer_subdata(_viewBuffer, std::span{ &_mat[0][0], 16 });
		};

		{
			const auto _mat = _projection.matrix();
			gl::program_id _shader = _state.resources().text_shader;
			const auto _uni = gl::get_resource_location(_shader, gl::resource_type::uniform, "projection").value();
			gl::set_uniform(_shader, _uni, _mat);
		};

		_state.clear();
		_state.draw();

		_arialTextArtist.draw();

		_state.swap_buffers();
		_state.pull_events();
	};

	return 0;
};
