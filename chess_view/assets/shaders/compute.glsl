#version 450 core

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;
layout(rgba32f, binding = 1) uniform image2D img_input;

void main()
{
  // get index in global work group i.e x,y position
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
 
  // base pixel colour for image
  vec4 pixel = imageLoad(img_input, pixel_coords);
  
  vec3 final = pixel.rgb * pixel_coords.x * 0.001;
  
  //
  // interesting stuff happens here later
  //
  
  // output to a specific pixel in the image
  imageStore(img_output, pixel_coords, vec4(final.rgb, 1.0));
}
