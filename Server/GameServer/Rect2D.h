#pragma once
#include "CommonType.h"

struct Rect2D {
	int x;
	int y;
	int endX;
	int endY;

	Rect2D() = default;

	Rect2D(int x_, int y_, int width, int height)
		: x(x_), y(y_), endX(x_ + width), endY(y_ + height) {
	}

	Rect2D(const J_Rect& temp)
		: x(temp.x), y(temp.y), endX(temp.x + temp.width), endY(temp.y + temp.height) {
	}
	int Width() const { return endX - x; }
	int Height() const { return endY - y; }

	bool Contains(int px, int py) const {
		return px >= x && px < endX && py >= y && py < endY;
	}

	Rect2D ClampTo(const Rect2D& bounds) const {
		Rect2D result;
		result.x = std::clamp(x, bounds.x, bounds.endX);
		result.y = std::clamp(y, bounds.y, bounds.endY);
		result.endX = std::clamp(endX, bounds.x, bounds.endX);
		result.endY = std::clamp(endY, bounds.y, bounds.endY);
		return result;
	}

	Rect2D& operator=(J_Rect& temp) {
		x = temp.x; y = temp.y; endX = temp.x + temp.width; endY = temp.y + temp.height;
		return *this;
	}
};