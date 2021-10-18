#include "Menu.h"
#include "Input.h"

void Menu::Select()
{
	// check if mouse over MenuItem
	if (focused)
	{
		int i(0);
		for (auto& btn : m_button)
		{
			if (input_1->Mouse.x > btn->location.x - btn->size.x / 2.f &&
				input_1->Mouse.y > btn->location.y - btn->size.y / 2.f &&
				input_1->Mouse.x < btn->location.x + btn->size.x / 2.f &&
				input_1->Mouse.y < btn->location.y + btn->size.y / 2.f)
			{
				btn->hover = true;
			}
			else
			{
				btn->hover = false;
			}
			++i;
		}
	}
	
}

void Menu::UpdateFocus()
{
	for (const auto& btn : m_button)
	{
		if (btn->focused)
		{
			focused = false;
			return;
		}
	}
	focused = true;
}


