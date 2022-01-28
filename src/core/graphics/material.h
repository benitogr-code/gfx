#pragma once

#include "shader.h"
#include "texture.h"

class Material;
typedef std::shared_ptr<Material> MaterialRef;

enum TextureType {
  // 2D Textures
  TextureType_Diffuse = 0,
  TextureType_Specular,
  TextureType_Normal,
  // 3D Textures
  TextureType_Cubemap_Skybox,

  TextureType_Count
};

enum MaterialParamType {
  MaterialParamType_Float = 0,
  MaterialParamType_Int,
  MaterialParamType_Vec3
};

enum MaterialSlotId {
  MaterialSlotId_0 = 0,
  MaterialSlotId_1,
  MaterialSlotId_2,
  MaterialSlotId_3,
  MaterialSlotId_Count,
  MaterialSlotId_Invalid = MaterialSlotId_Count,
};

struct MaterialParam {
  MaterialParamType type;
  union {
    float      _f;
    int        _i;
    glm::vec3  _vec3;
  } value;
};

struct MaterialSlot {
  std::string    name;
  TextureRef     texture;
};

class Material {
private:
  typedef std::array<MaterialSlot, MaterialSlotId_Count> MaterialSlots;
  typedef std::map<std::string, MaterialParam> MaterialParams;

public:
  void apply();

  ShaderRef& getShader() { return _shader; }

  void setTextureSlot(MaterialSlotId id, const char* name, TextureRef texture);
  void setParamFloat(const char* name, float value);
  void setParamInt(const char* name, int value);
  void setParamVec3(const char* name, const glm::vec3& value);

  static MaterialRef Create(ShaderRef shader);
  static MaterialRef Clone(MaterialRef material);

private:
  Material(ShaderRef shader);
  Material(Material& material) = delete;

private:
  ShaderRef _shader;
  MaterialSlots  _slots;
  MaterialParams _params;
};
