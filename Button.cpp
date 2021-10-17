#include "Button.h"

bool Button::InRect(float x, float y)
{
	D2D1_RECT_F rc = GetRect();
	return (
		x > rc.left  &&
		y > rc.top   &&
		x < rc.right &&
		y < rc.bottom
		);
}

bool Button::InRect(D2D1_POINT_2F xy)
{
	D2D1_RECT_F rc = GetRect();
	return (
		xy.x > rc.left  &&
		xy.y > rc.top   &&
		xy.x < rc.right &&
		xy.y < rc.bottom
		);
}

D2D1_RECT_F Button::GetRect()
{
	return {
		location.x - size.x / 2,
		location.y - size.y / 2,
		location.x + size.x / 2,
		location.y + size.y / 2
	};
}