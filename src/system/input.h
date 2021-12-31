#pragma once

enum KeyId {
  KeyId_Escape = 0,
  KeyId_Space,
  KeyId_Enter,
  KeyId_Up,
  KeyId_Down,
  KeyId_Left,
  KeyId_Right,
};

enum MouseButton {
  MouseButton_Left,
  MouseButton_Right,
  MouseButton_Middle,
  MouseButton_Motion
};

enum InputState: unsigned int {
  InputState_Pressed = 1,
  InputState_Released = 2,
  InputState_Hold = 3,
};

struct InputEvent {
  KeyId    keyId;
  uint32_t state;
};

struct MouseEvent {
  MouseButton button;
  uint32_t    state;
  glm::ivec2  pos;
};

class Input {
public:
  using EventCallback = std::function<void(const InputEvent&)>;
  using MouseCallback = std::function<void(const MouseEvent&)>;

public:
  Input();

  void registerCallbacks(EventCallback callback, MouseCallback mouseCallback) {
    _callback = callback;
    _mouseCallback = mouseCallback;
  }
  void update();

private:
  void updateKeybard();
  void updateMouse();

private:
  typedef std::map<uint32_t, KeyId> KeyCodes;
  typedef std::map<uint32_t, MouseButton> ButtonCodes;

  KeyCodes    _keyCodes;
  ButtonCodes _mouseButtonCodes;

  EventCallback _callback;
  MouseCallback _mouseCallback;
};
