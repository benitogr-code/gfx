#version 410 core

layout (location = 0) in vec3 aPosisition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uMatrix;

void main() {
    gl_Position = uMatrix * vec4(aPosisition.x, aPosisition.y, aPosisition.z, 1.0);
}