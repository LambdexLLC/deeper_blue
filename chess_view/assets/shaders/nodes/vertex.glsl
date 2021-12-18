#version 450 core

// Inputs

in vec2 in_pos;
in vec3 in_color;

out vec3 frag_color;

void main()
{
	frag_color = in_color;
	
	vec2 _pxPos = in_pos / 64.0;
	gl_Position = vec4(_pxPos.xy, 0.0, 1.0);
};