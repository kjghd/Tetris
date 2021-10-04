#pragma once
#include <array>

class Pattern
{
public:
	std::array<bool, 16> up;
	std::array<bool, 16> right;
	std::array<bool, 16> down;
	std::array<bool, 16> left;
	Pattern();
	~Pattern();
};
