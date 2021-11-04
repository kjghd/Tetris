#include "Game.h"
#include "Graphics.h"
#include "Input.h"

#include "Player.h"
#include "Pattern.h"
#include "Grid.h"
#include "Counter.h"
#include "Piece.h"

#include "Menu.h"
#include "MenuPortal.h"
#include "MenuToggle.h"
#include "MenuValue.h"

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
static CComPtr<ID2D1Bitmap> pBMP_Button_active;
static CComPtr<ID2D1Bitmap> pNumber;
static CComPtr<ID2D1Bitmap> pBMP_Characters;
static CComPtr<ID2D1Bitmap> pBMP_Title;

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
	GAME_LOSE,
	GAME_SCORES
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
float dropSpeed;
D2D1_RECT_F area_held;
D2D1_RECT_F area_next;
D2D1_RECT_F area_counter;
D2D1_RECT_F area_grid;
D2D1_RECT_F area_level;
D2D1_RECT_F area_title;
float character_width;
float character_height;
std::vector<int> highscores;

// Game Objects
Grid grid;
std::unique_ptr<Player> player;
Pool pool;
Counter counter_score;
Counter counter_level;
std::array<Counter, 5> vcounter_highscore;
Piece piece_next;                                                                            
Piece piece_held;
Menu menu_pause;
Menu menu_lose;
Menu menu_main;
Menu menu_levels;
Menu menu_highscore;

