#version 410 core

//#common.inc

struct Material {
  vec3 color;
};

uniform Material material;

out vec4 out_color;

void main() {
    out_color = vec4(material.color.x, material.color.y, material.color.z, 1.0f);
}
