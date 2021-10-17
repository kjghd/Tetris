#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include "Player.h"
#include "Pattern.h"
#include "Grid.h"
#include "Counter.h"
#include "Piece.h"
#include "Button.h"
#include "Menu.h"

#include <memory> // unique_ptr
#include <stdlib.h> // srand(), rand()
#include <sstream>

/**************** Globals ****************/
// graphics
auto graphics(std::make_unique<Graphics>());

// textures
static CComPtr<ID2D1Bitmap> pBackground;
static CComPtr<ID2D1Bitmap> pGrid;
static CComPtr<ID2D1Bitmap> pPiece;
static CComPtr<ID2D1Bitmap> pButton;
static CComPtr<ID2D1Bitmap> pButton_on;
static CComPtr<ID2D1Bitmap> pNumber;

// patterns
auto pattern_I(std::make_unique<Pattern>());
auto pattern_J(std::make_unique<Pattern>());
auto pattern_L(std::make_unique<Pattern>());
auto pattern_O(std::make_unique<Pattern>());
auto pattern_S(std::make_unique<Pattern>());
auto pattern_T(std::make_unique<Pattern>());
auto pattern_Z(std::make_unique<Pattern>());

// Game Variables
float deltaTime; // time between cpu cycles in ms
enum GameStates
{
	GAME_MAINMENU,
	GAME_LEVELS,
	GAME_PAUSE,
	GAME_PLAY,
	GAME_LOSE
};
int GameState;
enum PlayStates
{
	PLAY_INIT,
	PLAY_SWAP,
	PLAY_SERVING,
	PLAY_MOVING,
	PLAY_CHECKING
};
int PlayState;
int goalPoints;
float speed;
D2D1_RECT_F area_held;
D2D1_RECT_F area_next;
D2D1_RECT_F area_counter;
D2D1_RECT_F area_grid;
D2D1_RECT_F area_level;

// Game Objects
Grid grid;
std::unique_ptr<Player> player;
Pool pool;
Counter counter_score;
Counter counter_level;
Piece piece_next;
Piece piece_held;
Menu menu_pause;
Menu menu_main;
Menu menu_levels;

