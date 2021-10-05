#include "Pattern.h"
#include <algorithm>

Pattern::Pattern()
{
	for (size_t i = 0; i < 16; ++i)
	{
		up[i]    = 0;
		right[i] = 0;
		down[i]  = 0;
		left[i]  = 0;
	};
}
Pattern::~Pattern()
{
}

bool Pattern::operator == (const Pattern& p)
{
	return {
	std::equal(left.begin(),  left.end(),  p.left.begin(),  p.left.end())  &&
	std::equal(up.begin(),	  up.end(),	   p.up.begin(),	p.up.end())    &&
	std::equal(right.begin(), right.end(), p.right.begin(), p.right.end()) &&
	std::equal(down.begin(),  down.end(),  p.down.begin(),  p.down.end())
	};
}
