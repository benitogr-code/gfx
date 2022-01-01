#pragma once

class FontAtlas;
typedef std::shared_ptr<FontAtlas> FontAtlasRef;

struct FT_LibraryRec_;
struct FT_FaceRec_;

class FontAtlas {
public:
  struct CharacterInfo {
    glm::ivec2 advance;
    glm::ivec2 bearing;
    glm::ivec2 size;
    glm::vec2  atlasOffsets;
  };

public:
  ~FontAtlas();

  int getHeight() const { return _height; }
  int getWidth() const { return _width; }

  const CharacterInfo* getCharacterInfo(uint8_t c) const {
    return (c < _characters.max_size()) ? &_characters[c] : nullptr;
  }
  void bindTexture();

  static FontAtlasRef Create(FT_LibraryRec_* library, const char* fontFile, int pixelSize);

private:
  FontAtlas() = delete;
  FontAtlas(const FontAtlas&) = delete;
  FontAtlas(FT_FaceRec_* face, int pixelSize);

private:
  std::array<CharacterInfo, 128> _characters;
  uint32_t _textureId = 0;
  int _width = 0;
  int _height = 0;
};
