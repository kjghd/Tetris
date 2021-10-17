#pragma once
#include "Pattern.h"
#include "Coord.h"
#include <vector>
#include <d2d1.h>

class Piece
{
public:
	D2D1_POINT_2F location;
	bool active;
	float size;
	const Pattern* pattern;
	D2D1_RECT_F GetRect(int index);
};
