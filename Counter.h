#pragma once
#include "Coord.h"
#include <d2d1.h>

class Counter
{
public:
	int value;
	float digit_width;
	float digit_height;
	float digit_spacing;
	D2D1_POINT_2F location;
	int GetLength();
	int GetDigit(int digit);
	D2D1_RECT_F GetRect(int character);
	D2D1_RECT_F GetSpriteRect(int character);
};

