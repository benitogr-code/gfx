#pragma once

#include "mesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;

class Model3D;
typedef std::shared_ptr<Model3D> Model3DRef;

class Model3D {
public:
    void draw(ShaderRef& shader);

    static Model3DRef Create(const char* path);

private:
    Model3D() = delete;
    Model3D(const Model3D& shader) = delete;

    Model3D(const char* path);

    void load();
    void processNode(aiNode *node, const aiScene *scene);
    MeshRef processMesh(aiMesh *mesh, const aiScene *scene);

private:
    std::vector<MeshRef> _meshes;
    std::string _path;
};