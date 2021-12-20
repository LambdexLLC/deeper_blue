#version 450 core


// Inputs

in vec2 in_pos; // xy positions
in vec3 in_uvs;	// uv values + image layer


// Outputs to fragment shader

out vec3 frag_uvs;


// Standard uniform(s)

layout(std140) uniform UIView
{
	mat4 projection;
};
uniform mat4 model;


void main()
{
	frag_uvs = in_uvs;

	vec2 _pxPos = in_pos / 64.0;
	
	
	gl_Position = (projection * model) * vec4(_pxPos.xy, 0.2, 1.0);
};