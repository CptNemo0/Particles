#version 430 core

in struct VsOut
{
  vec4 world_position;
} vertex_shader_output;


out vec4 color;

void main()
{
   color = vec4(0.4, 0.4, 0.2, 1.0);
}