// Game Functions
bool WriteToFile(std::string buffer)
{
	// Access file
	HANDLE scoreFile = CreateFile(L"Saves/highscore.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD error = GetLastError();
	if (error == 0) // No file exists.
	{
		OutputDebugString(L"Created new file.\n");
	}
	else if (error == 80) // File already exists.
	{
		OutputDebugString(L"Replacing file.\n");
		DeleteFile(L"Saves/highscore.txt");
		scoreFile = CreateFile(L"Saves/highscore.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	OutputDebugString(L"Writing to file...\n");

	// Debug data being written.
	std::wstring wstr;
	wstr.assign(buffer.begin(), buffer.end());
	OutputDebugString(L"Buffer: ");
	OutputDebugString(wstr.c_str());
	OutputDebugString(L"\n");

	// Debug buffer size.
	DWORD bytesToWrite = buffer.size();
	OutputDebugString(L"Bytes: ");
	OutputDebugString(std::to_wstring(bytesToWrite).c_str());
	OutputDebugString(L"\n");

	DWORD bytesWritten;

	WriteFile(scoreFile, &buffer.front(), bytesToWrite, &bytesWritten, NULL);
	CloseHandle(scoreFile);

	OutputDebugString(L"\n");

	return true;
}
bool SetHighscore()
{
	OutputDebugString(L"Reading File...\n");
	HANDLE scoreFile = CreateFile(L"Saves/highscore.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD error = GetLastError();
	if (error == ERROR_FILE_NOT_FOUND)
	{
		OutputDebugString(L"Could not obtain handle to file; file not found.\n");
		return false;
	}
	else
	{
		OutputDebugString(L"Obtained handle to file.\n");
	}

	LARGE_INTEGER fileSize;
	GetFileSizeEx(scoreFile, &fileSize);

	std::string buffer;
	buffer.resize(static_cast<unsigned int>(fileSize.QuadPart));

	DWORD bytesRead;

	BOOL read = ReadFile(scoreFile, &buffer.front(), static_cast<DWORD>(fileSize.QuadPart), &bytesRead, NULL);
	if (read == FALSE)
	{
		OutputDebugString(L"Could not read file.\n");
		return false;
	}
	else
	{
		OutputDebugString(L"Read file successfully:\n");
		std::wstring wstr;
		wstr.assign(buffer.begin(), buffer.end());
		OutputDebugString(wstr.c_str());
	}

	CloseHandle(scoreFile);
	error = GetLastError();

	// Set highscores from buffer.
	std::string str_tmp;
	highscores.clear();
	for (const auto& ch : buffer)
	{
		if (ch != '\n')
		{
			str_tmp.push_back(ch);
		}
		else
		{
			highscores.push_back(std::stoi(str_tmp));
			str_tmp.clear();
		}
	}
	// ensure there's enough scores for the score board
	if (highscores.size() < 5)
	{
		int length(highscores.size());
		for (int i = 0; i < 5 - length; i++)
			highscores.push_back(0);
	}


	// update highscore counters
	for (size_t i = 0; i < vcounter_highscore.size(); i++)
		vcounter_highscore.at(i).value = highscores.at(i);

	return true;
}
void AddHighscore()
{
	// Place score
	for (size_t i = 0; i < highscores.size(); i++)
	{
		if (counter_score.value > highscores.at(i))
		{
			highscores.insert(highscores.begin() + i, counter_score.value);
			break;
		}
	}

	// Convert score to string for writing to a file.
	std::string buffer;
	for (const auto& score : highscores)
	{
		buffer += std::to_string(score) + "\n";
	}
	if (buffer.empty()) buffer.push_back('0');

	WriteToFile(buffer);
}


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
	graphics->CreateBitmap(L"Assets/Textures/characters.png", &pBMP_Characters);
	graphics->CreateBitmap(L"Assets/Textures/title.png", &pBMP_Title);
	graphics->CreateBitmap(L"Assets/Textures/button_active.png", &pBMP_Button_active);


	// init shape patterns
	{
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
	}

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
	dropSpeed = 0;
	character_width = 32.f;
	character_height = 96.f;
	if (!SetHighscore()) highscores.clear();


	// init game objects
	{
		//areas
		area_held = {
			static_cast<float>(screen_rect.left),
			static_cast<float>(screen_rect.top),
			static_cast<float>(screen_rect.left + screen_rect.right / 3.33333333),
			static_cast<float>(screen_rect.top + screen_rect.bottom / 6.66666666)
		};
		area_next = {
			static_cast<float>(screen_rect.right - screen_rect.right / 3.33333333),
			static_cast<float>(screen_rect.top),
			static_cast<float>(screen_rect.right),
			static_cast<float>(screen_rect.top + screen_rect.bottom / 6.66666666)
		};
		area_counter = {
			static_cast<float>(screen_rect.left + screen_rect.right / 3.33333333),
			static_cast<float>(screen_rect.top),
			static_cast<float>(screen_rect.right - screen_rect.right / 3.33333333),
			static_cast<float>(screen_rect.top + screen_rect.bottom / 6.66666666)
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
		area_title = { screen_rect.right / 8.f, screen_rect.bottom / 12.f, screen_rect.right - screen_rect.right / 8.f, screen_rect.bottom / 4.f };

		// grid
		grid.size.x = 10;
		grid.size.y = 20;
		grid.square_size = 24.f;
		grid.square_spacing = 24.f;
		grid.location.x = area_grid.right / 2 - grid.size.x * grid.square_spacing / 2;
		grid.location.y = area_grid.bottom / 2 + area_grid.top / 2 - grid.size.y * grid.square_spacing / 2;

		// player
		player = std::make_unique<Player>(grid.size);
		player->timer_down = 80.f;
		player->timer_down_initial = 150.f;
		player->timer_fall = 800.f;
		player->timer_side = 120.f;
		player->timer_side_initial = 200.f;
		player->timer_land = 1000.f;

		// pool
		pool.Init(&grid.size);

		// score
		counter_score.value = 0;
		counter_score.digit_height = 34.f;
		counter_score.digit_width = 18.f;
		counter_score.digit_spacing = 20.f;
		counter_score.location = {
			area_counter.right / 2 + area_counter.left / 2,
			area_counter.bottom / 2 + area_counter.top / 2
		};

		// level
		counter_level.value = 1;
		counter_level.digit_height = 34.f;
		counter_level.digit_width = 18.f;
		counter_level.digit_spacing = 20.f;
		counter_level.location = {
			area_level.right / 2 + area_level.left / 2,
			area_level.bottom / 2 + area_level.top / 2
		};

		// next piece
		piece_next.active = true;
		piece_next.size = area_next.right - area_next.left < area_next.bottom - area_next.top ?
			area_next.right - area_next.left :
			area_next.bottom - area_next.top;
		piece_next.location = {
			area_next.right / 2 + area_next.left / 2,
			area_next.bottom / 2 + area_next.top / 2
		};

		// held piece
		piece_held.active = false;
		piece_held.pattern = pattern_L.get();
		piece_held.size = { area_held.right - area_held.left < area_held.bottom - area_held.top ?
			area_held.right - area_held.left :
			area_held.bottom - area_held.top
		};
		piece_held.location = {
			area_held.right / 2 + area_held.left / 2,
			area_held.bottom / 2 + area_held.top / 2
		};
	}
		
	// Menus
	{
		float menuItem_height = screen_rect.bottom / 10.f;
		float menuItem_width  = screen_rect.right  / 1.5f;
		float menuSpacing = menuItem_height / 2.f;

		float text_height = menuItem_height / 1.6f;
		float text_width = text_height / 2.f;
		float text_spacing = 0;
		float text_leftPadding = menuItem_width / 16.f;


		// Pause
			// resume
		menu_pause.m_button.push_back(new MenuPortal());
		menu_pause.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_pause.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f - menuItem_height / 2.f - menuSpacing / 2.f };
		menu_pause.m_button.back()->text = "RESUME";
			// to menu
		menu_pause.m_button.push_back(new MenuPortal());
		menu_pause.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_pause.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f + menuItem_height / 2.f + menuSpacing / 2.f };
		menu_pause.m_button.back()->text = "MAIN MENU";
			// button style
		for (auto& btn : menu_pause.m_button)
		{
			btn->text_leftPadding = text_leftPadding;
			btn->text_width = text_width;
			btn->text_height = text_height;
			btn->text_spacing = text_spacing;
		}

		// Game Over
			// retry
		menu_lose.m_button.push_back(new MenuPortal());
		menu_lose.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_lose.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f - menuItem_height / 2.f - menuSpacing / 2.f };
		menu_lose.m_button.back()->text = "RETRY";
			// to menu
		menu_lose.m_button.push_back(new MenuPortal());
		menu_lose.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_lose.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f + menuItem_height / 2.f + menuSpacing / 2.f };
		menu_lose.m_button.back()->text = "MAIN MENU";
			// button style
		for (auto& btn : menu_lose.m_button)
		{
			btn->text_leftPadding = text_leftPadding;
			btn->text_width = text_width;
			btn->text_height = text_height;
			btn->text_spacing = text_spacing;
		}

		// Main
			// play from lv1
		menu_main.m_button.push_back(new MenuPortal());
		menu_main.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_main.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f - menuItem_height - menuSpacing};
		menu_main.m_button.back()->text = "NEW GAME";
			// to level selection
		menu_main.m_button.push_back(new MenuPortal());
		menu_main.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_main.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f };
		menu_main.m_button.back()->text = "LEVEL SELECT";
			// Show high scores
		menu_main.m_button.push_back(new MenuPortal());
		menu_main.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_main.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f + menuItem_height + menuSpacing};
		menu_main.m_button.back()->text = "HIGH SCORES";
			// quit game
		menu_main.m_button.push_back(new MenuPortal());
		menu_main.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_main.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f + 2.f * (menuItem_height + menuSpacing) };
		menu_main.m_button.back()->text = "QUIT";
			// play sound
		menu_main.m_button.push_back(new MenuPortal());
		menu_main.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_main.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f + 3.f * (menuItem_height + menuSpacing) };
		menu_main.m_button.back()->text = "Sound";
			// button style
		for (auto& btn : menu_main.m_button)
		{
			btn->text_leftPadding = text_leftPadding;
			btn->text_width = text_width;
			btn->text_height = text_height;
			btn->text_spacing = text_spacing;
		}

		// Level Select
			// play
		menu_levels.m_button.push_back(new MenuValue());
		menu_levels.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_levels.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f - menuItem_height - menuSpacing };
		menu_levels.m_button.back()->text = "PLAY";
			// levels
		menu_levels.m_button.push_back(new MenuValue());
		menu_levels.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_levels.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f };
		menu_levels.m_button.back()->text = "LEVEL";
			// back to menu
		menu_levels.m_button.push_back(new MenuPortal());
		menu_levels.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_levels.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 2.f + menuItem_height + menuSpacing };
		menu_levels.m_button.back()->text = "BACK";
			// button style
		for (auto& btn : menu_levels.m_button)
		{
			btn->text_leftPadding = text_leftPadding;
			btn->text_width = text_width;
			btn->text_height = text_height;
			btn->text_spacing = text_spacing;
		}

		// Highscores
			// counter style
		for (auto& cntr : vcounter_highscore)
		{
			cntr.digit_height = 34.f;
			cntr.digit_width = 18.f;
			cntr.digit_spacing = 20.f;
		}
			// counter location
		vcounter_highscore.at(0).location = { screen_rect.right / 2.f, screen_rect.bottom / 7.f									 };
		vcounter_highscore.at(1).location = { screen_rect.right / 2.f, screen_rect.bottom / 7.f + screen_rect.bottom / 7.f		 };
		vcounter_highscore.at(2).location = { screen_rect.right / 2.f, screen_rect.bottom / 7.f + screen_rect.bottom / 7.f * 2.f };
		vcounter_highscore.at(3).location = { screen_rect.right / 2.f, screen_rect.bottom / 7.f + screen_rect.bottom / 7.f * 3.f };
		vcounter_highscore.at(4).location = { screen_rect.right / 2.f, screen_rect.bottom / 7.f + screen_rect.bottom / 7.f * 4.f };
			// back to menu
		menu_highscore.m_button.push_back(new MenuPortal());
		menu_highscore.m_button.back()->size = { menuItem_width, menuItem_height };
		menu_highscore.m_button.back()->location = { screen_rect.right / 2.f, screen_rect.bottom / 7.f + screen_rect.bottom / 7.f * 5.f };
		menu_highscore.m_button.back()->text = "BACK";
		menu_highscore.m_button.back()->text_leftPadding = text_leftPadding;
		menu_highscore.m_button.back()->text_width = text_width;
		menu_highscore.m_button.back()->text_height = text_height;
		menu_highscore.m_button.back()->text_spacing = text_spacing;
	}
		
}

