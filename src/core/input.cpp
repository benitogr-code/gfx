#include "input.h"

#include "imgui/imgui_impl_sdl.h"
#include <SDL.h>

Input::Input() {
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_ESCAPE, KeyboardKey_Escape));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_SPACE, KeyboardKey_Space));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_RETURN, KeyboardKey_Enter));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_UP, KeyboardKey_Up));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_DOWN, KeyboardKey_Down));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_LEFT, KeyboardKey_Left));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_RIGHT, KeyboardKey_Right));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_A, KeyboardKey_A));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_S, KeyboardKey_S));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_D, KeyboardKey_D));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_W, KeyboardKey_W));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_1, KeyboardKey_1));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_2, KeyboardKey_2));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_3, KeyboardKey_3));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_4, KeyboardKey_4));
  _keyCodes.insert(KeyCodes::value_type(SDL_SCANCODE_F12, KeyboardKey_F12));

  _mouseButtonCodes.insert(ButtonCodes::value_type(SDL_BUTTON_LEFT, MouseButton_Left));
  _mouseButtonCodes.insert(ButtonCodes::value_type(SDL_BUTTON_RIGHT, MouseButton_Left));
  _mouseButtonCodes.insert(ButtonCodes::value_type(SDL_BUTTON_MIDDLE, MouseButton_Middle));
}

void Input::update() {
  updateKeybard();
  updateMouse();
}

void Input::updateKeybard() {
  const int maxEvents = 32;
  SDL_Event events[maxEvents];
  const int eventsRead = SDL_PeepEvents(events, maxEvents, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYUP);

  if (eventsRead == -1) {
    LOG_ERROR("[Input] Error reading keyboard events: {}", SDL_GetError());

    return;
  }

  std::vector<InputEvent> inputEvents;
  inputEvents.reserve(eventsRead);

  for (int i = 0; i < eventsRead; ++i) {
    const auto& keyEvent = events[i].key;

    const auto iter = _keyCodes.find(keyEvent.keysym.scancode);
    if (iter != _keyCodes.end()) {
      InputEvent input;
      input.key = iter->second;
      input.state = keyEvent.type == SDL_KEYDOWN ? InputState_Pressed : InputState_Released;
      if (keyEvent.repeat > 0)
        input.state = InputState_Hold;

      inputEvents.push_back(input);
    }

    ImGui_ImplSDL2_ProcessEvent(&events[i]);
  }

  for (auto iter = inputEvents.begin(); iter != inputEvents.end(); ++iter) {
    _callback(*iter);
  }
}

void Input::updateMouse() {
  const int maxEvents = 32;
  SDL_Event events[maxEvents];
  const int eventsRead = SDL_PeepEvents(events, maxEvents, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEWHEEL);

  if (eventsRead == -1) {
    LOG_ERROR("[Input] Error reading keyboard events: {}", SDL_GetError());

    return;
  }

  std::vector<MouseEvent> mouseEvents;
  mouseEvents.reserve(eventsRead);

  ImGuiIO& io = ImGui::GetIO();

  for (int i = 0; i < eventsRead; ++i) {
    if (events[i].type == SDL_MOUSEMOTION) {
      if (!io.WantCaptureMouse) {
        const auto& motionEvent = events[i].motion;

        MouseEvent mouse;
        mouse.button = MouseButton_Motion;
        mouse.state = motionEvent.state == SDL_PRESSED ? InputState_Pressed : InputState_Released;
        mouse.pos = glm::ivec2(motionEvent.x, motionEvent.y);

        mouseEvents.push_back(mouse);
      }
    }
    else if (events[i].type == SDL_MOUSEWHEEL) {
      if (!io.WantCaptureMouse) {
        const auto& wheel = events[i].wheel;

        MouseEvent mouse;
        mouse.button = MouseButton_MouseWheel;
        mouse.state = InputState_Pressed;
        mouse.pos = glm::ivec2(wheel.x, wheel.y);

        mouseEvents.push_back(mouse);
      }
    }
    else {
      const auto& buttonEvent = events[i].button;

      const auto iter = _mouseButtonCodes.find(buttonEvent.button);
      if (iter != _mouseButtonCodes.end() && !io.WantCaptureMouse) {
        MouseEvent mouse;
        mouse.button = iter->second;
        mouse.state = buttonEvent.state == SDL_PRESSED ? InputState_Pressed : InputState_Released;
        mouse.pos = glm::ivec2(buttonEvent.x, buttonEvent.y);

        mouseEvents.push_back(mouse);
      }
    }

    ImGui_ImplSDL2_ProcessEvent(&events[i]);
  }

  for (auto iter = mouseEvents.begin(); iter != mouseEvents.end(); ++iter) {
    _mouseCallback(*iter);
  }
}
