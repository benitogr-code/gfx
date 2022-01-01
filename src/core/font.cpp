#include "font.h"
#include "file_utils.h"

#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library gFTLibrary = nullptr;

bool Font::init() {
  if (gFTLibrary != nullptr)
    return true;

  if (FT_Init_FreeType(&gFTLibrary)) {
    return false;
  }

  return true;
}

void Font::shutdown() {
  if (gFTLibrary != nullptr) {
    FT_Done_FreeType(gFTLibrary);
    gFTLibrary = nullptr;
  }
}

FontAtlasRef Font::loadFont(const char* fontFile, int pixelSize) {
  auto fontPath = FileUtils::getAbsolutePath(fontFile);

  return FontAtlas::Create(gFTLibrary, fontPath.c_str(), pixelSize);
}
