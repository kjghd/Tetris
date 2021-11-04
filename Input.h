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
	BTN_0,
	BTN_1,
	BTN_2,
	BTN_3,
	BTN_4,
	BTN_5,
	BTN_6,
	BTN_7,
	BTN_8,
	BTN_9,

	BTN_A,
	BTN_B,
	BTN_C,
	BTN_D,
	BTN_E,
	BTN_F,
	BTN_G,
	BTN_H,
	BTN_I,
	BTN_J,
	BTN_K,
	BTN_L,
	BTN_M,
	BTN_N,
	BTN_O,
	BTN_P,
	BTN_Q,
	BTN_R,
	BTN_S,
	BTN_T,
	BTN_U,
	BTN_V,
	BTN_W,
	BTN_X,
	BTN_Y,
	BTN_Z,

	BTN_LEFT,
	BTN_UP,
	BTN_RIGHT,
	BTN_DOWN,

	BTN_SPACE,
	BTN_ENTER,
	BTN_BACK,
	BTN_ESC,

	BTN_LMB,
	BTN_RMB,


	BTN_COUNT
};

class Input
{
	ButtonState m_button[BTN_COUNT];
	
public:
	D2D1_POINT_2F Mouse;
	D2D1_POINT_2F MouseDown;

	Input();
	~Input() { OutputDebugString(L"Destroyed an Input\n"); };

	void SetMovedFalse();
	void SetDown(int button, DWORD vk_code, WPARAM wParam);
	void SetDownMouse(int button);
	void SetUp(int button, DWORD vk_code, WPARAM wParam);
	void SetUpMouse(int button);

	bool CheckPressed(int button);
	bool CheckDown(int button);
	bool CheckReleased(int button);

	int GetKeyPressed();
};

extern std::unique_ptr<Input> input_1;
