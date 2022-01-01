#pragma once

#include <string>

struct SDL_Window;

struct WindowDesc {
  int width;
  int height;
  const char* title;
};

class Window {
private:
  typedef void* SDL_GLContext;

public:
  Window(const WindowDesc& desc);
  ~Window();

  bool init();
  void resize(int width, int height);
  void onResized(int width, int height);
  void update();

  SDL_Window*   getWindow() const { return _window; }
  SDL_GLContext getGLContext() const { return _context; }
  float         getAspectRatio() const { return (float)_width / (float)_height; }

private:
  int _width;
  int _height;
  std::string _title;

  SDL_Window* _window;
  SDL_GLContext _context;
};
