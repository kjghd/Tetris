#pragma once
#include "Square.h"
#include "Pattern.h"
#include "Pool.h"
#include "Coord.h"
#include <vector>
#include <d2d1.h>

class Player
{
public:
	std::vector<Square> vSquare;
	const Pattern* m_pPattern;
	Coord location; // Top left corner of current piece's 4x4 grid.
	int rotation;

	float timer_fall; // Time in between falls.
	float timer_fall_update; // Time in ms since last fell.
	float timer_down;
	float timer_down_update;
	float timer_down_initial;
	float timer_down_initial_update;
	float timer_side;
	float timer_side_update;
	float timer_side_initial;
	float timer_side_initial_update;
	float timer_land;
	float timer_land_update;

	Player(const Coord& grid);
	~Player();

	void ResetTimers();
	void UpdateMoveTimers(float deltaTime);
	bool Landed(Pool* pool);
	bool Move(int x_direction, int y_direction, Pool* pool);

	bool Rotate(Pool& pool, bool clockwise);

	void UpdateSquare(); // Updates vSquare based on rotation.

private:
	const Coord& m_grid;
	bool TryRotation(Pool& pool, bool clockwise, int offsetX = 0, int offsetY = 0);
};
