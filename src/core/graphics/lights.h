#pragma once

struct DirectionalLight {
  glm::vec3 direction;
  ColorRGB  ambient;
  ColorRGB  diffuse;
  ColorRGB  specular;
};

struct PointLight {
  glm::vec3 position;
  ColorRGB  ambient;
  ColorRGB  diffuse;
  ColorRGB  specular;
  float attConstant;
  float attLinear;
  float attQuadratic;
};
