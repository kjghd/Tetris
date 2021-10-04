#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include "Piece.h"
#include "Square.h"
#include "Pattern.h"
#include <memory>
#include <array>
#include <vector>
#include <atlbase.h>
#include <algorithm> // find(), sort()
#include <sstream> // to_wstring
#include <stdlib.h> // srand(), rand()

enum GameState
{
	GAME_PAUSE,
	GAME_SERVE,
	GAME_PLAYING,
	GAME_CHECKING,
	GAME_LOSE
};

// globals
int Game;
int Score;
auto graphics(std::make_unique<Graphics>());
static CComPtr<ID2D1Bitmap> pBackground;
static CComPtr<ID2D1Bitmap> pPiece;
static CComPtr<ID2D1Bitmap> pButton;
static CComPtr<ID2D1Bitmap> pNumber;
std::vector<Square> vStuckSquares;
std::vector<int> SquaresToDelete;
auto pattern_L(std::make_unique<Pattern>());
auto pattern_J(std::make_unique<Pattern>());
auto pattern_S(std::make_unique<Pattern>());
auto pattern_Z(std::make_unique<Pattern>());
auto pattern_T(std::make_unique<Pattern>());
auto pattern_I(std::make_unique<Pattern>());
auto pattern_O(std::make_unique<Pattern>());
auto piece(std::make_unique<Piece>());
float deltaTime; // time between cpu cycles in ms
int grid_size_x;
int grid_size_y;
D2D1_RECT_F button_retry;
D2D1_RECT_F button_exit;
D2D1_RECT_F button_pause;


// game functions
bool Sort_Y_Decending(Square a, Square b)
{
	return a.y > b.y;
}
bool Sort_X_Decending(Square a, Square b)
{
	return a.x > b.x;
}
bool Sort_Delete_Decending(int a, int b)
{
	return a > b;
}
D2D1_RECT_F GetGridRect(int x, int y)
{
	// L T R B
	return {
		static_cast<float>(x)     * 20.f + 10.f,
		static_cast<float>(y)     * 20.f + 50.f,
		static_cast<float>(x + 1) * 20.f + 9.f,
		static_cast<float>(y + 1) * 20.f + 49.f
	};
}
void RandomisePiece()
{
	switch (rand() % 7)
	{
	case 0:
		piece->m_pattern = *pattern_L;
		piece->SetColour(4); // orange
		break;
	case 1:
		piece->m_pattern = *pattern_J;
		piece->SetColour(1); // blue
		break;
	case 2:
		piece->m_pattern = *pattern_S;
		piece->SetColour(6); // green
		break;
	case 3:
		piece->m_pattern = *pattern_Z;
		piece->SetColour(3); // red
		break;
	case 4:
		piece->m_pattern = *pattern_T;
		piece->SetColour(2); // purple
		break;
	case 5:
		piece->m_pattern = *pattern_I;
		piece->SetColour(0); // cyan
		break;
	case 6:
		piece->m_pattern = *pattern_O;
		piece->SetColour(5); // yellow
		break;
	}
}
D2D1_RECT_F GetNumberRect(int value)
{
	return {
		static_cast<float>(value * 17),
		0,
		static_cast<float>((value + 1) * 17),
		34.f
	};
}

void Game_Reset()
{
	vStuckSquares.clear();
}
bool Game_Retry()
{
	OutputDebugString(L"Game Lost\n");
	if (input_1->CheckDown(BTN_LMB) &&
		input_1->Mouse.x > button_retry.left &&
		input_1->Mouse.x < button_retry.right &&
		input_1->Mouse.y > button_retry.top &&
		input_1->Mouse.y < button_retry.bottom
		)
		return true;
	else
		return false;
}
void Game_Exit()
{
	OutputDebugString(L"Game Lost\n");
	if (input_1->CheckDown(BTN_LMB) &&
		input_1->Mouse.x > button_exit.left &&
		input_1->Mouse.x < button_exit.right &&
		input_1->Mouse.y > button_exit.top &&
		input_1->Mouse.y < button_exit.bottom
		)
		PostQuitMessage(0);
}