/********************** Main game functions **********************/
void GameInit(HWND hWnd)
{
	// init graphics and load images
	graphics->Init(hWnd);
	graphics->CreateBitmap(L"Assets/Textures/Board.png", &pBackground);
	graphics->CreateBitmap(L"Assets/Textures/grid.png", &pGrid);
	graphics->CreateBitmap(L"Assets/Textures/piece.png", &pPiece);
	graphics->CreateBitmap(L"Assets/Textures/button.png", &pButton);
	graphics->CreateBitmap(L"Assets/Textures/button_on.png", &pButton_on);
	graphics->CreateBitmap(L"Assets/Textures/number.png", &pNumber);

	// init shape patterns
	pattern_I->A = {
		0,0,0,0,
		1,1,1,1,
		0,0,0,0,
		0,0,0,0
	};
	pattern_I->B = {
		0,0,1,0,
		0,0,1,0,
		0,0,1,0,
		0,0,1,0
	};
	pattern_I->C = {
		0,0,0,0,
		0,0,0,0,
		1,1,1,1,
		0,0,0,0
	};
	pattern_I->D = {
		0,1,0,0,
		0,1,0,0,
		0,1,0,0,
		0,1,0,0
	};
	pattern_I->colour = 0;

	pattern_J->A = {
		0,0,0,0,
		1,0,0,0,
		1,1,1,0,
		0,0,0,0
	};
	pattern_J->B = {
		0,0,0,0,
		0,1,1,0,
		0,1,0,0,
		0,1,0,0
	};
	pattern_J->C = {
		0,0,0,0,
		0,0,0,0,
		1,1,1,0,
		0,0,1,0
	};
	pattern_J->D = {
		0,0,0,0,
		0,1,0,0,
		0,1,0,0,
		1,1,0,0
	};
	pattern_J->colour = 1;

	pattern_L->A = {
		0,0,0,0,
		0,0,1,0,
		1,1,1,0,
		0,0,0,0
	};
	pattern_L->B = {
		0,0,0,0,
		0,1,0,0,
		0,1,0,0,
		0,1,1,0
	};
	pattern_L->C = {
		0,0,0,0,
		0,0,0,0,
		1,1,1,0,
		1,0,0,0
	};
	pattern_L->D = {
		0,0,0,0,
		1,1,0,0,
		0,1,0,0,
		0,1,0,0
	};
	pattern_L->colour = 2;

	pattern_O->A = {
		0,0,0,0,
		0,1,1,0,
		0,1,1,0,
		0,0,0,0
	};
	pattern_O->B = {
		0,0,0,0,
		0,1,1,0,
		0,1,1,0,
		0,0,0,0
	};
	pattern_O->C = {
		0,0,0,0,
		0,1,1,0,
		0,1,1,0,
		0,0,0,0
	};
	pattern_O->D = {
		0,0,0,0,
		0,1,1,0,
		0,1,1,0,
		0,0,0,0
	};
	pattern_O->colour = 3;

	pattern_S->A = {
		0,0,0,0,
		0,1,1,0,
		1,1,0,0,
		0,0,0,0
	};
	pattern_S->B = {
		0,0,0,0,
		0,1,0,0,
		0,1,1,0,
		0,0,1,0
	};
	pattern_S->C = {
		0,0,0,0,
		0,0,0,0,
		0,1,1,0,
		1,1,0,0
	};
	pattern_S->D = {
		0,0,0,0,
		1,0,0,0,
		1,1,0,0,
		0,1,0,0
	};
	pattern_S->colour = 4;

	pattern_T->A = {
			0, 0, 0, 0,
			0, 1, 0, 0,
			1, 1, 1, 0,
			0, 0, 0, 0
	};
	pattern_T->B = {
		0, 0, 0, 0,
			0, 1, 0, 0,
			0, 1, 1, 0,
			0, 1, 0, 0
	};
	pattern_T->C = {
		0, 0, 0, 0,
			0, 0, 0, 0,
			1, 1, 1, 0,
			0, 1, 0, 0
	};
	pattern_T->D = {
		0,0,0,0,
		0,1,0,0,
		1,1,0,0,
		0,1,0,0
	};
	pattern_T->colour = 5;

	pattern_Z->A = {
		0,0,0,0,
		1,1,0,0,
		0,1,1,0,
		0,0,0,0
	};
	pattern_Z->B = {
		0,0,0,0,
		0,0,1,0,
		0,1,1,0,
		0,1,0,0
	};
	pattern_Z->C = {
		0,0,0,0,
		0,0,0,0,
		1,1,0,0,
		0,1,1,0
	};
	pattern_Z->D = {
		0,0,0,0,
		0,1,0,0,
		1,1,0,0,
		1,0,0,0
	};
	pattern_Z->colour = 6;

	// init random for piece selection
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	srand(static_cast<unsigned int>(li.QuadPart));

	// init game variables
	RECT screen_rect{};
	GetClientRect(hWnd, &screen_rect);
	deltaTime = 0;
	GameState = GAME_MAINMENU;
	PlayState = PLAY_INIT;
	goalPoints = 0;
	speed = 1.f;

	// init game objects
		//areas
	area_held = {
		static_cast<float>(screen_rect.left),
		static_cast<float>(screen_rect.top ),
		static_cast<float>(screen_rect.left + screen_rect.right  / 3.33333333),
		static_cast<float>(screen_rect.top  + screen_rect.bottom / 6.66666666)
	};
	area_next = {
		static_cast<float>(screen_rect.right - screen_rect.right / 3.33333333),
		static_cast<float>(screen_rect.top ),
		static_cast<float>(screen_rect.right),
		static_cast<float>(screen_rect.top	 + screen_rect.bottom / 6.66666666)
	};
	area_counter = {
		static_cast<float>(screen_rect.left + screen_rect.right / 3.33333333),
		static_cast<float>(screen_rect.top ),
		static_cast<float>(screen_rect.right - screen_rect.right  / 3.33333333),
		static_cast<float>(screen_rect.top   + screen_rect.bottom / 6.66666666)
	};
	area_grid = {
		static_cast<float>(screen_rect.left),
		static_cast<float>(screen_rect.top + screen_rect.bottom / 6.66666666),
		static_cast<float>(screen_rect.right - screen_rect.right / 5.f),
		static_cast<float>(screen_rect.bottom)
	};
	area_level = {
		static_cast<float>(screen_rect.right - screen_rect.right / 5.f),
		static_cast<float>(screen_rect.top + screen_rect.bottom / 6.66666666),
		static_cast<float>(screen_rect.right),
		static_cast<float>(screen_rect.top + screen_rect.bottom / 6.66666666 + screen_rect.right / 5.f)
	};
		// grid
	grid.size.x = 10;
	grid.size.y = 20;
	grid.square_size = 24.f;
	grid.square_spacing = 24.f;
	grid.location.x = area_grid.right  / 2 - grid.size.x * grid.square_spacing / 2;
	grid.location.y = area_grid.bottom / 2 + area_grid.top / 2 - grid.size.y * grid.square_spacing / 2;
	// player
	player = std::make_unique<Player>(grid.size);
	player->timer_down = 80.f;
	player->timer_down_initial = 200.f;
	player->timer_fall = 1000.f;
	player->timer_side = 120.f;
	player->timer_side_initial = 200.f;
		// pool
	pool.Init(&grid.size);
		// score
	counter_score.value = 0;
	counter_score.digit_height = 34.f;
	counter_score.digit_width = 18.f;
	counter_score.digit_spacing = 20.f;
	counter_score.location = {
		area_counter.right  / 2 + area_counter.left / 2,
		area_counter.bottom / 2 + area_counter.top  / 2
	};
		// level
	counter_level.value = 1;
	counter_level.digit_height = 34.f;
	counter_level.digit_width = 18.f;
	counter_level.digit_spacing = 20.f;
	counter_level.location = {
		area_level.right  / 2 + area_level.left / 2,
		area_level.bottom / 2 + area_level.top / 2
	};
		// next piece
	piece_next.active = true;
	piece_next.size = area_next.right - area_next.left < area_next.bottom - area_next.top ?
		area_next.right - area_next.left :
		area_next.bottom - area_next.top;
	piece_next.location = {
		area_next.right  / 2 + area_next.left / 2,
		area_next.bottom / 2 + area_next.top  / 2
	};
		// held piece
	piece_held.active = false;
	piece_held.pattern = pattern_L.get();
	piece_held.size = { area_held.right - area_held.left < area_held.bottom - area_held.top ?
		area_held.right  - area_held.left :
		area_held.bottom - area_held.top
	};
	piece_held.location = {
		area_held.right  / 2 + area_held.left / 2,
		area_held.bottom / 2 + area_held.top  / 2 
	};

	// Menus
		// Pause
			// resume
	menu_pause.m_button.push_back(Button());
	menu_pause.m_button.back().location = { screen_rect.right / 2.f, screen_rect.bottom / 3.f };
	menu_pause.m_button.back().size = { 200.f, 80.f };
	menu_pause.m_button.back().selected = false;
			// quit
	menu_pause.m_button.push_back(Button());
	menu_pause.m_button.back().location = { screen_rect.right / 2.f, screen_rect.bottom / 3.f * 2.f };
	menu_pause.m_button.back().size = { 200.f, 80.f };
	menu_pause.m_button.back().selected = false;
		// Main
			// play
	menu_main.m_button.push_back(Button());
	menu_main.m_button.back().location = { screen_rect.right / 2.f, screen_rect.bottom / 3.f - screen_rect.bottom / 6.f};
	menu_main.m_button.back().size = { 200.f, 80.f };
	menu_main.m_button.back().selected = false;
			// levels
	menu_main.m_button.push_back(Button());
	menu_main.m_button.back().location = { screen_rect.right / 2.f, screen_rect.bottom / 3.f * 2 - screen_rect.bottom / 6.f };
	menu_main.m_button.back().size = { 200.f, 80.f };
	menu_main.m_button.back().selected = false;
			// quit
	menu_main.m_button.push_back(Button());
	menu_main.m_button.back().location = { screen_rect.right / 2.f, screen_rect.bottom / 3.f * 3 - screen_rect.bottom / 6.f };
	menu_main.m_button.back().size = { 200.f, 80.f };
	menu_main.m_button.back().selected = false;
		// Level Select
			// levels
	int levelNum(9);
	float levelSize(40.f);
	for (size_t i = 0; i < levelNum; i++)
	{
		menu_levels.m_button.push_back(Button());
		menu_levels.m_button.back().location = {
			screen_rect.right / 2.f - levelNum * levelSize / 2 + levelSize * i + levelSize / 2,
			screen_rect.bottom / 2.f
		};
		menu_levels.m_button.back().size = { levelSize, levelSize };
		menu_levels.m_button.back().selected = false;
	}
			// to menu
	menu_levels.m_button.push_back(Button());
	menu_levels.m_button.back().location = { screen_rect.right / 2.f, screen_rect.bottom / 3.f * 3 - screen_rect.bottom / 6.f };
	menu_levels.m_button.back().size = { 200.f, 80.f };
	menu_levels.m_button.back().selected = false;
}

