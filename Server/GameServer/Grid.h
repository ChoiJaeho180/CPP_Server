#pragma once
#include "Vector2.h"
#include "Rect2D.h"

class ZoneInstance;

class Grid
{
public:
	Grid(ZoneInstanceRef owner, const Vector2<int> coord, const Rect2D area);
	virtual ~Grid();

private:
	const Vector2<int>			_coord;
	Rect2D						_area;
private:
	weak_ptr<ZoneInstance>		_owner;
};

