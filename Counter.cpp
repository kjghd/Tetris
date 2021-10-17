#include "Counter.h"
#include <sstream>

int Counter::GetLength()
{
	int val(value);
	int length(1);

	while (val /= 10)
		++length;

	return length;
}

int Counter::GetDigit(int digit)
{
	return std::to_string(value).at(digit) - 48;
}

D2D1_RECT_F Counter::GetRect(int digit)
{
	float offset = GetLength() * digit_spacing / 2;
	return {
		location.x + (digit_spacing * digit) + ((digit_spacing - digit_width) / 2) - offset,
		location.y - digit_height / 2,
		location.x + (digit_spacing * digit) + ((digit_spacing - digit_width) / 2 + digit_width) - offset,
		location.y + digit_height / 2
	};

}

D2D1_RECT_F Counter::GetSpriteRect(int digit)
{
	return {
		digit_width * digit,
		0,
		digit_width * (digit + 1),
		digit_height
	};


	//return {
	//	18.f * digit,
	//	0,
	//	18.f * (digit + 1),
	//	34.f
	//};
}