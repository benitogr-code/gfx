#version 410 core

layout (location = 0) in vec3 aPosisition;

uniform mat4 uMatrix;

void main() {
    gl_Position = uMatrix * vec4(aPosisition.x, aPosisition.y, aPosisition.z, 1.0);
}