void GameUpdate(float dt)
{
	deltaTime = dt;
	switch (GameState)
	{
	case GAME_PAUSE:
	{
		// select button
		menu_pause.Select();

		// unpause from anywhere
		if (input_1->CheckPressed(BTN_ESC))
		{
			GameState = GAME_PLAY;
		}

		// unpause
		else if (input_1->CheckReleased(BTN_LMB) && menu_pause.m_button.at(0).selected)
		{
			GameState = GAME_PLAY;
		}

		// to main menu
		else if (input_1->CheckReleased(BTN_LMB) && menu_pause.m_button.at(1).selected)
		{
			GameState = GAME_MAINMENU;
		}

		break;
	}

	case GAME_PLAY:
	{
		// pause game
		if (input_1->CheckPressed(BTN_ESC))
			GameState = GAME_PAUSE;

		// gameplay functions
		switch (PlayState)
		{
		case PLAY_INIT:
		{
			switch (rand() % 7)
			{
			case 0:
				piece_next.pattern = pattern_I.get();
				break;
			case 1:
				piece_next.pattern = pattern_J.get();
				break;
			case 2:
				piece_next.pattern = pattern_L.get();
				break;
			case 3:
				piece_next.pattern = pattern_O.get();
				break;
			case 4:
				piece_next.pattern = pattern_S.get();
				break;
			case 5:
				piece_next.pattern = pattern_T.get();
				break;
			case 6:
				piece_next.pattern = pattern_Z.get();
				break;
			}
			pool.vSquare.clear();
			counter_level.value = 1;
			counter_score.value = 0;
			goalPoints = 0;
			piece_held.pattern = nullptr;
			piece_held.active = false;
			PlayState = PLAY_SERVING;
			break;
		}
		case PLAY_SWAP:
		{
			// swap player and held pattern
			const Pattern* tmpPattern;
			tmpPattern = piece_held.pattern;
			piece_held.pattern = player->m_pPattern;
			player->m_pPattern = tmpPattern;

			// reset player
			player->location = { grid.size.x / 2 - 2, -1 };
			player->rotation = 0;
			player->ResetTimers();
			player->UpdateSquare();

			PlayState = PLAY_MOVING;
			break;
		}
		case PLAY_SERVING:
		{
			// level check
			if (goalPoints >= counter_level.value * 5)
			{
				goalPoints = 0;
				++counter_level.value;
			}

			// reset player to top and assign new values
			player->location = { grid.size.x / 2 - 2, -1 };
			player->rotation = 0;
			player->m_pPattern = piece_next.pattern;
			switch (rand() % 7)
			{
			case 0:
				piece_next.pattern = pattern_I.get();
				break;
			case 1:
				piece_next.pattern = pattern_J.get();
				break;
			case 2:
				piece_next.pattern = pattern_L.get();
				break;
			case 3:
				piece_next.pattern = pattern_O.get();
				break;
			case 4:
				piece_next.pattern = pattern_S.get();
				break;
			case 5:
				piece_next.pattern = pattern_T.get();
				break;
			case 6:
				piece_next.pattern = pattern_Z.get();
				break;
			}
			player->ResetTimers();
			player->UpdateSquare();

			PlayState = PLAY_MOVING;
			break;
		}
		case PLAY_MOVING:
		{
			// timers
			player->UpdateTimers(deltaTime);

			// move down
			if (input_1->CheckPressed(BTN_S))
			{
				player->timer_down_initial_update = 0;
				player->timer_fall_update = 0;
				player->Move(0, 1, &pool);
			}
			else if (input_1->CheckDown(BTN_S) &&
				player->timer_down_initial_update > player->timer_down_initial &&
				player->timer_down_update > player->timer_down)
			{
				player->timer_down_update = 0;
				player->timer_fall_update = 0;
				player->Move(0, 1, &pool);
			}

			// fall or land
			else if (player->timer_fall_update > player->timer_fall)
			{
				player->timer_fall_update = 0;
				if (!player->Move(0, 1, &pool))
					PlayState = PLAY_CHECKING;
			}

			// move left
			if (input_1->CheckPressed(BTN_A))
			{
				player->timer_side_initial_update = 0;
				player->Move(-1, 0, &pool);
			}
			else if (input_1->CheckDown(BTN_A) &&
				player->timer_side_initial_update > player->timer_side_initial &&
				player->timer_side_update > player->timer_side)
			{
				player->timer_side_update = 0;
				player->Move(-1, 0, &pool);
			}

			// move right
			if (input_1->CheckPressed(BTN_D))
			{
				player->timer_side_initial_update = 0;
				player->Move(1, 0, &pool);
			}
			else if (input_1->CheckDown(BTN_D) &&
				player->timer_side_initial_update > player->timer_side_initial &&
				player->timer_side_update > player->timer_side)
			{
				player->timer_side_update = 0;
				player->Move(1, 0, &pool);
			}

			// rotate clockwise
			if (input_1->CheckPressed(BTN_E))
			{
				player->Rotate(pool, true);
			}

			// rotate anti-clockwise
			if (input_1->CheckPressed(BTN_Q))
			{
				player->Rotate(pool, false);
			}

			// hold piece
			if (input_1->CheckPressed(BTN_SPACE) && piece_held.active)
			{
				// swap piece and serve
				PlayState = PLAY_SWAP;
				break;
			}
			else if (input_1->CheckPressed(BTN_SPACE) && !piece_held.active)
			{
				// store piece and serve
				piece_held.active = true;
				piece_held.pattern = player->m_pPattern;
				PlayState = PLAY_SERVING;
				break;
			}

			break;
		}
		case PLAY_CHECKING:
		{
			// add player squares to pool
			for (const auto& sqr : player->vSquare)
				pool.vSquare.push_back({ sqr.x + player->location.x, sqr.y + player->location.y, player->m_pPattern->colour }); // y is 1 too short

			// delete matches and score
			switch (pool.DeleteRows())
			{
			case 1:
				counter_score.value += 100 * counter_level.value;
				goalPoints += 1;
				break;
			case 2:
				counter_score.value += 300 * counter_level.value;
				goalPoints += 3;
				break;
			case 3:
				counter_score.value += 500 * counter_level.value;
				goalPoints += 5;
				break;
			case 4:
				counter_score.value += 800 * counter_level.value;
				goalPoints += 8;
				break;
			}

			// serve new piece
			PlayState = PLAY_SERVING;

			break;
		}
		}
		break;
	}

	case GAME_MAINMENU:
	{
		// select button
		menu_main.Select();

		// play
		if (input_1->CheckReleased(BTN_LMB) && menu_main.m_button.at(0).selected)
		{
			GameState = GAME_PLAY;
			PlayState = PLAY_INIT;
		}

		// level select
		else if (input_1->CheckReleased(BTN_LMB) && menu_main.m_button.at(1).selected)
		{
			GameState = GAME_LEVELS;
		}

		// quit game
		else if (input_1->CheckReleased(BTN_LMB) && menu_main.m_button.at(2).selected)
		{
			PostQuitMessage(0);
		}
		break;
	}

	case GAME_LEVELS:
	{
		// select button
		menu_levels.Select();

		// back
		if (input_1->CheckReleased(BTN_LMB) && menu_levels.m_button.back().selected)
		{
			GameState = GAME_MAINMENU;
		}

		// levels
		else if (input_1->CheckReleased(BTN_LMB))
		{
			for (size_t i = 0; i < menu_levels.m_button.size() - 1; i++)
			{
				if (menu_levels.m_button.at(i).selected)
				{
					switch (rand() % 7)
					{
					case 0:
						piece_next.pattern = pattern_I.get();
						break;
					case 1:
						piece_next.pattern = pattern_J.get();
						break;
					case 2:
						piece_next.pattern = pattern_L.get();
						break;
					case 3:
						piece_next.pattern = pattern_O.get();
						break;
					case 4:
						piece_next.pattern = pattern_S.get();
						break;
					case 5:
						piece_next.pattern = pattern_T.get();
						break;
					case 6:
						piece_next.pattern = pattern_Z.get();
						break;
					}
					pool.vSquare.clear();
					counter_level.value = i + 1;
					counter_score.value = 0;
					goalPoints = 0;
					piece_held.pattern = nullptr;
					piece_held.active = false;
					PlayState = PLAY_SERVING;
					GameState = GAME_PLAY;
				}
			}
		}
	
		

		break;
	}
	}
	
}

