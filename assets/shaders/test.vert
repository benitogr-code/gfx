#version 410 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModel;
uniform mat4 uViewProjection;

out vec3 vNormal;

void main() {
    vNormal = aNormal;
    gl_Position = uViewProjection * uModel * vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);
}