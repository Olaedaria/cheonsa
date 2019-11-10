#include "cheonsa_safe_reference_count.h"
#include "cheonsa___build.h"
#if defined( cheonsa_platform_windows )
#include <windows.h>
#endif

namespace cheonsa
{

	safe_reference_count_c::safe_reference_count_c( sint32_c initial_count )
		: _count( initial_count )
	{
	}

	sint32_c safe_reference_count_c::increment()
	{
#if defined( cheonsa_platform_windows )
		//return InterlockedIncrement( &_count );

		sint32_c const volatile * c = &_count;
		while ( true )
		{
			sint32_c tmp = static_cast< sint32_c const volatile & >( *c );
			if ( tmp == 0 ) // if zero it means the thing should be dead.
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

	sint32_c safe_reference_count_c::decrement()
	{
#if defined( cheonsa_platform_windows )
		return InterlockedDecrement( &_count );
#else
#error not implemented.
#endif
	}

	sint32_c safe_reference_count_c::get_count()
	{
		return _count;
	}

}
