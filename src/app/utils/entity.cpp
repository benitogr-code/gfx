#include "entity.h"
#include "core/renderer.h"

Entity::Entity() {
  _position = glm::vec3(0.0f);
  _rotation = glm::identity<glm::quat>();
  _scale = glm::vec3(1.0f);
  _flags = 0;
  updateWorldTM();
}

void Entity::setFlag(Flags flag, bool set) {
  if (set)
    _flags |= (uint32_t)flag;
  else
    _flags &= ~(uint32_t)flag;
}

bool Entity::hasFlag(Flags flag) const {
  return (_flags & (uint32_t)flag) != 0;
}

void Entity::setPosition(const glm::vec3& position) {
  _position = position;
  updateWorldTM();
}

void Entity::setRotation(const glm::quat& rotation) {
  _rotation = rotation;
  updateWorldTM();
}

void Entity::setScale(const glm::vec3& scale) {
  _scale = scale;
  updateWorldTM();
}

void Entity::attachModel(GfxModelRef model) {
  _model = model;
}

void Entity::attachLight(const Light::Properties& properties) {
  _light.reset(new Light(Light::Type::Point));
  _light->position = _position;
  _light->properties = properties;
}

void Entity::cloneModelMaterial() {
  if (_model) {
    _overrideMaterial = Material::Clone(_model->getMaterial());
  }
}

MaterialRef Entity::getModelMaterial() const {
  if (_overrideMaterial) {
    return _overrideMaterial;
  }

  return _model != nullptr ? _model->getMaterial() : nullptr;
}


void Entity::render(Renderer& renderer) {
  if (hasFlag(Flags::Hidden)) return;

  if (_light) {
    renderer.drawLight(*_light);
  }

  if (_model) {
    MaterialRef material = _overrideMaterial ? _overrideMaterial : _model->getMaterial();
    for (uint32_t idx = 0; idx < _model->getMeshCount(); ++idx) {
      renderer.drawMesh(_model->getMesh(idx), material, _worldTM);
    }
  }
}

void Entity::updateWorldTM() {
  auto s = glm::scale(glm::mat4(1.0f), _scale);
  auto r = glm::toMat4(_rotation);
  auto t = glm::translate(glm::mat4(1.0f), _position);

  _worldTM = t * r * s;

  if (_light)
    _light->position = _position;
}
