#include "font_atlas.h"

#include <glad/glad.h>

#include <ft2build.h>
#include FT_FREETYPE_H

FontAtlas::FontAtlas(FT_FaceRec_* face, int pixelSize) {
  FT_Set_Pixel_Sizes(face, 0, pixelSize);

  const int padding = 2;
  const int charCount = _characters.max_size();

  for(unsigned char i = 0; i < charCount; ++i) {
    if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
      LOG_WARN("Error loading font character {}, skipping", char(i));
      continue;
    }

    _width += face->glyph->bitmap.width + padding;
    _height = std::max(_height, (int)face->glyph->bitmap.rows);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &_textureId);
  glBindTexture(GL_TEXTURE_2D, _textureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

  int textureOffset = 0;

  std::vector<unsigned char> emptyBuffer;
  emptyBuffer.resize(_height, 0);

  for(unsigned char i = 0; i < charCount; ++i) {
    if(FT_Load_Char(face, i, FT_LOAD_RENDER))
      continue;

    // Add to texture atlas
    glTexSubImage2D(GL_TEXTURE_2D, 0, textureOffset, 0, 1, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, emptyBuffer.data());
    glTexSubImage2D(GL_TEXTURE_2D, 0, textureOffset, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
    glTexSubImage2D(GL_TEXTURE_2D, 0, textureOffset, 0, 1, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, emptyBuffer.data());

    // Store character info
    // Note: advance is the horizontal distance (in 1/64th pixels) from the origin to the origin of the next glyph
    _characters[i].advance = glm::ivec2(face->glyph->advance.x >> 6, face->glyph->advance.y >> 6);
    _characters[i].size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
    _characters[i].bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);

    const int nextTextureOffset = textureOffset + face->glyph->bitmap.width + padding;
    _characters[i].atlasOffsets[0] = (float)textureOffset / (float)_width;
    _characters[i].atlasOffsets[1] = (float)(nextTextureOffset) / (float)_width;

    textureOffset = nextTextureOffset;
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

FontAtlas::~FontAtlas() {
  glDeleteTextures(1, &_textureId);
}

void FontAtlas::bindTexture() {
  glBindTexture(GL_TEXTURE_2D, _textureId);
}

/*static*/ FontAtlasRef FontAtlas::Create(FT_LibraryRec_* library, const char* fontFile, int pixelSize) {
  FT_Face face;
  if (FT_New_Face(library, fontFile, 0, &face)) {
    LOG_ERROR("Failed to load font {}", fontFile);
    return nullptr;
  }

  auto fontAtlas = FontAtlasRef(new FontAtlas(face, pixelSize));

  FT_Done_Face(face);

  return fontAtlas;
}
