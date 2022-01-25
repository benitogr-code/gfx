#version 410 core

in vec3 vtx_texcoords;

struct Material {
  samplerCube cubemap_skybox;
};

uniform Material material;

out vec4 out_color;

void main() {
    out_color = texture(material.cubemap_skybox, vtx_texcoords);
}
