#version 410 core

//#common.inc

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec2 attr_texcoords;
layout (location = 2) in vec3 attr_color;

out VSOut {
  vec2 texcoords;
  vec3 color;
} vs_out;

void main() {
  float x = ((attr_position.x / camera.viewport.x) * 2.0f) - 1.0f;
  float y = ((attr_position.y / camera.viewport.y) * 2.0f) - 1.0f;

  vs_out.texcoords = attr_texcoords;
  vs_out.color = attr_color;
  gl_Position = vec4(x, y, attr_position.z, 1.0f);
}
