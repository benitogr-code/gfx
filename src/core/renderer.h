#pragma once

#include "camera.h"
#include "graphics/lights.h"
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
  typedef std::vector<PointLight> PointLights;

  enum {
    MaxPointLights = 8,
  };

public:
  Renderer();

  void init();

  Camera& getViewCamera() { return _viewCamera; };
  const Camera& getViewCamera() const { return _viewCamera; }

  void setClearColor(const ColorRGB& c) { _clearColor = c; }
  void setMainLight(const DirectionalLight& light) { _mainLight = light; }
  void toggleWireframe();

  void drawLight(const PointLight& light);
  void drawMesh(MeshRef mesh, MaterialRef material, const glm::mat4& modelMtx);

  void beginFrame();
  void endFrame();

private:
  Camera   _viewCamera;
  UBORef   _uboCamera;
  UBORef   _uboLights;

  RenderList _renderList;

  DirectionalLight _mainLight;
  PointLights      _pointLights;
  ColorRGB _clearColor;
  bool     _wireframeEnabled;
};
