#include "material_library.h"

MaterialLibrary* MaterialLibrary::_sThis = nullptr;

MaterialLibrary::MaterialLibrary() {
  _sThis = this;
}

MaterialLibrary::~MaterialLibrary() {
  _sThis = nullptr;
}

void MaterialLibrary::init() {
  _defaultShader = loadShader("color");
  loadShader("illum_pong");

  _defaultMaterial = Material::Create(getShader("illum_pong"));
  _defaultMaterial->setParamVec3("mat_color", glm::vec3(1.0f, 1.0f, 1.0f));
  _defaultMaterial->setParamFloat("mat_ambient", 0.15f);
  _defaultMaterial->setParamFloat("mat_specular", 0.6f);
}

MaterialRef MaterialLibrary::createMaterial(const char* name, const char* shader) {
  auto pair = _materials.insert_or_assign(std::string(name), Material::Create(getShader(shader)));

  return pair.first->second;
}

MaterialRef MaterialLibrary::getMaterial(const char* name) const {
  auto iter = _materials.find(std::string(name));

  return iter != _materials.end() ? iter->second : _defaultMaterial;
}

ShaderRef MaterialLibrary::loadShader(const char* name) {
  char vertexShader[128];
  char fragmentShader[128];

  snprintf(vertexShader, sizeof(vertexShader), "shaders/%s.vert", name);
  snprintf(fragmentShader, sizeof(fragmentShader), "shaders/%s.frag", name);

  LOG_INFO("[MaterialLibrary] Loading shader {}", name);

  ShaderCreateParams shaderParams;
  shaderParams.name = name;
  shaderParams.vertexShaderPath = vertexShader;
  shaderParams.fragmentShaderPath = fragmentShader;

  auto shader = Shader::Create(shaderParams);
  _shaders.insert(Shaders::value_type(std::string(name), shader));

  return shader;
}

ShaderRef MaterialLibrary::getShader(const char* name) const {
  auto iter = _shaders.find(std::string(name));

  return iter != _shaders.end() ? iter->second : _defaultShader;
}
