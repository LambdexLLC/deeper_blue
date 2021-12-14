#version 450 core

// Background color for the square
in vec3 background_color;

// The UV coordinates for the board piece to display
in vec3 piece_uvs;

// Alpha mask to allow empty squares
in float piece_alpha_mask;

// Texture sheet containing the chess piece textures
uniform sampler2DArray piece_textures;



out vec3 color;


void main()
{
	vec4 _rawTexel = texture(piece_textures, piece_uvs).rgba * 0.8;
	vec4 _pieceColor = vec4(_rawTexel.rgb, min(_rawTexel.a, piece_alpha_mask));

	// Mix piece color into background
	vec4 _finalColor = mix(vec4(background_color.rgb, 1.0), _pieceColor, _pieceColor.a);
	
	color = _finalColor.rgb;
};