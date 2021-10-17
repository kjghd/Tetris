#pragma once
#include "Coord.h"
#include <d2d1.h>

class Grid
{
public:
	Coord size;
	D2D1_POINT_2F location;
	float square_spacing;
	float square_size;
	D2D1_RECT_F GetSquareRect(Coord coord);
};

