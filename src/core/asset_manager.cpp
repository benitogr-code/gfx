#include "asset_manager.h"
#include "file_utils.h"

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
    if (value.isArray() && value.size() == 3) {
      for (int i = 0; i < value.size(); ++i) {
        color[i] = value[i].asFloat();
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
      //material->setParamFloat("mat_shininess", shininess);
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
  loadShader("illum_pong");

  _defaultMaterial = loadMaterial("default");
  loadMaterial("cyborg");
  loadMaterial("light_source");
}

MaterialRef AssetManager::getMaterial(const char* name) const {
  auto iter = _materials.find(std::string(name));

  return iter != _materials.end() ? iter->second : _defaultMaterial;
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
