#pragma once
#include <d2d1.h>

class Button
{
public:
	D2D1_POINT_2F location;
	D2D1_POINT_2F size;
	bool selected;
	bool InRect(float x, float y);
	bool InRect(D2D1_POINT_2F xy);
	D2D1_RECT_F GetRect();

};