bool Game_Serve()
{
	OutputDebugString(L"Game Serve\n");

	RandomisePiece();
	piece->origin = { 3,0 };
	piece->rotation = 0;
	piece->SetSquare(piece->m_pattern.up);

	// check if there's room to serve before serving
	bool overlap(false);
	for (const auto& sqr : piece->m_square)
	{
		if (sqr.show)
		{
			Square target{ sqr.x + piece->origin.x, sqr.y + piece->origin.y, true };
			if (std::find(vStuckSquares.begin(), vStuckSquares.end(), target) != vStuckSquares.end())
			{
				overlap = true;
				break;
			}
		}
	}


	if (!overlap)
		return true;
	else
		return false;
}
void Game_Move()
{
	// fall timers
	piece->timeSinceFall += deltaTime;
	piece->timeSinceMovedDown += deltaTime;
	// initial press fall
	if (input_1->CheckPressed(BTN_S))
	{
		piece->timeSinceFall = 0;
		piece->timeSinceMovedDown = 0;
		++piece->origin.y;
	}
	// holfing fall
	else if (input_1->CheckDown(BTN_S) && piece->timeSinceMovedDown > 200.f && piece->timeSinceFall > 100.f)
	{
		piece->timeSinceFall = 0;
		++piece->origin.y;
	}
	// auto fall
	else if (piece->timeSinceFall > 1000.f)
	{
		piece->timeSinceFall = 0;
		++piece->origin.y;
	}

	// move left
	if (input_1->CheckPressed(BTN_A) && piece->CanMoveLeft(vStuckSquares))
	{
		--piece->origin.x;
	}
	// move right
	if (input_1->CheckPressed(BTN_D) && piece->CanMoveRight(vStuckSquares, grid_size_x))
	{
		++piece->origin.x;
	}
	// rotate clockwise
	if (input_1->CheckPressed(BTN_E))
	{
		piece->RotateClockwise(vStuckSquares, grid_size_x, grid_size_y);
	}
	// rotate anti-clockwise
	if (input_1->CheckPressed(BTN_Q))
	{
		piece->RotateAntiClockwise(vStuckSquares, grid_size_x, grid_size_y);
	}
}
bool Game_Land()
{
	if (!piece->CanMoveDown(vStuckSquares, grid_size_y))
	{
		for (const auto& sqr : piece->m_square)
		{
			// only add visible squares
			if (sqr.show)
				vStuckSquares.push_back({
				piece->origin.x + sqr.x,
				piece->origin.y + sqr.y,
				true,
				sqr.colour
					});
		}
		return true;
	}
	else
		return false;
}
bool Game_Match()
{
	// sort stuck squares in decending order so indexes wont be jumbled while erasing.
	std::sort(vStuckSquares.begin(), vStuckSquares.end(), Sort_X_Decending);
	std::sort(vStuckSquares.begin(), vStuckSquares.end(), Sort_Y_Decending);

	SquaresToDelete.clear();

	// Iterate through each square on the Y-axis at X:0.
	for (int y = 0; y <= grid_size_y; y++)
	{
		// check if x row is full
		for (int x = 0; x <= grid_size_x; x++)
		{
			// check if current x pos is in vStuckSquares
			Square target{ x, y, true };
			auto it(std::find(vStuckSquares.begin(), vStuckSquares.end(), target));

			if (it != vStuckSquares.end())
				SquaresToDelete.push_back(std::distance(vStuckSquares.begin(), it));

			// if it isn't, remove what's been added and exit current x row
			else
			{
				for (int i = 0; i < x; i++)
					SquaresToDelete.pop_back();
				break;
			}
		}
	}
	return SquaresToDelete.empty() ? false : true;
}
void Game_Score()
{
	switch (SquaresToDelete.size())
	{
	case 10:
		Score += 100;
		break;
	case 20:
		Score += 300;
		break;
	case 30:
		Score += 500;
		break;
	case 40:
		Score += 800;
		break;
	}
}
void Game_Destroy()
{
	std::sort(SquaresToDelete.begin(), SquaresToDelete.end(), Sort_Delete_Decending);

	// erase squares marked for deletion
	for (const auto& x : SquaresToDelete)
		vStuckSquares.erase(vStuckSquares.begin() + x);
}
bool Game_Fall()
{
	bool squareMoved(false); // will remain false unless pieces move down.

	// check each row
	for (int y = 0; y <= grid_size_y; y++)
	{
		// look for a square in that row
		for (int x = 0; x <= grid_size_x; x++)
		{
			// if there's a square in this row then move to the next row.
			Square target{ x, y, true };
			if (std::find(vStuckSquares.begin(), vStuckSquares.end(), target) != vStuckSquares.end())
				break;

			// if there's no sqaure in this row, move all the squares above this row down 1 space
			else if (x == grid_size_x)
				for (auto& sqr : vStuckSquares)
					if (sqr.y < y)
					{
						++sqr.y;
						squareMoved = true; // a piece has moved down;
					}
		}
	}
	return squareMoved;
}


