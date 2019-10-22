#pragma once

#include "cheonsa___build.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// these are templated dictionary hash functions, used to convert a value to a hash that can be used to index buckets in the dictionary.
	// we use these with the dictionary type to hard code default hash functions for certain types, so that we don't need to use function pointers.

	namespace fnv
	{
		// fnv-1a hash function.
		// magic numbers from http://www.isthe.com/chongo/tech/comp/fnv/
		#if defined( cheonsa_32_bit )
		uint_native_c const offset_basis = 2166136261;
		uint_native_c const prime = 16777619;
		#elif defined( cheonsa_64_bit )
		uint_native_c const offset_basis = 14695981039346656037;
		uint_native_c const prime = 1099511628211;
		#else
		#error unsupported.
		#endif
	}

	template< typename type_c > 
	struct hasher
	{
		static uint_native_c hash( type_c const & value )
		{
			// jenkins hash function.
			uint_native_c result = 5381;
			char8_c const * value_bytes = reinterpret_cast< char8_c const * >( &value );
			for ( sint32_c i = 0; i < sizeof( type_c ); i++ )
			{
				result = ( ( result << 5 ) + result ) + value_bytes[ i ];
			}
			return result;
		};
	};

	template<>
	struct hasher< string8_c >
	{
		static uint_native_c hash( string8_c const & value )
		{
			// fnv-1a hash function.
			uint_native_c result = fnv::offset_basis;
			char8_c const * c = value.character_list.get_internal_array();
			while ( *c )
			{
				result = result ^ static_cast< uint_native_c >( *c );
				result = result * fnv::prime;
				c++;
			}
			return result;

			//// jenkins hash function.
			//// https://en.wikipedia.org/wiki/Jenkins_hash_function
			//char8_c const * c = value.character_list.get_internal_array();
			//uint_native_c result = 5381;
			//while ( *c )
			//{
			//	result = ( ( result << 5 ) + result ) + static_cast< uint_native_c >( *c );
			//	c++;
			//}
			//return result;
		}
	};

	template<>
	struct hasher< string16_c >
	{
		static uint_native_c hash( string16_c const & value )
		{
			// fnv-1a hash function.
			uint_native_c result = fnv::offset_basis;
			char16_c const * c = value.character_list.get_internal_array();
			while ( *c )
			{
				result = result ^ static_cast< uint_native_c >( *c );
				result = result * fnv::prime;
				c++;
			}
			return result;
		}
	};

}
