#pragma once

#include "cheonsa___build.h"
#include "cheonsa__ops.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// these are templated dictionary hash functions, used to convert a value to a hash that can be used to index buckets in the dictionary.
	// there was a time when i used function pointers to define custom hash functions, but this way is easier but less flexible.

	template< typename type_c > 
	struct hasher
	{
		static uint_native_c hash( type_c const & value )
		{
			return ops::xxhash_native( &value, sizeof( type_c ) );
		};
	};

	template<>
	struct hasher< string8_c >
	{
		static uint_native_c hash( string8_c const & value )
		{
			return ops::xxhash_native( value.character_list.get_internal_array(), value.character_list.get_internal_array_size_used() );
		}
	};

	template<>
	struct hasher< string16_c >
	{
		static uint_native_c hash( string16_c const & value )
		{
			return ops::xxhash_native( value.character_list.get_internal_array(), value.character_list.get_internal_array_size_used() );
		}
	};

}