/********************** Main game functions **********************/
void GameInit(HWND hWnd)
{
	// init graphics and load images
	graphics->Init(hWnd);
	graphics->CreateBitmap(L"Assets/Textures/Board.png", &pBackground);
	graphics->CreateBitmap(L"Assets/Textures/piece.png", &pPiece);
	graphics->CreateBitmap(L"Assets/Textures/button.png", &pButton);
	graphics->CreateBitmap(L"Assets/Textures/number.png", &pNumber);

	// create shapes
	pattern_L->up = {
		0,0,0,0,
		1,0,0,0,
		1,0,0,0,
		1,1,0,0
	};
	pattern_L->right = {
		0,0,0,0,
		0,0,0,0,
		1,1,1,0,
		1,0,0,0
	};
	pattern_L->down = {
		0,0,0,0,
		1,1,0,0,
		0,1,0,0,
		0,1,0,0 };
	pattern_L->left = {
		0,0,0,0,
		0,0,0,0,
		0,0,1,0,
		1,1,1,0
	};

	pattern_J->up = {
		0,0,0,0,
		0,1,0,0,
		0,1,0,0,
		1,1,0,0
	};
	pattern_J->right = {
		0,0,0,0,
		0,0,0,0,
		1,0,0,0,
		1,1,1,0
	};
	pattern_J->down = {
		0,0,0,0,
		1,1,0,0,
		1,0,0,0,
		1,0,0,0 };
	pattern_J->left = {
		0,0,0,0,
		0,0,0,0,
		1,1,1,0,
		0,0,1,0
	};

	pattern_S->up = {
		0,0,0,0,
		1,0,0,0,
		1,1,0,0,
		0,1,0,0
	};
	pattern_S->right = {
		0,0,0,0,
		0,0,0,0,
		0,1,1,0,
		1,1,0,0
	};
	pattern_S->down = {
		0,0,0,0,
		1,0,0,0,
		1,1,0,0,
		0,1,0,0
	};
	pattern_S->left = {
		0,0,0,0,
		0,0,0,0,
		0,1,1,0,
		1,1,0,0
	};

	pattern_Z->up = {
		0,0,0,0,
		0,1,0,0,
		1,1,0,0,
		1,0,0,0
	};
	pattern_Z->right = {
		0,0,0,0,
		0,0,0,0,
		1,1,0,0,
		0,1,1,0
	};
	pattern_Z->down = {
		0,0,0,0,
		0,1,0,0,
		1,1,0,0,
		1,0,0,0
	};
	pattern_Z->left = {
		0,0,0,0,
		0,0,0,0,
		1,1,0,0,
		0,1,1,0
	};

	pattern_T->up = {
		0,0,0,0,
		0,0,0,0,
		0,1,0,0,
		1,1,1,0
	};
	pattern_T->right = {
		0,0,0,0,
		1,0,0,0,
		1,1,0,0,
		1,0,0,0
	};
	pattern_T->down = {
		0,0,0,0,
		0,0,0,0,
		1,1,1,0,
		0,1,0,0
	};
	pattern_T->left = {
		0,0,0,0,
		0,1,0,0,
		1,1,0,0,
		0,1,0,0
	};
								  		   		    
	pattern_I->up =	{
		1,0,0,0,
		1,0,0,0,
		1,0,0,0,
		1,0,0,0
	};
	pattern_I->right = {
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		1,1,1,1
	};
	pattern_I->down = {
		1,0,0,0,
		1,0,0,0,
		1,0,0,0,
		1,0,0,0
	};
	pattern_I->left = {
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		1,1,1,1
	};

	pattern_O->up = {
		0,0,0,0,
		0,0,0,0,
		1,1,0,0,
		1,1,0,0
	};
	pattern_O->right = {
		0,0,0,0,
		0,0,0,0,
		1,1,0,0,
		1,1,0,0
	};
	pattern_O->down = {
		0,0,0,0,
		0,0,0,0,
		1,1,0,0,
		1,1,0,0
	};
	pattern_O->left = {
		0,0,0,0,
		0,0,0,0,
		1,1,0,0,
		1,1,0,0
	};

	Game = 0;
	Score = 0;

	// init random for piece selection
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	srand(static_cast<unsigned int>(li.QuadPart));
	RandomisePiece();

	// grid dimensions
	grid_size_x = 9;
	grid_size_y = 19;

	// button rects
	button_retry = { 50.f,90.f,170.f,150.f};
	button_exit = { 50.f,190.f,170.f,250.f };

}

