#include "Game.h"
#include "Input.h"
#include <Windows.h>
#include <sstream>



LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Application entry point.
int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// Create Window Class.
	LPCWSTR wcName{ L"WndClass" };
	WNDCLASS wc{ NULL };
	wc.lpfnWndProc	 = WndProc;
	wc.hInstance	 = hInstance;
	wc.lpszClassName = wcName;
	RegisterClass(&wc);

	// Create Window.
	RECT WndSize{ 0, 0, 400, 600 };
	AdjustWindowRect(&WndSize, WS_CAPTION, FALSE);
	HWND hWindow{ CreateWindow(
		wcName,
		L"Tetris",
		WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WndSize.right - WndSize.left,
		WndSize.bottom - WndSize.top,
		NULL,
		NULL,
		hInstance,
		NULL
		)
	};
	ShowWindow(hWindow, nCmdShow);

	// Set up game resources.
	GameInit(hWindow);

	// Set up timer.
	float deltaTime{ 0 }; // milliseconds since last update
	LARGE_INTEGER timer_start{ 0 };
	LARGE_INTEGER timer_end{   0 };
	LARGE_INTEGER timer_freq{  0 };
	QueryPerformanceFrequency(&timer_freq);
	QueryPerformanceCounter(&timer_start);

	// Message Loop.
	MSG msg{ NULL };
	while (msg.message != WM_QUIT)
	{
		input_1->SetMovedFalse();
		// Detect Input.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_MOUSEMOVE:
				input_1->Mouse.x = static_cast<float>(LOWORD(msg.lParam));
				input_1->Mouse.y = static_cast<float>(HIWORD(msg.lParam));
				break;
			case WM_LBUTTONDOWN:
				input_1->SetDownMouse(BTN_LMB);
				break;
			case WM_LBUTTONUP:
				input_1->SetUpMouse(BTN_LMB);
				break;

			case WM_KEYDOWN:
				input_1->SetDown(BTN_A,		0x41,		msg.wParam);
				input_1->SetDown(BTN_D,		0x44,		msg.wParam);
				input_1->SetDown(BTN_E,		0x45,		msg.wParam);
				input_1->SetDown(BTN_Q,		0x51,		msg.wParam);
				input_1->SetDown(BTN_S,		0x53,		msg.wParam);
				input_1->SetDown(BTN_W,		0x57,		msg.wParam);
				input_1->SetDown(BTN_LEFT,	VK_LEFT,	msg.wParam);
				input_1->SetDown(BTN_UP,	VK_UP,		msg.wParam);
				input_1->SetDown(BTN_RIGHT, VK_RIGHT,	msg.wParam);
				input_1->SetDown(BTN_DOWN,	VK_DOWN,	msg.wParam);
				input_1->SetDown(BTN_SPACE, VK_SPACE,	msg.wParam);
				input_1->SetDown(BTN_ENTER, VK_RETURN,	msg.wParam);
				input_1->SetDown(BTN_ESC,	VK_ESCAPE,	msg.wParam);
				break;
			case WM_KEYUP:
				input_1->SetUp(BTN_A,		0x41,		msg.wParam);
				input_1->SetUp(BTN_D,		0x44,		msg.wParam);
				input_1->SetUp(BTN_E,		0x45,		msg.wParam);
				input_1->SetUp(BTN_Q,		0x51,		msg.wParam);
				input_1->SetUp(BTN_S,		0x53,		msg.wParam);
				input_1->SetUp(BTN_W,		0x57,		msg.wParam);
				input_1->SetUp(BTN_LEFT,	VK_LEFT,	msg.wParam);
				input_1->SetUp(BTN_UP,		VK_UP,		msg.wParam);
				input_1->SetUp(BTN_RIGHT,	VK_RIGHT,	msg.wParam);
				input_1->SetUp(BTN_DOWN,	VK_DOWN,	msg.wParam);
				input_1->SetUp(BTN_SPACE,	VK_SPACE,	msg.wParam);
				input_1->SetUp(BTN_ENTER,	VK_RETURN,	msg.wParam);
				input_1->SetUp(BTN_ESC,		VK_ESCAPE,	msg.wParam);
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Update and Render.
		GameUpdate(deltaTime); // resetting input_1
		GameRender();

		// Reset Timer.
		QueryPerformanceCounter(&timer_end);
		deltaTime =
			static_cast<float>(timer_end.QuadPart - timer_start.QuadPart) /
			static_cast<float>(timer_freq.QuadPart) *
			1000.f; // Time is in microseconds, so multiplying by 1000 gives us milliseconds.
		QueryPerformanceCounter(&timer_start);
	}

	return 0;
}