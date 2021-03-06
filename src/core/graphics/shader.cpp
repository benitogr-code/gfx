#include "shader.h"
#include "core/file_utils.h"

#include <glad/glad.h>

// Helpers

static void checkCompileErrors(GLuint shader, const char* type) {
  GLint success;
  GLchar infoLog[1024];

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    LOG_ERROR("[Shader] Compile error: Type {0}, Info: {1}", type, infoLog);
  }
}

static void checkLinkErrors(GLuint shader, const char* name) {
  GLint success;
  GLchar infoLog[1024];

  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
    LOG_ERROR("[Shader] Link error: Name {0}, Info: {1}", name, infoLog);
  }
}

// Shader

Shader::Shader(const char* name)
  : _name(name)
  , _id(0) {

}

Shader::~Shader() {
  glDeleteProgram(_id);
}

void Shader::use() {
  glUseProgram(_id);
}

void Shader::setUniformFloat(const char* name, float value) {
  glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniformInt(const char* name, int value) {
  glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniformUInt(const char* name, uint value) {
  glUniform1ui(getUniformLocation(name), value);
}

void Shader::setUniformVec2(const char* name, const glm::vec2& value){
  glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setUniformVec3(const char* name, const glm::vec3& value){
  glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setUniformMatrix4(const char* name, const glm::mat4x4& value) {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniformBlockBind(const char* name, int bindId) {
  auto index = getUniformBlockIndex(name);
  if (index >= 0) {
    glUniformBlockBinding(_id, getUniformBlockIndex(name), bindId);
  }
}

void Shader::buildFromSources(const char* vsSources, const char* fsSources) {
  // Vertex Shader
  unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vsSources, NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "Vertex");

  // Fragment Shader
  unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fsSources, NULL);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "Fragment");

  // Shader Program
  _id = glCreateProgram();
  glAttachShader(_id, vertex);
  glAttachShader(_id, fragment);

  glLinkProgram(_id);
  checkLinkErrors(_id, _name.c_str());

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

int Shader::getUniformLocation(const char* name) {
  auto iter = _uniformsCache.find(name);

  if (iter != _uniformsCache.end())
    return iter->second;

  GLint location = glGetUniformLocation(_id, name);
  _uniformsCache.insert(UniformLocations::value_type(name, location));

  return location;
}

int Shader::getUniformBlockIndex(const char* name) {
  auto iter = _blockIndices.find(name);

  if (iter != _blockIndices.end())
    return iter->second;

  GLint index = glGetUniformBlockIndex(_id, name);
  _blockIndices.insert(UniformLocations::value_type(name, index));

  return index;
}

/*static*/ ShaderRef Shader::Create(const ShaderCreateParams& params) {
  ShaderRef shader(new Shader(params.name));

  std::vector<char> incBuffer;
  FileUtils::readTextFile("shaders/_common.inc", incBuffer);

  std::vector<char> vsBuffer, fsBuffer;
  if (FileUtils::readTextFile(params.vertexShaderPath, vsBuffer)
    && FileUtils::readTextFile(params.fragmentShaderPath, fsBuffer)) {

    auto regex = std::regex("//#common.inc");

    shader->buildFromSources(
      std::regex_replace(vsBuffer.data(), regex, incBuffer.data()).c_str(),
      std::regex_replace(fsBuffer.data(), regex, incBuffer.data()).c_str()
    );
  }
  else {
    LOG_ERROR("[Shader] Loading error '{}'", params.name);
  }

  return shader;
}
