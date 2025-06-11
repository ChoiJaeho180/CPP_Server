#pragma once
#include "../Libraries/Include/nlohmann/json.hpp"

struct Position {
    int x;
    int y;
    int z;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Position, x, y, z);
