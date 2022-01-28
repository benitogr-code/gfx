#version 410 core

in VSOut {
  vec2 texcoords;
  vec3 color;
} fs_in;

uniform sampler2D texture_font;

out vec4 out_color;

void main() {
  out_color = vec4(fs_in.color, texture(texture_font, fs_in.texcoords).r);
}
