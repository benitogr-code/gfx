#include "material.h"

#include <glad/glad.h>

Material::Material(ShaderRef shader)
  : _shader(shader) {
    _slots.fill(MaterialSlot());
}

/*static*/ MaterialRef Material::Create(ShaderRef shader) {
  MaterialRef material(new Material(shader));
  return material;
}

/*static*/ MaterialRef Material::Clone(MaterialRef material) {
  MaterialRef cloned(new Material(material->getShader()));
  cloned->_slots = material->_slots;
  cloned->_params = material->_params;

  return cloned;
}

void Material::apply() {
  uint activeSlots = 0;
  for (int i = 0; i < _slots.size(); ++i) {
    auto& slot = _slots[i];
    if (slot.texture) {
      glActiveTexture(GL_TEXTURE0 + i);
      _shader->setUniformInt(slot.name.c_str(), i);

      glBindTexture(slot.texture->target(), slot.texture->id());

      activeSlots |= BIT(i);
    }
  }

  _shader->setUniformUInt("material.active_slots", activeSlots);

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

void Material::setTextureSlot(MaterialSlotId id, const char* name, TextureRef texture) {
  if (id >= 0 && id < _slots.max_size()) {
    _slots[id].name = name;
    _slots[id].texture = texture;
  }
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
