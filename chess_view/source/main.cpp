#include "application/config.hpp"

#include "gfx/scene.hpp"
#include "gfx/elements/text.hpp"

#include "gfx/gfx.hpp"
#include "gfx/shader.hpp"

#include "gfx/artists/board_artist.hpp"
#include "gfx/artists/text_artist.hpp"

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

	using namespace lbx;

	_state.insert_artist(jc::make_unique<BoardArtist>(chess::make_standard_board()));





	auto _fontSize = text::FontSize_Pixels{ 0, 64 };
	auto _arialFont = text::load_font_face_file("C:/Windows/Fonts/ariblk.ttf", _fontSize).value();

	// Draws text
	auto& _arialTextArtist = *_state.insert_artist(jc::make_unique<TextArtist>(jc::reference_ptr{ _arialFont }));

	Scene _scene{};
	_scene.insert(jc::make_unique<TextElement>
	(
		jc::reference_ptr{ _arialTextArtist }, "your mom", 200.0f, 200.0f
	));



	UniformBlock_UIView _viewBuffer{ _state.window_ };

	{
		gl::program_id _program = _state.resources().board_shader;
		_viewBuffer.configure(_program);
	};
	{
		gl::program_id _program = _state.resources().text_shader;
		_viewBuffer.configure(_program);
	};

	_scene.show();


	while (_state.keep_running())
	{
		_viewBuffer.update();
		_state.update();
	};


	return 0;
};
