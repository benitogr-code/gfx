#version 410 core

in VSOut {
  vec2 texcoords;
} fs_in;

uniform sampler2D texture_font;

out vec4 out_color;

void main() {
  out_color = vec4(1.0, 1.0, 1.0, texture(texture_font, fs_in.texcoords).r);
}
