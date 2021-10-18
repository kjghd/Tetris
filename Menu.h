#pragma once
#include "MenuItem.h"
#include <vector>

class Menu
{
public:
	std::vector<MenuItem*> m_button;
	bool focused;
	void Select();
	void UpdateFocus();
};

