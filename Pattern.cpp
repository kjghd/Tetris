#include "Pattern.h"

Pattern::Pattern()
{
	for (size_t i = 0; i < 16; ++i)
	{
		up[i] = 0;
		right[i] = 0;
		down[i] = 0;
		left[i] = 0;
	};
}
Pattern::~Pattern()
{
}