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

	std::unordered_map<std::string, gl::unique_program> _shaderPrograms{};
	try
	{
		const auto _shadersJson = get_config_value("shaders");
		const auto _root = get_application_root();

		for (auto& s : _shadersJson)
		{
			const std::string _name = s.at("name");
			const std::string_view _vertexPath = s.at("vertex_path");
			const std::string_view _fragmentPath = s.at("fragment_path");

			auto _program = load_simple_shader_program(_root / _vertexPath, _root / _fragmentPath);
			_shaderPrograms.insert_or_assign(_name, std::move(_program));
		};
	}
	catch (const lbx::json::exception& _exc)
	{
		std::cout << _exc.what() << '\n';
	};



	 
	

	auto _boardShader = _shaderPrograms.at("board").id();
	auto _textShader = _shaderPrograms.at("text").id();






	WindowOrthoProjection _projection{ _state.window_.size_buffer() };
	_projection.invoke(_state.window_);

	using namespace lbx;

	auto _fontSize = text::FontSize_Pixels{ 0, 64 };
	auto _arialFont = text::load_font_face_file("C:/Windows/Fonts/ariblk.ttf", _fontSize).value();


	// Draws text
	text::TextArtist _arialTextArtist{ _arialFont, _textShader };

	auto _block = _arialTextArtist.add_text("Your mom", 400.0f, 400.0f);
	_arialTextArtist.set_text(_block, "No, your mom.");



	{
		auto _artist = jc::make_unique<BoardArtist>(chess::make_standard_board(), _state.window_);
		_artist->configure_attributes(_boardShader);
		_state.insert_artist(std::move(_artist));
	};


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
	};

	return 0;
};
