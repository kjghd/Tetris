#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <memory>

struct ButtonState
{
	bool moved{ false };
	bool down{ false };
};

enum Buttons
{
	BTN_LMB,
	BTN_RMB,

	BTN_A,
	BTN_D,
	BTN_E,
	BTN_Q,
	BTN_S,
	BTN_W,

	BTN_LEFT,
	BTN_UP,
	BTN_RIGHT,
	BTN_DOWN,

	BTN_ENTER,
	BTN_ESC,

	BTN_COUNT
};

class Input
{
	ButtonState m_button[BTN_COUNT];
	
public:
	D2D1_POINT_2F Mouse;
	D2D1_POINT_2F MouseDown;

	Input() { OutputDebugString(L"Created an Input\n"); };
	~Input() { OutputDebugString(L"Destroyed an Input\n"); };

	void SetMovedFalse();
	void SetDown(int button, DWORD vk_code, WPARAM wParam);
	void SetDownMouse(int button);
	void SetUp(int button, DWORD vk_code, WPARAM wParam);
	void SetUpMouse(int button);

	bool CheckPressed(int button);
	bool CheckDown(int button);
	bool CheckReleased(int button);

};

extern std::unique_ptr<Input> input_1;
