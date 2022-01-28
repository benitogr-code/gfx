#version 410 core

//#common.inc

in vec3 vtx_fragpos;
in vec3 vtx_normal;

struct Material {
  samplerCube cubemap;
  int         refract;
  float       refract_index;
};

uniform Material material;

out vec4 out_color;

void main() {
  vec3 I = normalize(vtx_fragpos - camera.pos);
  vec3 R;
  if (material.refract == 1) {
    float ratio = 1.00 / material.refract_index;
    R = refract(I, normalize(vtx_normal), ratio);
  }
  else {
    R = reflect(I, normalize(vtx_normal));
  }

  out_color = vec4(texture(material.cubemap, R).rgb, 1.0);
}
