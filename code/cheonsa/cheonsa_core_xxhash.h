#pragma once

#include "cheonsa_base_types.h"

namespace cheonsa
{

	// intended to be used for error checking of data in packets sent/received over network.
	uint32_c xxhash32( void_c const * data, uint_native_c data_size );
	// intended to be used for error checking of data in packets sent/received over network.
	uint64_c xxhash64( void_c * data, uint_native_c data_size );

}
