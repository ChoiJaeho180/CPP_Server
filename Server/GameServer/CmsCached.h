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
                _cache[obj.id] = std::move(obj);
            }
            catch (...) {
                // todo ·Î±× 
            }
        }
    }

    static const T* Get(int id)
    {
        auto it = _cache.find(id);
        return (it != _cache.end()) ? &it->second : nullptr;
    }

    template<typename Func>
    static void ForEach(Func&& func)
    {
        for (const auto& [id, obj] : _cache)
        {
            func(id, obj);
        }
    }

private:
    static inline std::unordered_map<int, T> _cache;
};


