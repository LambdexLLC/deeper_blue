#include "text_artist.hpp"


namespace lbx::chess_view
{
	/**
	 * @brief Initializes and acquires resources.
	 * @param _state Graphics state.
	 * @return True on good init, false otherwise.
	*/
	bool TextArtist::init(GraphicsState& _state)
	{
		const auto _shader = _state.resources().text_shader.id();
		this->artist_.configure_attributes(_shader);
		return true;
	};

	/**
	 * @brief Draws the text.
	 * @param _state Graphics state.
	*/
	void TextArtist::draw(GraphicsState& _state)
	{
		this->artist_.draw();
	};

	/**
	 * @brief Sets the font for this artist to draw.
	 * @param _font Loaded font pointer.
	*/
	TextArtist::TextArtist(jc::reference_ptr<text::LoadedFontFace> _font) :
		artist_{ _font }
	{};

}