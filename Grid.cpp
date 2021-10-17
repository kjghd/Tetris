#include "Grid.h"

D2D1_RECT_F Grid::GetSquareRect(Coord coord)
{
	//return {
	//	static_cast<float>(coord.x) * square_spacing + (square_spacing - square_size) + location.x, // left
	//	static_cast<float>(coord.y) * square_spacing + (square_spacing - square_size) + location.y, // top
	//	static_cast<float>(coord.x) * square_spacing + square_size + location.x, // right
	//	static_cast<float>(coord.y) * square_spacing + square_size + location.y  // bottom
	//};

	return {
		static_cast<float>(coord.x) * square_spacing + ((square_spacing - square_size) / 2) + location.x, // left
		static_cast<float>(coord.y) * square_spacing + ((square_spacing - square_size) / 2) + location.y, // top
		static_cast<float>(coord.x) * square_spacing + ((square_spacing - square_size) / 2 + square_size) + location.x, // right
		static_cast<float>(coord.y) * square_spacing + ((square_spacing - square_size) / 2 + square_size) + location.y  // bottom
	};
}
