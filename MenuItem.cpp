#include "MenuItem.h"
#include <sstream>

MenuItem::MenuItem()
	:
	location({ 0,0 }),
	size({ 0,0 }),
	hover(false),
	focused(false)
{
}
MenuItem::~MenuItem()
{
}

D2D1_RECT_F MenuItem::GetRect()
{
	return {
		location.x - size.x / 2,
		location.y - size.y / 2,
		location.x + size.x / 2,
		location.y + size.y / 2
	};
}

D2D1_RECT_F MenuItem::GetTextRect(int char_position)
{
	// Report an error.
	if (char_position >= text.size())
	{
		OutputDebugString(L"ERROR (MenuItem): Desired char postion is greater than text length.\n");
		return { 0,0,0,0 };
	}
	else if (size.y - text_height < 0)
	{
		OutputDebugString(L"ERROR (MenuItem): Text height is greater than menu item size X.\n");
		return { 0,0,0,0 };
	}

	// Return rect based on text values and char position.
	else
	{
		float left = location.x - size.x / 2;
		float top  = location.y - size.y / 2;
		float top_buffer = size.y - text_height;

		return {
			left + text_leftPadding + text_spacing * char_position + text_width * char_position,
			top  + top_buffer / 2.f,
			left + text_leftPadding + text_spacing * char_position + text_width * (char_position + 1),
			top  + top_buffer / 2.f + text_height
		};
		
	}
}

D2D1_RECT_F MenuItem::GetTextUVRect(int char_position, float UV_width, float UV_height)
{
	if (text.at(char_position) >= '0' && text.at(char_position) <= '9')
	{
		int character_place = static_cast<int>(text.at(char_position) - '0');
		return {
			character_place * UV_width,
			0,
			(character_place + 1) * UV_width,
			UV_height
		};
	}
	else if (text.at(char_position) >= 'A' && text.at(char_position) <= 'Z')
	{
		int character_place = static_cast<int>(text.at(char_position) - 'A');
		return {
			character_place* UV_width,
			UV_height,
			(character_place + 1)* UV_width,
			UV_height * 2
		};
	}
	else if (text.at(char_position) >= 'a' && text.at(char_position) <= 'z')
	{
		int character_place = static_cast<int>(text.at(char_position) - 'a');
		return {
			character_place * UV_width,
			UV_height * 2,
			(character_place + 1) * UV_width,
			UV_height * 3
		};
	}
	else if (text.at(char_position) == ' ')
	{
		return { 0,0,0,0 };
	}
	else
	{
		OutputDebugString(L"ERROR (MenuItem): Text char is not on bitmap.\n");
		return { 0,0,0,0 };
	}
}