#version 410 core

uniform vec3 flat_color;

out vec4 out_color;

void main() {
    out_color = vec4(flat_color.x, flat_color.y, flat_color.z, 1.0f);
}