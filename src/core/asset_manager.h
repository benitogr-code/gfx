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

  static AssetManager* Get() { return _sThis; }

  void init();

  MaterialRef getDefaultMaterial() const { return _defaultMaterial; }
  MaterialRef getMaterial(const char* name) const;

  GfxModelRef loadModel(const char* name);
  GfxModelRef getModel(const char* name) const;

private:
  ShaderRef loadShader(const char* name);
  ShaderRef getShader(const char* name) const;

  MaterialRef loadMaterial(const char* name);

private:
  Shaders     _shaders;
  Materials   _materials;
  Models      _models;

  MaterialRef _defaultMaterial;

  static AssetManager* _sThis;
};
