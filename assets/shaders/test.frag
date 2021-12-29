#version 410 core

uniform vec3 uColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(uColor.x, uColor.y, uColor.z, 1.0f);
}