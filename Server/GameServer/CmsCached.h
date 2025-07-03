#pragma once
#include "../ServerCore/JsonUtils.h"

template<typename T>
class CmsCached
{
public:
    static void Load(const std::string& path, const std::string& fileName)
    {
        using json = nlohmann::json;
        
        const std::string fullPath = path + fileName + ".json";
        json j;
        if (!JsonUtils::LoadJsonFromFile(fullPath, j)) {
            return;
        }

        if (!j.contains(fileName) || !j[fileName].is_array()) {
            return;
        }

        for (const auto& elem : j[fileName])
        {
            try {
                T obj = elem.get<T>();
                _cache[obj.cmsId] = std::move(obj);
            }
            catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to parse element in " << fullPath
                    << ": " << e.what() << std::endl;
            }
        }
    }

    static const T* Get(int cmsId)
    {
        auto it = _cache.find(cmsId);
        return (it != _cache.end()) ? &it->second : nullptr;
    }

    static const std::unordered_map<int, T>& GetAll() { return _cache; }

private:
    static inline std::unordered_map<int, T> _cache;
};


