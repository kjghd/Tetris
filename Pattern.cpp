#include "Pattern.h"
#include <algorithm>
#include <d2d1.h>

Pattern::Pattern()
{
	for (size_t i = 0; i < 16; ++i)
	{
		A[i] = 0;
		B[i] = 0;
		C[i] = 0;
		D[i] = 0;
	};
}
Pattern::~Pattern()
{
}
