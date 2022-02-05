#pragma once

enum KeyboardKey {
  KeyboardKey_Escape = 0,
  KeyboardKey_Space,
  KeyboardKey_Enter,

  KeyboardKey_Up,
  KeyboardKey_Down,
  KeyboardKey_Left,
  KeyboardKey_Right,

  KeyboardKey_A,
  KeyboardKey_S,
  KeyboardKey_D,
  KeyboardKey_W,

  KeyboardKey_1,
  KeyboardKey_2,
  KeyboardKey_3,
  KeyboardKey_4,
};

enum MouseButton {
  MouseButton_Left,
  MouseButton_Right,
  MouseButton_Middle,
  MouseButton_Motion,
  MouseButton_MouseWheel,
};

enum InputState: unsigned int {
  InputState_Pressed = 1,
  InputState_Released = 2,
  InputState_Hold = 3,
};

struct InputEvent {
  KeyboardKey    key;
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
  typedef std::map<uint32_t, KeyboardKey> KeyCodes;
  typedef std::map<uint32_t, MouseButton> ButtonCodes;

  KeyCodes    _keyCodes;
  ButtonCodes _mouseButtonCodes;

  EventCallback _callback;
  MouseCallback _mouseCallback;
};
