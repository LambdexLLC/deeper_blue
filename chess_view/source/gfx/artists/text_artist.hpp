#pragma once

/*
	Implements an IArtist interface for the text artist.
*/

#include "gfx/gfx.hpp"
#include "text/artist.hpp"

namespace lbx::chess_view
{
	/**
	 * @brief IArtist implementation for the text artist.
	*/
	class TextArtist : public IArtist
	{
	public:

		text::TextArtist* operator->() noexcept
		{
			return &this->artist_;
		};
		const text::TextArtist* operator->() const noexcept
		{
			return &this->artist_;
		};



		/**
		 * @brief Initializes and acquires resources.
		 * @param _state Graphics state.
		 * @return True on good init, false otherwise.
		*/
		bool init(GraphicsState& _state) final;

		/**
		 * @brief Draws the text.
		 * @param _state Graphics state.
		*/
		void draw(GraphicsState& _state) final;

		/**
		 * @brief Sets the font for this artist to draw.
		 * @param _font Loaded font pointer.
		*/
		TextArtist(jc::reference_ptr<text::LoadedFontFace> _font);

	private:

		/**
		 * @brief Underlying text artist.
		*/
		text::TextArtist artist_;

	};
};
