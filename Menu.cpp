#include "Menu.h"
#include "Input.h"

void Menu::Select()
{
	// mouse hover
	int i(0);
	for (auto& btn : m_button)
	{
		if (input_1->Mouse.x > btn.location.x - btn.size.x / 2.f &&
			input_1->Mouse.y > btn.location.y - btn.size.y / 2.f &&
			input_1->Mouse.x < btn.location.x + btn.size.x / 2.f &&
			input_1->Mouse.y < btn.location.y + btn.size.y / 2.f)
		{
			btn.selected = true;
		}
		else
		{
			btn.selected = false;
		}
		++i;
	}
}
