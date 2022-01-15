#include "asset_manager.h"
#include "file_utils.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Json helpers

namespace JsonHelper {
  std::string readString(const Json::Value& value, const char* key, const char* defaultValue) {
    return value.get(key, defaultValue).asString();
  }

  float readFloat(const Json::Value& value, const char* key, float defaultValue) {
    return value.get(key, defaultValue).asFloat();
  }

  ColorRGB readColorRGB(const Json::Value& value, const char* key, const ColorRGB& defaultValue) {
    ColorRGB color = defaultValue;
    if (value[key].isArray() && value[key].size() == 3) {
      for (int i = 0; i < value[key].size(); ++i) {
        color[i] = value[key][i].asFloat();
      }
    }

    return color;
  }

  void readIllumPongShaderParameters(const Json::Value& root, MaterialRef material) {
    char textureFile[128];

    if (root["texture_maps"].isObject()){
      std::string diffuseTexture = JsonHelper::readString(root["texture_maps"], "diffuse", "");
      std::string specularTexture = JsonHelper::readString(root["texture_maps"], "specular", "");

      if (diffuseTexture.length() > 0) {
        snprintf(textureFile, sizeof(textureFile), "materials/%s", diffuseTexture.c_str());

        LOG_INFO("[AssetManager] Loading texture {}", textureFile);

        TextureCreateParams params;
        params.filePath = textureFile;
        material->setTexture(TextureType_Diffuse, Texture::Create(params));
      }

      if (specularTexture.length() > 0) {
        snprintf(textureFile, sizeof(textureFile), "materials/%s", specularTexture.c_str());

        LOG_INFO("[AssetManager] Loading texture {}", textureFile);

        TextureCreateParams params;
        params.filePath = textureFile;
        material->setTexture(TextureType_Specular, Texture::Create(params));
      }
    }

    if (root["properties"].isObject()){
      auto properties = root["properties"];

      ColorRGB color = JsonHelper::readColorRGB(properties, "color", ColorRGB(1.0f));
      ColorRGB ambient = JsonHelper::readColorRGB(properties, "ambient", ColorRGB(0.0f));
      ColorRGB specular = JsonHelper::readColorRGB(properties, "specular", ColorRGB(0.3f));;
      float shininess = JsonHelper::readFloat(properties, "shininess", 10.0f);

      material->setParamVec3("mat_color", color);
      material->setParamFloat("mat_ambient", ambient.r);
      material->setParamFloat("mat_specular", specular.r);
      material->setParamFloat("mat_shininess", shininess);
    }
  }

  void readColorShaderParameters(const Json::Value& root, MaterialRef material) {
    if (root["properties"].isObject()){
      auto properties = root["properties"];

      ColorRGB color = JsonHelper::readColorRGB(properties, "color", ColorRGB(1.0f));
      material->setParamVec3("mat_color", color);
    }
  }
}

// Assimp Helper

namespace AssimpHelper {
  MeshRef processMesh(aiMesh *mesh, const aiScene *scene) {
    MeshCreateParams params;
    params.vertices.reserve(512);
    params.indices.reserve(512);

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;
      vertex.position.x = mesh->mVertices[i].x;
      vertex.position.y = mesh->mVertices[i].y;
      vertex.position.z = mesh->mVertices[i].z;
      vertex.normal.x = mesh->mNormals[i].x;
      vertex.normal.y = mesh->mNormals[i].y;
      vertex.normal.z = mesh->mNormals[i].z;
      if(mesh->HasTextureCoords(0)) {
        vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
        vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
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

    return Mesh::Create(params);
  }

  void processNode(aiNode *node, const aiScene *scene, std::vector<MeshRef>& meshes) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++) {
      processNode(node->mChildren[i], scene, meshes);
    }
  }
}

// AssetManager

AssetManager* AssetManager::_sThis = nullptr;

AssetManager::AssetManager() {
  _sThis = this;
}

AssetManager::~AssetManager() {
  _sThis = nullptr;
}

