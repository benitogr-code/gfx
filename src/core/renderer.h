#pragma once

#include "camera.h"
#include "graphics/material.h"
#include "graphics/mesh.h"

struct RenderItem {
  MeshRef     mesh;
  MaterialRef material;
  glm::mat4   modelTM;
};

class Renderer {
private:
  typedef std::vector<RenderItem> RenderList;

public:
  Renderer();

  void init();

  Camera& getViewCamera() { return _viewCamera; };
  const Camera& getViewCamera() const { return _viewCamera; }

  void setClearColor(const ColorRGB& c) { _clearColor = c; }
  void toggleWireframe();

  void draw(const RenderItem& item);

  void beginFrame();
  void endFrame();

private:
  Camera   _viewCamera;
  UBORef   _uboCamera;

  RenderList _renderList;

  ColorRGB _clearColor;
  bool     _wireframeEnabled;
};
