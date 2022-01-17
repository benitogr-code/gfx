#include "entity.h"
#include "core/renderer.h"

Entity::Entity(GfxModelRef model) {
  this->model = model;
  this->worldTM = glm::mat4(1.0f);
}

Entity::Entity(GfxModelRef model, const glm::vec3 position) {
  this->model = model;
  this->worldTM = glm::translate(glm::mat4(1.0f), position);
}

void Entity::Render(const Entity& entity, Renderer& renderer) {
  for (uint32_t idx = 0; idx < entity.model->getMeshCount(); ++idx) {
    RenderItem item;
    item.mesh = entity.model->getMesh(idx);
    item.material = entity.model->getMaterial();
    item.modelTM = entity.worldTM;

    renderer.draw(item);
  }
}
