#include "pch.h"
#include "Grid.h"
#include "GameConst.h"
#include "Rect2D.h"

Grid::Grid(ZoneInstanceRef owner, const Vector2<int> coord, const Rect2D area)
	: _owner(owner), _coord(coord), _area(area)
{
	
}

Grid::~Grid()
{
}
