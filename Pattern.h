#pragma once
#include <array>

class Pattern
{
public:
	std::array<bool, 16> A;
	std::array<bool, 16> B;
	std::array<bool, 16> C;
	std::array<bool, 16> D;
	int colour;



	Pattern();
	~Pattern();
};