void GameUpdate(float dt)
{
	deltaTime = dt;
	switch (GameState)
	{
	case GAME_PAUSE:
	{
		// select button
		menu_pause.UpdateFocus();
		menu_pause.Select();

		// unpause from anywhere
		if (input_1->CheckPressed(BTN_ESC))
		{
			GameState = GAME_PLAY;
		}

		// unpause
		else if (input_1->CheckReleased(BTN_LMB) && menu_pause.m_button.at(0)->hover)
		{
			GameState = GAME_PLAY;
		}

		// to main menu
		else if (input_1->CheckReleased(BTN_LMB) && menu_pause.m_button.at(1)->hover)
		{
			GameState = GAME_MAINMENU;
		}

		break;
	}
	
	case GAME_LOSE:
	{
		// select button
		menu_lose.UpdateFocus();
		menu_lose.Select();

		// play
		if (input_1->CheckReleased(BTN_LMB) && menu_lose.m_button.at(0)->hover)
		{
			GameState = GAME_PLAY;
			PlayState = PLAY_INIT;
		}

		// to main menu
		else if (input_1->CheckReleased(BTN_LMB) && menu_lose.m_button.at(1)->hover)
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
			// drop speed
			switch (counter_level.value)
			{
			case  1: dropSpeed = 48.f / 60.f * 1000.f; break;
			case  2: dropSpeed = 43.f / 60.f * 1000.f; break;
			case  3: dropSpeed = 38.f / 60.f * 1000.f; break;
			case  4: dropSpeed = 33.f / 60.f * 1000.f; break;
			case  5: dropSpeed = 28.f / 60.f * 1000.f; break;
			case  6: dropSpeed = 23.f / 60.f * 1000.f; break;
			case  7: dropSpeed = 18.f / 60.f * 1000.f; break;
			case  8: dropSpeed = 13.f / 60.f * 1000.f; break;
			case  9: dropSpeed =  8.f / 60.f * 1000.f; break;
			case 10: dropSpeed =  6.f / 60.f * 1000.f; break;
			case 11: dropSpeed =  5.f / 60.f * 1000.f; break;
			case 12: dropSpeed =  5.f / 60.f * 1000.f; break;
			case 13: dropSpeed =  5.f / 60.f * 1000.f; break;
			case 14: dropSpeed =  4.f / 60.f * 1000.f; break;
			case 15: dropSpeed =  4.f / 60.f * 1000.f; break;
			case 16: dropSpeed =  4.f / 60.f * 1000.f; break;
			case 17: dropSpeed =  3.f / 60.f * 1000.f; break;
			case 18: dropSpeed =  3.f / 60.f * 1000.f; break;
			case 19: dropSpeed =  3.f / 60.f * 1000.f; break;
			case 20: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 21: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 22: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 23: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 24: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 25: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 26: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 27: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 28: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 29: dropSpeed =  2.f / 60.f * 1000.f; break;
			case 30: dropSpeed =  1.f / 60.f * 1000.f; break;
			default: dropSpeed =  1.f / 60.f * 1000.f; break;

			}

			player->timer_down = dropSpeed / 10.f;


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

			// check if lost
			for (const auto& sqr : player->vSquare)
			{
				Square tmp{ sqr.x + player->location.x, sqr.y + player->location.y };
				if (std::find(pool.vSquare.begin(), pool.vSquare.end(), tmp) != pool.vSquare.end())
				{
					AddHighscore();
					GameState = GAME_LOSE;
					break;
				}
			}

			PlayState = PLAY_MOVING;

			break;
		}
		case PLAY_MOVING:
		{
			// timers
			player->UpdateMoveTimers(deltaTime);

			if (!player->Landed(&pool))
			{
				player->timer_land_update = 0;

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

				// fall
				else if (player->timer_fall_update > dropSpeed)
				{
					player->timer_fall_update = 0;
					player->Move(0, 1, &pool);
				}
			}
			// land
			else
			{
				player->timer_land_update += deltaTime;
				if (player->timer_land_update > player->timer_land)
				{
					PlayState = PLAY_CHECKING;
				}
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
		menu_main.UpdateFocus();
		menu_main.Select();

		// play
		if (input_1->CheckReleased(BTN_LMB) && menu_main.m_button.at(0)->hover)
		{
			GameState = GAME_PLAY;
			PlayState = PLAY_INIT;
		}

		// level select
		else if (input_1->CheckReleased(BTN_LMB) && menu_main.m_button.at(1)->hover)
		{
			GameState = GAME_LEVELS;
		}

		// highscores
		else if (input_1->CheckReleased(BTN_LMB) && menu_main.m_button.at(2)->hover)
		{
			SetHighscore();
			GameState = GAME_SCORES;
		}

		// quit game
		else if (input_1->CheckReleased(BTN_LMB) && menu_main.m_button.at(3)->hover)
		{
			PostQuitMessage(0);
		}

		// play sound
		else if (input_1->CheckReleased(BTN_LMB) && menu_main.m_button.at(4)->hover)
		{
			OutputDebugString(L"Finish learing XAudio first.\n");
		}
		break;
	}

	case GAME_LEVELS:
	{
		// select button
		menu_levels.UpdateFocus();
		menu_levels.Select();

		// play
		if (input_1->CheckReleased(BTN_LMB) && menu_levels.m_button.at(0)->hover)
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
			counter_level.value = dynamic_cast<MenuValue*>(menu_levels.m_button.at(1))->value;
			counter_score.value = 0;
			goalPoints = 0;
			piece_held.pattern = nullptr;
			piece_held.active = false;
			PlayState = PLAY_SERVING;
			GameState = GAME_PLAY;
			break; 
		}

		// type level
		if (menu_levels.m_button.at(1)->focused == true)
		{
			// set value to pressed key
			if (input_1->GetKeyPressed() == BTN_ESC || input_1->GetKeyPressed() == BTN_ENTER)
			{
				menu_levels.m_button.at(1)->focused = false;
				dynamic_cast<MenuValue*>(menu_levels.m_button.at(1))->CapValue();
			}
			else
			{
				dynamic_cast<MenuValue*>(menu_levels.m_button.at(1))->SetValue(input_1->GetKeyPressed());
			}
		}

		// enter level selector
		if (input_1->CheckReleased(BTN_LMB) && menu_levels.m_button.at(1)->hover)
		{
			dynamic_cast<MenuValue*>(menu_levels.m_button.at(1))->focused = true;
		}

		// back to main menu
		else if (input_1->CheckReleased(BTN_LMB) && menu_levels.m_button.at(2)->hover)
		{
			GameState = GAME_MAINMENU;
		}

		break;
	}

	case GAME_SCORES:
	{
		menu_highscore.UpdateFocus();
		menu_highscore.Select();

		// back to main menu
		if (input_1->CheckReleased(BTN_LMB) && menu_highscore.m_button.at(0)->hover)
		{
			GameState = GAME_MAINMENU;
		}
		break;
	}
	}
	
}

