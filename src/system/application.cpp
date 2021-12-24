#include "application.h"
#include "input.h"

#include <SDL.h>

Application::Application()
  : _running(true) {
}

Application::~Application() {
}

bool Application::init(const WindowDesc& desc) {
  _window.reset(new Window(desc));
  if (!_window->init()) {
    LOG_ERROR("Failed to initialize window");

    return false;
  }

  _input.reset(new Input());
  _input->registerCallback(std::bind(&Application::onInputEvent, this, std::placeholders::_1));

  if (!onInit()) {
    LOG_ERROR("Failed to initialize application");

    return false;
  }

  return true;
}

void Application::shutdown() {
  onShutdown();
}

void Application::run() {
  uint64_t timeNow = SDL_GetPerformanceCounter();
  uint64_t timeLast = 0;
  uint32_t frameId = 0;

  while (_running) {
    // Input + events
    {
      SDL_PumpEvents();

      checkSystemEvents();
      _input->update();
    }

    // Update
    {
      frameId++;
      timeLast = timeNow;
      timeNow  = SDL_GetPerformanceCounter();
      const auto deltaTime = (double)((timeNow - timeLast) / (double)SDL_GetPerformanceFrequency());

      onUpdate(UpdateContext(deltaTime, frameId));

      _window->update();
    }
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
      if (events[i].window.event == SDL_WINDOWEVENT_RESIZED) {
        _window->onResized(events[i].window.data1, events[i].window.data2);
      }
    }
  }
}
