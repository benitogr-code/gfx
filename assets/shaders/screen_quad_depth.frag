#version 410  core

in VSOut {
  vec2 texcoords;
} fs_in;

out vec4 out_color;

uniform sampler2D depth_map;

void main() {
  float depth = texture(depth_map, fs_in.texcoords).r;
  out_color = vec4(vec3(depth), 1.0);
}
