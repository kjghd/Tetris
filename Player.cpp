#include "Player.h"
#include <cmath>
#include <algorithm>


Player::Player(const Coord& grid)
	:
	m_grid(grid),
	m_pPattern(0),
	// transforms
	location({3, -2}),
	rotation(0),
	// timers
	timer_fall(0),
	timer_fall_update(0),
	timer_down(0),
	timer_down_update(0),
	timer_down_initial(0),
	timer_down_initial_update(0),
	timer_side(0),
	timer_side_update(0),
	timer_side_initial(0),
	timer_side_initial_update(0)
{
}
Player::~Player()
{
}

void Player::ResetTimers()
{
	timer_down_update		  = 0;
	timer_down_initial_update = 0;
	timer_fall_update		  = 0;
	timer_side_update		  = 0;
	timer_side_initial_update = 0;
}
void Player::UpdateMoveTimers(float deltaTime)
{
	timer_down_update		  += deltaTime;
	timer_down_initial_update += deltaTime;
	timer_fall_update		  += deltaTime;
	timer_side_update		  += deltaTime;
	timer_side_initial_update += deltaTime;
}

bool Player::Landed(Pool* pool)
{
	for (const auto& sqr : vSquare)
	{
		Square target{ location.x + sqr.x, location.y + sqr.y + 1 };

		// check bounds
		if (target.y >= m_grid.y)
			return true;

		// check pool
		if (std::find(pool->vSquare.begin(), pool->vSquare.end(), target) != pool->vSquare.end())
			return true;
	}
	return false;
}

bool Player::Move(int x_direction, int y_direction, Pool* pool)
{
	// cannot move if destination is on top of a pool square or beyond grid
	for (const auto& sqr : vSquare)
	{
		Square target{ location.x + sqr.x + x_direction, location.y + sqr.y + y_direction };

		// check bounds
		if (target.x >= m_grid.x ||
			target.y >= m_grid.y ||
			target.x < 0 ||
			target.y < 0)
			return false;

		// check pool
		if (std::find(pool->vSquare.begin(), pool->vSquare.end(), target) != pool->vSquare.end())
			return true;
	}
	location.x += x_direction;
	location.y += y_direction;
	return true;
}

bool Player::Rotate(Pool& pool, bool clockwise)
{
	Pattern pat_i;
	pat_i.A = {
			0,0,0,0,
			1,1,1,1,
			0,0,0,0,
			0,0,0,0
	};
	Pattern pat_o;
	pat_o.A = {
			0,0,0,0,
			0,1,1,0,
			0,1,1,0,
			0,0,0,0
	};

	// handle based on pattern
	// pattern i
	if (std::equal(m_pPattern->A.begin(), m_pPattern->A.end(),pat_i.A.begin(), pat_i.A.end()))
	{
		if (clockwise)
		{
			switch (rotation)
			{
			case 0:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, -2,  0)) return true;
				else if (TryRotation(pool, clockwise,  1,  0)) return true;
				else if (TryRotation(pool, clockwise, -2,  1)) return true;
				else if (TryRotation(pool, clockwise,  1, -2)) return true;
				else return false;
			}
			case 1:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, -1,  0)) return true;
				else if (TryRotation(pool, clockwise,  2,  0)) return true;
				else if (TryRotation(pool, clockwise, -1, -2)) return true;
				else if (TryRotation(pool, clockwise,  2,  1)) return true;
				else return false;
			}
			case 2:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise,  2,  0)) return true;
				else if (TryRotation(pool, clockwise, -1,  0)) return true;
				else if (TryRotation(pool, clockwise,  2, -1)) return true;
				else if (TryRotation(pool, clockwise, -1,  2)) return true;
				else return false;
			}
			case 3:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise,  1, 0)) return true;
				else if (TryRotation(pool, clockwise, -2, 0)) return true;
				else if (TryRotation(pool, clockwise,  1, 2)) return true;
				else if (TryRotation(pool, clockwise, -2, 1)) return true;
				else return false;
			}
			}
		}
		else
		{
			switch (rotation)
			{
			case 1:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise,  2,  0)) return true;
				else if (TryRotation(pool, clockwise, -1,  0)) return true;
				else if (TryRotation(pool, clockwise,  2, -1)) return true;
				else if (TryRotation(pool, clockwise, -1,  2)) return true;
				else return false;
			}
			case 2:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise,  1,  0)) return true;
				else if (TryRotation(pool, clockwise, -2,  0)) return true;
				else if (TryRotation(pool, clockwise,  1,  2)) return true;
				else if (TryRotation(pool, clockwise, -2, -1)) return true;
				else return false;
			}
			case 3:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, -2,  0)) return true;
				else if (TryRotation(pool, clockwise,  1,  0)) return true;
				else if (TryRotation(pool, clockwise, -2, -1)) return true;
				else if (TryRotation(pool, clockwise,  1, -2)) return true;
				else return false;
			}
			case 0:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, -1, 0)) return true;
				else if (TryRotation(pool, clockwise,  2, 0)) return true;
				else if (TryRotation(pool, clockwise, -1, 2)) return true;
				else if (TryRotation(pool, clockwise,  2, 1)) return true;
				else return false;
			}
			}
		}
	}
	// pattern o
	else if (std::equal(m_pPattern->A.begin(), m_pPattern->A.end(), pat_o.A.begin(), pat_o.A.end()))
	{
		return false;
	}
	// every other pattern
	else
	{
		if (clockwise)
		{
			switch (rotation)
			{
			case 0:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, -1, 0)) return true;
				else if (TryRotation(pool, clockwise, -1, -1)) return true;
				else if (TryRotation(pool, clockwise, 0, 2)) return true;
				else if (TryRotation(pool, clockwise, -1, 2)) return true;
				else return false;
			}
			case 1:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, 1, 0)) return true;
				else if (TryRotation(pool, clockwise, 1, 1)) return true;
				else if (TryRotation(pool, clockwise, 0, -2)) return true;
				else if (TryRotation(pool, clockwise, 1, -2)) return true;
				else return false;
			}
			case 2:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, 1, 0)) return true;
				else if (TryRotation(pool, clockwise, 1, -1)) return true;
				else if (TryRotation(pool, clockwise, 0, 2)) return true;
				else if (TryRotation(pool, clockwise, 1, 2)) return true;
				else return false;
			}
			case 3:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, -1, 0)) return true;
				else if (TryRotation(pool, clockwise, -1, 1)) return true;
				else if (TryRotation(pool, clockwise, 0, -2)) return true;
				else if (TryRotation(pool, clockwise, -1, -2)) return true;
				else return false;
			}
			}
		}
		else
		{
			switch (rotation)
			{
			case 1:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, 1, 0)) return true;
				else if (TryRotation(pool, clockwise, 1, 1)) return true;
				else if (TryRotation(pool, clockwise, 0, -2)) return true;
				else if (TryRotation(pool, clockwise, 1, -2)) return true;
				else return false;
			}
			case 2:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, -1, 0)) return true;
				else if (TryRotation(pool, clockwise, -1, -1)) return true;
				else if (TryRotation(pool, clockwise, 0, 2)) return true;
				else if (TryRotation(pool, clockwise, -1, 2)) return true;
				else return false;
			}
			case 3:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, -1, 0)) return true;
				else if (TryRotation(pool, clockwise, -1, 1)) return true;
				else if (TryRotation(pool, clockwise, 0, -2)) return true;
				else if (TryRotation(pool, clockwise, -1, -2)) return true;
				else return false;
			}
			case 0:
			{
				if (TryRotation(pool, clockwise)) return true;
				else if (TryRotation(pool, clockwise, 1, 0)) return true;
				else if (TryRotation(pool, clockwise, 1, -1)) return true;
				else if (TryRotation(pool, clockwise, 0, 2)) return true;
				else if (TryRotation(pool, clockwise, 1, 2)) return true;
				else return false;
			}
			}
		}
	}

}

