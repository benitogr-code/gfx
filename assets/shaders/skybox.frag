#version 410 core

in VSOut {
  vec3 texcoords;
} fs_in;

struct Material {
  samplerCube cubemap_skybox;
};

uniform Material material;

out vec4 out_color;

void main() {
    out_color = texture(material.cubemap_skybox, fs_in.texcoords);
}
