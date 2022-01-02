#version 410 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 mtx_model;
uniform mat4 mtx_view;
uniform mat4 mtx_viewProj;

out vec3 vNormal;

void main() {
    vNormal = aNormal;
    gl_Position = mtx_viewProj * mtx_model * vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
}