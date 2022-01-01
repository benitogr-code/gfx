#pragma once

struct ImageData;

class Texture;
typedef std::shared_ptr<Texture> TextureRef;

struct TextureCreateParams {
  TextureCreateParams()
    : filePath(nullptr) {
    }

  const char* filePath;
};

class Texture {
public:
  ~Texture();

  void bind();

  static TextureRef Create(const TextureCreateParams& params);

private:
  Texture();
  Texture(const Texture& texture) = delete;

  void loadImage(const ImageData& image);

private:
  unsigned int _id;
};