bool Player::TryRotation(Pool& pool, bool clockwise, int offsetX, int offsetY)
{
	int rotation_old = rotation;

	// rotate the squares clockwise
	if (rotation_old < 3 && clockwise)
		++rotation;
	else if (clockwise)
		rotation = 0;

	// rotate the squares anti-clockwise
	else if (rotation_old > 0 && !clockwise)
		--rotation;
	else if (!clockwise)
		rotation = 3;

	// ERROR: No rotation direction specified.
	else
	{
		OutputDebugString(L"ERROR (Player): No rotation direction specified.\n");
		return false;
	}

	UpdateSquare();


	int trying(0);

	// if new rotation is overlapping something, rotate it back and return false
	for (const auto& sqr : vSquare)
	{
		if (location.x + sqr.x + offsetX >= m_grid.x ||
			location.y + sqr.y + offsetY >= m_grid.y ||
			location.x + sqr.x + offsetX < 0 ||
			location.y + sqr.y + offsetY < 0 ||
			std::find(pool.vSquare.begin(), pool.vSquare.end(), Square({ sqr.x + location.x + offsetX, sqr.y + location.y + offsetY })) != pool.vSquare.end())
		{
			rotation = rotation_old;
			UpdateSquare();
			return false;
		}
	}

	location.x += offsetX;
	location.y += offsetY;

	return true;
}

void Player::UpdateSquare()
{
	vSquare.clear();

	int i(0);

	switch (rotation)
	{
	case 0:
		for (const auto& x : m_pPattern->A)
		{
			if (x) vSquare.push_back({ i - (i / 4 * 4), i / 4, m_pPattern->colour });
			++i;
		}
		break;

	case 1:
		for (const auto& x : m_pPattern->B)
		{
			if (x) vSquare.push_back({ i - (i / 4 * 4), i / 4, m_pPattern->colour });
			++i;
		}
		break;

	case 2:
		for (const auto& x : m_pPattern->C)
		{
			if (x) vSquare.push_back({ i - (i / 4 * 4), i / 4, m_pPattern->colour });
			++i;
		}
		break;

	case 3:
		for (const auto& x : m_pPattern->D)
		{
			if (x) vSquare.push_back({ i - (i / 4 * 4), i / 4, m_pPattern->colour });
			++i;
		}
		break;
	}
}