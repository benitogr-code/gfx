#version 410 core

in vec3 vNormal;

out vec4 FragColor;

void main() {
    FragColor = vec4(vNormal.x, vNormal.y, vNormal.z, 1.0f);
}