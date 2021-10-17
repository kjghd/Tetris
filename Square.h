#pragma once
#include <array>

struct Square
{
	int x; // X grid location.
	int y; // X grid location.
	int colour = 0;
	bool operator == (Square s) { return s.x == x && s.y == y; };
};

