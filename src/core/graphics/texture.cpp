#include "texture.h"
#include "core/file_utils.h"

#include <glad/glad.h>

// Helpers
static GLint MapWrapMode(TextureWrapMode mode) {
  switch(mode) {
    case TextureWrapMode::Repeat: return GL_REPEAT;
    case TextureWrapMode::Mirror: return GL_MIRRORED_REPEAT;
    case TextureWrapMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
    case TextureWrapMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
  }
}

Texture::Texture()
  : _id(0) {
}

Texture::~Texture() {
  glDeleteTextures(1, &_id);
}

void Texture::bind() {
  glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::load2DImage(const ImageData& image, TextureWrapMode wrapMode) {
  glGenTextures(1, &_id);
  glBindTexture(GL_TEXTURE_2D, _id);

  // Wrapping params
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, MapWrapMode(wrapMode)); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, MapWrapMode(wrapMode));
  // Filtering params
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const GLenum format = image.bytesPerPixel == 4 ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data.data());
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::load3DImage(const std::vector<ImageData>& images) {
  glGenTextures(1, &_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

  // Wrapping params
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Filtering params
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  for (int i = 0; i < images.size(); ++i) {
    auto& image = images[i];

    const GLenum format = image.bytesPerPixel == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data.data());
  }
}


/*static*/ TextureRef Texture::Create(const TextureCreateParams& params) {
  TextureRef texture(new Texture());

  ImageData image;
  if (FileUtils::readImageFile(params.filePath, image)) {
    texture->load2DImage(image, params.wrapmode);
  }

  return texture;
}

/*static*/ TextureRef Texture::CreateCubemap(const Texture3DCreateParams& params) {
  TextureRef texture(new Texture());

  const int kRequiredTextures = 6;
  const int fileCount = std::min((int)params.filePaths.size(), kRequiredTextures);

  std::vector<ImageData> images;
  images.resize(fileCount);
  int texturesLoaded = 0;
  for (int i = 0; i < fileCount; ++i) {
    if (FileUtils::readImageFile(params.filePaths[i].c_str(), images[i])) {
      texturesLoaded++;
    }
  }

  if (texturesLoaded == kRequiredTextures) {
    texture->load3DImage(images);
  }
  else {
    LOG_WARN("[Texture] Cube map requires {} textures but only got {}", kRequiredTextures, texturesLoaded);
  }


  return texture;
}
