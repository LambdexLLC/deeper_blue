#pragma once

#include <jclib/config.h>

#include <span>
#include <ranges>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <filesystem>

namespace lbx::chess_view
{
	/**
	 * @brief Possible pixel data encodings that a loaded image may use
	*/
	struct ImageEncoding
	{
		/**
		 * @brief Number of color channels (rgb = 3, rgba = 4, ...)
		*/
		uint8_t channels;

		/**
		 * @brief Number of bits per color channel
		*/
		uint8_t bitdepth;

		/**
		 * @brief Gets the number of bits per pixel using this encoding
		 * @return Bits per pixel
		*/
		constexpr uint8_t bits_per_pixel() const noexcept
		{
			return this->bitdepth * this->channels;
		};

		/**
		 * @brief Gets the number of bytes per pixel using this encoding
		 * @return Bytes per pixel
		*/
		constexpr uint8_t bytes_per_pixel() const noexcept
		{
			return this->bits_per_pixel() / (uint8_t)8;
		};

		constexpr ImageEncoding() = default;
		constexpr ImageEncoding(uint8_t _channels, uint8_t _bitdepth) :
			channels{ _channels }, bitdepth{ _bitdepth }
		{};
	};

	/**
	 * @brief Holds an image and provides a clean interface
	*/
	class Image
	{
	public:

		using value_type = std::byte;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		/**
		 * @brief Type used to measure sizes
		*/
		using size_type = uint32_t;

		size_type width() const noexcept
		{
			return this->width_;
		};
		size_type height() const noexcept
		{
			return this->height_;
		};

		ImageEncoding encoding() const noexcept
		{
			return this->encoding_;
		};

		size_type size() const noexcept
		{
			return this->width() * this->height();
		};
		size_type size_bytes() const noexcept
		{
			return this->size() * this->encoding().bytes_per_pixel();
		};

		pointer data()
		{
			return this->data_.get();
		};
		const_pointer data() const
		{
			return this->data_.get();
		};

		/**
		 * @brief Gets a pointer to the first byte in a row
		 * @param n Row number to get
		 * @return Pointer to first byte in row
		*/
		pointer row_data(size_type n)
		{
			JCLIB_ASSERT(n <= this->height());
			return this->data() + (this->width() * n * this->encoding().bytes_per_pixel());
		};

		/**
		 * @brief Gets a pointer to the first byte in a row
		 * @param n Row number to get
		 * @return Pointer to first byte in row
		*/
		const_pointer row_data(size_type n) const
		{
			JCLIB_ASSERT(n <= this->height());
			return this->data() + (this->width() * n * this->encoding().bytes_per_pixel());
		};

		std::span<value_type> row(size_type n)
		{
			JCLIB_ASSERT(n < this->height());
			return std::span<value_type>{ this->row_data(n), this->row_data(n + 1) };
		};
		std::span<const value_type> row(size_type n) const
		{
			JCLIB_ASSERT(n < this->height());
			return std::span<const value_type>{ this->row_data(n), this->row_data(n + 1) };
		};



		Image() = default;
		
		/**
		 * @brief Preallocates for a image size
		*/
		explicit Image(size_type _width, size_type _height, ImageEncoding _encoding) :
			data_{ new value_type[_width * _height * _encoding.bytes_per_pixel()] },
			width_{ _width }, height_{ _height }, encoding_{ _encoding }
		{};

		/**
		 * @brief Assigns image data
		*/
		explicit Image(std::span<const value_type> _data, size_type _width, size_type _height, ImageEncoding _encoding) :
			Image{ _width, _height, _encoding }
		{
			std::ranges::copy(_data, this->data_.get());
		};

	private:

		/**
		 * @brief The raw pixel data
		*/
		std::unique_ptr<value_type[]> data_;

		/**
		 * @brief Width of the image in pixels
		*/
		size_type width_;

		/**
		 * @brief Height of the image in pixels
		*/
		size_type height_;

		/**
		 * @brief The encoding of the image data
		*/
		ImageEncoding encoding_;

	};



	



	Image load_png_file(const char* _filename) noexcept;
	inline auto load_png_file(const std::filesystem::path& _filepath) noexcept
	{
		const auto _str = _filepath.generic_string();
		return load_png_file(_str.c_str());
	};

};
