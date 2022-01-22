#include "entity.h"
#include "core/renderer.h"

Entity::Entity() {
  _position = glm::vec3(0.0f);
  _rotation = glm::identity<glm::quat>();
  _scale = glm::vec3(1.0f);
  updateWorldTM();
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

void Entity::cloneModelMaterial() {
  if (_model != nullptr) {
    _model->setMaterial(Material::Clone(_model->getMaterial()));
  }
}

MaterialRef Entity::getModelMaterial() const {
  return _model != nullptr ? _model->getMaterial() : nullptr;
}


void Entity::render(Renderer& renderer) {
  if (_model != nullptr) {
    for (uint32_t idx = 0; idx < _model->getMeshCount(); ++idx) {
      renderer.drawMesh(_model->getMesh(idx), _model->getMaterial(), _worldTM);
    }
  }
}

void Entity::updateWorldTM() {
  auto s = glm::scale(glm::mat4(1.0f), _scale);
  auto r = glm::toMat4(_rotation);
  auto t = glm::translate(glm::mat4(1.0f), _position);
  _worldTM = t * r * s;
}
