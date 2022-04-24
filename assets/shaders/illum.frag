#version 410 core

//#common.inc

const uint SlotFlag_Diffuse  = 0x00000001u;
const uint SlotFlag_Specular = 0x00000002u;
const uint SlotFlag_Normal   = 0x00000004u;

in VSOut {
  vec3 fragpos;
  vec4 fragpos_lightspace;
  vec3 normal;
  vec2 texcoords;
  mat3 tbn;
} fs_in;

struct Material {
  uint      active_slots;
  sampler2D texture_diffuse;
  sampler2D texture_specular;
  sampler2D texture_normal;

  vec3     color;
  vec3     specular;
  float    shininess;
};

uniform Material  material;
uniform sampler2D shadow_depth_map;

out vec4 out_color;

vec3 calculateDirectionalLight(MainLight light, vec3 diffColor, vec3 specColor, vec3 normal, vec3 viewDir, float shininess, float shadow) {
  float shadowFactor = 1.0 - shadow;

  // Ambient
  vec3 ambient = light.ambient * diffColor;

  // Diffuse
  float diffuseFactor = max(dot(-light.direction, normal), 0.0);
  vec3 diffuse = light.diffuse * diffuseFactor * diffColor * shadowFactor;

  // Specular
  vec3 reflectDir = reflect(light.direction, normal);
  float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  vec3 specular = light.specular * specularFactor * specColor * shadowFactor;

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

float shadowFactor(vec4 fragpos, float bias) {
  vec3 projCoords = fragpos.xyz / fragpos.w;
  projCoords = (projCoords * 0.5) + vec3(0.5);

  float fragDepth = projCoords.z;
  if(fragDepth > 1.0)
    return 0.0;

  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadow_depth_map, 0);
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      float pcfDepth = texture(shadow_depth_map, projCoords.xy + vec2(x, y) * texelSize).r;
      shadow += fragDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }

  return (shadow /= 9.0);
}

void main() {
  vec3 diffColor = material.color;
  vec3 specColor = material.specular;
  float shininess = material.shininess;

  vec3 normal = normalize(fs_in.normal);
  vec3 viewDir = normalize(camera.pos.xyz - fs_in.fragpos);

  if ((material.active_slots & SlotFlag_Diffuse) != 0) {
      diffColor = texture(material.texture_diffuse, fs_in.texcoords).rgb;
  }
  if ((material.active_slots & SlotFlag_Specular) != 0) {
      specColor = texture(material.texture_specular, fs_in.texcoords).rgb;
  }
  if ((material.active_slots & SlotFlag_Normal) != 0) {
    normal = texture(material.texture_normal, fs_in.texcoords).rgb;
    normal = normalize(normal * 2.0f - 1.0f);
    normal = normalize(fs_in.tbn * normal);
  }

  float shadowBias = max(0.0025 * (1.0 - dot(normal, lights.main.direction)), 0.0005);
  float shadow = shadowFactor(fs_in.fragpos_lightspace, shadowBias);

  vec3 result = vec3(0.0, 0.0, 0.0);

  result += calculateDirectionalLight(lights.main, diffColor, specColor, normal, viewDir, shininess, shadow);

  for (int i = 0; i < lights.numPointLights; ++i) {
    result += calculatePointLight(lights.points[i], diffColor, specColor, fs_in.fragpos, normal, viewDir, shininess);
  }

  out_color = vec4(result, 1.0);
}
