#version 410 core

in vec3 vNormal;

out vec4 FragColor;

void main() {
    vec3 color = (vNormal + vec3(1.0f, 1.0f, 1.0f)) * 0.5f;
    FragColor = vec4(color.x, color.y, color.z, 1.0f);
}