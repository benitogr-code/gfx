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

void VAO::addVertexBuffer(VBORef buffer) {
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

VBORef VAO::getVertexBuffer(size_t i) const {
  return i < _vertexBuffers.size() ? _vertexBuffers[i] : nullptr;
}

void VAO::setIndexBuffer(IBORef buffer) {
  glBindVertexArray(_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->id());
  glBindVertexArray(0);

  _indexBuffer = buffer;
}

// UBO
UBO::UBO(uint32_t bindIndex, std::vector<UBO::Item> items) {
  _block = UBO::Item(UBO::newStruct(items));
  _size = _block.getSize();
  _bindIndex = bindIndex;
  _stackDepth = -1;
  _writeBuffer.resize(_size);

  glGenBuffers(1, &_id);
  glBindBuffer(GL_UNIFORM_BUFFER, _id);
  glBufferData(GL_UNIFORM_BUFFER, _size, NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferBase(GL_UNIFORM_BUFFER, bindIndex, _id);
}

UBO::~UBO() {
  glDeleteBuffers(1, &_id);
}

UBORef UBO::Create(uint32_t bindIndex, const std::vector<UBO::Item>& items) {
  UBORef buffer(new UBO(bindIndex, items));

  return buffer;
}

void UBO::writeBegin() {
  _stack.clear();
  _stack.push_back({0, &_block});
  _stackDepth = 0;
  _writePos = _writePoppedPos = 0;
  _writeBuffer.clear();
}

void UBO::writeEnd() {
  glBindBuffer(GL_UNIFORM_BUFFER, _id);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, std::min(_writePos, _size), _writeBuffer.data());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UBO::advanceCursor(uint32_t n) {
  for (int i = 0; i < n; ++i) {
    auto item = nextItem();
    if (item != nullptr) {
      _writePos = roundUpPow2(_writePos, item->alignPow2());
      if (_writePoppedPos) {
        _writePos = _writePoppedPos;
      }
      else {
        _writePos += item->getSize();
      }
    }
    else {
      LOG_ERROR("[UBO] Advance cursor error, all items traversed");
    }
  }
}

void UBO::advanceArray(uint32_t n) {
  if (_stackDepth < 0) return;

  auto item = _stack[_stackDepth].second;

  if (item->type == ItemType::Struct) {
    item = &item->list[_stack[_stackDepth].first];
    uint32_t  depthAdd = 0;
    IndexStack stackAdd;

    while (item->type == ItemType::Struct) {
      depthAdd++;
      stackAdd.push_back({0, item});
      item = &item->list[0];
    }

    if (item->type != ItemType::Array) {
      return;
    }

    // Found next array
    _stackDepth += depthAdd + 1;
    _stack.insert(_stack.end(), stackAdd.begin(), stackAdd.end());
    _stack.push_back({0, item});
  }

  uint32_t finalIdx = _stack[_stackDepth].first + n;
  uint32_t advanceCount = n;
  if (finalIdx > _stack[_stackDepth].second->length) {
    advanceCount = _stack[_stackDepth].second->length - _stack[_stackDepth].first;
  }

  _writePos += advanceCount * roundUpPow2(item->list[0].getSize(), item->alignPow2());
  _writePoppedPos = _writePos;
  if (pop()) {
    _writePos = _writePoppedPos;
  }

  //LOG_INFO("[UBO]{} advanced array {} elements, new position {}", _bindIndex, advanceCount, _writePos);
}

void UBO::writeInt(int value) {
  const uint32_t size = sizeof(int);

  write(&value, size);
}

void UBO::writeFloat(float value) {
  const uint32_t size = sizeof(float);

  write(&value, size);
}

void UBO::writeVec2(const glm::vec2& value) {
  const uint32_t size = sizeof(float)*2;

  write(&value, size);
}

void UBO::writeVec3(const glm::vec3& value) {
  const uint32_t size = sizeof(float)*3;

  write(glm::value_ptr(value), size);
}

void UBO::writeVec4(const glm::vec4& value) {
  const uint32_t size = sizeof(float)*4;

  write(glm::value_ptr(value), size);
}

void UBO::writeMat4(const glm::mat4& value) {
  const uint32_t size = sizeof(float)*16;

  for (int i = 0; i < 4; ++i) {
    writeVec4(value[i]);
  }
}

void UBO::write(const void* data, uint32_t size) {
    auto item = nextItem();

    if (item != nullptr) {
      _writePos = roundUpPow2(_writePos, item->alignPow2());

      if (_writePos + size <= _size) {
        //LOG_INFO("[UBO]{} write {} at position {} size {}/{}", _bindIndex, item->toStr(), _writePos, size, item->getSize());
        memcpy(_writeBuffer.data() + _writePos, data, size);
        if (_writePoppedPos) {
          _writePos = _writePoppedPos;
        }
        else {
          _writePos += item->getSize();
        }
      }
      else {
        LOG_ERROR("[UBO] Write error, buffer is full");
      }
    }
    else {
      LOG_ERROR("[UBO] Write error, all items traversed");
    }
}

UBO::Item* UBO::nextItem() {
  if (_stackDepth < 0) return nullptr;

  auto currentItem = _stack[_stackDepth].second;

  if (currentItem->type == ItemType::Struct) {
    currentItem = &currentItem->list[_stack[_stackDepth].first];
  }
  else { //array
    currentItem = &currentItem->list[0];
  }

  while (currentItem->type == ItemType::Struct || currentItem->type == ItemType::Array) {
    _stackDepth++;
    _stack.push_back({0, currentItem});
    currentItem = &currentItem->list[0];
  }

  _writePoppedPos = roundUpPow2(_writePos, currentItem->alignPow2()) + currentItem->getSize();
  if (!pop()) {
    _writePoppedPos = 0;
  }

  return currentItem;
}

bool UBO::pop() {
  bool pop = false;

  for (int i = _stackDepth; i >= 0; i--) {
    int nextIdx = ++_stack[i].first;
    if (nextIdx >= _stack[i].second->length) {
      _writePoppedPos = roundUpPow2(_writePoppedPos, _stack[i].second->alignPow2());
      _stack.erase(_stack.begin() + i);
      _stackDepth--;
      pop = true;
    }
    else {
      break;
    }
  }

  return pop;
}

// FBO
FBO::FBO(const FBOSpec& spec)
  : _spec(spec) {

  glGenFramebuffers(1, &_id);
  glBindFramebuffer(GL_FRAMEBUFFER, _id);

  // Color attachment
  glGenTextures(1, &_colorAttachment);
  glBindTexture(GL_TEXTURE_2D, _colorAttachment);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, spec.width, spec.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorAttachment, 0);

  // Depth and stencil
  glGenRenderbuffers(1, &_depthAttachment);
  glBindRenderbuffer(GL_RENDERBUFFER, _depthAttachment);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, spec.width, spec.height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthAttachment);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    LOG_ERROR("[FBO] Framebuffer not complete");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::~FBO() {
  glDeleteFramebuffers(1, &_id);
  glDeleteTextures(1, &_colorAttachment);
  glDeleteRenderbuffers(1, &_depthAttachment);
}

/*static*/ FBORef FBO::Create(const FBOSpec& spec) {
  FBORef buffer(new FBO(spec));

  return buffer;
}
