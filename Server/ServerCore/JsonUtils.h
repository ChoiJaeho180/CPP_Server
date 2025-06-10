#pragma once
#include "../Libraries/Include/nlohmann/json.hpp"
#include <string>
#include <vector>


class JsonUtils
{
public:
	static bool LoadJsonFromFile(const std::string& path, nlohmann::json& outJson);
	static Vector<std::string> GetAllJsonFilesInDirectory(const std::string& dirPath);
};

