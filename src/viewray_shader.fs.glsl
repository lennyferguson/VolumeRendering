#version 430 core

layout (location = 0) out vec4 color;

in VS_OUT
{
  vec4 Pt;
  vec3 tc;
} fs_in;

void main(void)
{
    color = vec4(fs_in.tc,1.0);
}
