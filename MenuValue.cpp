#include "MenuValue.h"
#include <climits>

MenuValue::MenuValue()
	:
	MenuItem(),
	value(0),
	value_min(1),
	value_max(30)
{
}
MenuValue::~MenuValue()
{
}

void MenuValue::SetValue(int key)
{
	// remove old value from text
	for (const auto& chr : sValue)
		text.pop_back();

	// alter value string
	if (key < 10 && sValue.size() < 9) // 0 - 9
	{
		sValue.push_back(key + 48); // 48 is '0' in unicode
	}
	else if (key == 42 && !sValue.empty()) // bakcspace
		sValue.pop_back();

	// update value int
	if (!sValue.empty())
	{
		value = std::stoi(sValue);
	}
	else
	{
		value = value_min;
	}

	// add new value to text
	for (const auto& chr : sValue)
		text.push_back(chr);
}

void MenuValue::CapValue()
{
	// remove old value from text
	for (const auto& chr : sValue)
		text.pop_back();

	// cap value
	if (value < value_min)
	{
		value = value_min;
		sValue = std::to_string(value_min);
	}
	else if (value > value_max)
	{
		value = value_max;
		sValue = std::to_string(value_max);
	}

	// add new value to text
	for (const auto& chr : sValue)
		text.push_back(chr);
}

D2D1_RECT_F MenuValue::GetTextRect(int char_position)
{
	// Report an error.
	if (char_position >= text.size())
	{
		OutputDebugString(L"ERROR (MenuValue): Desired char postion is greater than text length and value.\n");
		return { 0,0,0,0 };
	}
	else if (size.y - text_height < 0)
	{
		OutputDebugString(L"ERROR (MenuValue): Text height is greater than menu item size X.\n");
		return { 0,0,0,0 };
	}

	// Return rect based on text values and char position.
	else
	{
		float left = location.x - size.x / 2;
		float top = location.y - size.y / 2;
		float top_buffer = size.y - text_height;

		return {
			left + text_leftPadding + text_spacing * char_position + text_width * char_position,
			top + top_buffer / 2.f,
			left + text_leftPadding + text_spacing * char_position + text_width * (char_position + 1),
			top + top_buffer / 2.f + text_height
		};

	}
}