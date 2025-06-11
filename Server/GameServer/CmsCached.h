#pragma once
#include "../ServerCore/JsonUtils.h"

template<typename T>
class CmsCached
{
public:
    static void Load(const std::string& file)
    {
        using json = nlohmann::json;
       
        json j;
        if (!JsonUtils::LoadJsonFromFile(file, j)) {
            return;
        }

        const string rootKey = j.begin().key();
        if (!j.contains(rootKey) || !j[rootKey].is_array()) {
            return;
        }

        for (const auto& elem : j[rootKey])
        {
            try {
                T obj = elem.get<T>();
                _cache[obj.cmsId] = std::move(obj);
            }
            catch (...) {
                // todo ·Î±× 
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


