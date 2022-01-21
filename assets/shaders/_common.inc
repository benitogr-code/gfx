struct MainLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float attConstant;
  float attLinear;
  float attQuadratic;
};

#define MAX_POINT_LIGHTS 8

layout(std140) uniform Camera {
  vec3 pos;
  mat4 view;
  mat4 viewproj;
} camera;

layout(std140) uniform Lights {
  MainLight main;
  int  numPointLights;
  PointLight points[MAX_POINT_LIGHTS];
} lights;
