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
			{
				input_1->SetDown(BTN_0, 0x30, msg.wParam);
				input_1->SetDown(BTN_1, 0x31, msg.wParam);
				input_1->SetDown(BTN_2, 0x32, msg.wParam);
				input_1->SetDown(BTN_3, 0x33, msg.wParam);
				input_1->SetDown(BTN_4, 0x34, msg.wParam);
				input_1->SetDown(BTN_5, 0x35, msg.wParam);
				input_1->SetDown(BTN_6, 0x36, msg.wParam);
				input_1->SetDown(BTN_7, 0x37, msg.wParam);
				input_1->SetDown(BTN_8, 0x38, msg.wParam);
				input_1->SetDown(BTN_9, 0x39, msg.wParam);

				input_1->SetDown(BTN_A, 0x41, msg.wParam);
				input_1->SetDown(BTN_B, 0x42, msg.wParam);
				input_1->SetDown(BTN_C, 0x43, msg.wParam);
				input_1->SetDown(BTN_D, 0x44, msg.wParam);
				input_1->SetDown(BTN_E, 0x45, msg.wParam);
				input_1->SetDown(BTN_F, 0x46, msg.wParam);
				input_1->SetDown(BTN_G, 0x47, msg.wParam);
				input_1->SetDown(BTN_H, 0x48, msg.wParam);
				input_1->SetDown(BTN_I, 0x49, msg.wParam);
				input_1->SetDown(BTN_J, 0x4A, msg.wParam);
				input_1->SetDown(BTN_K, 0x4B, msg.wParam);
				input_1->SetDown(BTN_L, 0x4C, msg.wParam);
				input_1->SetDown(BTN_M, 0x4D, msg.wParam);
				input_1->SetDown(BTN_N, 0x4E, msg.wParam);
				input_1->SetDown(BTN_O, 0x4F, msg.wParam);
				input_1->SetDown(BTN_P, 0x50, msg.wParam);
				input_1->SetDown(BTN_Q, 0x51, msg.wParam);
				input_1->SetDown(BTN_R, 0x52, msg.wParam);
				input_1->SetDown(BTN_S, 0x53, msg.wParam);
				input_1->SetDown(BTN_T, 0x54, msg.wParam);
				input_1->SetDown(BTN_U, 0x55, msg.wParam);
				input_1->SetDown(BTN_V, 0x56, msg.wParam);
				input_1->SetDown(BTN_W, 0x57, msg.wParam);
				input_1->SetDown(BTN_X, 0x58, msg.wParam);
				input_1->SetDown(BTN_Y, 0x59, msg.wParam);
				input_1->SetDown(BTN_Z, 0x5A, msg.wParam);

				input_1->SetDown(BTN_LEFT,	VK_LEFT,	msg.wParam);
				input_1->SetDown(BTN_UP,	VK_UP,		msg.wParam);
				input_1->SetDown(BTN_RIGHT, VK_RIGHT,	msg.wParam);
				input_1->SetDown(BTN_DOWN,	VK_DOWN,	msg.wParam);
				input_1->SetDown(BTN_SPACE, VK_SPACE,	msg.wParam);
				input_1->SetDown(BTN_ENTER, VK_RETURN,	msg.wParam);
				input_1->SetDown(BTN_BACK,	VK_BACK,	msg.wParam);
				input_1->SetDown(BTN_ESC,	VK_ESCAPE,	msg.wParam);
				break;
			}

			case WM_KEYUP:
			{
				input_1->SetUp(BTN_0, 0x30, msg.wParam);
				input_1->SetUp(BTN_1, 0x31, msg.wParam);
				input_1->SetUp(BTN_2, 0x32, msg.wParam);
				input_1->SetUp(BTN_3, 0x33, msg.wParam);
				input_1->SetUp(BTN_4, 0x34, msg.wParam);
				input_1->SetUp(BTN_5, 0x35, msg.wParam);
				input_1->SetUp(BTN_6, 0x36, msg.wParam);
				input_1->SetUp(BTN_7, 0x37, msg.wParam);
				input_1->SetUp(BTN_8, 0x38, msg.wParam);
				input_1->SetUp(BTN_9, 0x39, msg.wParam);

				input_1->SetUp(BTN_A, 0x41, msg.wParam);
				input_1->SetUp(BTN_B, 0x42, msg.wParam);
				input_1->SetUp(BTN_C, 0x43, msg.wParam);
				input_1->SetUp(BTN_D, 0x44, msg.wParam);
				input_1->SetUp(BTN_E, 0x45, msg.wParam);
				input_1->SetUp(BTN_F, 0x46, msg.wParam);
				input_1->SetUp(BTN_G, 0x47, msg.wParam);
				input_1->SetUp(BTN_H, 0x48, msg.wParam);
				input_1->SetUp(BTN_I, 0x49, msg.wParam);
				input_1->SetUp(BTN_J, 0x4A, msg.wParam);
				input_1->SetUp(BTN_K, 0x4B, msg.wParam);
				input_1->SetUp(BTN_L, 0x4C, msg.wParam);
				input_1->SetUp(BTN_M, 0x4D, msg.wParam);
				input_1->SetUp(BTN_N, 0x4E, msg.wParam);
				input_1->SetUp(BTN_O, 0x4F, msg.wParam);
				input_1->SetUp(BTN_P, 0x50, msg.wParam);
				input_1->SetUp(BTN_Q, 0x51, msg.wParam);
				input_1->SetUp(BTN_R, 0x52, msg.wParam);
				input_1->SetUp(BTN_S, 0x53, msg.wParam);
				input_1->SetUp(BTN_T, 0x54, msg.wParam);
				input_1->SetUp(BTN_U, 0x55, msg.wParam);
				input_1->SetUp(BTN_V, 0x56, msg.wParam);
				input_1->SetUp(BTN_W, 0x57, msg.wParam);
				input_1->SetUp(BTN_X, 0x58, msg.wParam);
				input_1->SetUp(BTN_Y, 0x59, msg.wParam);
				input_1->SetUp(BTN_Z, 0x5A, msg.wParam);

				input_1->SetUp(BTN_LEFT,	VK_LEFT,	msg.wParam);
				input_1->SetUp(BTN_UP,		VK_UP,		msg.wParam);
				input_1->SetUp(BTN_RIGHT,	VK_RIGHT,	msg.wParam);
				input_1->SetUp(BTN_DOWN,	VK_DOWN,	msg.wParam);
				input_1->SetUp(BTN_SPACE,	VK_SPACE,	msg.wParam);
				input_1->SetUp(BTN_ENTER,	VK_RETURN,	msg.wParam);
				input_1->SetUp(BTN_BACK,	VK_BACK,	msg.wParam);
				input_1->SetUp(BTN_ESC,		VK_ESCAPE,	msg.wParam);
				break;
			}
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