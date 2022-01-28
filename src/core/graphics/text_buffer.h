#pragma once

#include "buffers.h"

struct TextVertex {
  glm::vec3 position;
  glm::vec2 texCoords;
  glm::vec3 color;
};

class TextBuffer;
typedef std::shared_ptr<TextBuffer> TextBufferRef;

class TextBuffer {
public:
  static TextBufferRef Create(uint32_t capacity);

  uint32_t draw(const std::vector<TextVertex>& vertices);

private:
  TextBuffer() = delete;
  TextBuffer(const TextBuffer& buffer) = delete;
  TextBuffer(uint32_t capacity);

  void setup();

private:
  VAORef   _vao;
  uint32_t _maxVertices;
  uint32_t _maxSize;
};
