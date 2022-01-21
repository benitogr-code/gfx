#include "material.h"

#include <glad/glad.h>

const char* TEXTURE_SLOT_NAMES[TextureType_Count] = {
  "material.texture_diffuse",
  "material.texture_specular"
};

Material::Material(ShaderRef shader)
  : _shader(shader) {
    _textures.fill(nullptr);
}

/*static*/ MaterialRef Material::Create(ShaderRef shader) {
  MaterialRef material(new Material(shader));
  return material;
}

/*static*/ MaterialRef Material::Clone(MaterialRef material) {
  MaterialRef cloned(new Material(material->getShader()));
  cloned->_textures = material->_textures;
  cloned->_params = material->_params;

  return cloned;
}

void Material::apply() {
  int textureSlots = 0;
  for (int slot = 0; slot < _textures.size(); ++slot) {
    auto texture = _textures[slot];
    if (texture) {
      glActiveTexture(GL_TEXTURE0 + slot);
      _shader->setUniformInt(TEXTURE_SLOT_NAMES[slot], slot);
      glBindTexture(GL_TEXTURE_2D, texture->id());

      textureSlots |= BIT(slot);
    }
  }

  _shader->setUniformInt("material.texture_slots", textureSlots);

  for (auto iter = _params.begin(); iter != _params.end(); ++iter) {
    switch (iter->second.type)
    {
    case MaterialParamType_Float:
      _shader->setUniformFloat(iter->first.c_str(), iter->second.value._f);
      break;

    case MaterialParamType_Vec3:
      _shader->setUniformVec3(iter->first.c_str(), iter->second.value._vec3);
      break;

    default:
      break;
    }
  }
}

void Material::setTexture(TextureType type, TextureRef texture) {
  _textures[type] = texture;
}

void Material::setParamFloat(const char* name, float value) {
  MaterialParam param;
  param.type = MaterialParamType_Float;
  param.value._f = value;
  _params.insert_or_assign(std::string(name), param);
}

void Material::setParamVec3(const char* name, const glm::vec3& value) {
  MaterialParam param;
  param.type = MaterialParamType_Vec3;
  param.value._vec3 = value;
  _params.insert_or_assign(std::string(name), param);
}
