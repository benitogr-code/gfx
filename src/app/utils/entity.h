#pragma once

#include "core/gfx_model.h"

class Renderer;

struct Entity {
  Entity() = default;
  Entity(GfxModelRef model);
  Entity(GfxModelRef model, const glm::vec3 position);

  static void Render(const Entity& entity, Renderer& renderer);

  GfxModelRef model;
  glm::mat4   worldTM;
};
