#version 410 core

//#common.inc

in vec3 vtx_fragpos;
in vec3 vtx_normal;
in vec2 vtx_texcoords;

struct Material {
  int       texture_slots;
  sampler2D texture_diffuse;
  sampler2D texture_specular;

  vec3     color;
  vec3     specular;
  float    shininess;
};

uniform Material material;

out vec4 out_color;

vec3 calculateDirectionalLight(MainLight light, vec3 diffColor, vec3 specColor, vec3 normal, vec3 viewDir, float shininess) {
  // Ambient
  vec3 ambient = light.ambient * diffColor;

  // Diffuse
  float diffuseFactor = max(dot(-light.direction, normal), 0.0);
  vec3 diffuse = light.diffuse * diffuseFactor * diffColor;

  // Specular
  vec3 reflectDir = reflect(light.direction, normal);
  float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  vec3 specular = light.specular * specularFactor * specColor;

  return (ambient + diffuse + specular);
}

vec3 calculatePointLight(PointLight light, vec3 diffColor, vec3 specColor, vec3 fragmentPos, vec3 normal, vec3 viewDir, float shininess) {
  vec3 lightDir = normalize(light.position - fragmentPos);

  // Ambient
  vec3 ambient = light.ambient * diffColor;

  // Diffuse
  float diffuseFactor = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = light.diffuse * diffuseFactor * diffColor;

  // Specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  vec3 specular = light.specular * specularFactor * specColor;

  // Attenuation
  float distance = length(light.position - fragmentPos);
  float attenuation = 1.0 / (light.attConstant + (light.attLinear * distance) + (light.attQuadratic * distance * distance));

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);
}


void main() {
  vec3 diffColor = material.color;
  vec3 specColor = material.specular;
  float shininess = material.shininess;

  if (material.texture_slots != 0) {
      diffColor = texture(material.texture_diffuse, vtx_texcoords).rgb;
      specColor = texture(material.texture_specular, vtx_texcoords).rgb;
  }

  vec3 normal = normalize(vtx_normal);
  vec3 viewDir = normalize(camera.pos.xyz - vtx_fragpos);

  vec3 result = vec3(0.0, 0.0, 0.0);

  result += calculateDirectionalLight(lights.main, diffColor, specColor, normal, viewDir, shininess);

  for (int i = 0; i < lights.numPointLights; ++i) {
    result += calculatePointLight(lights.points[i], diffColor, specColor, vtx_fragpos, normal, viewDir, shininess);
  }

  out_color = vec4(result, 1.0);
}
