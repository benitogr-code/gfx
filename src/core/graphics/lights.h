#pragma once

struct Light {
  enum class Type {
    Directional = 0,
    Point,
  };

  struct Properties {
    Properties() {
      color = ColorRGB(1.0f);
      ambientMultiplier = 0.1f;
      specularMultiplier = 0.8f;
      attenuationConstant = 1.0f;
      attenuationLinear = 0.14f;
      attenuationQuadratic = 0.07f;
    }

    ColorRGB color;
    float    ambientMultiplier;
    float    specularMultiplier;
    float    attenuationConstant;
    float    attenuationLinear;
    float    attenuationQuadratic;
  };

  Light(Type type = Type::Directional)
    : type(type)
    , position(0.0f) {}

  Type       type;
  Properties properties;
  glm::vec3  position;
};
