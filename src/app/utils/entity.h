#pragma once

#include "core/gfx_model.h"
#include "core/graphics/lights.h"

class Renderer;

class Entity {
public:
  Entity();

  void setPosition(const glm::vec3& position);
  void setRotation(const glm::quat& rotation);
  void setScale(const glm::vec3& scale);

  glm::vec3 getPosition() const { return _position; }
  glm::quat getRotation() const { return _rotation; }
  glm::vec3 getScale() const { return _scale; }

  void attachModel(GfxModelRef model);
  void attachLight(const Light::Properties& properties);

  void cloneModelMaterial();
  MaterialRef getModelMaterial() const;

  void render(Renderer& renderer);

private:
  void updateWorldTM();

private:
  GfxModelRef _model;
  std::unique_ptr<Light> _light;

  glm::vec3   _position;
  glm::quat   _rotation;
  glm::vec3   _scale;
  glm::mat4   _worldTM;
};
