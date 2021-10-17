#include "Pool.h"
#include <algorithm>

Pool::Pool()
{
}

void Pool::Init(const Coord* grid)
{
	m_grid = grid;
}

bool sortSquareY(Square i, Square j)
{
	return i.y < j.y;
}
bool sortMatch(int i, int j)
{
	return i > j;
}


int Pool::DeleteRows()
{
	// find squares to delete
	std::vector<int> vSquaresToDelete;
	for (int y = 0; y < m_grid->y; y++)
	{
		for (int x = 0; x < m_grid->x; x++)
		{
			Square target{ x,y };
			auto it = std::find(vSquare.begin(), vSquare.end(), target);
			if (it != vSquare.end())
			{
				vSquaresToDelete.push_back(std::distance(vSquare.begin(), it));
			}
			else
			{
				vSquaresToDelete.erase(vSquaresToDelete.end() - x, vSquaresToDelete.end());
				break;
			}

		}
	}

	// get return value
	int deletedRows = vSquaresToDelete.size() / m_grid->x;

	// delete rows
	std::sort(vSquaresToDelete.begin(), vSquaresToDelete.end(), sortMatch);
	for (const auto& index : vSquaresToDelete)
	{
		vSquare.erase(vSquare.begin() + index);
	}

	// move rows down
	std::sort(vSquare.begin(), vSquare.end(), sortSquareY);
	for (int y = 0; y < m_grid->y; ++y)
	{
		bool emptyRow(true);
		for (const auto& sqr : vSquare)
		{
			if (sqr.y == y)
			{
				emptyRow = false;
				break;
			}
		}
		if (emptyRow)
		{
			for (auto& sqr : vSquare)
				if (sqr.y < y)
					++sqr.y;
		}
			
	}
	
	return deletedRows;
}