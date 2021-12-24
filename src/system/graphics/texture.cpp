#include "texture.h"
#include "system/file_utils.h"

#include <glad/glad.h>

Texture::Texture()
  : _id(0) {
}

Texture::~Texture() {
  glDeleteTextures(1, &_id);
}

void Texture::bind() {
  glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::loadImage(const ImageData& image) {
  glGenTextures(1, &_id);
  glBindTexture(GL_TEXTURE_2D, _id);
  // Wrapping params
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // Filtering params
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const GLenum format = image.bytesPerPixel == 4 ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data.data());
  glGenerateMipmap(GL_TEXTURE_2D);
}

/*static*/ TextureRef Texture::Create(const TextureCreateParams& params) {
  TextureRef texture(new Texture());

  ImageData image;
  if (FileUtils::readPngFile(params.filePath, image)) {
    texture->loadImage(image);
  }

  return texture;
}
