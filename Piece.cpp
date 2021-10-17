#include "Piece.h"

D2D1_RECT_F Piece::GetRect(int index)
{
	if (pattern->A.at(index))
	{
		// x and y position of pattern square at index
		float x = index - (index / 4 * 4);
		float y = index / 4;

		// get starting index
		int x_start(3);
		int y_start(3);

		// get total x and y length
		std::array<bool, 4> arr_x{ false,false,false,false };
		std::array<bool, 4> arr_y{ false,false,false,false };
		for (size_t i = 0; i < pattern->A.size(); i++)
		{
			if (pattern->A.at(i))
			{
				arr_x.at(i - (i / 4 * 4)) = true;
				arr_y.at(i / 4) = true;

				if (i - (i / 4 * 4) < x_start)
					x_start = i - (i / 4 * 4);
				if (i / 4 < y_start)
					y_start = i / 4;
			}
		}


		int x_max(0);
		int y_max(0);
		for (const auto& b : arr_x)
			if (b) ++x_max;
		for (const auto& b : arr_y)
			if (b) ++y_max;

		// work out the size of each square
		float scale = size / 4.f;

		return {
			// location				  // padding					 // square location
			(location.x - size / 2) + ((size - x_max * scale) / 2) + ((x - x_start) * scale),
			(location.y - size / 2) + ((size - y_max * scale) / 2) + ((y - y_start) * scale),
			(location.x - size / 2) + ((size - x_max * scale) / 2) + ((x - x_start + 1) * scale),
			(location.y - size / 2) + ((size - y_max * scale) / 2) + ((y - y_start + 1) * scale)
		};
	}
	else
		return { 0,0,0,0 };
	/*
	if (pattern->A.at(index))
	{
		// x and y position of pattern square at index
		float x = index - (index / 4 * 4);
		float y = index / 4;

		// get starting index
		int x_start(3);
		int y_start(3);


		// get total x and y length
		std::array<bool, 4> arr_x{ false,false,false,false };
		std::array<bool, 4> arr_y{ false,false,false,false };
		for (size_t i = 0; i < pattern->A.size(); i++)
		{
			if (pattern->A.at(i))
			{
				arr_x.at(i - (i / 4 * 4)) = true;
				arr_y.at(i / 4) = true;

				if (i - (i / 4 * 4) < x_start)
					x_start = i - (i / 4 * 4);
				if (i / 4 < y_start)
					y_start = i / 4;
			}
		}
			

		int x_max(0);
		int y_max(0);
		for (const auto& b : arr_x)
			if (b) ++x_max;
		for (const auto& b : arr_y)
			if (b) ++y_max;

		// work out the size of each square
		float scale = x_max > y_max ?
			size / x_max :
			size / y_max;

		return {
			// location				  // padding					 // square location
			(location.x - size / 2) + ((size - x_max * scale) / 2) + ((x - x_start)		* scale),
			(location.y - size / 2) + ((size - y_max * scale) / 2) + ((y - y_start)		* scale),
			(location.x - size / 2) + ((size - x_max * scale) / 2) + ((x - x_start + 1) * scale),
			(location.y - size / 2) + ((size - y_max * scale) / 2) + ((y - y_start + 1) * scale)
		};
	}
	else
		return { 0,0,0,0 };
	*/
}