#pragma once

#include "font.hpp"

#include <jclib/memory.h>

namespace lbx::text
{
	class TextArtist
	{
	private:

		struct Vertex
		{
			float x;
			float y;

			float u;
			float v;
			float layer;
		};

	public:

		void configure_attributes(jc::gl::program_id _program);

		bool init();

		void draw();

		TextArtist(jc::reference_ptr<LoadedFontFace> _font);

	private:

		jc::reference_ptr<LoadedFontFace> font_;
		jc::gl::unique_texture texture_;
		jc::gl::program_id shader_{};
		jc::gl::unique_vao vao_;
		jc::gl::unique_vbo vbo_;

		size_t vertices_ = 0;
	};

};
