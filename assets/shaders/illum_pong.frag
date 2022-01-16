#version 410 core

//#common.inc

in vec3 vtx_fragpos;
in vec3 vtx_normal;
in vec2 vtx_texcoords;

struct Material {
  int       texture_slots;
  sampler2D texture_diffuse;
  sampler2D texture_specular;

  vec3      color;
  float     ambient;
  float     specular;
  float     shininess;
};

uniform Material material;

// Directional light
uniform vec3      light_pos;
uniform vec3      light_color;

out vec4 out_color;

void main() {
    vec3 amb_color = vec3(material.ambient, material.ambient, material.ambient);
    vec3 diff_color = material.color;
    vec3 spec_color = vec3(material.specular, material.specular, material.specular);

    if (material.texture_slots != 0) {
        diff_color = texture(material.texture_diffuse, vtx_texcoords).rgb;
        spec_color = texture(material.texture_specular, vtx_texcoords).rgb;
    }

    vec3 normal = normalize(vtx_normal);
    vec3 light_dir = normalize(light_pos - vtx_fragpos);

    vec3 ambient = amb_color * light_color;

    float diffuse_factor = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diffuse_factor * diff_color * light_color;

    vec3 view_dir = normalize(camera.pos - vtx_fragpos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec_factor = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = spec_factor * spec_color * light_color;

    vec3 result = ambient + diffuse + specular;
    out_color = vec4(result, 1.0);
}
