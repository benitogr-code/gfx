#include "debug_utils.h"

#include <glad/glad.h>

void glCheckError_(const char *file, int line) {
  GLenum errorCode = glGetError();

  while (errorCode != GL_NO_ERROR) {
    const char* errorMsg;
    switch (errorCode) {
      case GL_INVALID_ENUM:
        errorMsg = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        errorMsg = "INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        errorMsg = "INVALID_OPERATION";
        break;
      case GL_OUT_OF_MEMORY:
        errorMsg = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        errorMsg = "INVALID_FRAMEBUFFER_OPERATION";
        break;
      default:
        errorMsg = "UNKNOWN_ERROR";
        break;
    }

    LOG_ERROR("OpenGL error: {0} | {1} ({2})", errorMsg, file, line);

    errorCode = glGetError();
  }
}
