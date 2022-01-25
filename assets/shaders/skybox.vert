#version 410 core

//#common.inc

layout (location = 0) in vec3 attr_position;

out vec3 vtx_texcoords;

void main() {
  vtx_texcoords = attr_position;
  vec4 position = camera.proj * camera.viewrot * vec4(attr_position.x, attr_position.y, attr_position.z, 1.0);
  gl_Position = position.xyww;
}
