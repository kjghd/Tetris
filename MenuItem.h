#pragma once
#include <d2d1.h>
#include <string>

class MenuItem
{
public:
	D2D1_POINT_2F location; // Center location from top left of window.
	D2D1_POINT_2F size; // Total width and height.
	bool hover; // Is the player hovering over this item?
	bool focused; // Is currently the point of focus for input.
	std::string text;
	float text_leftPadding;
	float text_width;
	float text_height;
	float text_spacing;
	D2D1_RECT_F GetRect(); // Returns screen rect of this item.
	virtual D2D1_RECT_F GetTextRect(int char_position);
	D2D1_RECT_F GetTextUVRect(int char_position, float UV_width, float UV_height);

	MenuItem();
	virtual ~MenuItem();
};

