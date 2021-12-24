#pragma once

class Shader;
typedef std::shared_ptr<Shader> ShaderRef;

struct ShaderCreateParams {
  ShaderCreateParams()
    : name(nullptr)
    , vertexShaderPath(nullptr)
    , fragmentShaderPath(nullptr) {

    }

  const char* name;
  const char* vertexShaderPath;
  const char* fragmentShaderPath;
};

class Shader {
public:
  ~Shader();

  const std::string& getName() const { return _name; }

  void use();
  void setUniformFloat(const char* name, float value);
  void setUniformVec2(const char* name, const glm::vec2& value);
  void setUniformVec3(const char* name, const glm::vec3& value);
  void setUniformMatrix4(const char* name, const glm::mat4x4& value);

  static ShaderRef Create(const ShaderCreateParams& params);

private:
  Shader() = delete;
  Shader(const Shader& shader) = delete;

  Shader(const char* name);

  void buildFromSources(const char* vsSources, const char* fsSources);
  int getUniformLocation(const char* name);

private:
  typedef std::unordered_map<std::string, int> UniformLocations;

  std::string _name;
  unsigned int _id;

  UniformLocations _uniformsCache;
};
