#include "application.h"
#include "input.h"

#include <imgui.h>
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include <SDL.h>

Application::Application()
  : _running(true) {
}

Application::~Application() {
}

bool Application::init(const WindowDesc& desc) {
  _window.reset(new Window(desc));
  if (!_window->init()) {
    LOG_ERROR("[Application] Failed to initialize window");

    return false;
  }

  _input.reset(new Input());
  _input->registerCallbacks(
    std::bind(&Application::onInputEvent, this, std::placeholders::_1),
    std::bind(&Application::onMouseEvent, this, std::placeholders::_1)
  );

  _renderer.reset(new Renderer());
  _renderer->init();
  _renderer->getViewCamera().setAspectRatio(_window->getAspectRatio());

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsClassic();

  ImGui_ImplSDL2_InitForOpenGL(_window->getWindow(), _window->getGLContext());
  ImGui_ImplOpenGL3_Init("#version 150");

  _assetManager.reset(new AssetManager());
  _assetManager->init();

  if (!onInit()) {
    LOG_ERROR("[Application] Failed to initialize application");

    return false;
  }

  return true;
}

void Application::shutdown() {
  onShutdown();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

void Application::run() {
  uint64_t timeNow = SDL_GetPerformanceCounter();
  uint64_t timeLast = 0;
  uint32_t frameId = 0;

  while (_running) {
    // Input + events
    SDL_PumpEvents();

    checkSystemEvents();
    _input->update();

    // Update
    frameId++;
    timeLast = timeNow;
    timeNow  = SDL_GetPerformanceCounter();
    const auto deltaTime = (double)((timeNow - timeLast) / (double)SDL_GetPerformanceFrequency());

    _renderer->beginFrame();

    onUpdate(UpdateContext(deltaTime, frameId));
    onGUI();

    _renderer->endFrame();
    _window->update();
  }
}

void Application::checkSystemEvents() {
  const int maxEvents = 16;
  SDL_Event events[maxEvents];

  // Quit
  {
    const int eventsRead = SDL_PeepEvents(events, maxEvents, SDL_GETEVENT, SDL_QUIT, SDL_QUIT);

    for (int i = 0; i < eventsRead; ++i) {
      if (events[i].type == SDL_QUIT) {
        _running = false;
        break;
      }
    }
  }

  // Window
  {
    const int eventsRead = SDL_PeepEvents(events, maxEvents, SDL_GETEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT);
    for (int i = 0; i < eventsRead; ++i) {
      ImGui_ImplSDL2_ProcessEvent(&events[i]);

      if (events[i].window.event == SDL_WINDOWEVENT_RESIZED) {
        _window->onResized(events[i].window.data1, events[i].window.data2);
        _renderer->getViewCamera().setAspectRatio(_window->getAspectRatio());
      }
    }
  }
}
