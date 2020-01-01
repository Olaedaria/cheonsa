#include "cheonsa_core_safe_reference_counter.h"
#include "cheonsa___build.h"
#if defined( cheonsa_platform_windows )
#include <windows.h>
#endif

namespace cheonsa
{

	core_safe_reference_counter_c::core_safe_reference_counter_c( sint32_c initial_count )
		: _count( initial_count )
	{
	}

	sint32_c core_safe_reference_counter_c::increment()
	{
#if defined( cheonsa_platform_windows )
		sint32_c const volatile * c = &_count;
		while ( true )
		{
			sint32_c tmp = static_cast< sint32_c const volatile & >( *c ); // code from godot, but why even bother with the static cast?
			if ( tmp == 0 ) // if 0 it means the thing was released by another thread already.
			{
				return 0;
			}
			if ( InterlockedCompareExchange( (LONG volatile *)(c), tmp + 1, tmp ) == tmp )
			{
				return tmp + 1;
			}
		}
#else
#error not implemented.
#endif
	}

	sint32_c core_safe_reference_counter_c::decrement()
	{
#if defined( cheonsa_platform_windows )
		return InterlockedDecrement( &_count );
#else
#error not implemented.
#endif
	}

	sint32_c core_safe_reference_counter_c::get_count()
	{
		return _count;
	}

}
