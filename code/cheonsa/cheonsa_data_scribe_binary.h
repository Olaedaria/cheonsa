#pragma once

#include "cheonsa__types.h"
#include "cheonsa_data_stream.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// used to read and write binary data to and from a stream.
	// all load and save functions may throw an exception if the operation fails for some reason.
	class data_scribe_binary_c
	{
	private:
		data_stream_c * _stream;
		byte_order_e _byte_order;

		boolean_c ( data_scribe_binary_c::*_load_function )( void_c * const, sint32_c );
		boolean_c ( data_scribe_binary_c::*_save_function )( void_c const * const, sint32_c );

		boolean_c _load_straight( void_c * const data, sint32_c size ); // preserves byte order.
		boolean_c _save_straight( void_c const * const data, sint32_c size ); // preserves byte order.
		boolean_c _load_flipped( void_c * const data, sint32_c size ); // reverses byte order.
		boolean_c _save_flipped( void_c const * const data, sint32_c size ); // reverses byte order.

	public:
		data_scribe_binary_c();

		data_stream_c * get_stream();
		void_c set_stream( data_stream_c * stream );

		byte_order_e get_byte_order();
		void_c set_byte_order( byte_order_e byte_order );

		boolean_c load_generic( void_c * const value, sint32_c value_size ); // performs byte-order swizzling if needed.
		boolean_c load_uint8( uint8_c & value );
		boolean_c load_sint8( sint8_c & value );
		boolean_c load_uint16( uint16_c & value );
		boolean_c load_sint16( sint16_c & value );
		boolean_c load_uint32( uint32_c & value );
		boolean_c load_sint32( sint32_c & value );
		boolean_c load_uint64( uint64_c & value );
		boolean_c load_sint64( sint64_c & value );
		boolean_c load_float32( float32_c & value );
		boolean_c load_float64( float64_c & value );
		boolean_c load_char8( char8_c & value );
		boolean_c load_char16( char16_c & value );
		boolean_c load_four_character_code( uint32_c & value ); // always saves as big endian regardless of currently configured byte order.
		boolean_c load_string8( string8_c & value ); // loads a uint16_c length prefixed ascii or utf-8 encoded string.
		boolean_c load_string16( string16_c & value ); // loads a uint16_c length prefixed char16_c string.

		boolean_c save_generic( uint8_c const * value, sint32_c value_size ); // saves a plain old data value, performs byte-order swizzling if needed.
		boolean_c save_uint8( uint8_c value );
		boolean_c save_sint8( sint8_c value );
		boolean_c save_uint16( uint16_c value );
		boolean_c save_sint16( sint16_c value );
		boolean_c save_uint32( uint32_c value );
		boolean_c save_sint32( sint32_c value );
		boolean_c save_uint64( uint64_c value );
		boolean_c save_sint64( sint64_c value );
		boolean_c save_float32( float32_c value );
		boolean_c save_float64( float64_c value );
		boolean_c save_char8( char8_c value );
		boolean_c save_char16( char16_c value );
		boolean_c save_four_character_code( uint32_c value );
		boolean_c save_string8( string8_c const & string ); // saves a uint16_c length prefixed char8_c string (not null terminated).
		boolean_c save_string16( string16_c const & string ); // saves a uint16_c length prefixed char16_c string (not null terminated).

	};

}
