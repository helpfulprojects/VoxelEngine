#pragma once
#include <string>
namespace VoxelEngine {
class Utils {
public:
  static std::string ExtractNameFromFilePath(const std::string filepath) {
    auto lastSlash = filepath.find_last_of("/\\");
    lastSlash = (lastSlash == std::string::npos ? 0 : lastSlash + 1);
    auto lastDot = filepath.rfind('.');

    auto count = lastDot == std::string::npos ? filepath.size() - lastSlash
                                              : lastDot - lastSlash;
    return filepath.substr(lastSlash, count);
  }
  static std::string ReadFile(const std::string &filepath) {
    VE_PROFILE_FUNCTION;
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in) {
      in.seekg(0, std::ios::end);
      result.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(&result[0], result.size());
      in.close();
    } else {
      VE_CORE_ERROR("Could not open file {0}", filepath);
    }
    return result;
  }
};
} // namespace VoxelEngine