void GameAudio()
{
}

void GameRender()
{
	graphics->BeginDraw();

	switch (GameState)
	{
	case GAME_PAUSE:
	{
		// draw menu items
		for (const auto& item : menu_pause.m_button)
		{
			item->hover ?
				graphics->DrawBitmap(&pButton_on.p, item->GetRect()) :
				graphics->DrawBitmap(&pButton.p, item->GetRect());

			for (size_t i = 0; i < item->text.size(); i++)
				graphics->DrawBitmapArea(
					&pBMP_Characters.p,
					item->GetTextRect(i),
					item->GetTextUVRect(i, character_width, character_height)
				);
		}
		break;
	}

	case GAME_LOSE:
	{
		// draw menu items
		for (const auto& item : menu_lose.m_button)
		{
			item->hover ?
				graphics->DrawBitmap(&pButton_on.p, item->GetRect()) :
				graphics->DrawBitmap(&pButton.p,	item->GetRect());

			for (size_t i = 0; i < item->text.size(); i++)
				graphics->DrawBitmapArea(
					&pBMP_Characters.p,
					item->GetTextRect(i),
					item->GetTextUVRect(i, character_width, character_height)
				);
		}
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
				for (size_t i(0); i < piece_next.pattern->A.size(); ++i)
					graphics->DrawBitmapArea(&pPiece.p, piece_next.GetRect(i), {
						piece_next.pattern->colour * 1.f,
						0,
						(piece_next.pattern->colour + 1) * 1.f,
						1.f
						}
					);

			// held piece
			if (piece_held.active)
				for (size_t i(0); i < piece_held.pattern->A.size(); ++i)
					graphics->DrawBitmapArea(&pPiece.p, piece_held.GetRect(i), {
						piece_held.pattern->colour * 1.f,
						0,
						(piece_held.pattern->colour + 1) * 1.f,
						1.f
						}
					);

			// draw score
			for (int i = 0; i < counter_score.GetLength(); i++)
				graphics->DrawBitmapArea(&pNumber.p, counter_score.GetRect(i), counter_score.GetSpriteRect(counter_score.GetDigit(i)));

			// draw level
			for (int i = 0; i < counter_level.GetLength(); i++)
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

		graphics->DrawBitmap(&pBMP_Title.p, area_title);

		// draw menu items
		for (const auto& item : menu_main.m_button)
		{
			item->hover ?
				graphics->DrawBitmap(&pButton_on.p, item->GetRect()) :
				graphics->DrawBitmap(&pButton.p,	item->GetRect());

			for (size_t i = 0; i < item->text.size(); i++)
				graphics->DrawBitmapArea(
					&pBMP_Characters.p,
					item->GetTextRect(i),
					item->GetTextUVRect(i, character_width, character_height)
				);
		}
		break;
	}

	case GAME_LEVELS:
	{
		graphics->ClearScreen();
		// draw menu items
		for (const auto& item : menu_levels.m_button)
		{
			if (item->focused)
				graphics->DrawBitmap(&pBMP_Button_active.p, item->GetRect());
			else if (item->hover)
				graphics->DrawBitmap(&pButton_on.p, item->GetRect());
			else
				graphics->DrawBitmap(&pButton.p, item->GetRect());

			for (size_t i = 0; i < item->text.size(); i++)
				graphics->DrawBitmapArea(
					&pBMP_Characters.p,
					item->GetTextRect(i),
					item->GetTextUVRect(i, character_width, character_height)
				);
		}
		break;
	}

	case GAME_SCORES:
	{
		graphics->ClearScreen();

		// draw scores
		for (auto& cntr : vcounter_highscore)
		{
			for (int i = 0; i < cntr.GetLength(); i++)
				graphics->DrawBitmapArea(&pNumber.p, cntr.GetRect(i), cntr.GetSpriteRect(cntr.GetDigit(i)));
		}

		// draw back button
		for (const auto& item : menu_highscore.m_button)
		{
			item->hover ?
				graphics->DrawBitmap(&pButton_on.p, item->GetRect()) :
				graphics->DrawBitmap(&pButton.p, item->GetRect());

			for (size_t i = 0; i < item->text.size(); i++)
				graphics->DrawBitmapArea(
					&pBMP_Characters.p,
					item->GetTextRect(i),
					item->GetTextUVRect(i, character_width, character_height)
				);
		}
		break;
	}
	}

	graphics->EndDraw();
}

void GameDestroy()
{
}
