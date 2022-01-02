#version 410 core

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_normal;
layout (location = 2) in vec2 attr_texcoords;

uniform mat4 mtx_model;
uniform mat4 mtx_view;
uniform mat4 mtx_viewProj;

out vec3 vtx_fragpos;
out vec3 vtx_normal;
out vec2 vtx_texcoords;

void main() {
    vtx_fragpos = vec3(mtx_model * vec4(attr_pos, 1.0));
    vtx_normal = attr_normal;
    vtx_texcoords = attr_texcoords;

    gl_Position = mtx_viewProj * mtx_model * vec4(attr_pos.x, attr_pos.y, attr_pos.z, 1.0);
}