void GameUpdate(float dt)
{
	deltaTime = dt;

	switch (Game)
	{
	case GAME_PAUSE:

	case GAME_SERVE:
		if (Game_Serve())
			Game = GAME_PLAYING;
		else
			Game = GAME_LOSE;
		break;

	case GAME_PLAYING:
		if (Game_Land())
			Game = GAME_CHECKING;
		else
			Game_Move();
		break;

	case GAME_CHECKING:
		if (Game_Match())
		{
			Game_Score();
			Game_Destroy();
		}
		if (!Game_Fall())
			Game = GAME_SERVE;
		break;

	case GAME_LOSE:
		if (Game_Retry())
		{
			Game_Reset();
			Game = GAME_SERVE;
		}
		Game_Exit();
		break;
	}


}

void GameRender()
{
	graphics->BeginDraw();
	graphics->ClearScreen();

	graphics->DrawBitmap(&pBackground.p, D2D1::RectF(0, 0, 220.f, 460.f));

	// score (13 from edge, 14 from top, 17x34)

	std::string str(std::to_string(Score));
	while (str.size() < 6)
		str.insert(0, "0");
	int dgt1(str.at(0) - '0');
	int dgt2(str.at(1) - '0');
	int dgt3(str.at(2) - '0');
	int dgt4(str.at(3) - '0');
	int dgt5(str.at(4) - '0');
	int dgt6(str.at(5) - '0');


	graphics->DrawBitmapArea(&pNumber.p, { 13.f,  14.f, 29.f,  48.f }, { 0, 0, 17.f, 34.f });
	graphics->DrawBitmapArea(&pNumber.p, { 33.f,  14.f, 49.f,  48.f }, { 0, 0, 17.f, 34.f });
	graphics->DrawBitmapArea(&pNumber.p, { 53.f,  14.f, 69.f,  48.f }, GetNumberRect(dgt3));
	graphics->DrawBitmapArea(&pNumber.p, { 73.f,  14.f, 89.f,  48.f }, GetNumberRect(dgt4));
	graphics->DrawBitmapArea(&pNumber.p, { 93.f,  14.f, 109.f, 48.f }, GetNumberRect(dgt5));
	graphics->DrawBitmapArea(&pNumber.p, { 113.f, 14.f, 129.f, 48.f }, GetNumberRect(dgt6));




	// player piece
	for (size_t i = 0; i < piece->m_square.size(); ++i)
	{
		if (Game != GAME_LOSE && piece->m_square.at(i).show)
		{
			// render piece according to colour
			switch (piece->m_square.at(i).colour)
			{
			case 0:
				graphics->DrawBitmapArea(&pPiece.p, piece->GetSquareRect(i), { 0,0,18.f,18.f });
				break;
			case 1:
				graphics->DrawBitmapArea(&pPiece.p, piece->GetSquareRect(i), { 18.f,0,36.f,18.f });
				break;
			case 2:
				graphics->DrawBitmapArea(&pPiece.p, piece->GetSquareRect(i), { 36.f,0,54.f,18.f });
				break;
			case 3:
				graphics->DrawBitmapArea(&pPiece.p, piece->GetSquareRect(i), { 0,18.f,18.f,36.f });
				break;
			case 4:
				graphics->DrawBitmapArea(&pPiece.p, piece->GetSquareRect(i), { 18.f,18.f,36.f,36.f });
				break;
			case 5:
				graphics->DrawBitmapArea(&pPiece.p, piece->GetSquareRect(i), { 36.f,18.f,54.f,36.f });
				break;
			case 6:
				graphics->DrawBitmapArea(&pPiece.p, piece->GetSquareRect(i), { 0,36.f,18.f,54.f });
				break;
			}
		}
	}

	// stuck pieces
	for (size_t i = 0; i < vStuckSquares.size(); ++i)
	{
		switch (vStuckSquares.at(i).colour)
		{
		case 0:
			graphics->DrawBitmapArea(
				&pPiece.p,
				GetGridRect(vStuckSquares.at(i).x, vStuckSquares.at(i).y),
				{ 0,0,18.f,18.f }
			);
			break;
		case 1:
			graphics->DrawBitmapArea(
				&pPiece.p,
				GetGridRect(vStuckSquares.at(i).x, vStuckSquares.at(i).y),
				{ 18.f,0,36.f,18.f }
			);
			break;
		case 2:
			graphics->DrawBitmapArea(
				&pPiece.p,
				GetGridRect(vStuckSquares.at(i).x, vStuckSquares.at(i).y),
				{ 36.f,0,54.f,18.f }
			);
			break;
		case 3:
			graphics->DrawBitmapArea(
				&pPiece.p,
				GetGridRect(vStuckSquares.at(i).x, vStuckSquares.at(i).y),
				{ 0,18.f,18.f,36.f }
			);
			break;
		case 4:
			graphics->DrawBitmapArea(
				&pPiece.p,
				GetGridRect(vStuckSquares.at(i).x, vStuckSquares.at(i).y),
				{ 18.f,18.f,36.f,36.f }
			);
			break;
		case 5:
			graphics->DrawBitmapArea(
				&pPiece.p,
				GetGridRect(vStuckSquares.at(i).x, vStuckSquares.at(i).y),
				{ 36.f,18.f,54.f,36.f }
			);
			break;
		case 6:
			graphics->DrawBitmapArea(
				&pPiece.p,
				GetGridRect(vStuckSquares.at(i).x, vStuckSquares.at(i).y),
				{ 0,36.f,18.f,54.f }
			);
			break;
		}
	}

	// lose menu
	if (Game == GAME_LOSE)
	{
		graphics->DrawBitmapArea(&pButton.p, button_retry, { 0,0,120.f,60.f });
		graphics->DrawBitmapArea(&pButton.p, button_exit, { 0,60.f,120.f,120.f });

		if (input_1->Mouse.x > button_retry.left &&
			input_1->Mouse.x < button_retry.right &&
			input_1->Mouse.y > button_retry.top &&
			input_1->Mouse.y < button_retry.bottom
			)
			graphics->DrawRect(button_retry);

		if (input_1->Mouse.x > button_exit.left &&
			input_1->Mouse.x < button_exit.right &&
			input_1->Mouse.y > button_exit.top &&
			input_1->Mouse.y < button_exit.bottom
			)
			graphics->DrawRect(button_exit);
	}

	graphics->EndDraw();
}

void GameDestroy()
{
}



