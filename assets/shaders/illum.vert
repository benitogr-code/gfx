#version 410 core

//#common.inc

layout (location = 0) in vec3 attr_pos;
layout (location = 1) in vec3 attr_normal;
layout (location = 2) in vec2 attr_texcoords;
layout (location = 3) in vec3 attr_tangent;

uniform mat4 mtx_model;

out vec3 vtx_fragpos;
out vec3 vtx_normal;
out vec2 vtx_texcoords;
out mat3 vtx_tbn;

void main() {
    vec3 t = normalize(vec3(mtx_model * vec4(attr_tangent, 0.0f)));
    vec3 n = normalize(vec3(mtx_model * vec4(attr_normal, 0.0f)));
    vec3 b = cross(n, t);

    vtx_fragpos = vec3(mtx_model * vec4(attr_pos, 1.0));
    vtx_normal = attr_normal;
    vtx_texcoords = attr_texcoords;
    vtx_tbn = mat3(t, b, n);

    gl_Position = camera.viewproj * mtx_model * vec4(attr_pos.x, attr_pos.y, attr_pos.z, 1.0);
}
