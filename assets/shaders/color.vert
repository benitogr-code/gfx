#version 410 core

//#common.inc

layout (location = 0) in vec3 attr_position;

uniform mat4 mtx_model;

void main() {
    gl_Position = camera.viewproj * mtx_model * vec4(attr_position, 1.0);
}
