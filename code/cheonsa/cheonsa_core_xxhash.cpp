#include "cheonsa_core_xxhash.h"

#include "third_party/xxhash.h"

namespace cheonsa
{

	uint32_c xxhash32( void_c const * data, uint_native_c data_size )
	{
		return XXH32( static_cast< const void_c * >( data ), static_cast< size_t >( data_size ), 0 );
	}

	uint64_c xxhash64( void_c * data, uint_native_c data_size )
	{
		return XXH64( static_cast< const void_c * >( data ), static_cast< size_t >( data_size ), 0 );
	}

}
