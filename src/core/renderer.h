#pragma once

#include "camera.h"
#include "graphics/lights.h"
#include "graphics/material.h"
#include "graphics/mesh.h"

class Renderer {
private:
  struct RenderItem {
    MeshRef     mesh;
    MaterialRef material;
    glm::mat4   modelTM;
  };

  typedef std::vector<RenderItem>  RenderList;
  typedef std::vector<Light> LightsList;

  enum {
    MaxPointLights = 8,
  };

public:
  Renderer();

  void init();

  Camera& getViewCamera() { return _viewCamera; };
  const Camera& getViewCamera() const { return _viewCamera; }

  Light& getMainLight() { return _mainLight; }
  const Light& getMainLight() const { return _mainLight; }

  void setClearColor(const ColorRGB& c) { _clearColor = c; }
  void toggleWireframe();

  void drawLight(const Light& light);
  void drawMesh(MeshRef mesh, MaterialRef material, const glm::mat4& worldTM);

  void beginFrame();
  void endFrame();

private:
  Camera   _viewCamera;
  UBORef   _uboCamera;
  UBORef   _uboLights;

  RenderList _renderList;
  Light      _mainLight;
  LightsList _lightsList;

  ColorRGB _clearColor;
  bool     _wireframeEnabled;
};
