#version 410 core

layout (location = 0) in vec3 attr_position;

uniform mat4 mtx_light_vp;
uniform mat4 mtx_model;

void main() {
    gl_Position = mtx_light_vp * mtx_model * vec4(attr_position, 1.0);
}
