#pragma once
#include <array>

struct Square
{
	int x;
	int y;
	bool show;
	int colour;
	bool operator == (Square s) { return s.x == x && s.y == y && s.show == show; };
};

