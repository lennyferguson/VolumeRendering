// Viewray Shader

#version 430 core

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

layout (location = 0) in vec4 vPosition;

out VS_OUT {
  vec4 Pt;
  vec3 tc;
  } vs_out;

void main(void)
{
    vec4 point = mv_matrix * vPosition;
    vs_out.Pt = point;
    vs_out.tc =  (vPosition.xyz + vec3(1.0)) * 0.5;
    gl_Position = proj_matrix * point;
}
