#version 410 core

//#common.inc

layout (location = 0) in vec3 attr_position;
layout (location = 1) in vec3 attr_normal;
layout (location = 2) in vec2 attr_texcoords;

uniform mat4 mtx_model;
uniform mat4 mtx_view;
uniform mat4 mtx_viewProj;

void main() {
    gl_Position = mtx_viewProj * mtx_model * vec4(attr_position.x, attr_position.y, attr_position.z, 1.0);
}