void AssetManager::init() {
  const uint32_t SHADER_COUNT = 2;
  const char* SHADERS[SHADER_COUNT] = {
    "color",
    "illum_pong"
  };

  for (uint32_t i = 0; i < SHADER_COUNT; ++i) {
    loadShader(SHADERS[i]);
  }

  _defaultMaterial = loadMaterial("default");
  loadMaterial("cyborg");
  loadMaterial("light_source");
}

MaterialRef AssetManager::getMaterial(const char* name) const {
  auto iter = _materials.find(std::string(name));

  return iter != _materials.end() ? iter->second : _defaultMaterial;
}

GfxModelRef AssetManager::loadModel(const char* name) {
  char modelFile[128];
  snprintf(modelFile, sizeof(modelFile), "models/%s.gfx", name);

  LOG_INFO("[AssetManager] Loading model {}", name);

  Json::Value root;
  if (!FileUtils::readJsonFile(modelFile, root)) {
    return nullptr;
  }

  std::string geometryFile = JsonHelper::readString(root, "geometry", "");
  std::string materialName = JsonHelper::readString(root, "material", "");

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
    FileUtils::getAbsolutePath(geometryFile.c_str()).c_str(),
    aiProcess_Triangulate | aiProcess_CalcTangentSpace
  );

  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    LOG_ERROR("[Model3D] Failed to load model {0}. Reason: {1}", geometryFile.c_str(), importer.GetErrorString());
    return nullptr;
  }

  std::vector<MeshRef> meshes;
  AssimpHelper::processNode(scene->mRootNode, scene, meshes);

  GfxModelRef model = GfxModel::Create();
  for (auto mesh : meshes) {
    model->addMesh(mesh);
  }
  model->setMaterial(getMaterial(materialName.c_str()));

  _models.insert_or_assign(std::string(name), model);

  return model;
}

GfxModelRef AssetManager::getModel(const char* name) const {
  auto iter = _models.find(std::string(name));

  return iter != _models.end() ? iter->second : nullptr;
}

ShaderRef AssetManager::loadShader(const char* name) {
  char vertexShader[128];
  char fragmentShader[128];

  snprintf(vertexShader, sizeof(vertexShader), "shaders/%s.vert", name);
  snprintf(fragmentShader, sizeof(fragmentShader), "shaders/%s.frag", name);

  LOG_INFO("[AssetManager] Loading shader {}", name);

  ShaderCreateParams shaderParams;
  shaderParams.name = name;
  shaderParams.vertexShaderPath = vertexShader;
  shaderParams.fragmentShaderPath = fragmentShader;

  auto shader = Shader::Create(shaderParams);
  _shaders.insert(Shaders::value_type(std::string(name), shader));

  return shader;
}

ShaderRef AssetManager::getShader(const char* name) const {
  auto iter = _shaders.find(std::string(name));

  return iter != _shaders.end() ? iter->second : nullptr;
}

MaterialRef AssetManager::loadMaterial(const char* name) {
  char materialFile[128];
  snprintf(materialFile, sizeof(materialFile), "materials/%s.mtl", name);

  LOG_INFO("[AssetManager] Loading material {}", name);

  Json::Value root;
  if (!FileUtils::readJsonFile(materialFile, root)) {
    return nullptr;
  }

  std::string shaderName = JsonHelper::readString(root, "shader", "");
  auto shader = getShader(shaderName.c_str());

  if (!shader) {
    LOG_WARN("[AssetManager] Invalid shader '{}' for material {}", shaderName.c_str(), name);
    return nullptr;
  }

  auto material = Material::Create(shader);

  if (shaderName.compare("color") == 0) {
    JsonHelper::readColorShaderParameters(root, material);
  }
  else if (shaderName.compare("illum_pong")== 0) {
    JsonHelper::readIllumPongShaderParameters(root, material);
  }

  _materials.insert_or_assign(std::string(name), material);

  return material;
}
