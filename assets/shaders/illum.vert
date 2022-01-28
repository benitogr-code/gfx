#version 410 core

//#common.inc

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_normal;
layout (location = 2) in vec2 attr_texcoords;
layout (location = 3) in vec3 attr_tangent;

uniform mat4 mtx_model;

out VSOut {
  vec3 fragpos;
  vec3 normal;
  vec2 texcoords;
  mat3 tbn;
} vs_out;

void main() {
    vec3 t = normalize(vec3(mtx_model * vec4(attr_tangent, 0.0f)));
    vec3 n = normalize(vec3(mtx_model * vec4(attr_normal, 0.0f)));
    vec3 b = cross(n, t);

    vs_out.fragpos = vec3(mtx_model * vec4(attr_pos, 1.0));
    vs_out.normal = vec3(mtx_model * vec4(attr_normal, 0.0f));
    vs_out.texcoords = attr_texcoords;
    vs_out.tbn = mat3(t, b, n);

    gl_Position = camera.viewproj * mtx_model * vec4(attr_pos, 1.0);
}
