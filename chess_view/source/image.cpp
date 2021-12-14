#include "image.hpp"

#include <jclib/config.h>

#include <lodepng.h>

#include <ranges>
#include <iostream>
#include <algorithm>

namespace lbx::chess_view
{
	inline ImageEncoding convert_lodepng_encoding(LodePNGColorMode _color)
	{
		ImageEncoding _encoding{};
		
		// Determine number of color channels
		switch (_color.colortype)
		{
		case LodePNGColorType::LCT_GREY:
			_encoding.channels = 1;
			break;
		case LodePNGColorType::LCT_GREY_ALPHA:
			_encoding.channels = 1;
			break;
		case LodePNGColorType::LCT_RGB:
			_encoding.channels = 3;
			break;
		case LodePNGColorType::LCT_RGBA:
			_encoding.channels = 4;
			break;
		default:
			JCLIB_ABORT();
			break;
		};

		// Convert bitdepth
		_encoding.bitdepth = static_cast<uint8_t>(_color.bitdepth);
		return _encoding;
	};

	Image load_png_file(const char* _filename) noexcept
	{
		std::vector<unsigned char> _pixels{};
		unsigned _width, _height;

		LodePNGColorMode _colorMode{};
		_colorMode.bitdepth = 8;
		_colorMode.colortype = LCT_RGBA;

		unsigned _error = lodepng::decode(_pixels, _width, _height, _filename, _colorMode.colortype, _colorMode.bitdepth);
		if (_error)
		{
			std::cout << "error " << _error << ": " << lodepng_error_text(_error) << '\n';
		};

		// Convert encoding
		const auto _encoding = convert_lodepng_encoding(_colorMode);
		const auto _data = std::span{ _pixels };
		Image _out(std::as_bytes(_data), _width, _height, _encoding);

		return _out;
	};

	namespace
	{
		void split_textures_vertically()
		{
			const std::filesystem::path _path = SOURCE_ROOT "/assets/chess_pieces_out.png";
			const std::filesystem::path _outputPath = SOURCE_ROOT "/assets/chess_piece_texturesheet.png";

			auto _image = lbx::chess_view::load_png_file(_path);

			std::vector<unsigned char> _newPixels(_image.size_bytes(), 0);
			auto _iter = _newPixels.begin();

			const size_t _eachRowSize = _image.width() / 12;
			const size_t _eachRowSizeBytes = _eachRowSize * _image.encoding().bytes_per_pixel();

			for (size_t _at = 0; _at != 12; ++_at)
			{
				for (size_t _rowNumber = 0; _rowNumber != _image.height(); ++_rowNumber)
				{
					auto _row = std::span{ _image.row(_rowNumber).begin() + (_eachRowSizeBytes * _at), _eachRowSizeBytes };
					std::ranges::copy(_row, reinterpret_cast<std::byte*>(&*_iter));
					_iter += _eachRowSizeBytes;
				};
			};

			const auto r = lodepng::encode(_outputPath.generic_string(), _newPixels, _image.width() / 12, _image.height() * 12);
			if (r != 0)
			{
				std::cout << lodepng_error_text(r) << '\n';
			};
		};
		void split_textures_horizontally()
		{
			const std::filesystem::path _path = SOURCE_ROOT "/assets/chess_pieces.png";
			const std::filesystem::path _outputPath = SOURCE_ROOT "/assets/chess_pieces_out.png";

			auto _image = lbx::chess_view::load_png_file(_path);
			const auto _bbp = _image.encoding().bytes_per_pixel();

			std::vector<unsigned char> _newPixels(_image.size_bytes(), 0);

			lbx::chess_view::Image::size_type _topRow = 0;
			lbx::chess_view::Image::size_type _bottomRow = _image.height() / 2;
			size_t _outRow = 0;
			size_t _bytesPerRow = _image.width() * _image.encoding().bytes_per_pixel();

			for (; _outRow != _image.height() / 2; ++_outRow)
			{
				auto _outData = reinterpret_cast<std::byte*>(_newPixels.data() + _outRow * _bytesPerRow * 2);
				std::ranges::copy(_image.row(_topRow), _outData);
				std::ranges::copy(_image.row(_bottomRow), _outData + _bytesPerRow);
				++_topRow;
				++_bottomRow;
			};

			const auto _result = lodepng::encode(_outputPath.generic_string(), _newPixels, _image.width() * 2, _image.height() / 2);
			if (_result != 0)
			{
				std::cout << lodepng_error_text(_result) << '\n';
			};

		};
	};


};