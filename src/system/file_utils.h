#pragma once

struct ImageData {
  ImageData()
    : width(0)
    , height(0)
    , bytesPerPixel(3) {
  }

  std::vector<unsigned char> data;
  int width;
  int height;
  int bytesPerPixel;
};

class FileUtils {
public:
  static void init(const std::string& assetsFolder);

  static bool readTextFile(const char* filePath, std::vector<char>& data);
  static bool readPngFile(const char* filePath, ImageData& data);
  static std::string getAbsolutePath(const char* filePath);

private:
  static std::string _assetsFolder;
};
