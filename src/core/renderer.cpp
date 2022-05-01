#include "renderer.h"
#include "file_utils.h"
#include "font.h"
#include "graphics/debug_utils.h"

#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>

#define UBO_CAMERA_IDX 0
#define UBO_LIGHTS_IDX 1

#define TEXT_BUFFER_CAPACITY 2048
#define TEXT_VERTICES_CAPACITY 2048 * 6

#define SHADOW_MAP_HEIGHT 1024
#define SHADOW_MAP_WIDTH  1024
#define SHADOW_MAP_TEXTURE_SLOT 4

Renderer::Renderer()
  : _clearColor(0.0f)
  , _wireframeEnabled(false)
  , _debugEnabled(false) {
    _viewCamera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), 1.0f, 65.0f, 0.1f, 50.0f);
    _mainPassList.reserve(256);
    _shadowPassList.reserve(256);
}

void Renderer::init(int width, int height) {
  LOG_INFO("[Renderer] Initializing resources");

  setViewport(width, height);

  _font = Font::loadFont("fonts/meslo_lgs_bold.ttf", 20);
  _textBuffer = TextBuffer::Create(TEXT_BUFFER_CAPACITY);
  _textVertices.reserve(TEXT_VERTICES_CAPACITY);

  FBOSpec shadowmapSpec;
  shadowmapSpec.height = SHADOW_MAP_HEIGHT;
  shadowmapSpec.width = SHADOW_MAP_WIDTH;
  shadowmapSpec.type = FBOType::Shadowmap;
  _fboShadowmap = FBO::Create(shadowmapSpec);

  _uboCamera = UBO::Create(
      UBO_CAMERA_IDX,
      {
        UBO::newVec(3),             // position
        UBO::newVec(2),             // viewport size (w, h)
        UBO::newColumnMatrix(4, 4), // view
        UBO::newColumnMatrix(4, 4), // projection
        UBO::newColumnMatrix(4, 4), // view-projection
        UBO::newColumnMatrix(4, 4), // view-rotation
      }
  );

  _uboLights = UBO::Create(
    UBO_LIGHTS_IDX,
      {
      UBO::newStruct({  // Main Light
        UBO::newVec(3), // - direction
        UBO::newVec(3), // - ambient
        UBO::newVec(3), // - diffuse
        UBO::newVec(3)  // - specular
      }),
      UBO::newScalar(), // Number of point lights
      UBO::newArray(MaxPointLights, UBO::newStruct({  // Point Lights Array
          UBO::newVec(3), // - position
          UBO::newVec(3), // - ambient
          UBO::newVec(3), // - diffuse
          UBO::newVec(3), // - specular
          UBO::newScalar(), // - attenuation constant
          UBO::newScalar(), // - attenuation linear
          UBO::newScalar(), // - attenuation quadratic
        })
      )
      }
  );

  MeshCreateParams quadParams;
  quadParams.vertices = {
    { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
    { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
    { glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
    { glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
    { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },
    { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
  };
  _screenDebugQuad = Mesh::Create(quadParams);

  ShaderCreateParams params;
  params.name = "text";
  params.vertexShaderPath = "shaders/text.vert";
  params.fragmentShaderPath = "shaders/text.frag";
  _textShader = Shader::Create(params);
  _textShader->setUniformBlockBind("Camera", UBO_CAMERA_IDX);

  params.name = "shadowmap_depth";
  params.vertexShaderPath = "shaders/shadowmap_depth.vert";
  params.fragmentShaderPath = "shaders/shadowmap_depth.frag";
  _shadowmapShader = Shader::Create(params);

  params.name = "screen_quad_depth";
  params.vertexShaderPath = "shaders/screen_quad_depth.vert";
  params.fragmentShaderPath = "shaders/screen_quad_depth.frag";
  _screenQuadDepthShader = Shader::Create(params);

  _mainLight.position = glm::vec3(20.0f, 30.0f, 100.0f);
  _mainLight.properties.color = ColorRGB(0.6f);
  _mainLight.properties.ambientMultiplier = 0.2f;
  _mainLight.properties.specularMultiplier = 0.85f;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
}

void Renderer::setViewport(int width, int height) {
  const float aspectRatio = (float)width / (float)height;

  _viewportWidth = width;
  _viewportHeight = height;
  _viewCamera.setViewport(width, height);
  _viewCamera.setAspectRatio(aspectRatio);
}

void Renderer::toggleWireframe() {
  _wireframeEnabled = !_wireframeEnabled;
  if (_wireframeEnabled) {
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else {
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void Renderer::toggleDebug() {
  _debugEnabled = !_debugEnabled;
}

void Renderer::drawText(const std::string& text, const glm::vec3& position, const ColorRGB& color, bool center, float scale) {
  float xOffset = 0.0f;

  glm::vec2 viewport = _viewCamera.getViewport();
  glm::vec3 screenPos = _viewCamera.worldToScreenCoordinates(position);

  const bool visible = fabsf(screenPos.z) <= 1.0f
    && (screenPos.x > -20.0f && screenPos.x < viewport.x + 20.0f)
    && (screenPos.y > -20.0f && screenPos.y < viewport.y + 20.0f);

  if (!visible) return;

  if (center) {
    float width = 0.0f;

    for (size_t i = 0; i < text.length(); ++i) {
      const unsigned char c = text.at(i);
      auto charInfo = _font->getCharacterInfo(c);

      width += charInfo ? charInfo->advance.x * scale : 0.0f;
    }

    screenPos.x -= (width * 0.5f);
  }

  for (size_t i = 0; i < text.size(); ++i) {
    const unsigned char c = text.at(i);
    auto charInfo = _font->getCharacterInfo(c);

    if (charInfo) {
      const float xpos = xOffset + screenPos.x + (charInfo->bearing.x * scale);
      const float ypos = screenPos.y - (charInfo->size.y - (charInfo->bearing.y * scale));
      const float zpos = screenPos.z;

      const float w = charInfo->size.x * scale;
      const float h = charInfo->size.y * scale;
      const auto& offsets = charInfo->atlasOffsets;

      _textVertices.push_back({ glm::vec3(xpos, ypos + h, zpos),     glm::vec2(offsets[0], 0.0f), color });
      _textVertices.push_back({ glm::vec3(xpos, ypos, zpos),         glm::vec2(offsets[0], 1.0f), color });
      _textVertices.push_back({ glm::vec3(xpos + w, ypos, zpos),     glm::vec2(offsets[1], 1.0f), color });
      _textVertices.push_back({ glm::vec3(xpos, ypos + h, zpos),     glm::vec2(offsets[0], 0.0f), color });
      _textVertices.push_back({ glm::vec3(xpos + w, ypos, zpos),     glm::vec2(offsets[1], 1.0f), color });
      _textVertices.push_back({ glm::vec3(xpos + w, ypos + h, zpos), glm::vec2(offsets[1], 0.0f), color });

      xOffset += (charInfo->advance.x * scale);
    }
  }
}

void Renderer::drawLight(const Light& light) {
  if (_lightsList.size() < MaxPointLights) {
    _lightsList.push_back(light);
  }
}

void Renderer::drawMesh(MeshRef mesh, MaterialRef material, const glm::mat4& worldTM, uint32_t drawFlags) {
  RenderItem item;
  item.mesh = mesh;
  item.material = material;
  item.modelTM = worldTM;

  _mainPassList.push_back(item);

  if ((drawFlags & DrawFlags_Shadow) != 0) {
    _shadowPassList.push_back(item);
  }
}

void Renderer::beginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  _mainPassList.clear();
  _shadowPassList.clear();
  _lightsList.clear();
  _textVertices.clear();
}

void Renderer::endFrame() {
  _stats.reset();

  // Prepare UBOs
  _uboCamera->writeBegin();
  _uboCamera->writeVec3(_viewCamera.getPosition());
  _uboCamera->writeVec2(_viewCamera.getViewport());
  _uboCamera->writeMat4(_viewCamera.getView());
  _uboCamera->writeMat4(_viewCamera.getProjection());
  _uboCamera->writeMat4(_viewCamera.getViewProjection());
  _uboCamera->writeMat4(glm::mat4(glm::mat3(_viewCamera.getView())));
  _uboCamera->writeEnd();

  _uboLights->writeBegin();
  _uboLights->writeVec3(-glm::normalize(_mainLight.position));
  _uboLights->writeVec3(_mainLight.properties.color * _mainLight.properties.ambientMultiplier);
  _uboLights->writeVec3(_mainLight.properties.color);
  _uboLights->writeVec3(_mainLight.properties.color * _mainLight.properties.specularMultiplier);

  _uboLights->writeInt((int)_lightsList.size());
  for (auto& light : _lightsList) {
    _uboLights->writeVec3(light.position);
    _uboLights->writeVec3(light.properties.color * light.properties.ambientMultiplier);
    _uboLights->writeVec3(light.properties.color);
    _uboLights->writeVec3(light.properties.color * light.properties.specularMultiplier);
    _uboLights->writeFloat(light.properties.attenuationConstant);
    _uboLights->writeFloat(light.properties.attenuationLinear);
    _uboLights->writeFloat(light.properties.attenuationQuadratic);
  }
  _uboLights->writeEnd();

  uint32_t drawcalls = 0;
  uint32_t drawcallsShadows = 0;

  // Shadow pass
  // TODO: Fit light projection to camera view frustum
  auto lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);
  auto lightView = glm::lookAt(glm::normalize(_mainLight.position), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
  auto lightViewProj = lightProj * lightView;

  glBindFramebuffer(GL_FRAMEBUFFER, _fboShadowmap->id());
  glViewport(0, 0, _fboShadowmap->width(), _fboShadowmap->height());
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);

  _shadowmapShader->use();
  _shadowmapShader->setUniformMatrix4("mtx_light_vp", lightViewProj);

  for (auto item : _shadowPassList) {
    _shadowmapShader->setUniformMatrix4("mtx_model", item.modelTM);
    item.mesh->draw();

    drawcallsShadows++;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Main pass
  glViewport(0, 0, _viewportWidth, _viewportHeight);
  glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_SLOT);
  glBindTexture(GL_TEXTURE_2D, _fboShadowmap->depthAttachment());

  for (auto item : _mainPassList) {
    auto shader = item.material->getShader();
    shader->use();
    shader->setUniformBlockBind("Camera", UBO_CAMERA_IDX);
    shader->setUniformBlockBind("Lights", UBO_LIGHTS_IDX);
    shader->setUniformInt("shadow_depth_map", SHADOW_MAP_TEXTURE_SLOT);
    shader->setUniformMatrix4("mtx_model", item.modelTM);
    shader->setUniformMatrix4("mtx_light_vp", lightViewProj);
    item.material->apply();
    item.mesh->draw();

    drawcalls++;
  }

  // Render text
  if (_textVertices.size() > 0) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _font->id());
    _textShader->use();
    _textShader->setUniformInt("texture_font", 0);
    drawcalls += _textBuffer->draw(_textVertices);

    glDisable(GL_BLEND);
  }

  if (_debugEnabled) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _fboShadowmap->depthAttachment());
    _screenQuadDepthShader->use();
    _screenQuadDepthShader->setUniformMatrix4(
      "mtx_model",
      glm::translate(glm::mat4(1.0f), glm::vec3(0.6f,-0.6f,0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.65f))
    );
    _screenQuadDepthShader->setUniformInt("depth_map", 0);
    _screenDebugQuad->draw();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  _stats.pointlights = _lightsList.size();
  _stats.drawcalls = drawcalls;
  _stats.drawcallsShadows = drawcallsShadows;

  GL_CHECK_ERROR();
}

void Renderer::captureScreen() {
  ImageData img;
  img.width = _viewportWidth;
  img.height = _viewportHeight;
  img.bytesPerPixel = 3; // RGB
  img.data.resize(_viewportHeight*_viewportWidth*3);

  glReadPixels(0, 0, _viewportWidth, _viewportWidth, GL_RGB, GL_UNSIGNED_BYTE, img.data.data());

  char filename[128];
  long int t = time(NULL);
  sprintf(filename, "_captures/screenshot_%ld.png", t);

  FileUtils::saveImageToFile(FileUtils::getAbsolutePath(filename).c_str(), img);
}
