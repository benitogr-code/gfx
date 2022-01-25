#include "file_utils.h"

#include <filesystem>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::string FileUtils::_assetsFolder;

void FileUtils::init(const std::string& assetsFolder) {
  if (!assetsFolder.empty()) {
    _assetsFolder = assetsFolder;
  }
  else {
    auto path = std::filesystem::current_path();
    _assetsFolder = path.generic_string().append("/assets/");
  }

  LOG_INFO("[FileUtils] Assets folder path: {}", _assetsFolder);
}

bool FileUtils::readTextFile(const char* filePath, std::vector<char>& data) {
  const auto absolutePath = getAbsolutePath(filePath);
  std::ifstream inStream(absolutePath);

  if (!inStream) {
    LOG_WARN("[FileUtils] Failed to open text file {0}, {1}", absolutePath, strerror(errno));
    return false;
  }

  inStream.seekg(0, inStream.end);
  const size_t fileLength = inStream.tellg();
  inStream.seekg(0, inStream.beg);

  data.resize(fileLength+1);

  inStream.read(data.data(), fileLength);
  data[fileLength] = '\0';

  inStream.close();

  return true;
}

bool FileUtils::readImageFile(const char* filePath, ImageData& data) {
  const auto absolutePath = getAbsolutePath(filePath);

  const int kRequiredComponents = 4;
  int width, height, components;

  const bool flip = std::filesystem::path(filePath).extension().generic_string().compare(".png") == 0;
  stbi_set_flip_vertically_on_load(flip);
  unsigned char* pData = stbi_load(absolutePath.c_str(), &width, &height, &components, kRequiredComponents);

  if (pData == nullptr) {
    LOG_WARN("[FileUtils] Failed to load image file {0}, {1}", absolutePath, strerror(errno));
    return false;
  }

  const size_t imgSize = kRequiredComponents * width * height;
  data.data.resize(imgSize);
  data.data.assign(pData, pData + (imgSize - 1));
  data.width = width;
  data.height = height;
  data.bytesPerPixel = kRequiredComponents;

  STBI_FREE(pData);

  return true;
}

bool FileUtils::readJsonFile(const char* filePath, Json::Value& root) {
  const auto absolutePath = getAbsolutePath(filePath);
  std::ifstream inStream(absolutePath);

  if (!inStream) {
    LOG_WARN("[FileUtils] Failed to open json file {0}, {1}", absolutePath, strerror(errno));
    return false;
  }

  Json::CharReaderBuilder builder;
  std::string errors;

  if (!Json::parseFromStream(builder, inStream, &root, &errors)) {
    LOG_WARN("[FileUtils] Failed to parse json file {0}, {1}", absolutePath, errors);
    return false;
  }

  return true;
}

std::string FileUtils::removeExtension(const std::string& filename) {
  char sep = '.';

  size_t pos = filename.find(sep, 0);
  if (pos == std::string::npos)
    return "";

  return(filename.substr(0, pos));
}

std::string FileUtils::getAbsolutePath(const char* filePath) {
  return _assetsFolder + filePath;
}
