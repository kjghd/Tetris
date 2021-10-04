#pragma once
#include "Square.h"
#include "Pattern.h"
#include "Coord.h"
#include <array>
#include <vector>
#include <d2d1.h>

class Piece
{
public:
	std::array<Square, 16> m_square;
	Pattern m_pattern;
	Coord origin; // Top left corner of current piece's 4x4 grid.
	int rotation;
	float timeSinceFall; // Time in ms since last fell.
	float timeSinceLanded; // Time in ms since piece landed.
	float timeSinceMovedDown;

	Piece(int origin_x = 3, int origin_y = 0);
	~Piece();

	void SetSquare(const std::array<bool, 16>& pattern);
	D2D1_RECT_F GetSquareRect(int index);
	bool CanMoveLeft(const std::vector<Square>& set_squares);
	bool CanMoveRight(const std::vector<Square>& set_squares, int grid_sz_x);
	bool CanMoveDown(const std::vector<Square>& set_squares, int grid_sz_y);
	bool CanRotate(const std::array<bool, 16>& pattern, const std::vector<Square>& set_squares, int grid_sz_x, int grid_sz_y);
	bool RotateClockwise(const std::vector<Square>& set_squares, int grid_sz_x, int grid_sz_y);
	bool RotateAntiClockwise(const std::vector<Square>& set_squares, int grid_sz_x, int grid_sz_y);
	void SetColour(int colour);
};
