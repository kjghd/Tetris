#pragma once
#include "MenuItem.h"

class MenuValue : public MenuItem
{
public:
	std::string sValue;
	int value;
	int value_min;
	int value_max;
	MenuValue();
	virtual ~MenuValue();
	void SetValue(int key);
	void CapValue();
	virtual D2D1_RECT_F GetTextRect(int char_position);
};

