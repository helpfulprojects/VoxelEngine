#pragma once
#include <string>
namespace VoxelEngine {
	class Utils {
	public:
		static std::string ExtractNameFromFilePath(const std::string filepath) {
			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = (lastSlash == std::string::npos ? 0 : lastSlash + 1);
			auto lastDot = filepath.rfind('.');

			auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
			return filepath.substr(lastSlash, count);
		}
	};
}
