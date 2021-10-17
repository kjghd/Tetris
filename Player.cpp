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
void Player::UpdateTimers(float deltaTime)
{
	timer_down_update		  += deltaTime;
	timer_down_initial_update += deltaTime;
	timer_fall_update		  += deltaTime;
	timer_side_update		  += deltaTime;
	timer_side_initial_update += deltaTime;
}

bool Player::Move(int x_direction, int y_direction, Pool* pool)
{
	// cannot move if destination is on top of a pool square or beyond grid
	for (const auto& sqr : vSquare)
	{
		if (location.x + sqr.x + x_direction >= m_grid.x ||
			location.y + sqr.y + y_direction >= m_grid.y ||
			location.x + sqr.x + x_direction < 0 ||
			location.y + sqr.y + y_direction < 0)
			return false;

		for (const auto& sqr_pool : pool->vSquare)
		{
			if (location.x + sqr.x + x_direction == sqr_pool.x && location.y + sqr.y + y_direction == sqr_pool.y)
				return false;
		}
	}
	location.x += x_direction;
	location.y += y_direction;
	return true;
}

bool Player::Rotate(Pool& pool, bool clockwise)
{

	// IMPLIMENT: Wall kick


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
		return false;

	UpdateSquare();


	int trying(0);

	// if new rotation is overlapping something, rotate it back and return false
	for (const auto& sqr : vSquare)
	{
		if (location.x + sqr.x >= m_grid.x ||
			location.y + sqr.y >= m_grid.y ||
			location.x + sqr.x < 0		   ||
			location.y + sqr.y < 0		   ||
			std::find(pool.vSquare.begin(), pool.vSquare.end(), Square({ sqr.x + location.x, sqr.y + location.y })) != pool.vSquare.end())
		{
			rotation = rotation_old;
			UpdateSquare();
			return false;
		}
	}
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