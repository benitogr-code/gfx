#include "buffers.h"

#include <glad/glad.h>

// Helpers
static uint16_t BufferItemTypeSize(BufferItemType type){
  switch (type) {
    case BufferItemType::Float:   return sizeof(float);
    case BufferItemType::Float2:  return sizeof(float) * 2;
    case BufferItemType::Float3:  return sizeof(float) * 3;
    case BufferItemType::Float4:   return sizeof(float) * 4;
    case BufferItemType::Int:      return sizeof(int);
  }

  return 0;
}

static GLenum BufferItemTypeToOpenGLBaseType(BufferItemType type) {
  switch (type) {
    case BufferItemType::Float:    return GL_FLOAT;
    case BufferItemType::Float2:   return GL_FLOAT;
    case BufferItemType::Float3:   return GL_FLOAT;
    case BufferItemType::Float4:   return GL_FLOAT;
    case BufferItemType::Int:      return GL_INT;
  }

  return 0;
}

// BufferItem
BufferItem::BufferItem(BufferItemType _type, const std::string& _name)
  : name(_name)
  , type(_type)
  , size(BufferItemTypeSize(_type))
  , offset(0) {
}

// BufferLayout
BufferLayout::BufferLayout()
  : _stride(0) {

}

BufferLayout::BufferLayout(std::initializer_list<BufferItem> items)
  : _items(items) {

  uint16_t offset = 0;
  uint16_t stride = 0;

  for (auto& item : _items) {
    item.offset = offset;
    offset += item.size;
    stride += item.size;
  }

  _stride = stride;
}

// VBO
VBO::VBO(const void* data, uint32_t size, const BufferLayout& layout)
  : _id(0)
  , _size(size)
  , _flags(0) {

  glGenBuffers(1, &_id);
  glBindBuffer(GL_ARRAY_BUFFER, _id);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

  _layout = layout;
}

VBO::VBO(uint32_t size, const BufferLayout& layout)
  : _id(0)
  , _size(size)
  , _flags(0) {

  glGenBuffers(1, &_id);
  glBindBuffer(GL_ARRAY_BUFFER, _id);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

  _layout = layout;
  setFlag(Flag_Dynamic);
}

VBO::~VBO() {
  glDeleteBuffers(1, &_id);
}

void VBO::uploadData(const void* data, uint32_t size) {
  if (!hasFlag(Flag_Dynamic)) {
    LOG_WARN("VBO cannot upload data to static buffer");
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, _id);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

/*static*/ VBORef VBO::Create(uint32_t size, const BufferLayout& layout) {
  VBORef buffer(new VBO(size, layout));

  return buffer;
}

/*static*/ VBORef VBO::Create(const void* data, uint32_t size, const BufferLayout& layout) {
  VBORef buffer(new VBO(data, size, layout));

  return buffer;
}

// IBO
IBO::IBO(const uint32_t* indices, uint32_t count)
  : _count(count) {
  glGenBuffers(1, &_id);
  glBindBuffer(GL_ARRAY_BUFFER, _id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t) * count, indices, GL_STATIC_DRAW);
}

IBO::~IBO() {
  glDeleteBuffers(1, &_id);
}

/*static*/ IBORef IBO::Create(const uint32_t* indices, uint32_t count) {
  IBORef buffer(new IBO(indices, count));

  return buffer;
}

// VAO
VAO::VAO()
  : _attributeCount(0) {
  glGenVertexArrays(1, &_id);
}

VAO::~VAO() {
  glDeleteVertexArrays(1, &_id);
}

void VAO::bind() {
  glBindVertexArray(_id);
}

void VAO::unbind() {
  glBindVertexArray(0);
}

void VAO::addVertextBuffer(VBORef buffer) {
  glBindVertexArray(_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer->id());

  const auto attribDivisor = buffer->hasFlag(VBO::Flag_Instance) ? 1 : 0;
  const auto& layout = buffer->layout();
  for (uint32_t i = 0; i < layout.itemCount(); ++i) {
    const auto& item = layout.itemAt(i);

    switch (item.type){
    case BufferItemType::Float:
    case BufferItemType::Float2:
    case BufferItemType::Float3:
    case BufferItemType::Float4:
      {
        const uint32_t idx = _attributeCount;
        glVertexAttribPointer(
          idx,
          item.getComponentCount(),
          BufferItemTypeToOpenGLBaseType(item.type),
          GL_FALSE,
          layout.stride(),
          INT_TO_VOIDPTR(item.offset)
        );
        glEnableVertexAttribArray(idx);
        glVertexAttribDivisor(idx, attribDivisor);

        _attributeCount++;
      }
      break;
    case BufferItemType::Int:
      {
        const uint32_t idx = _attributeCount;

        glVertexAttribIPointer(
          idx,
          item.getComponentCount(),
          BufferItemTypeToOpenGLBaseType(item.type),
          layout.stride(),
          INT_TO_VOIDPTR(item.offset)
        );
        glEnableVertexAttribArray(idx);
        glVertexAttribDivisor(idx, attribDivisor);

        _attributeCount++;
      }
      break;
    }
  }

  glBindVertexArray(0);

  _vertexBuffers.push_back(buffer);
}

void VAO::setIndexBuffer(IBORef buffer) {
  glBindVertexArray(_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id());
  glBindVertexArray(0);

  _indexBuffer = buffer;
}

/*static*/ VAORef VAO::Create() {
  VAORef vertexArray(new VAO());

  return vertexArray;
}
