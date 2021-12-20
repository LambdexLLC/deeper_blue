#pragma once

#include <cppfreetype/bitmap.hpp>

#include <jclib/gl/gl.hpp>

#include <ranges>
#include <vector>
#include <optional>
#include <algorithm>
#include <filesystem>

namespace lbx::text
{
	/**
	 * @brief Holds the data associated with a loaded (and rendered) glyph
	*/
	struct LoadedGlyph
	{
		ft::bitmap bitmap{};
		FT_Glyph_Metrics metrics{};
		uint32_t codepoint = 0;
		int32_t bitmap_left = 0;
		int32_t bitmap_top = 0;
	};

	/**
	 * @brief Holds a font size in pixels
	*/
	struct FontSize_Pixels
	{
		uint32_t width;
		uint32_t height;
	};


	/**
	 * @brief Holds the data associated with a single font face
	*/
	class LoadedFontFace
	{
	private:

		using container_type = std::vector<LoadedGlyph>;

	public:

		// Iterator semantics

		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

		iterator begin() noexcept { return this->glyphs_.begin(); };
		const_iterator begin() const noexcept { return this->glyphs_.begin(); };
		const_iterator cbegin() const noexcept { return this->glyphs_.begin(); };

		iterator end() noexcept { return this->glyphs_.end(); };
		const_iterator end() const noexcept { return this->glyphs_.end(); };
		const_iterator cend() const noexcept { return this->glyphs_.end(); };



		using size_type = uint32_t;


		/**
		 * @brief Gets the maximum size for a glyph in this face.
		 * @return Glyph size in pixels.
		*/
		FontSize_Pixels max_glyph_size() const noexcept
		{
			return FontSize_Pixels{ this->width_px_, this->height_px_ };
		};





		void reserve_codepoint_range(uint32_t _maxCodepoint)
		{
			const auto _nullGlyph = LoadedGlyph{};
			this->glyphs_.resize(_maxCodepoint, _nullGlyph);
		};

		/**
		 * @brief Erases the data for any unused codepoints
		*/
		void erase_unused_codepoints()
		{
			// Remove the glyphs for any unused codepoints AFTER the initial codepoint = 0 glyph
			if (!this->glyphs_.empty())
			{
				std::erase_if(this->glyphs_, [](const auto& _glyph) -> bool
					{
						return _glyph.codepoint == 0;
					});
			};
		};

		/**
		 * @brief Sorts the glyphs by codepoint
		 *
		 * The glyph with the lowest codepoint will be at the front, while the highest
		 * codepoint will be at the back.
		*/
		void sort_glyphs()
		{
			std::ranges::sort(this->glyphs_, [](const auto& lhs, const auto& rhs) -> bool
				{
					return lhs.codepoint < rhs.codepoint;
				});
		};

		iterator find(uint32_t _codepoint)
		{
			return std::ranges::find_if(this->glyphs_, [_codepoint](const auto& g)
				{
					return g.codepoint == _codepoint;
				});
		};
		const_iterator find(uint32_t _codepoint) const
		{
			return std::ranges::find_if(this->glyphs_, [_codepoint](const auto& g)
				{
					return g.codepoint == _codepoint;
				});
		};

		/**
		 * @brief Gets the index of the glyph for a character codepoint.
		 *
		 * @param _codepoint Character codepoint to get glyph for.
		 *
		 * @return Index of the character's glyph, returns 0 if codepoint is not found.
		*/
		size_t glyph_index(uint32_t _codepoint) const
		{
			const auto _it = this->find(_codepoint);
			if (_it != this->end())
			{
				return _it - this->begin();
			}
			else
			{
				// Codepoint not found
				return 0;
			};
		};

		/**
		 * @brief Gets the number of glyphs in this loaded font face.
		 * @return Number of glyphs.
		*/
		size_t size() const noexcept
		{
			return this->glyphs_.size();
		};






		std::vector<LoadedGlyph> glyphs_{};
		uint32_t width_px_ = 0;
		uint32_t height_px_ = 0;

		int32_t x_scale_ = 0;
		int32_t y_scale_ = 0;
	};

	/**
	 * @brief Holds a font size in pixels
	*/
	struct FontSize_Pixels
	{
		uint32_t width;
		uint32_t height;
	};



	std::optional<LoadedFontFace> load_font_face_file(const char* _fontFilePath, FontSize_Pixels _sizePx, uint32_t _maxCodepoint = 255);
	std::optional<LoadedFontFace> load_font_face_file(const std::filesystem::path& _fontFilePath, FontSize_Pixels _sizePx, uint32_t _maxCodepoint = 255);
};
