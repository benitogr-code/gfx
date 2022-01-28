#pragma once

#include "graphics/material.h"
#include "graphics/shader.h"
#include "gfx_model.h"

class AssetManager {
private:
  typedef std::map<std::string, ShaderRef> Shaders;
  typedef std::map<std::string, MaterialRef> Materials;
  typedef std::map<std::string, GfxModelRef> Models;

public:
  AssetManager();
  ~AssetManager();

  void init();

  MaterialRef getDefaultMaterial() const { return _defaultMaterial; }
  MaterialRef getMaterial(const char* name) const;
  ShaderRef   getShader(const char* name) const;

  GfxModelRef loadModel(const char* path);

private:
  ShaderRef   loadShader(const char* name);
  MaterialRef loadMaterial(const char* name);

private:
  Shaders     _shaders;
  Materials   _materials;
  Models      _models;

  MaterialRef _defaultMaterial;
};
