#include "font.hpp"

#include <cppfreetype/face.hpp>
#include <cppfreetype/bitmap.hpp>

#include <jclib/gl/gl.hpp>
#include <jclib/gl/glparam.hpp>

#include <jclib/maybe.h>
#include <jclib/config.h>
#include <jclib/memory.h>

#include <lodepng.h>

#include <map>
#include <format>
#include <ranges>
#include <iostream>

namespace gl = jc::gl;

namespace lbx::text
{
	inline LoadedFontFace make_font_face_glyphs(ft::library_ptr _lib, ft::face_ptr _face, uint32_t _maxCodepoint, ft::error& _err)
	{
		// Output face
		LoadedFontFace _loaded{};

		// Reserve glyphs
		_loaded.reserve_codepoint_range(_maxCodepoint);

		// Set any face-wide metrics
		_loaded.x_scale_ = _face->size->metrics.x_scale;
		_loaded.y_scale_ = _face->size->metrics.y_scale;

		// Glyph output iterator
		auto _glyphIter = _loaded.glyphs_.begin();

		// Loop through codepoints
		for (uint32_t _codepoint = 0; _codepoint != _maxCodepoint; ++_codepoint)
		{
			// Get the glyph index, skip if not found
			const auto _index = ft::get_char_index(_face, _codepoint);
			if (_index == 0)
			{
				continue;
			};

			// Load the glyph
			if (!ft::load_glyph(_face, _index, _err))
			{
				// TODO: Proper error handling such as continuing or checking error code
				return LoadedFontFace{};
			};

			// TOOD: Handle bitmap glyph format
			JCLIB_ASSERT(_face->glyph->format != FT_GLYPH_FORMAT_BITMAP);

			// Render the glyph
			_err = ft::error_code(FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_NORMAL));
			if (_err)
			{
				// TODO: Proper error handling such as continuing or checking error code
				return LoadedFontFace{};
			};

			// Assign our in-memory representation
			_glyphIter->codepoint = _codepoint;
			_glyphIter->metrics = _face->glyph->metrics;
			_glyphIter->bitmap = _face->glyph->bitmap;
			_glyphIter->bitmap_left = _face->glyph->bitmap_left;
			_glyphIter->bitmap_top = _face->glyph->bitmap_top;

			// Advance the glyph output iterator for the next glyph
			++_glyphIter;
		};

		// Remove any unused codepoint's data and resort container
		_loaded.erase_unused_codepoints();
		_loaded.sort_glyphs();

		// Return finished object
		return _loaded;
	};

	/**
	 * @brief Sets an opengl 2D texture array to contain a font face's glyphs.
	 *
	 * @param _texture OpenGL 2D Texture Array objectID
	 * @param _face Loaded font face to use
	*/
	inline void set_face_texture(jc::gl::texture_id _texture, const LoadedFontFace& _face)
	{
		namespace gl = jc::gl;

		// Allocate storage
		gl::set_storage_3D
		(
			_texture, gl::internal_format::r8,
			_face.width_px_, _face.height_px_, _face.glyphs_.size(),
			0
		);

		// TODO: Handle other pixel modes
		JCLIB_ASSERT(_face.glyphs_.front().bitmap.pixel_mode_ == ft::pixel_mode(FT_PIXEL_MODE_GRAY));

		// Set each layer in the texture to a glyph
		GLsizei _glyphNum = 0;
		for (auto& g : _face.glyphs_)
		{
			// Upload image data for the glyph
			auto& _bmap = g.bitmap;
			gl::set_subimage_3D
			(
				_texture, gl::format::red,
				gl::pixel_typecode::gl_unsigned_byte, _bmap.data(),
				_bmap.width_, _bmap.height_, 1,
				g.bitmap_left, g.bitmap_top, _glyphNum
			);

			++_glyphNum;
		};
	};




	inline std::optional<LoadedFontFace> load_font_face_file(ft::library_ptr _freetype, const char* _fontFilePath,
		FontSize_Pixels _sizePx, uint32_t _maxCodepoint)
	{
		ft::error _err{};
		
		LoadedFontFace _outFace{};

		auto _face = ft::load_font_file(_freetype, _fontFilePath, _err);
		if (_err)
		{
			return std::nullopt;
		};
		
		ft::set_pixel_sizes(_face, _sizePx.width, _sizePx.height, _err);
		
		if (_err)
		{
			return std::nullopt;
		};

		_outFace = make_font_face_glyphs(_freetype, _face, std::min(_maxCodepoint, (uint32_t)_face->num_glyphs), _err);
		if (_err)
		{
			return std::nullopt;
		};

		struct Size
		{
			uint32_t width;
			uint32_t height;
		};
		const auto _maxSize = jc::accumulate
		(
			_outFace,
			[](Size _max, const LoadedGlyph& _val) -> Size
			{
				_max.height = std::max(_max.height, _val.bitmap.height_);
				_max.width = std::max(_max.width, _val.bitmap.width_);
				return _max;
			},
			Size{}
		);

		_outFace.width_px_ = _maxSize.width;
		_outFace.height_px_ = _maxSize.height;

		// Make texture
		return _outFace;
	};

	std::optional<LoadedFontFace> load_font_face_file(const char* _fontFilePath, FontSize_Pixels _sizePx, uint32_t _maxCodepoint)
	{
		auto _ft = ft::new_library();
		return load_font_face_file(_ft.get(), _fontFilePath, _sizePx, _maxCodepoint);
	};
	std::optional<LoadedFontFace> load_font_face_file(const std::filesystem::path& _fontFilePath, FontSize_Pixels _sizePx, uint32_t _maxCodepoint)
	{
		const auto _filepathStr = _fontFilePath.generic_string();
		return load_font_face_file(_filepathStr.c_str(), _sizePx, _maxCodepoint);
	};

};