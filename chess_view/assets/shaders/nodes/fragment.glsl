#version 450 core

in vec3 frag_color;

out vec4 color;

void main()
{
	color = vec4(frag_color.rgb, 1.0);
};