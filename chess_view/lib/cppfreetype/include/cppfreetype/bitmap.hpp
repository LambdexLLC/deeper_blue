#pragma once
#ifndef CPP_FREETYPE_BITMAP_HPP
#define CPP_FREETYPE_BITMAP_HPP

#include "freetype.hpp"

#include <cstddef>
#include <vector>
#include <algorithm>
#include <type_traits>

#define _CPP_FREETYPE_BITMAP_

namespace ft
{
	/**
	 * @brief Encoding modes for pixel data
	*/
	enum class pixel_mode : std::underlying_type_t<FT_Pixel_Mode>
	{
		none = FT_PIXEL_MODE_NONE,
		brga = FT_PIXEL_MODE_BGRA,
		mono = FT_PIXEL_MODE_MONO,
		gray = FT_PIXEL_MODE_GRAY,
		gray2 = FT_PIXEL_MODE_GRAY2,
		gray4 = FT_PIXEL_MODE_GRAY4,
		lcd = FT_PIXEL_MODE_LCD,
		lcd_v = FT_PIXEL_MODE_LCD_V,
	};


	/**
	 * @brief Holds a simplified bitmap image
	*/
	class bitmap
	{
	public:

		using value_type = std::byte;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		using size_type = uint32_t;

		/**
		 * @brief Assigns the bitmap by copying from a freetype bitmap.
		 *
		 * @param _bitmap Freetype bitmap to copy from.
		*/
		void assign(const ::FT_Bitmap& _bitmap)
		{
			// Size of each row of the image data in bytes
			const auto _rowSizeBytes = (_bitmap.pitch > 0) ? _bitmap.pitch : -_bitmap.pitch;

			// Copy various attributes
			this->width_ = _bitmap.width;
			this->height_ = _bitmap.rows;
			this->pixel_mode_ = pixel_mode(_bitmap.pixel_mode);

			// Check if no bitmap data is present or bitmap is otherwise empty
			if (!_bitmap.buffer || _bitmap.rows == 0)
			{
				// Clear our pixel data storage
				this->pixels_.clear();
			}
			else
			{
				// Copy pixel data from the bitmap

				// Allocate enough space for the bitmap image data
				this->pixels_.resize(_bitmap.rows * _rowSizeBytes, value_type{});

				// Copy pixel data
				auto it = this->pixels_.begin();
				auto _srcIt = reinterpret_cast<pointer>(_bitmap.buffer);
				for (size_type _row = 0; _row != _bitmap.rows; ++_row)
				{
					std::copy_n(_srcIt, _rowSizeBytes, it);
					_srcIt += _rowSizeBytes;
					it += _rowSizeBytes;
				};
			};
		};


		pointer data() noexcept
		{
			return this->pixels_.data();
		};
		const_pointer data() const noexcept
		{
			return this->pixels_.data();
		};


		auto begin()
		{
			return this->pixels_.begin();
		};
		auto begin() const
		{
			return this->pixels_.cbegin();
		};
		auto cbegin() const
		{
			return this->begin();
		};

		auto end()
		{
			return this->pixels_.end();
		};
		auto end() const
		{
			return this->pixels_.cend();
		};
		auto cend() const
		{
			return this->end();
		};




		// Initializes with size = 0
		bitmap() = default;

		/**
		 * @brief Assigns the bitmap by copying from a freetype bitmap.
		 *
		 * Same as calling "BitmapImage::assign(const ::FT_Bitmap& _bitmap)" after
		 * default initializing the BitmapImage object.
		 *
		 * @param _bitmap Freetype bitmap to copy, "buffer" member MUST NOT BE NULL!
		*/
		bitmap(const ::FT_Bitmap& _bitmap)
		{
			this->assign(_bitmap);
		};

		/**
		 * @brief Assigns the bitmap by copying from a freetype bitmap.
		 *
		 * Same as calling "BitmapImage::assign(const ::FT_Bitmap& _bitmap)".
		 *
		 * @param _bitmap Freetype bitmap to copy, "buffer" member MUST NOT BE NULL!
		*/
		bitmap& operator=(const ::FT_Bitmap& _bitmap)
		{
			this->assign(_bitmap);
			return *this;
		};

		/**
		 * @brief Raw image pixel data
		*/
		std::vector<value_type> pixels_{};

		/**
		 * @brief Width of the image in pixels
		*/
		size_type width_{};

		/**
		 * @brief Height of the bitmap in pixels
		*/
		size_type height_{};

		/**
		 * @brief Mode used to encode pixel data.
		*/
		pixel_mode pixel_mode_;
	};

};

#endif // CPP_FREETYPE_BITMAP_HPP