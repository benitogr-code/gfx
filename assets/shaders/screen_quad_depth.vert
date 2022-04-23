#version 410 core

layout (location = 0) in vec3 attr_position;
layout (location = 2) in vec2 attr_texcoords;

uniform mat4 mtx_model;

out VSOut {
  vec2 texcoords;
} vs_out;

void main() {
  vs_out.texcoords = attr_texcoords;
  gl_Position = mtx_model * vec4(attr_position, 1.0);
}
