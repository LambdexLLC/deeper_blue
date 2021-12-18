#pragma once

#include "font.hpp"

#include <jclib/memory.h>

#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

#include <map>
#include <string>
#include <string_view>

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

		void set_model_uniform();

		struct TextBlock
		{
			std::string text;
			float x;
			float y;
		};

		// Recalculates and uploads all gl data
		void buffer_data();

	public:

		using TextBlockID = uint32_t;

		void configure_attributes(jc::gl::program_id _program);

		bool init();

		void draw();

		TextBlockID add_text(const std::string_view _text, float _x, float _y);
		
		void set_text(TextBlockID _textBlock, const std::string_view _text);

		/**
		 * @brief Sets the position of a text block.
		 * @param _textBlock Text block ID.
		 * @param _x X position in pixels.
		 * @param _y Y position in pixels.
		*/
		void set_text_position(TextBlockID _textBlock, float _x, float _y);


		void append_text(TextBlockID _textBlock, const std::string_view _appendText);


		TextArtist(jc::reference_ptr<LoadedFontFace> _font);
		TextArtist(jc::reference_ptr<LoadedFontFace> _font, jc::gl::program_id _shaderProgram);


		// Transform values

		glm::vec3 position_{ 0.0f };
		glm::vec3 scale_{ 1.0f };
		glm::quat rotation_{};

	private:

		std::map<TextBlockID, TextBlock> blocks_{};
		TextBlockID block_counter_ = 0;

		jc::reference_ptr<LoadedFontFace> font_;
		jc::gl::unique_texture texture_;
		jc::gl::program_id shader_{};
		jc::gl::unique_vao vao_;
		jc::gl::unique_vbo vbo_;

		jc::gl::uniform_location model_uni_;

		size_t vertices_ = 0;
	};

};
