#pragma once
#include "../Libraries/Include/nlohmann/json.hpp"

struct J_Rect {
    int x;
    int y;
    int width;
    int height;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(J_Rect, x, y, width, height);

struct J_Vector2 {
    int x;
    int y;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(J_Vector2, x, y);
