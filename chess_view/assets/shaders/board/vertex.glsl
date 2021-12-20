#version 450 core


// Data for the base

in vec3 base_pos;
in vec2 base_uvs;


// Data for each instance

in vec2 instance_pos;
in vec3 instance_background_color;
in float instance_piece;
in float instance_alpha;


// Outputs
out vec3 background_color;
out vec3  piece_uvs;
out float piece_alpha_mask;


// Size of each square, used for instance positioning
uniform vec2 square_size;

// Standard object uniforms
uniform mat4 model;

layout(std140) uniform UIView
{
	mat4 projection;
};



void main()
{
	// Forward fragment shader inputs
	background_color = instance_background_color;
	piece_uvs = vec3(base_uvs.xy, instance_piece);
	piece_alpha_mask = instance_alpha;
	
	// Instance position adjusted for board square size
	vec2 _adjustedInstancePos = instance_pos * square_size;

	// Calculate the position for this instance
	vec3 _realPos = vec3(base_pos.xy + _adjustedInstancePos.xy, base_pos.z);
	
	// Set the final position
	gl_Position = (projection * model) * vec4(_realPos.xyz, 1.0);
};