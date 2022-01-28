#version 410 core

//#common.inc

layout (location = 0) in vec3 attr_position;

out VSOut {
  vec3 texcoords;
} vs_out;

void main() {
  vs_out.texcoords = attr_position;
  vec4 position = camera.proj * camera.viewrot * vec4(attr_position, 1.0);
  gl_Position = position.xyww;
}
