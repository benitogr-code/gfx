#pragma once

struct ImageData;

class Texture;
typedef std::shared_ptr<Texture> TextureRef;

enum class TextureWrapMode {
  Repeat = 0,
  Mirror,
  ClampToEdge,
  ClampToBorder
};

struct TextureCreateParams {
  TextureCreateParams()
    : filePath(nullptr)
    , wrapmode(TextureWrapMode::Repeat) {
    }

  const char*     filePath;
  TextureWrapMode wrapmode;
};

struct Texture3DCreateParams {
  enum Faces {
    Right = 0,
    Left,
    Top,
    Bottom,
    Front,
    Back,
    Count
  };

  Texture3DCreateParams() {}

  std::array<std::string, Faces::Count> filePaths;
};

class Texture {
public:
  ~Texture();

  unsigned int id() const { return _id; }
  unsigned int target() const { return _target; }

  static TextureRef Create(const TextureCreateParams& params);
  static TextureRef CreateCubemap(const Texture3DCreateParams& params);

private:
  Texture();
  Texture(const Texture& texture) = delete;

  void load2DImage(const ImageData& image, TextureWrapMode wrapMode);
  void load3DImage(const std::vector<ImageData>& images);

private:
  unsigned int _id;
  unsigned int _target;
};
