#include "window.h"

#include <SDL.h>
#include <glad/glad.h>

Window::Window(const WindowDesc& desc)
  : _window(nullptr)
  , _context(nullptr) {
  _width = desc.width;
  _height = desc.height;
  _title = desc.title;
}

Window::~Window() {
  if (_context != nullptr)
    SDL_GL_DeleteContext(_context);

  if (_window)
    SDL_DestroyWindow(_window);
}

bool Window::init() {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  _window = SDL_CreateWindow(
    _title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    _width, _height, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
  );

  if (_window == nullptr) {
    LOG_ERROR("Failed to create SDL window: {}", SDL_GetError());

    return false;
  }

  _context = SDL_GL_CreateContext(_window);
  if (_context == nullptr) {
    LOG_ERROR("Failed to create GL context for window. {}", SDL_GetError());

    return false;
  }

  gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

  return true;
}

void Window::resize(int width, int height) {
  SDL_SetWindowSize(_window, width, height);
  glViewport(0, 0, width, height);
}

void Window::onResized(int width, int height) {
  glViewport(0, 0, width, height);
}

void Window::update() {
  SDL_GL_SwapWindow(_window);
}
