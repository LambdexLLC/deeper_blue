#pragma once

#include "gfx/element.hpp"
#include "gfx/artists/text_artist.hpp"

#include <iostream>

namespace lbx::chess_view
{
	/**
	 * @brief Element type for a text box.
	*/
	class TextElement : public IElement
	{
	public:
		
		void hide() final 
		{
			if (this->id_ != 0)
			{
				auto& _artist = *this->artist_;
				_artist->remove_text(this->id_);
				this->id_ = 0;
			};
		};
		void show() final
		{
			if (this->id_ == 0)
			{
				auto& _artist = *this->artist_;
				this->id_ = _artist->add_text(this->text_, this->x_, this->y_);
			};
		};

		TextElement(jc::reference_ptr<TextArtist> _artist, const std::string& _text, float _x, float _y) :
			artist_{ _artist },
			text_{ _text }, x_{ _x }, y_{ _y }
		{};

	private:

		// Artist
		jc::reference_ptr<TextArtist> artist_;

		// This text block
		text::TextArtist::TextBlockID id_{};

		std::string text_{};
		float x_ = 0.0f;
		float y_ = 0.0f;
	};

};
