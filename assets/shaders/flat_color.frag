#version 410 core

uniform vec3 mat_color;

out vec4 out_color;

void main() {
    out_color = vec4(color.x, color.y, color.z, 1.0f);
}