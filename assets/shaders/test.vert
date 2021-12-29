#version 410 core

layout (location = 0) in vec3 aPosisition;

void main() {
    gl_Position = vec4(aPosisition.x, aPosisition.y, aPosisition.z, 1.0);
}