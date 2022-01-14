#version 410 core

uniform vec3 mat_color;

out vec4 out_color;

void main() {
    out_color = vec4(mat_color.x, mat_color.y, mat_color.z, 1.0f);
}
