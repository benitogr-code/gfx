#pragma once

#include "graphics/material.h"
#include "graphics/shader.h"

class AssetManager {
private:
  typedef std::map<std::string, ShaderRef> Shaders;
  typedef std::map<std::string, MaterialRef> Materials;

public:
  AssetManager();
  ~AssetManager();

  static AssetManager* Get() { return _sThis; }

  void init();

  MaterialRef getDefaultMaterial() const { return _defaultMaterial; }
  MaterialRef getMaterial(const char* name) const;

private:
  ShaderRef loadShader(const char* name);
  ShaderRef getShader(const char* name) const;

  MaterialRef loadMaterial(const char* name);

private:
  Shaders     _shaders;
  Materials   _materials;

  MaterialRef _defaultMaterial;

  static AssetManager* _sThis;
};
