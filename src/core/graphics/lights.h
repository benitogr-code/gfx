#pragma once

struct DirectionalLight {
  glm::vec3 direction;
  ColorRGB  ambient;
  ColorRGB  diffuse;
  ColorRGB  specular;
};
