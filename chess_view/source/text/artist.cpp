#include "artist.hpp"

#include <iostream>

#include <lodepng.h>

namespace gl = jc::gl;

namespace lbx::text
{
	/**
	 * @brief Creates an opengl 2D texture array containing a font's glyphs
	 * 
	 * @param _font Loaded font face to make texture for.
	 * @return OpenGL 2D texture array owning handle.
	*/
	inline jc::gl::unique_texture new_font_face_texture_sheet(const LoadedFontFace& _font)
	{
		auto _texture = gl::new_texture(gl::texture_target::array2D);
		
		gl::set(_texture, gl::texture_parameter::min_filter, GL_LINEAR);
		gl::set(_texture, gl::texture_parameter::mag_filter, GL_LINEAR);
		gl::set(_texture, gl::texture_parameter::wrap_s, GL_CLAMP_TO_EDGE);
		gl::set(_texture, gl::texture_parameter::wrap_t, GL_CLAMP_TO_EDGE);
		
		gl::set_storage_3D(_texture, gl::internal_format::r8, _font.width_px_, _font.height_px_, _font.glyphs_.size());


		std::vector<uint8_t> _fillData(_font.width_px_ * _font.height_px_, 0);

		size_t n = 0;
		for (auto& _glyph : _font.glyphs_)
		{
			auto _rowIter = _glyph.bitmap.begin();
			for (uint32_t _row = 0; _row != _glyph.bitmap.height_; ++_row)
			{
				gl::set_subimage_3D
				(
					_texture,
					gl::format::red, gl::pixel_typecode::gl_unsigned_byte, &*_rowIter, // input format
					_glyph.bitmap.width_, 1, 1, // size of input data
					0, _row, n					// output offset
				);
				_rowIter += _glyph.bitmap.width_;
			};
			++n;
		};

		return _texture;
	};


	inline void dump_texture_to_disk_grayscale(gl::texture_target _target, uint32_t _widthPx, uint32_t _heightPx)
	{
		const auto _path = SOURCE_ROOT "/gltexturedump.png";

		std::vector<unsigned char> _pixels(_widthPx * _heightPx, 0);
		glGetTexImage(jc::to_underlying(_target), 0, GL_RED, GL_UNSIGNED_BYTE, _pixels.data());

		const auto _err = lodepng::encode(_path, _pixels, _widthPx, _heightPx, LodePNGColorType::LCT_GREY);
		if (_err != 0)
		{
			std::cout << lodepng_error_text(_err) << '\n';
		};
	};


	void TextArtist::configure_attributes(gl::program_id _program)
	{
		this->shader_ = _program;

		gl::bind(this->vao_);

		gl::vertex_binding_index _baseBindingIdx{ 1 };
		gl::bind_vertex_buffer(_baseBindingIdx, this->vbo_, 0, sizeof(Vertex));
		gl::set_vertex_divisor(_baseBindingIdx, 0);

		gl::vertex_attribute_index _basePos = gl::get_program_resource_location(_program, gl::resource_type::program_input, "in_pos").value();
		gl::enable_attribute_array(_basePos);
		gl::set_attribute_binding(_basePos, _baseBindingIdx);

		gl::vertex_attribute_index _baseUVs = gl::get_program_resource_location(_program, gl::resource_type::program_input, "in_uvs").value();
		gl::enable_attribute_array(_baseUVs);
		gl::set_attribute_binding(_baseUVs, _baseBindingIdx);

		gl::set_attribute_format(_basePos, gl::typecode::gl_float, 2, false, 0);
		gl::set_attribute_format(_baseUVs, gl::typecode::gl_float, 3, false, sizeof(float) * 2);

	};

	bool TextArtist::init()
	{
		auto& _texture = this->texture_;
		auto& _font = *this->font_;
		auto& _vao = this->vao_;
		auto& _vbo = this->vbo_;

		// Create and upload the image
		_texture = new_font_face_texture_sheet(_font);
		gl::bind(_texture, gl::texture_target::array2D);

		_vao = gl::new_vao();
		gl::bind(_vao);

		_vbo = gl::new_vbo();
		gl::bind(_vbo, gl::vbo_target::array);

		int _penX = 0;
		int _penY = 0;

		const auto scale_x = [&](auto v)
		{
			return FT_MulFix(v, _font.x_scale_);
		};
		const auto scale_y = [&](auto v)
		{
			return FT_MulFix(v, _font.y_scale_);
		};

		std::vector<Vertex> _verts{};
		const auto add_rect = [&](size_t _index)
		{
			const LoadedGlyph& _glyph = _font.glyphs_.at(_index);

			
			std::array<Vertex, 4> _rectVerts{};
			auto& _ll = _rectVerts[0];
			auto& _lr = _rectVerts[1];
			auto& _ul = _rectVerts[2];
			auto& _ur = _rectVerts[3];

			const auto& _metrics = _glyph.metrics;

			const float _x = _metrics.horiBearingX;
			const float _y = -(_metrics.height - _metrics.horiBearingY);
			const auto _width = _metrics.width;
			const auto _height = _metrics.height;
			const auto _advance = _metrics.horiAdvance;

			const auto _texHeight = static_cast<float>(_glyph.bitmap.height_) / static_cast<float>(_font.height_px_);
			const auto _texWidth = static_cast<float>(_glyph.bitmap.width_) / static_cast<float>(_font.width_px_);

			const auto _texLeft = 0.0f;
			const auto _texBottom = 0.0f;
			const auto _texRight = _texLeft + _texWidth;
			const auto _texTop = _texBottom + _texHeight;


			// Set positions

			_ll.x = _x + _penX;
			_ul.x = _x + _penX;
			_lr.x = _ll.x + _width;
			_ur.x = _ul.x + _width;

			_ll.y = _y + _penY;
			_lr.y = _y + _penY;
			_ul.y = _ll.y + _height;
			_ur.y = _lr.y + _height;

			// UVs

			_ll.u = _texLeft;
			_ll.v = _texTop;
			_lr.u = _texRight;
			_lr.v = _texTop;
			_ul.u = _texLeft;
			_ul.v = _texBottom;
			_ur.u = _texRight;
			_ur.v = _texBottom;

			for (auto& v : _rectVerts)
			{
				v.layer = static_cast<float_t>(_index);
			};

			// Expanded verts and add to container
			{
				const auto _expandedVerts = std::array
				{
					_ll, _ul, _ur,
					_ll, _ur, _lr
				};
				_verts.insert(_verts.end(), _expandedVerts.begin(), _expandedVerts.end());
			};

			// Move pen
			_penX += _advance;

		};

		const std::string_view _someText = "your mom";
		for (auto& c : _someText)
		{
			add_rect(_font.find(c) - _font.glyphs_.begin());
		};

		gl::buffer_data(_vbo, _verts);
		this->vertices_ = _verts.size();

		return true;
	};

	void TextArtist::draw()
	{
		gl::bind(this->shader_);
		gl::bind(this->texture_, gl::texture_target::array2D);
		gl::bind(this->vao_);
		gl::draw_arrays(gl::primitive::triangles, this->vertices_);
	};




	TextArtist::TextArtist(jc::reference_ptr<LoadedFontFace> _font) :
		font_{ _font }
	{};

}