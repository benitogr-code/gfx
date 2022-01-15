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
    case BufferItemType::Mat4:     return sizeof(float) * 16;
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
    case BufferItemType::Mat4:     return GL_FLOAT;
  }

  return 0;
}

static uint32_t Align(uint32_t pos, uint32_t alignment) {
  while(pos % alignment != 0) {
    pos +=1;
  }

  return pos;
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

/*static*/ VBORef VBO::Create(uint32_t size, const BufferLayout& layout) {
  VBORef buffer(new VBO(size, layout));

  return buffer;
}

/*static*/ VBORef VBO::Create(const void* data, uint32_t size, const BufferLayout& layout) {
  VBORef buffer(new VBO(data, size, layout));

  return buffer;
}

void VBO::uploadData(const void* data, uint32_t size) {
  if (!hasFlag(Flag_Dynamic)) {
    LOG_WARN("[Renderer] VBO cannot upload data to static buffer");
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, _id);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
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

/*static*/ VAORef VAO::Create() {
  VAORef vertexArray(new VAO());

  return vertexArray;
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
    case BufferItemType::Mat4:
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

// UBO
UBO::UBO(uint32_t size, uint32_t bindIndex) {
  _size = size;
  _bindIndex = bindIndex;
  _writeBuffer.resize(size);
  _writePos = 0;

  glGenBuffers(1, &_id);
  glBindBuffer(GL_UNIFORM_BUFFER, _id);
  glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferBase(GL_UNIFORM_BUFFER, bindIndex, _id);
}

UBO::~UBO() {
  glDeleteBuffers(1, &_id);
}

/*static*/ UBORef UBO::Create(uint32_t bindIndex, const BufferLayout& layout) {
  uint32_t size = 0;

  for (uint32_t i = 0; i < layout.itemCount(); ++i) {
    const auto& item = layout.itemAt(i);

    size = Align(size, item.getStd140Alignment());
    size += item.size;
  }

  UBORef buffer(new UBO(size, bindIndex));

  return buffer;
}

void UBO::writeBegin() {
  _writeBuffer.clear();
  _writeBuffer.resize(_size);
  _writePos = 0;
}

void UBO::writeEnd() {
  glBindBuffer(GL_UNIFORM_BUFFER, _id);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, _writeBuffer.size(), _writeBuffer.data());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::writeInt(int value) {
  const uint32_t size = sizeof(int);
  const uint32_t alignment = 4;

  write(&value, size, alignment);
}

void UBO::writeFloat(float value) {
  const uint32_t size = sizeof(float);
  const uint32_t alignment = 4;

  write(&value, size, alignment);
}

void UBO::writeVec2(const glm::vec2& value) {
  const uint32_t size = sizeof(float)*2;
  const uint32_t alignment = 8;

  write(&value, size, alignment);
}

void UBO::writeVec3(const glm::vec3& value) {
  const uint32_t size = sizeof(float)*3;
  const uint32_t alignment = 16;

  write(glm::value_ptr(value), size, alignment);
}

void UBO::writeVec4(const glm::vec4& value) {
  const uint32_t size = sizeof(float)*4;
  const uint32_t alignment = 16;

  write(glm::value_ptr(value), size, alignment);
}

void UBO::writeMat4(const glm::mat4& value) {
  const uint32_t size = sizeof(float)*16;
  const uint32_t alignment = 16;

  write(glm::value_ptr(value), size, alignment);
}

void UBO::write(const void* data, uint32_t size, uint32_t alignment) {
  _writePos = Align(_writePos, alignment);

  if (_writePos + size <= _size) {
    memcpy(_writeBuffer.data() + _writePos, data, size);
    _writePos += size;
  }
  else {
    LOG_ERROR("[UBO] Write error, buffer is full");
  }
}
