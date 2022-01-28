#version 410 core

//#common.inc

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec3 attr_normal;

uniform mat4 mtx_model;

out vec3 vtx_fragpos;
out vec3 vtx_normal;

void main() {
  //vtx_normal = mat3(transpose(inverse(mtx_model))) * attr_normal;
  vtx_normal = vec3(mtx_model * vec4(attr_normal, 0.0f));
  vtx_fragpos = vec3(mtx_model * vec4(attr_position, 1.0));
  gl_Position = camera.viewproj * vec4(vtx_fragpos, 1.0);
}