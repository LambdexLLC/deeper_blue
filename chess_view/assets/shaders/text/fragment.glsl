#version 450 core


// UV position + image layer

in vec3 frag_uvs;




uniform sampler2DArray glyph_texture_sheet;


out vec4 color;


// Temp constant for the text color
const vec3 frag_base_color = vec3(1.0);



void main()
{
	// Get and apply weight of this fragment to the text color
	float _weight = texture(glyph_texture_sheet, frag_uvs).r;
	vec4 _weightedColor = vec4(frag_base_color.rgb, _weight);

	color = _weightedColor;
};
