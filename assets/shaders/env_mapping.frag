#version 410 core

//#common.inc

in VSOut {
  vec3 fragpos;
  vec3 normal;
} fs_in;

struct Material {
  samplerCube cubemap;
  int         refract;
  float       refract_index;
};

uniform Material material;

out vec4 out_color;

void main() {
  vec3 i = normalize(fs_in.fragpos - camera.pos);
  vec3 r;
  if (material.refract == 1) {
    float ratio = 1.00 / material.refract_index;
    r = refract(i, normalize(fs_in.normal), ratio);
  }
  else {
    r = reflect(i, normalize(fs_in.normal));
  }

  out_color = vec4(texture(material.cubemap, r).rgb, 1.0);
}
