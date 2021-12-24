#pragma once

#include "graphics/font_atlas.h"

class Font {
public:
  static bool init();
  static void shutdown();
  static FontAtlasRef loadFont(const char* fontFile, int pixelSize);
};
