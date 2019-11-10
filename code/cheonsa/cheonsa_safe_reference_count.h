#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{

	// reference counter that doesn't miscount when multiple threads try to use it at the same time.
	class safe_reference_count_c
	{
	private:
		sint32_c _count;

	public:
		safe_reference_count_c( sint32_c initial_count = 1 );
		sint32_c increment(); // if returns 0, then it failed. otherwise, it returns the new reference count.
		sint32_c decrement(); // returns the new reference count. if it returns 0, then the thing should be disposed.
		sint32_c get_count();

	};

}
