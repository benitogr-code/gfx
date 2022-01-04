#pragma once

enum class BufferItemType {
  Float,
  Float2,
  Float3,
  Float4,
  Int,
  Mat4,
};

struct BufferItem {
  std::string    name;
  BufferItemType type;
  uint16_t       size;
  uint16_t       offset;

  BufferItem() {}
  BufferItem(BufferItemType _type, const std::string& _name);

  uint32_t getComponentCount() const {
    switch (type) {
      case BufferItemType::Float:    return 1;
      case BufferItemType::Float2:   return 2;
      case BufferItemType::Float3:   return 3;
      case BufferItemType::Float4:   return 4;
      case BufferItemType::Int:      return 1;
      case BufferItemType::Mat4:     return 16;
    }

    return 0;
  }

  uint32_t getStd140Alignment() const {
    switch (type) {
      case BufferItemType::Float:    return 4;
      case BufferItemType::Float2:   return 8;
      case BufferItemType::Float3:   return 16;
      case BufferItemType::Float4:   return 16;
      case BufferItemType::Int:      return 4;
      case BufferItemType::Mat4:     return 16;
    }

    return 0;
  }
};

class BufferLayout {
public:
  BufferLayout();
  BufferLayout(std::initializer_list<BufferItem> items);

  uint32_t itemCount() const { return _items.size(); }
  const BufferItem& itemAt(uint32_t idx) const { return _items[idx]; }
  uint32_t stride() const { return _stride; }

private:
  std::vector<BufferItem> _items;
  uint32_t _stride;
};

class VBO;
typedef std::shared_ptr<VBO> VBORef;

class VBO {
public:
  enum Flag {
    Flag_Dynamic = BIT(1),
    Flag_Instance = BIT(2),
  };

public:
  ~VBO();

  static VBORef Create(uint32_t size, const BufferLayout& layout);
  static VBORef Create(const void* data, uint32_t size, const BufferLayout& layout);

  uint32_t id() const { return _id; }
  void setFlag(Flag flag) { _flags |= flag; }

  bool hasFlag(Flag flag) const { return (_flags & flag) != 0; }
  const BufferLayout& layout() const { return _layout; }

  void uploadData(const void* data, uint32_t size);

private:
  VBO() = delete;
  VBO(const VBO&) = delete;

  VBO(uint32_t size, const BufferLayout& layout);
  VBO(const void* data, uint32_t size, const BufferLayout& layout);

private:
  BufferLayout _layout;
  uint32_t _id;
  uint32_t _size;
  uint32_t _flags;
};

class IBO;
typedef std::shared_ptr<IBO> IBORef;

class IBO {
public:
  ~IBO();

  static IBORef Create(const uint32_t* indices, uint32_t count);

  uint32_t id() const { return _id; }
  uint32_t count() const { return _count; }

private:
  IBO() = delete;
  IBO(const IBO&) = delete;

  IBO(const uint32_t* indices, uint32_t count);

private:
  uint32_t _id;
  uint32_t _count;
};

class VAO;
typedef std::shared_ptr<VAO> VAORef;

class VAO {
public:
  ~VAO();

  static VAORef Create();

  void bind();
  void unbind();

  void addVertextBuffer(VBORef buffer);
  void setIndexBuffer(IBORef buffer);

  const uint32_t indexCount() const { return _indexBuffer ? _indexBuffer->count() : 0; }
private:
  VAO();
  VAO(const VAO&) = delete;

private:
  uint32_t _id;
  std::vector<VBORef> _vertexBuffers;
  IBORef _indexBuffer;
  uint32_t _attributeCount;
};

class UBO;
typedef std::shared_ptr<UBO> UBORef;

class UBO {
public:
  ~UBO();

  void writeBegin();
  void writeEnd();
  void writeInt(int value);
  void writeFloat(float value);
  void writeVec2(const glm::vec2& value);
  void writeVec3(const glm::vec3& value);
  void writeVec4(const glm::vec4& value);
  void writeMat4(const glm::mat4& value);

  static UBORef Create(uint32_t bindIndex, const BufferLayout& layout);

private:
  UBO(uint32_t size, uint32_t bindIndex);
  UBO(const UBO&) = delete;

  void write(const void* data, uint32_t size, uint32_t alignment);

private:
  uint32_t _id;
  uint32_t _bindIndex;
  uint32_t _size;

  std::vector<uint8_t> _writeBuffer;
  uint32_t             _writePos;
};
