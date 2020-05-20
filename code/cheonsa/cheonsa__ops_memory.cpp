#include "cheonsa__ops_memory.h"
#include "third_party/xxhash.h"
#include <cstdlib>

namespace cheonsa
{
	namespace ops
	{

		void_c * memory_aligned_allocate( sint32_c size, sint32_c alignment )
		{
			return _aligned_malloc( size, alignment );
		}

		void_c memory_aligned_deallocate( void_c * block )
		{
			_aligned_free( block );
		}

		void_c * memory_allocate( sint32_c size )
		{
			assert( size > 0 );
			return std::malloc( size );
		}

		void_c * memory_reallocate( void_c * block, sint32_c new_size )
		{
			assert( new_size > 0 );
			return std::realloc( block, new_size );
		}

		void_c memory_deallocate( void_c * block )
		{
			std::free( block );
		}

		void_c memory_move( void_c const * buffer_source, void_c * buffer_destination, sint32_c buffer_size )
		{
			std::memmove( buffer_destination, buffer_source, buffer_size );
		}

		void_c memory_copy( void_c const * buffer_source, void_c * buffer_destination, sint32_c buffer_size )
		{
			std::memcpy( buffer_destination, buffer_source, buffer_size );
		}

		void_c memory_zero( void_c * buffer, sint32_c buffer_size )
		{
			std::memset( buffer, 0, buffer_size );
		}

		boolean_c memory_is_zero( void_c const * buffer, sint32_c buffer_size )
		{
			uint8_c const * bytes = reinterpret_cast< uint8_c const * >( buffer );
			for ( sint32_c i = 0; i < buffer_size; i++ )
			{
				if ( bytes[ i ] != 0 )
				{
					return false;
				}
			}
			return true;

			/*
			uint_native_c bytes_before = reinterpret_cast< uint_native_c >( buffer ) % sizeof( uint_native_c );
			if ( bytes_before != 0 )
			{
				bytes_before = sizeof( uint_native_c ) - bytes_before;
			}
			uint_native_c bytes_after = ( reinterpret_cast< uint_native_c >( buffer ) + buffer_size ) % sizeof( uint_native_c );
			uint_native_c natives_in_the_middle = ( reinterpret_cast< uint_native_c >( buffer ) + bytes_before + buffer_size - bytes_after - reinterpret_cast< uint_native_c >( buffer ) ) / sizeof( uint_native_c );
			uint8_c const * buffer_uint8 = reinterpret_cast< uint8_c const * >( buffer );
			for ( uint_native_c i = 0; i < bytes_before; i++ )
			{
				if ( buffer_uint8[ i ] != 0 )
				{
					return false;
				}
			}
			assert( reinterpret_cast< uint_native_c >( buffer_uint8 + bytes_before ) % sizeof( uint_native_c ) == 0 );
			uint_native_c const * buffer_uint_native = reinterpret_cast< uint_native_c const * >( buffer_uint8 + bytes_before );
			for ( uint_native_c i = 0; i < natives_in_the_middle; i++ )
			{
				if( buffer_uint_native[ i ] != 0 )
				{
					return false;
				}
			}
			buffer_uint8 = reinterpret_cast< uint8_c const * >( buffer_uint_native + natives_in_the_middle );
			for ( uint_native_c i = 0; i < bytes_after; i++ )
			{
				if ( buffer_uint8[ i ] != 0 )
				{
					return false;
				}
			}
			return true;
			*/
		}

		boolean_c memory_compare( void_c const * buffer_a, void_c const * buffer_b, sint32_c byte_count )
		{
			return memcmp( buffer_a, buffer_b, byte_count ) == 0;
		}

		sint32_c get_data_type_size( data_type_e type )
		{
			switch( type )
			{
			case data_type_e_uint8:
			case data_type_e_sint8:
				return 1;
			case data_type_e_uint16:
			case data_type_e_sint16:
				return 2;
			case data_type_e_uint32:
			case data_type_e_sint32:
			case data_type_e_float32:
				return 4;
			case data_type_e_uint64:
			case data_type_e_sint64:
			case data_type_e_float64:
				return 8;
			}
			return 0;
		}

		byte_order_e get_native_byte_order()
		{
			const union { uint32_c u; uint8_c c[ 4 ]; } probe = { 1 };
			return probe.c[ 0 ] != 0 ? byte_order_e_little : byte_order_e_big;
		}

		void_c flip_byte_order( void_c * buffer, sint32_c buffer_size )
		{
			assert( buffer_size > 0 && buffer_size < 16 );
			uint8_c * data_bytes = reinterpret_cast< uint8_c * >( buffer );
			for ( sint32_c i = 0; i < buffer_size; i++ )
			{
				uint8_c t = data_bytes[ i ];
				data_bytes[ i ] = data_bytes[ buffer_size - i - 1 ];
				data_bytes[ buffer_size - i - 1 ] = t;
			}
		}

		uint32_c xxhash32( void_c const * data, uint_native_c data_size )
		{
			return XXH32( static_cast< const void_c * >( data ), static_cast< size_t >( data_size ), 0 );
		}

		uint64_c xxhash64( void_c const * data, uint_native_c data_size )
		{
			return XXH64( static_cast< const void_c * >( data ), static_cast< size_t >( data_size ), 0 );
		}

	}
}
