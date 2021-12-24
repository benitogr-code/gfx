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

enum InputState: unsigned int {
  InputState_Pressed = 0,
  InputState_Released = 1,
  InputState_Hold = 2,
};

struct InputEvent {
  KeyId keyId;
  unsigned int state;
};

class Input {
public:
  using EventCallback = std::function<void(const InputEvent&)>;

public:
  Input();

  void registerCallback(EventCallback callback) {
    _callback = callback;
  }
  void update();

private:
  typedef std::map<unsigned int, KeyId> TScanCodes;

  TScanCodes _scanCodes;
  EventCallback _callback;
};
