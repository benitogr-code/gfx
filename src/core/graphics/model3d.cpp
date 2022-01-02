#include "model3d.h"
#include "../file_utils.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model3D::Model3D(const char* path) {
    _path = path;
    load();
}

/*static*/ Model3DRef Model3D::Create(const char* path) {
    Model3DRef model(new Model3D(path));

    return model;
}


void Model3D::draw(ShaderRef& shader) {
    for(auto mesh : _meshes) {
        mesh->draw(shader);
    }
}

void Model3D::load() {
    LOG_INFO("[Model3D] Loading {0}", _path);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        FileUtils::getAbsolutePath(_path.c_str()).c_str(),
        aiProcess_Triangulate | aiProcess_FlipUVs
    );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR("[Model3D] Failed to load model {0}. Reason: {1}", _path, importer.GetErrorString());
        return;
    }

    processNode(scene->mRootNode, scene);
}

void Model3D::processNode(aiNode *node, const aiScene *scene) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(processMesh(mesh, scene));
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

MeshRef Model3D::processMesh(aiMesh *mesh, const aiScene *scene) {
    MeshCreateParams params;
    params.vertices.reserve(512);
    params.indices.reserve(512);

    auto textureCoords = mesh->mTextureCoords[0];
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;
        if(textureCoords) {
            vertex.texCoords.x = textureCoords[i].x;
            vertex.texCoords.y = textureCoords[i].y;
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        params.vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            params.indices.push_back(face.mIndices[j]);
    }

    if(mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        auto diffTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        int diffCounter = 1;
        for (auto texture : diffTextures) {
            params.textures.push_back(TextureSlot({
                texture,
                std::string("texture_diffuse") + std::to_string(diffCounter++),
            }));
        }

        auto specTextures = loadMaterialTextures(material, aiTextureType_SPECULAR);
        int specCounter = 1;
        for (auto texture : specTextures) {
            params.textures.push_back(TextureSlot({
                texture,
                std::string("texture_specular") + std::to_string(specCounter++),
            }));
        }
    }

    return Mesh::Create(params);
}

std::vector<TextureRef> Model3D::loadMaterialTextures(aiMaterial *mat, uint32_t type) {
    std::vector<TextureRef> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount((aiTextureType)type); i++) {
        aiString fileName;
        mat->GetTexture((aiTextureType)type, i, &fileName);

        std::string filePath = _path.substr(0, _path.find_last_of('/')+1) + std::string(fileName.C_Str());
        auto iter = _textures.find(filePath);

        if (iter != _textures.end()) {
            textures.push_back(iter->second);
        }
        else {
            LOG_INFO("[Model3D] Loading texture {0}", filePath);

            TextureCreateParams params;
            params.filePath = filePath.c_str();
            textures.push_back(Texture::Create(params));
        }
    }

    return textures;
}
