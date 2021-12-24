#pragma once

enum class BufferItemType {
  Float,
  Float2,
  Float3,
  Float4,
  Int,
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

class VertexBuffer;
typedef std::shared_ptr<VertexBuffer> VertexBufferRef;

class VertexBuffer {
public:
  enum Flag {
    Flag_Dynamic = BIT(1),
    Flag_Instance = BIT(2),
  };

public:
  ~VertexBuffer();

  uint32_t id() const { return _id; }
  void setFlag(Flag flag) { _flags |= flag; }

  bool hasFlag(Flag flag) const { return (_flags & flag) != 0; }
  const BufferLayout& layout() const { return _layout; }

  void uploadData(const void* data, uint32_t size);

  static VertexBufferRef Create(uint32_t size, const BufferLayout& layout);
  static VertexBufferRef Create(const void* data, uint32_t size, const BufferLayout& layout);

private:
  VertexBuffer() = delete;
  VertexBuffer(const VertexBuffer&) = delete;

  VertexBuffer(uint32_t size, const BufferLayout& layout);
  VertexBuffer(const void* data, uint32_t size, const BufferLayout& layout);

private:
  BufferLayout _layout;
  uint32_t _id;
  uint32_t _size;
  uint32_t _flags;
};

class IndexBuffer;
typedef std::shared_ptr<IndexBuffer> IndexBufferRef;

class IndexBuffer {
public:
  ~IndexBuffer();

  uint32_t id() const { return _id; }
  uint32_t count() const { return _count; }

  static IndexBufferRef Create(const uint32_t* indices, uint32_t count);

private:
  IndexBuffer() = delete;
  IndexBuffer(const IndexBuffer&) = delete;

  IndexBuffer(const uint32_t* indices, uint32_t count);

private:
  uint32_t _id;
  uint32_t _count;
};

class VertexArray;
typedef std::shared_ptr<VertexArray> VertexArrayRef;

class VertexArray {
public:
  ~VertexArray();

  void bind();
  void unbind();

  void addVertextBuffer(VertexBufferRef buffer);
  void setIndexBuffer(IndexBufferRef buffer);

  const uint32_t indexCount() const { return _indexBuffer ? _indexBuffer->count() : 0; }

  static VertexArrayRef Create();
private:
  VertexArray();
  VertexArray(const VertexArray&) = delete;

private:
  uint32_t _id;
  std::vector<VertexBufferRef> _vertexBuffers;
  IndexBufferRef _indexBuffer;
  uint32_t _attributeCount;
};
