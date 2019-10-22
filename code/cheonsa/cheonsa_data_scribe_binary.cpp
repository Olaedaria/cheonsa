#include "cheonsa_data_scribe_binary.h"
#include "cheonsa_debug.h"

namespace cheonsa
{

	static const sint32_c flip_buffer_size = 8;

	void_c data_scribe_binary_c::_load_straight( void_c * const data, uint32_c size )
	{
		cheonsa_assert( size > 0 && size <= flip_buffer_size );
		_stream->load( data, size );
	}

	void_c data_scribe_binary_c::_load_switched( void_c * const data, uint32_c size )
	{
		cheonsa_assert( size > 0 && size <= flip_buffer_size );
		uint8_c * data_bytes = reinterpret_cast< uint8_c * >( data );
		uint8_c data_bytes_copy[ flip_buffer_size ];
		_stream->load( data_bytes_copy, size );
		for ( uint32_c i = 0; i < size; i++ )
		{
			data_bytes[ i ] = data_bytes_copy[ size - i - 1 ];
		}
	}

	void_c data_scribe_binary_c::_save_straight( void_c const * const data, uint32_c size )
	{
		cheonsa_assert( size > 0 && size <= flip_buffer_size );
		_stream->save( data, size );
	}

	void_c data_scribe_binary_c::_save_switched( void_c const * const data, uint32_c size )
	{
		cheonsa_assert( size > 0 && size <= flip_buffer_size );
		uint8_c const * const data_bytes = reinterpret_cast< uint8_c const * const >( data );
		uint8_c data_bytes_copy[ flip_buffer_size ];
		for ( uint32_c i = 0; i < size; i++ )
		{
			data_bytes_copy[ i ] = data_bytes[ size - i - 1 ];
		}
		_stream->save( data_bytes_copy, size );
	}

	data_scribe_binary_c::data_scribe_binary_c()
		: _stream( nullptr )
		, _endianness( data_get_native_endianness() )
		, _load_function( nullptr )
		, _save_function( nullptr )
	{
	}

	void_c data_scribe_binary_c::open( data_stream_c * stream, data_endianness_e endianness )
	{
		cheonsa_assert( stream );
		_stream = stream;
		_endianness = endianness;
		if ( endianness == data_get_native_endianness() )
		{
			_load_function = & data_scribe_binary_c::_load_straight;
			_save_function = & data_scribe_binary_c::_save_straight;
		}
		else
		{
			_load_function = & data_scribe_binary_c::_load_switched;
			_save_function = & data_scribe_binary_c::_save_switched;
		}
	}

	void_c data_scribe_binary_c::close()
	{
		cheonsa_assert( _stream );
		_stream = nullptr;
		_endianness = data_get_native_endianness();

	}


	data_stream_c * data_scribe_binary_c::get_stream()
	{
		return _stream;
	}

	void_c data_scribe_binary_c::set_stream( data_stream_c * stream )
	{
		cheonsa_assert( stream );
		_stream = stream;
	}

	data_endianness_e data_scribe_binary_c::get_endianness()
	{
		return _endianness;
	}

	void_c data_scribe_binary_c::set_endianness( data_endianness_e endianness )
	{
		_endianness = endianness;
	}

	void_c data_scribe_binary_c::load_generic( void_c * const value, sint32_c value_size )
	{
		( this->*_load_function )( value, value_size );
	}

	uint8_c data_scribe_binary_c::load_uint8()
	{
		uint8_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	sint8_c data_scribe_binary_c::load_sint8()
	{
		sint8_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	uint16_c data_scribe_binary_c::load_uint16()
	{
		uint16_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	sint16_c data_scribe_binary_c::load_sint16()
	{
		sint16_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	uint32_c data_scribe_binary_c::load_uint32()
	{
		uint32_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	sint32_c data_scribe_binary_c::load_sint32()
	{
		sint32_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	uint64_c data_scribe_binary_c::load_uint64()
	{
		uint64_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	sint64_c data_scribe_binary_c::load_sint64()
	{
		sint64_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	float32_c data_scribe_binary_c::load_float32()
	{
		float32_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	float64_c data_scribe_binary_c::load_float64()
	{
		float64_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	char8_c data_scribe_binary_c::load_char8()
	{
		char8_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	char16_c data_scribe_binary_c::load_char16()
	{
		char16_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	four_character_code_c data_scribe_binary_c::load_four_character_code()
	{
		four_character_code_c result;
		_stream->load( &result.characters[ 0 ], 1 );
		_stream->load( &result.characters[ 1 ], 1 );
		_stream->load( &result.characters[ 2 ], 1 );
		_stream->load( &result.characters[ 3 ], 1 );
		return result;
	}

	string8_c data_scribe_binary_c::load_string8()
	{
		string8_c result;
		uint16_c length = load_uint16();
		result.character_list.construct_mode_dynamic( length + 1, length + 1 );
		result.character_list[ length ] = 0;
		_stream->load( result.character_list.get_internal_array(), length );
		return result;
	}

	string16_c data_scribe_binary_c::load_string16()
	{
		string16_c result;
		uint16_c length = load_uint16();
		result.character_list.construct_mode_dynamic( length + 1, length + 1 );
		result.character_list[ length ] = 0;
		_stream->load( result.character_list.get_internal_array(), length * 2 );
		if ( _endianness != data_get_native_endianness() )
		{
			uint8_c * bytes = reinterpret_cast< uint8_c * >( result.character_list.get_internal_array() );
			for ( sint32_c i = 0; i < length; i++ )
			{
				uint8_c temp = bytes[ i * 2 ];
				bytes[ i * 2 ] = bytes[ i * 2 + 1 ];
				bytes[ i * 2 + 1 ] = temp;
			}
		}
		return result;
	}

	void_c data_scribe_binary_c::save_generic( uint8_c const * value, sint32_c value_size )
	{
		( this->*_save_function )( value, value_size );
	}

	void_c data_scribe_binary_c::save_uint8( uint8_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_sint8( sint8_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_uint16( uint16_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_sint16( sint16_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_uint32( uint32_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_sint32( sint32_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_uint64( uint64_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_sint64( sint64_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_float32( float32_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_float64( float64_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_char8( char8_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_char16( char16_c value )
	{
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_four_character_code( four_character_code_c value )
	{
		_stream->save( &value.characters[ 0 ], 1 );
		_stream->save( &value.characters[ 1 ], 1 );
		_stream->save( &value.characters[ 2 ], 1 );
		_stream->save( &value.characters[ 3 ], 1 );
	}

	void_c data_scribe_binary_c::save_string8( string8_c const & string )
	{
		sint32_c count = string.character_list.get_length() - 1;
		cheonsa_assert( count >= 0 && count < constants< uint16_c >::maximum() );
		save_uint16( static_cast< uint16_c >( count ) );
		_stream->save( string.character_list.get_internal_array(), count );
	}

	void_c data_scribe_binary_c::save_string16( string16_c const & string )
	{
		sint32_c count = string.character_list.get_length() - 1;
		cheonsa_assert( count >= 0 && count < constants< uint16_c >::maximum() );
		save_uint16( static_cast< uint16_c >( count ) );
		char16_c const * string_buffer = string.character_list.get_internal_array();
		if ( _endianness == data_get_native_endianness() )
		{
			_stream->save( string_buffer, count * 2 );
		}
		else
		{
			for ( sint32_c i = 0; i < count; i++ )
			{
				( this->*_save_function )( &string_buffer[ i ], 2 );
			}
		}
		
	}

}
