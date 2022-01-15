#pragma once

#include "core/graphics/material.h"
#include "core/graphics/mesh.h"

struct aiScene;
struct aiMaterial;
struct aiNode;
struct aiMesh;

class GfxModel;
typedef std::shared_ptr<GfxModel> GfxModelRef;

class GfxModel {
public:
  void addMesh(MeshRef mesh) { _meshes.push_back(mesh); }
  void setMaterial(MaterialRef material) { _material = material; }

  uint32_t getMeshCount() const { return _meshes.size(); }
  MeshRef  getMesh(uint32_t idx) const { return _meshes[idx]; }
  MaterialRef getMaterial() const { return _material; }

  static GfxModelRef Create();

private:
  GfxModel() = default;
  GfxModel(const GfxModel& model) = delete;

private:
  std::vector<MeshRef>     _meshes;
  MaterialRef              _material;
};
