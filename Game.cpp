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

enum GameStates
{
	GAME_PAUSE,
	GAME_MAINMENU,
	GAME_PLAY,
	GAME_LOSE
};

enum PlayStates
{
	PLAY_SERVING,
	PLAY_MOVING,
	PLAY_CHECKING,
};

// globals
int GameState;
int PlayState;
int CurrentScore;
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
D2D1_RECT_F button_play;
D2D1_RECT_F button_retry;
D2D1_RECT_F button_exit;
D2D1_RECT_F button_pause;
D2D1_RECT_F button_resume;
Pattern nextPattern;
int nextColour;
int nextRotation;


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
		static_cast<float>(x)     * 20.f,
		static_cast<float>(y)     * 20.f + 80.f,
		static_cast<float>(x + 1) * 20.f,
		static_cast<float>(y + 1) * 20.f + 79.f
	};
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

bool Button_Play()
{
	if (input_1->CheckDown(BTN_LMB) &&
		input_1->Mouse.x > button_play.left &&
		input_1->Mouse.x < button_play.right &&
		input_1->Mouse.y > button_play.top &&
		input_1->Mouse.y < button_play.bottom
		)
		return true;
	else
		return false;
}
bool Button_Resume()
{
	if (input_1->CheckDown(BTN_LMB) &&
		input_1->Mouse.x > button_resume.left &&
		input_1->Mouse.x < button_resume.right &&
		input_1->Mouse.y > button_resume.top &&
		input_1->Mouse.y < button_resume.bottom
		)
		return true;
	else
		return false;
}
bool Button_Retry()
{
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
void Button_Exit()
{
	if (input_1->CheckDown(BTN_LMB) &&
		input_1->Mouse.x > button_exit.left &&
		input_1->Mouse.x < button_exit.right &&
		input_1->Mouse.y > button_exit.top &&
		input_1->Mouse.y < button_exit.bottom
		)
		PostQuitMessage(0);
}
void Play_NewGame()
{
	vStuckSquares.clear();
	CurrentScore = 0;
}
void Play_QueuePiece()
{
	nextRotation = rand() % 4;
	switch (rand() % 7)
	{
	case 0:
		nextPattern = *pattern_L;
		nextColour = 4; // orange
		break;
	case 1:
		nextPattern = *pattern_J;
		nextColour = 1; // blue
		break;
	case 2:
		nextPattern = *pattern_S;
		nextColour = 6; // green
		break;
	case 3:
		nextPattern = *pattern_Z;
		nextColour = 3; // red
		break;
	case 4:
		nextPattern = *pattern_T;
		nextColour = 2; // purple
		break;
	case 5:
		nextPattern = *pattern_I;
		nextColour = 0; // cyan
		break;
	case 6:
		nextPattern = *pattern_O;
		nextColour = 5; // yellow
		break;
	}
}
bool Play_Serve()
{
	// get new piece ready
	piece->m_pattern = nextPattern;
	piece->SetColour(nextColour);
	piece->origin = { 3,0 };
	piece->rotation = nextRotation;
	switch (nextRotation)
	{
	case 0:
		piece->SetSquare(piece->m_pattern.up);
		break;
	case 1:
		piece->SetSquare(piece->m_pattern.right);
		break;
	case 2:
		piece->SetSquare(piece->m_pattern.down);
		break;
	case 3:
		piece->SetSquare(piece->m_pattern.left);
		break;
	}
	piece->timeSinceMovedDown = 0;
	piece->timeSinceFall = 0;

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
	return !overlap;
}
void Play_Move()
{
	// fall timers
	piece->timeSinceFall += deltaTime;
	piece->timeSinceMovedDown += deltaTime;
	if (piece->CanMoveDown(vStuckSquares, grid_size_y))
	{
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
bool Play_Land()
{
	if (!piece->CanMoveDown(vStuckSquares, grid_size_y))
	{
		piece->timeSinceLanded += deltaTime;
		if (piece->timeSinceLanded > 1000.f)
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
	else
	{
		piece->timeSinceLanded = 0;
		return false;
	}
}
bool Play_Match()
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
void Play_Score()
{
	switch (SquaresToDelete.size())
	{
	case 10: // single
		CurrentScore += 100;
		break;
	case 20: // double
		CurrentScore += 300;
		break;
	case 30: // triple
		CurrentScore += 500;
		break;
	case 40: // tetris
		CurrentScore += 800;
		break;
	}
}
void Play_Destroy()
{
	std::sort(SquaresToDelete.begin(), SquaresToDelete.end(), Sort_Delete_Decending);

	// erase squares marked for deletion
	for (const auto& x : SquaresToDelete)
		vStuckSquares.erase(vStuckSquares.begin() + x);
}
bool Play_Fall()
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

	GameState = GAME_MAINMENU;
	PlayState = PLAY_SERVING;
	CurrentScore = 0;

	// init random for piece selection
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	srand(static_cast<unsigned int>(li.QuadPart));
	Play_QueuePiece();

	// grid dimensions
	grid_size_x = 9;
	grid_size_y = 19;

	// button rects
	button_play   = { 40.f, 140.f, 160.f, 200.f };
	button_retry  =	{ 40.f, 140.f, 160.f, 200.f };
	button_resume = { 40.f, 140.f, 160.f, 200.f };
	button_exit   =	{ 40.f, 240.f, 160.f, 300.f };

}

void GameUpdate(float dt)
{
	deltaTime = dt;

	switch (GameState)
	{
	case GAME_MAINMENU:
		Button_Retry();
		if (Button_Play()) GameState = GAME_PLAY;
		Button_Exit();
		break;
		/************** Game is Paused ************/
	case GAME_PAUSE:
		if (Button_Resume() || input_1->CheckPressed(BTN_ESC)) GameState = GAME_PLAY;
		Button_Exit();
		break;

		/************** Game is Playing ************/
	case GAME_PLAY:
		if (input_1->CheckPressed(BTN_ESC))
			GameState = GAME_PAUSE;

		switch (PlayState)
		{
		case PLAY_SERVING:
			if (Play_Serve())
			{
				Play_QueuePiece();
				PlayState = PLAY_MOVING;
			}
			else GameState = GAME_LOSE;
			break;

		case PLAY_MOVING:
			if (Play_Land()) PlayState = PLAY_CHECKING;
			else Play_Move();
			break;

		case PLAY_CHECKING:
			if (Play_Match())
			{
				Play_Score();
				Play_Destroy();
			}
			if (!Play_Fall()) PlayState = PLAY_SERVING;
			break;
		}
		break;

		/************** Game is Lost ************/
	case GAME_LOSE:
		if (Button_Retry())
		{
			GameState = GAME_PLAY;
			PlayState = PLAY_SERVING;
			Play_NewGame();
		}
		Button_Exit();
		break;
	}




}

void GameRender()
{
	graphics->BeginDraw();
	graphics->ClearScreen();

	if (GameState != GAME_MAINMENU)
	{
		graphics->DrawBitmap(&pBackground.p, D2D1::RectF(0, 0, 200.f, 480.f));

		// scoring
		
		// convert score to string to break into digits
		std::string str(std::to_string(CurrentScore));
		while (str.size() < 6)
			str.insert(0, "0");
		graphics->DrawBitmapArea(&pNumber.p, { 3.f,   4.f, 19.f,  38.f }, GetNumberRect(str.at(0) - '0'));
		graphics->DrawBitmapArea(&pNumber.p, { 23.f,  4.f, 39.f,  38.f }, GetNumberRect(str.at(1) - '0'));
		graphics->DrawBitmapArea(&pNumber.p, { 43.f,  4.f, 59.f,  38.f }, GetNumberRect(str.at(2) - '0'));
		graphics->DrawBitmapArea(&pNumber.p, { 63.f,  4.f, 79.f,  38.f }, GetNumberRect(str.at(3) - '0'));
		graphics->DrawBitmapArea(&pNumber.p, { 83.f,  4.f, 99.f,  38.f }, GetNumberRect(str.at(4) - '0'));
		graphics->DrawBitmapArea(&pNumber.p, { 103.f, 4.f, 119.f, 38.f }, GetNumberRect(str.at(5) - '0'));

		// next piece
		D2D1_RECT_F nextPieceRect{ 125.f,5.f,195.f,75.f };
		float npXBound(nextPieceRect.right - nextPieceRect.left);
		float npYBound(nextPieceRect.bottom - nextPieceRect.top);
		std::vector<D2D1_RECT_F> vNextPieceRect;
		float npX(0); // total x size of next piece
		float npY(0); // total y size of next piece
		float npYMax(72.f);
		float scale(1.f);
		// set relative locationss
		std::array<bool, 16U> target;
		switch (nextRotation)
		{
		case 0:
			target = nextPattern.up;
			break;
		case 1:
			target = nextPattern.right;
			break;
		case 2:
			target = nextPattern.down;
			break;
		case 3:
			target = nextPattern.left;
			break;
		}
		for (size_t i = 0; i < target.size(); i++)
		{


			if (target.at(i))
			{
				// A marks default location
				// 0 0 0 0
				// 0 0 0 0
				// 0 0 0 0
				// A 0 0 0
				// x will move right
				// y will move up

				vNextPieceRect.push_back({ 0,54.f,18.f,72.f }); // create a square sized rect originating from bottom left (A on above comment)
				// x
				if (i == 0 || i == 4 || i == 8 || i == 12)
				{
					if (npX < 18.f) npX = 18.f;
				}
				else if (i == 1 || i == 5 || i == 9 || i == 13)
				{
					vNextPieceRect.back().left += 18.f;
					vNextPieceRect.back().right += 18.f;
					if (npX < 36.f) npX = 36.f;
				}
				else if (i == 2 || i == 6 || i == 10 || i == 14)
				{
					vNextPieceRect.back().left += 36.f;
					vNextPieceRect.back().right += 36.f;
					if (npX < 54.f) npX = 54.f;
				}
				else if (i == 3 || i == 7 || i == 11 || i == 15)
				{
					vNextPieceRect.back().left += 54.f;
					vNextPieceRect.back().right += 54.f;
					if (npX < 72.f) npX = 72.f;
				}
				// y
				if (i == 15 || i == 14 || i == 13 || i == 12)
				{
					if (npY < 18.f) npY = 18.f;
				}
				else if (i == 11 || i == 10 || i == 9 || i == 8)
				{
					vNextPieceRect.back().top -= 18.f;
					vNextPieceRect.back().bottom -= 18.f;
					if (npY < 36.f) npY = 36.f;
				}
				else if (i == 7 || i == 6 || i == 5 || i == 4)
				{
					vNextPieceRect.back().top -= 36.f;
					vNextPieceRect.back().bottom -= 36.f;
					if (npY < 54.f) npY = 54.f;
				}
				else if (i == 3 || i == 2 || i == 1 || i == 0)
				{
					vNextPieceRect.back().top -= 54.f;
					vNextPieceRect.back().bottom -= 54.f;
					if (npY < 72.f) npY = 72.f;
				}
			}
		}
		// set board locatioins
		for (auto& nxtPce : vNextPieceRect)
		{
			// adjust y to top so its the same as x
			nxtPce.top -= npYMax - npY;
			nxtPce.bottom -= npYMax - npY;

			// shrink if piece goes over bounds
			if (npX > npXBound)
			{
				scale = npXBound / npX;
				npX *= scale;
				npY *= scale;
			}
			else if (npY > npYBound)
			{
				scale = npYBound / npY;
				npX *= scale;
				npY *= scale;
			}

			// put in center of area
			nxtPce.left += nextPieceRect.left + (npXBound - npX) / 2.f;
			nxtPce.right += nextPieceRect.left + (npXBound - npX) / 2.f;
			nxtPce.top += nextPieceRect.top + (npYBound - npY) / 2.f;
			nxtPce.bottom += nextPieceRect.top + (npYBound - npY) / 2.f;

			graphics->DrawBitmapArea(&pPiece.p, nxtPce, { 0,0,18.f,18.f });

			switch (nextColour)
			{
			case 0:
				graphics->DrawBitmapArea(&pPiece.p, nxtPce, { 0,0,18.f,18.f });
				break;
			case 1:
				graphics->DrawBitmapArea(&pPiece.p, nxtPce, { 18.f,0,36.f,18.f });
				break;
			case 2:
				graphics->DrawBitmapArea(&pPiece.p, nxtPce, { 36.f,0,54.f,18.f });
				break;
			case 3:
				graphics->DrawBitmapArea(&pPiece.p, nxtPce, { 0,18.f,18.f,36.f });
				break;
			case 4:
				graphics->DrawBitmapArea(&pPiece.p, nxtPce, { 18.f,18.f,36.f,36.f });
				break;
			case 5:
				graphics->DrawBitmapArea(&pPiece.p, nxtPce, { 36.f,18.f,54.f,36.f });
				break;
			case 6:
				graphics->DrawBitmapArea(&pPiece.p, nxtPce, { 0,36.f,18.f,54.f });
				break;
			}

		}

		// player piece
		for (size_t i = 0; i < piece->m_square.size(); ++i)
		{
			if (PlayState != GAME_LOSE && piece->m_square.at(i).show)
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

		// menus
		if (GameState == GAME_LOSE)
		{
			graphics->DrawBitmapArea(&pButton.p, button_retry, { 0, 0,    120.f, 60.f });
			graphics->DrawBitmapArea(&pButton.p, button_exit, { 0, 60.f, 120.f, 120.f });

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
		else if (GameState == GAME_PAUSE)
		{
			graphics->DrawBitmapArea(&pButton.p, button_resume, { 120.f, 0,	   240.f, 60.f });
			graphics->DrawBitmapArea(&pButton.p, button_exit, { 0,	 60.f, 120.f, 120.f });

			if (input_1->Mouse.x > button_resume.left &&
				input_1->Mouse.x < button_resume.right &&
				input_1->Mouse.y > button_resume.top &&
				input_1->Mouse.y < button_resume.bottom
				)
				graphics->DrawRect(button_resume);

			if (input_1->Mouse.x > button_exit.left &&
				input_1->Mouse.x < button_exit.right &&
				input_1->Mouse.y > button_exit.top &&
				input_1->Mouse.y < button_exit.bottom
				)
				graphics->DrawRect(button_exit);
		}
	}
	else if (GameState == GAME_MAINMENU)
	{
		graphics->DrawBitmapArea(&pButton.p, button_play, { 120.f, 60.f, 240.f, 120.f });
		graphics->DrawBitmapArea(&pButton.p, button_exit, { 0,	   60.f, 120.f, 120.f });

		if (input_1->Mouse.x > button_play.left &&
			input_1->Mouse.x < button_play.right &&
			input_1->Mouse.y > button_play.top &&
			input_1->Mouse.y < button_play.bottom
			)
			graphics->DrawRect(button_play);

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



