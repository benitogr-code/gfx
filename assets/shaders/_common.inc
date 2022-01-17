struct MainLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

layout(std140) uniform Camera {
  vec3 pos;
  mat4 view;
  mat4 viewproj;
} camera;

layout(std140) uniform Lights {
  MainLight main;
} lights;
