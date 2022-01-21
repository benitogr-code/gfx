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

protected:
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

#define UBO_WORDSIZE 4

class UBO {
public:

  enum class ItemType : uint8_t {
    Scalar = 0,
    Vec2,
    Vec3,
    Vec4,
    Array,
    Struct,
    Invalid
  };

  struct Item {
    Item(ItemType type = ItemType::Scalar)
      : type(type)
      , length(0) {

      switch(type) {
        case ItemType::Scalar:  baseAlign = UBO_WORDSIZE; break;
        case ItemType::Vec2:    baseAlign = 2 * UBO_WORDSIZE; break;
        case ItemType::Vec3:    baseAlign = 4 * UBO_WORDSIZE; break;
        case ItemType::Vec4:    baseAlign = 4 * UBO_WORDSIZE; break;
        case ItemType::Array:
        case ItemType::Struct:
        default:
          baseAlign = 0;
      }
    }

    uint32_t alignPow2() const {
      switch(baseAlign) {
        case 2: return 1;
        case 4: return 2;
        case 8: return 3;
        case 16: return 4;
        default: return 0;
      }
    }

    uint32_t getSize() const {
      switch(type) {
        case ItemType::Scalar: return UBO_WORDSIZE;
        case ItemType::Vec2:    return 2 * UBO_WORDSIZE;
        case ItemType::Vec3:    return 3 * UBO_WORDSIZE;
        case ItemType::Vec4:    return 4 * UBO_WORDSIZE;
        case ItemType::Array:
          return length * roundUpPow2(list[0].getSize(), alignPow2());
        case ItemType::Struct: {
          uint32_t offset = 0;
          for (auto& item : list) {
            offset = roundUpPow2(offset, item.alignPow2());
            offset += item.getSize();
          }
          return offset;
        }
        default:
          return 0;
      }
    }

    std::string toStr() const {
      switch(type) {
        case ItemType::Scalar:  return "scalar";
        case ItemType::Vec2:    return "vec2";
        case ItemType::Vec3:    return "vec3";
        case ItemType::Vec4:    return "vec4";
        case ItemType::Array:   return std::string("array<").append(list[0].toStr()).append(">");
        case ItemType::Struct:  return "struct";
        case ItemType::Invalid: return "???";
      }
    }

    ItemType type;
    uint32_t length;          // lenght of array / members in struct
    uint32_t baseAlign;
    std::vector<Item> list;
  };

  static uint32_t roundUpPow2(uint32_t value, uint8_t n) {
    uint32_t pow2n   = 1 << n;
    uint32_t divisor = pow2n - 1;
    uint32_t reminder = value & divisor;

    return reminder != 0 ? (value + pow2n - reminder) : value;
  }

  static Item newScalar() {
    return Item(ItemType::Scalar);
  }

  static Item newVec(uint8_t dimensions) {
    switch(dimensions) {
      case 2: return Item(ItemType::Vec2);
      case 3: return Item(ItemType::Vec3);
      case 4:
      default:
        return Item(ItemType::Vec4);
    }
  }

  static Item newArray(uint32_t length, Item arrayItem) {
    Item item(ItemType::Array);
    item.length = length;
    item.list = { arrayItem };
    item.list.shrink_to_fit();

    if (arrayItem.type == ItemType::Struct) {
      item.baseAlign = arrayItem.baseAlign;
    }
    else {
      item.baseAlign = roundUpPow2(arrayItem.baseAlign, 4);
    }

    return item;
  }

  static Item newStruct(std::vector<Item> items) {
    Item item(ItemType::Struct);
    item.list.insert(item.list.end(), items.begin(), items.end());
    item.list.shrink_to_fit();
    item.length = item.list.size();

    if (items.size() > 0) {
      for (Item& i : items) {
        item.baseAlign = std::max(item.baseAlign, i.baseAlign);
      }

      item.baseAlign = roundUpPow2(item.baseAlign, 4);
    }

    return item;
  }

  static Item newColumnMatrix(uint8_t columns, uint8_t rows) {
    return newArray(columns, newVec(rows));
  }

  static Item newColumnMatrixArray(uint32_t count, uint8_t columns, uint8_t rows) {
    return newArray(count * columns, newVec(rows));
  }

public:
  ~UBO();

  static UBORef Create(uint32_t bindIndex, const std::vector<UBO::Item>& items);

  void writeBegin();
  void writeEnd();
  void advanceCursor(uint32_t n);
  void advanceArray(uint32_t n);

  void writeInt(int value);
  void writeFloat(float value);
  void writeVec2(const glm::vec2& value);
  void writeVec3(const glm::vec3& value);
  void writeVec4(const glm::vec4& value);
  void writeMat4(const glm::mat4& value);

private:
  void write(const void* data, uint32_t size);
  UBO::Item* nextItem();
  bool pop();

private:
  typedef std::vector<std::pair<uint32_t, Item*>> IndexStack;

  UBO() = delete;
  UBO(uint32_t bindIndex, std::vector<UBO::Item> items);

  uint32_t   _id;
  uint32_t   _bindIndex;
  uint32_t   _size;

  UBO::Item  _block;
  IndexStack _stack;
  int        _stackDepth;

  std::vector<uint8_t> _writeBuffer;
  uint32_t   _writePos;
  uint32_t   _writePoppedPos;
};
