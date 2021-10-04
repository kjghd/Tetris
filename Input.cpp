#include "Input.h"

void Input::SetMovedFalse()
{
	for (size_t i = 0; i < BTN_COUNT; i++)
	{
		m_button[i].moved = false;
	}
}
void Input::SetDown(int button, DWORD vk_code, WPARAM wParam)
{
	if (wParam == vk_code && !m_button[button].down)
	{
		m_button[button].moved = true;
		m_button[button].down = true;
	}
}
void Input::SetDownMouse(int button)
{
	m_button[button].down = true;
}
void Input::SetUp(int button, DWORD vk_code, WPARAM wParam)
{
	if (wParam == vk_code && m_button[button].down)
	{
		m_button[button].moved = true;
		m_button[button].down = false;
	}
}
void Input::SetUpMouse(int button)
{
	m_button[button].down = false;
}

bool Input::CheckPressed(int button)
{
	return m_button[button].down && m_button[button].moved ? true : false;
}
bool Input::CheckDown(int button)
{
	return m_button[button].down ? true : false;
}
bool Input::CheckReleased(int button)
{
	return !m_button[button].down && m_button[button].moved ? true : false;
}

std::unique_ptr<Input> input_1{ new Input };