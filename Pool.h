#pragma once
#include "Square.h"
#include "Grid.h"
#include <vector>

class Pool
{
public:
	std::vector<Square> vSquare;

	Pool();
	void Init(const Coord* grid);

	int DeleteRows();
private:
	const Coord* m_grid;

};

