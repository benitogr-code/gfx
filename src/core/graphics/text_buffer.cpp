#include "text_buffer.h"

#include <glad/glad.h>

static constexpr uint32_t kVerticesPerChar = 6;

TextBuffer::TextBuffer(uint32_t capacity) {
    _maxSize =  sizeof(TextVertex) * kVerticesPerChar * capacity;
    _maxVertices = kVerticesPerChar * capacity;

    setup();
}

TextBufferRef TextBuffer::Create(uint32_t capacity) {
  return TextBufferRef(new TextBuffer(capacity));
}

uint32_t TextBuffer::draw(const std::vector<TextVertex>& vertices) {
  uint32_t drawcalls = 0;

  _vao->bind();

  const int vertexCount = (int)vertices.size();
  int currentVertex = 0;

  while(currentVertex < vertexCount) {
    int count = std::min(vertexCount-currentVertex, (int)_maxVertices);

    _vao->getVertexBuffer(0)->uploadData((const void*)&vertices[currentVertex], sizeof(TextVertex)*count);
    glDrawArrays(GL_TRIANGLES, 0, count);

    currentVertex += count;
    drawcalls++;
  }

  return drawcalls;
}

void TextBuffer::setup() {
  auto vbo = VBO::Create(
    _maxSize,
    BufferLayout({
      { BufferItemType::Float3, "position" },
      { BufferItemType::Float2, "texCoords"}
    })
  );

  _vao = VAO::Create();
  _vao->addVertexBuffer(vbo);
}
