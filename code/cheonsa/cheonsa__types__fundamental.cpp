#include "cheonsa__types__fundamental.h"

namespace cheonsa
{

	uint32_c fourcc( char8_c const * chars, boolean_c flipped )
	{
		return
			static_cast< uint32_c >( chars[ flipped ? 3 : 0 ] ) << 24 |
			static_cast< uint32_c >( chars[ flipped ? 2 : 1 ] ) << 16 |
			static_cast< uint32_c >( chars[ flipped ? 1 : 2 ] ) << 8 |
			static_cast< uint32_c >( chars[ flipped ? 0 : 3 ] );
	}

}
