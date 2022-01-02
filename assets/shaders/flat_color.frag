#version 410 core

in vec3 vtx_normal;

out vec4 out_color;

void main() {
    vec3 color = (vtx_normal + vec3(1.0f, 1.0f, 1.0f)) * 0.5f;
    out_color = vec4(color.x, color.y, color.z, 1.0f);
}