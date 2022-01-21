#pragma once

#include "shader.h"
#include "texture.h"

class Material;
typedef std::shared_ptr<Material> MaterialRef;

enum TextureType {
  TextureType_Diffuse = 0,
  TextureType_Specular,
  TextureType_Count
};

enum MaterialParamType {
  MaterialParamType_Float = 0,
  MaterialParamType_Vec3
};

struct MaterialParam {
  MaterialParamType type;
  union {
    float      _f;
    glm::vec3  _vec3;
  } value;
};

class Material {
private:
  typedef std::array<TextureRef, TextureType_Count> TextureSlots;
  typedef std::map<std::string, MaterialParam> MaterialParams;

public:
  void apply();

  ShaderRef& getShader() { return _shader; }

  void setTexture(TextureType type, TextureRef texture);
  void setParamFloat(const char* name, float value);
  void setParamVec3(const char* name, const glm::vec3& value);

  static MaterialRef Create(ShaderRef shader);
  static MaterialRef Clone(MaterialRef material);

private:
  Material(ShaderRef shader);
  Material(Material& material) = delete;

private:
  ShaderRef _shader;
  TextureSlots   _textures;
  MaterialParams _params;
};
