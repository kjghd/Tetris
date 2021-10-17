#pragma once
#include "Button.h"
#include <vector>

class Menu
{
public:
	std::vector<Button> m_button;
	// select item, mouse position, nav input
	void Select();
};

