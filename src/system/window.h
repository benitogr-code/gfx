#pragma once

#include <string>

struct SDL_Window;

struct WindowDesc {
  int width;
  int height;
  const char* title;
};

class Window {
public:
  Window(const WindowDesc& desc);
  ~Window();

  bool init();
  void resize(int width, int height);
  void onResized(int width, int height);
  void update();

private:
  typedef void* SDL_GLContext;

  int _width;
  int _height;
  std::string _title;

  SDL_Window* _window;
  SDL_GLContext _context;
};
