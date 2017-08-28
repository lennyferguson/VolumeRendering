// Volume Vertex Shader

#version 430 core

// Creates Quad that fills screen
layout (location = 0) in vec4 vPosition;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out VS_OUT {
  vec3 V;
  } vs_out;

void main(void) {
  vec4 point = mv_matrix * vPosition;
  vec3 P = point.xyz;
  vs_out.V = normalize(-P);
  gl_Position = proj_matrix * point;
}
