#pragma once

#include "cheonsa__types.h"
#include "cheonsa_data_types.h"

namespace cheonsa
{
	namespace ops
	{

		void_c * memory_aligned_allocate( sint32_c size, sint32_c alignment );
		void_c memory_aligned_deallocate( void_c * block );
		void_c * memory_allocate( sint32_c size );
		void_c * memory_reallocate( void_c * block, sint32_c new_size ); // reallocates or resizes a block of memory previously allocated with memory_allocate() or memory_reallocate().
		void_c memory_deallocate( void_c * block );
		void_c memory_move( void_c const * buffer_source, void_c * buffer_destination, sint32_c buffer_size );
		void_c memory_copy( void_c const * buffer_source, void_c * buffer_destination, sint32_c buffer_size );
		void_c memory_zero( void_c * buffer, sint32_c buffer_size );
		boolean_c memory_is_zero( void_c const * buffer, sint32_c buffer_size );
		boolean_c memory_compare( void_c const * buffer_a, void_c const * buffer_b, sint32_c byte_count );

		sint32_c get_data_type_size( data_type_e type ); // gets the size in bytes of the given type. returns a valid value for all numeric types. returns 0 for anything else.
		byte_order_e get_native_byte_order(); // gets the native endianess of the cpu.
		void_c flip_byte_order( void_c * buffer, sint32_c buffer_size ); // flips the byte order of the data in buffer.

		uint32_c xxhash32( void_c const * data, uint_native_c data_size );
		uint64_c xxhash64( void_c const * data, uint_native_c data_size );

	}
}
