#pragma once

#include "core/graphics/material.h"
#include "core/graphics/mesh.h"

struct aiScene;
struct aiMaterial;
struct aiNode;
struct aiMesh;

class Model3D;
typedef std::shared_ptr<Model3D> Model3DRef;

class Model3D {
public:
    uint32_t getMeshCount() const { return _meshes.size(); }
    MeshRef  getMesh(uint32_t idx) const { return _meshes[idx]; }
    MaterialRef getMeshMaterial(uint32_t idx) const { return _materials[idx]; }

    static Model3DRef Create(const char* path);

private:
    Model3D() = delete;
    Model3D(const Model3D& model) = delete;

    Model3D(const char* path);

    void load();
    void processNode(aiNode* node, const aiScene* scene);
    MeshRef processMesh(aiMesh* mesh, const aiScene* scene);
    MaterialRef processMeshMaterial(aiMesh* mesh, const aiScene* scene);
    std::vector<TextureRef> loadMaterialTextures(aiMaterial *mat, uint32_t type);

private:
    std::vector<MeshRef>     _meshes;
    std::vector<MaterialRef> _materials;
    std::map<std::string, TextureRef>  _textures;
    std::string _path;
};
