#pragma once

#include "camera.h"
#include "graphics/font_atlas.h"
#include "graphics/lights.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/text_buffer.h"

class Renderer {
private:
  struct RenderItem {
    MeshRef     mesh;
    MaterialRef material;
    glm::mat4   modelTM;
  };

  struct Stats {
    Stats() {
      reset();
    }

    void reset() {
      drawcalls =  0;
      pointlights = 0;
    }

    uint32_t drawcalls;
    uint32_t pointlights;
  };

  typedef std::vector<RenderItem>  RenderList;
  typedef std::vector<Light> LightsList;

  enum {
    MaxPointLights = 8,
  };

public:
  Renderer();

  void init(int width, int height);

  Camera& getViewCamera() { return _viewCamera; };
  const Camera& getViewCamera() const { return _viewCamera; }

  Light& getMainLight() { return _mainLight; }
  const Light& getMainLight() const { return _mainLight; }

  const Stats& getStats() const { return _stats; }

  void setViewport(int width, int height);
  void setClearColor(const ColorRGB& c) { _clearColor = c; }
  void toggleWireframe();

  void drawText(const std::string& text, const glm::vec3& position, const ColorRGB& = ColorRGB(1.0f), bool center = true, float scale = 1.0f);
  void drawLight(const Light& light);
  void drawMesh(MeshRef mesh, MaterialRef material, const glm::mat4& worldTM);

  void beginFrame();
  void endFrame();

private:
  Camera   _viewCamera;
  UBORef   _uboCamera;
  UBORef   _uboLights;

  FontAtlasRef  _font;
  ShaderRef     _textShader;
  TextBufferRef _textBuffer;
  std::vector<TextVertex> _textVertices;

  RenderList _renderList;
  Light      _mainLight;
  LightsList _lightsList;

  Stats    _stats;
  uint32_t _viewportHeight;
  uint32_t _viewportWidth;
  ColorRGB _clearColor;
  bool     _wireframeEnabled;
};
