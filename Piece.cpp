#include "Piece.h"


Piece::Piece(int origin_x, int origin_y)
	:
	origin({ origin_x, origin_y }),
	rotation(0),
	timeSinceFall(0),
	timeSinceLanded(0),
	timeSinceMovedDown(0)
{
	for (size_t i = 0; i < m_square.size(); ++i)
	{
		m_square[i].x = i - (floor(i / 4) * 4);
		m_square[i].y = floor(i / 4);
	}
}
Piece::~Piece()
{
}

void Piece::SetSquare(const std::array<bool, 16>& pattern)
{
	for (size_t i = 0; i < pattern.size(); ++i)
		m_square[i].show = pattern[i];
}
D2D1_RECT_F Piece::GetSquareRect(int index)
{
	return {
		static_cast<float>(origin.x + m_square[index].x) * 20.f + 10.f,
		static_cast<float>(origin.y + m_square[index].y) * 20.f + 50.f,
		static_cast<float>(origin.x + m_square[index].x + 1) * 20.f + 9.f,
		static_cast<float>(origin.y + m_square[index].y + 1) * 20.f + 49.f
	};
}
bool Piece::CanMoveLeft(const std::vector<Square>& set_squares)
{
	// check hit left wall
	for (const auto& value : m_square)
	{
		if (value.show && origin.x + value.x <= 0)
			return false;
	}
	// check hit stuck squares
	for (size_t i = 0; i < m_square.size(); ++i)
	{
		if (m_square[i].show)
		{
			for (size_t ii = 0; ii < set_squares.size(); ++ii)
			{
				if (origin.y + m_square[i].y == set_squares[ii].y && origin.x + m_square[i].x - 1 == set_squares[ii].x)
				{
					return false;
				}
			}
		}
	}
	return true;
}
bool Piece::CanMoveRight(const std::vector<Square>& set_squares, int grid_sz_x)
{
	// check hit right wall
	for (const auto& value : m_square)
	{
		if (value.show && origin.x + value.x >= grid_sz_x)
			return false;
	}
	// check hit stuck squares
	for (size_t i = 0; i < m_square.size(); ++i)
	{
		if (m_square[i].show)
		{
			for (size_t ii = 0; ii < set_squares.size(); ++ii)
			{
				if (origin.y + m_square[i].y == set_squares[ii].y && origin.x + m_square[i].x + 1 == set_squares[ii].x)
				{
					return false;
				}
			}
		}
	}
	return true;
}
bool Piece::CanMoveDown(const std::vector<Square>& set_squares, int grid_sz_y)
{
	// check hit bottom
	for (const auto& value : m_square)
	{
		if (value.show && origin.y + value.y >= grid_sz_y)
			return false;
	}
	// check hit stuck squares
	for (size_t i = 0; i < m_square.size(); ++i)
	{
		if (m_square[i].show)
		{
			for (size_t ii = 0; ii < set_squares.size(); ++ii)
			{
				if (origin.y + m_square[i].y + 1 == set_squares[ii].y && origin.x + m_square[i].x == set_squares[ii].x)
				{
					return false;
				}
			}
		}
	}
	return true;
}
bool Piece::CanRotate(const std::array<bool, 16>& pattern, const std::vector<Square>& set_squares, int grid_sz_x, int grid_sz_y)
{
	for (size_t i = 0; i < pattern.size(); i++)
	{
		// check if target pattern will be past right side
		if (pattern[i] && origin.x + m_square[i].x > grid_sz_x ||
			pattern[i] && origin.y + m_square[i].y > grid_sz_y)
		{
			return false;
		}
		for (size_t ii = 0; ii < set_squares.size(); ii++)
		{
			if (pattern[i] &&
				origin.x + m_square[i].x == set_squares[ii].x &&
				origin.y + m_square[i].y == set_squares[ii].y)
			{
				return false;
			}
		}

	}
	return true;
}
bool Piece::RotateClockwise(const std::vector<Square>& set_squares, int grid_sz_x, int grid_sz_y)
{
	switch (rotation)
	{
	case 0: // if up set right
		if (CanRotate(m_pattern.right, set_squares, grid_sz_x, grid_sz_y))
		{
			rotation = 1;
			SetSquare(m_pattern.right);
		}
		break;
	case 1: // if right set down
		if (CanRotate(m_pattern.down, set_squares, grid_sz_x, grid_sz_y))
		{
			rotation = 2;
			SetSquare(m_pattern.down);
		}
		break;
	case 2: // if down set left
		if (CanRotate(m_pattern.left, set_squares, grid_sz_x, grid_sz_y))
		{
			rotation = 3;
			SetSquare(m_pattern.left);
		}
		break;
	case 3: // if left set up
		if (CanRotate(m_pattern.up, set_squares, grid_sz_x, grid_sz_y))
		{
			rotation = 0;
			SetSquare(m_pattern.up);
		}
		break;
	}
	return true;
}
bool Piece::RotateAntiClockwise(const std::vector<Square>& set_squares, int grid_sz_x, int grid_sz_y)
{
	switch (rotation)
	{
	case 0: // if up set left
		if (CanRotate(m_pattern.left, set_squares, grid_sz_x, grid_sz_y))
		{
			rotation = 3;
			SetSquare(m_pattern.left);
		}
		break;
	case 3: // if left set down
		if (CanRotate(m_pattern.down, set_squares, grid_sz_x, grid_sz_y))
		{
			rotation = 2;
			SetSquare(m_pattern.down);
		}
		break;
	case 2: // if down set right
		if (CanRotate(m_pattern.right, set_squares, grid_sz_x, grid_sz_y))
		{
			rotation = 1;
			SetSquare(m_pattern.right);
		}
		break;
	case 1: // if right set up
		if (CanRotate(m_pattern.up, set_squares, grid_sz_x, grid_sz_y))
		{
			rotation = 0;
			SetSquare(m_pattern.up);
		}
		break;
	}
	return true;
}
void Piece::SetColour(int colour)
{
	for (auto& sqr : m_square)
		sqr.colour = colour;
}