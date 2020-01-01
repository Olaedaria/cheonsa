#pragma once

#include "cheonsa__types.h"

namespace cheonsa
{

	// reference counter that doesn't miscount when multiple threads try to use it at the same time.
	// uses "interlocked" functions provided by the operating system to manipulate the count, rather than using a critical section.
	class core_safe_reference_counter_c
	{
	private:
		sint32_c _count;

	public:
		core_safe_reference_counter_c( sint32_c initial_count = 1 );
		sint32_c increment(); // if it returns 0 then it means the thing was disposed by another thread, and that the calling thread should just forget its reference to the thing. otherwise it returns the new reference count.
		sint32_c decrement(); // returns the new reference count. if it returns 0 then it means that the calling thread should dispose the thing.
		sint32_c get_count(); // gets the current reference count.

	};

}