void GameRender()
{
	graphics->BeginDraw();

	switch (GameState)
	{
	case GAME_PAUSE:
	{
		// resume button
		menu_pause.m_button.at(0).selected ?
			graphics->DrawBitmapArea(&pButton_on.p, menu_pause.m_button.at(0).GetRect(), { 0,  0,   50.f, 30.f }) :
			graphics->DrawBitmapArea(&pButton.p,	menu_pause.m_button.at(0).GetRect(), { 0,  0,   50.f, 30.f });
		// menu button
		menu_pause.m_button.at(1).selected ?
			graphics->DrawBitmapArea(&pButton_on.p, menu_pause.m_button.at(1).GetRect(), { 0, 60.f, 50.f, 90.f }) :
			graphics->DrawBitmapArea(&pButton.p,	menu_pause.m_button.at(1).GetRect(), { 0, 60.f, 50.f, 90.f });
		break;
	}

	case GAME_PLAY:
		if (PlayState != PLAY_INIT)
		{
			graphics->ClearScreen();

			// debug areas
			//graphics->DrawRect(area_held);
			//graphics->DrawRect(area_next);
			//graphics->DrawRect(area_counter);
			//graphics->DrawRect(area_grid);
			//graphics->DrawRect(area_level);

			// draw grid
			for (int x(0); x < grid.size.x; ++x)
				for (int y(0); y < grid.size.y; ++y)
					graphics->DrawBitmap(&pGrid.p, grid.GetSquareRect({ x,y }));

			// next piece
			if (piece_next.active)
				for (int i(0); i < piece_next.pattern->A.size(); ++i)
					graphics->DrawBitmapArea(&pPiece.p, piece_next.GetRect(i), {
						piece_next.pattern->colour * 1.f,
						0,
						(piece_next.pattern->colour + 1) * 1.f,
						1.f
						}
					);

			// held piece
			if (piece_held.active)
				for (int i(0); i < piece_held.pattern->A.size(); ++i)
					graphics->DrawBitmapArea(&pPiece.p, piece_held.GetRect(i), {
						piece_held.pattern->colour * 1.f,
						0,
						(piece_held.pattern->colour + 1) * 1.f,
						1.f
						}
					);

			// draw score
			for (size_t i = 0; i < counter_score.GetLength(); i++)
				graphics->DrawBitmapArea(&pNumber.p, counter_score.GetRect(i), counter_score.GetSpriteRect(counter_score.GetDigit(i)));

			// draw level
			for (size_t i = 0; i < counter_level.GetLength(); i++)
				graphics->DrawBitmapArea(&pNumber.p, counter_level.GetRect(i), counter_level.GetSpriteRect(counter_level.GetDigit(i)));

			// draw pool
			for (const auto& sqr : pool.vSquare)
				graphics->DrawBitmapArea(&pPiece.p, grid.GetSquareRect({ sqr.x, sqr.y }), {
						sqr.colour * 1.f,
						0,
						(sqr.colour + 1) * 1.f,
						1.f
					}
				);

			// draw player
			if (PlayState == PLAY_MOVING)
				for (const auto& sqr : player->vSquare)
					graphics->DrawBitmapArea(&pPiece.p, grid.GetSquareRect({ player->location.x + sqr.x , player->location.y + sqr.y }), {
						player->m_pPattern->colour * 1.f,
						0,
						(player->m_pPattern->colour + 1) * 1.f,
						1.f
						}
			);
		}
		break;

	case GAME_MAINMENU:
	{
		graphics->ClearScreen();
		// play button
		menu_main.m_button.at(0).selected ?
			graphics->DrawBitmapArea(&pButton_on.p, menu_main.m_button.at(0).GetRect(), {  0,   30.f,  50.f, 60.f }) :
			graphics->DrawBitmapArea(&pButton.p,	menu_main.m_button.at(0).GetRect(), {  0,   30.f,  50.f, 60.f });
		// level select button															   	   
		menu_main.m_button.at(1).selected ?												   	   
			graphics->DrawBitmapArea(&pButton_on.p, menu_main.m_button.at(1).GetRect(), { 50.f, 30.f, 100.f, 60.f }) :
			graphics->DrawBitmapArea(&pButton.p,	menu_main.m_button.at(1).GetRect(), { 50.f, 30.f, 100.f, 60.f });
		// quit button
		menu_main.m_button.at(2).selected ?
			graphics->DrawBitmapArea(&pButton_on.p, menu_main.m_button.at(2).GetRect(), { 50.f,  0,   100.f, 30.f }) :
			graphics->DrawBitmapArea(&pButton.p,	menu_main.m_button.at(2).GetRect(), { 50.f,  0,   100.f, 30.f });
		break;
	}
	case GAME_LEVELS:
	{
		graphics->ClearScreen();

		// level buttons
		for (size_t i = 0; i < menu_levels.m_button.size() - 1; i++)
		{
			if (menu_levels.m_button.at(i).selected)
				graphics->DrawRect(menu_levels.m_button.at(i).GetRect());
			else
				graphics->DrawBitmapArea(&pNumber.p, menu_levels.m_button.at(i).GetRect(), { 18.f * (i + 1), 0, 18.f * (i + 2), 34.f });
		}

		// to main menu button
		if (menu_levels.m_button.back().selected)
			graphics->DrawBitmapArea(&pButton_on.p, menu_levels.m_button.back().GetRect(), { 0, 60.f, 50.f, 90.f });
		else
			graphics->DrawBitmapArea(&pButton.p,	menu_levels.m_button.back().GetRect(), { 0, 60.f, 50.f, 90.f });

		break;
	}
	}

	graphics->EndDraw();
}

void GameDestroy()
{
}



