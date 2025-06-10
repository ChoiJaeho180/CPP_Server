#include "pch.h"
#include "JsonUtils.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

Vector<std::string> JsonUtils::GetAllJsonFilesInDirectory(const std::string& dirPath)
{
	namespace fs = std::filesystem;
	Vector<std::string> files;

	for (const auto& entry : fs::directory_iterator(dirPath)) {
		if (entry.path().extension() == ".json")
			files.push_back(entry.path().string());
	}
	return files;
}

bool JsonUtils::LoadJsonFromFile(const std::string& path, nlohmann::json& outJson)
{
    std::ifstream in(path);
    if (!in.is_open())
        return false;

    try {
        in >> outJson;
        return true;
	}
	catch (const std::exception& e) {
		std::cerr << "[json] " << e.what() << std::endl;
		return false;
	}
   
}