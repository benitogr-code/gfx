#version 410 core

//#common.inc

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec3 attr_normal;

uniform mat4 mtx_model;

out VSOut {
  vec3 fragpos;
  vec3 normal;
} vs_out;

void main() {
  vs_out.normal = vec3(mtx_model * vec4(attr_normal, 0.0f));
  vs_out.fragpos = vec3(mtx_model * vec4(attr_position, 1.0));

  gl_Position = camera.viewproj * vec4(vs_out.fragpos, 1.0);
}
