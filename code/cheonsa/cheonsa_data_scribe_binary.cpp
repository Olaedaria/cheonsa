#include "cheonsa_data_scribe_binary.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	static const sint32_c flip_buffer_size = 8;

	boolean_c data_scribe_binary_c::_load_straight( void_c * const data, sint32_c size )
	{
		assert( _stream != nullptr && size > 0 && size <= flip_buffer_size );
		return _stream->load( data, size );
	}

	boolean_c data_scribe_binary_c::_save_straight( void_c const * const data, sint32_c size )
	{
		assert( _stream != nullptr && size > 0 && size <= flip_buffer_size );
		return _stream->save( data, size );
	}

	boolean_c data_scribe_binary_c::_load_flipped( void_c * const data, sint32_c size )
	{
		assert( _stream != nullptr && size > 0 && size <= flip_buffer_size );
		uint8_c * data_bytes = reinterpret_cast< uint8_c * >( data );
		uint8_c data_bytes_copy[ flip_buffer_size ];
		if ( !_stream->load( data_bytes_copy, size ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < size; i++ )
		{
			data_bytes[ i ] = data_bytes_copy[ size - i - 1 ];
		}
		return true;
	}

	boolean_c data_scribe_binary_c::_save_flipped( void_c const * const data, sint32_c size )
	{
		assert( _stream != nullptr && size > 0 && size <= flip_buffer_size );
		uint8_c const * const data_bytes = reinterpret_cast< uint8_c const * const >( data );
		uint8_c data_bytes_copy[ flip_buffer_size ];
		for ( sint32_c i = 0; i < size; i++ )
		{
			data_bytes_copy[ i ] = data_bytes[ size - i - 1 ];
		}
		return _stream->save( data_bytes_copy, size );
	}

	data_scribe_binary_c::data_scribe_binary_c()
		: _stream( nullptr )
		, _byte_order( ops::get_native_byte_order() )
		, _load_function( &data_scribe_binary_c::_load_straight )
		, _save_function( &data_scribe_binary_c::_save_straight )
	{
	}

	data_stream_c * data_scribe_binary_c::get_stream()
	{
		return _stream;
	}

	void_c data_scribe_binary_c::set_stream( data_stream_c * stream )
	{
		_stream = stream;
	}

	byte_order_e data_scribe_binary_c::get_byte_order()
	{
		return _byte_order;
	}

	void_c data_scribe_binary_c::set_byte_order( byte_order_e byte_order )
	{
		_byte_order = byte_order;
		if ( _byte_order == ops::get_native_byte_order() )
		{
			_load_function = &data_scribe_binary_c::_load_straight;
			_save_function = &data_scribe_binary_c::_save_straight;
		}
		else
		{
			_load_function = &data_scribe_binary_c::_load_flipped;
			_save_function = &data_scribe_binary_c::_save_flipped;
		}
	}

	/*
	void_c data_scribe_binary_c::load_generic( void_c * const value, sint32_c value_size )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		( this->*_load_function )( value, value_size );
	}

	uint8_c data_scribe_binary_c::load_uint8()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		uint8_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	sint8_c data_scribe_binary_c::load_sint8()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		sint8_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	uint16_c data_scribe_binary_c::load_uint16()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		uint16_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	sint16_c data_scribe_binary_c::load_sint16()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		sint16_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	uint32_c data_scribe_binary_c::load_uint32()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		uint32_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	sint32_c data_scribe_binary_c::load_sint32()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		sint32_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	uint64_c data_scribe_binary_c::load_uint64()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		uint64_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	sint64_c data_scribe_binary_c::load_sint64()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		sint64_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	float32_c data_scribe_binary_c::load_float32()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		float32_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	float64_c data_scribe_binary_c::load_float64()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		float64_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	char8_c data_scribe_binary_c::load_char8()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		char8_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	char16_c data_scribe_binary_c::load_char16()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		char16_c result;
		( this->*_load_function )( & result, sizeof( result ) );
		return result;
	}

	uint32_c data_scribe_binary_c::load_four_character_code()
	{
		assert( _stream != nullptr );
		uint32_c result = 0;
		_stream->load( &result, 4 );
		return result;
	}

	string8_c data_scribe_binary_c::load_string8()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		string8_c result;
		uint16_c length = load_uint16();
		result.character_list.construct_mode_dynamic( length + 1, length + 1 );
		result.character_list[ length ] = 0;
		_stream->load( result.character_list.get_internal_array(), length );
		return result;
	}

	string16_c data_scribe_binary_c::load_string16()
	{
		assert( _stream != nullptr && _load_function != nullptr );
		string16_c result;
		uint16_c length = load_uint16();
		result.character_list.construct_mode_dynamic( length + 1, length + 1 );
		result.character_list[ length ] = 0;
		_stream->load( result.character_list.get_internal_array(), length * 2 );
		if ( _byte_order != ops::get_native_byte_order() )
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
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( value, value_size );
	}

	void_c data_scribe_binary_c::save_uint8( uint8_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_sint8( sint8_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_uint16( uint16_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_sint16( sint16_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_uint32( uint32_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_sint32( sint32_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_uint64( uint64_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_sint64( sint64_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_float32( float32_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_float64( float64_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_char8( char8_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_char16( char16_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		( this->*_save_function )( &value, sizeof( value ) );
	}

	void_c data_scribe_binary_c::save_four_character_code( uint32_c value )
	{
		assert( _stream != nullptr );
		_stream->save( &value, 4 );
	}

	void_c data_scribe_binary_c::save_string8( string8_c const & string )
	{
		assert( _stream != nullptr );
		sint32_c count = string.character_list.get_length() - 1;
		assert( count >= 0 && count < constants< uint16_c >::maximum() );
		save_uint16( static_cast< uint16_c >( count ) );
		_stream->save( string.character_list.get_internal_array(), count );
	}

	void_c data_scribe_binary_c::save_string16( string16_c const & string )
	{
		assert( _stream != nullptr );
		sint32_c count = string.character_list.get_length() - 1;
		assert( count >= 0 && count < constants< uint16_c >::maximum() );
		save_uint16( static_cast< uint16_c >( count ) );
		char16_c const * string_buffer = string.character_list.get_internal_array();
		if ( _byte_order == ops::get_native_byte_order() )
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
	*/

	boolean_c data_scribe_binary_c::load_generic( void_c * const value, sint32_c value_size )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( value, value_size );
	}

	boolean_c data_scribe_binary_c::load_uint8( uint8_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_sint8( sint8_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_uint16( uint16_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_sint16( sint16_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_uint32( uint32_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_sint32( sint32_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_uint64( uint64_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_sint64( sint64_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_float32( float32_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_float64( float64_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_char8( char8_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_char16( char16_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		return ( this->*_load_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::load_four_character_code( uint32_c & value )
	{
		assert( _stream != nullptr );
		return _stream->load( &value, 4 );
	}

	boolean_c data_scribe_binary_c::load_string8( string8_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		uint16_c length = 0;
		if ( !load_uint16( length ) )
		{
			return false;
		}
		value.character_list.construct_mode_dynamic( length + 1, length + 1 );
		value.character_list[ length ] = 0;
		return _stream->load( value.character_list.get_internal_array(), length );
	}

	boolean_c data_scribe_binary_c::load_string16( string16_c & value )
	{
		assert( _stream != nullptr && _load_function != nullptr );
		uint16_c length = 0;
		if ( !load_uint16( length ) )
		{
			return false;
		}
		value.character_list.construct_mode_dynamic( length + 1, length + 1 );
		value.character_list[ length ] = 0;
		if ( !_stream->load( value.character_list.get_internal_array(), length * 2 ) )
		{
			return false;
		}
		if ( _byte_order != ops::get_native_byte_order() )
		{
			uint8_c * bytes = reinterpret_cast< uint8_c * >( value.character_list.get_internal_array() );
			for ( sint32_c i = 0; i < length; i++ )
			{
				uint8_c temp = bytes[ i * 2 ];
				bytes[ i * 2 ] = bytes[ i * 2 + 1 ];
				bytes[ i * 2 + 1 ] = temp;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_c::save_generic( uint8_c const * value, sint32_c value_size )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( value, value_size );
	}

	boolean_c data_scribe_binary_c::save_uint8( uint8_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_sint8( sint8_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_uint16( uint16_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_sint16( sint16_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_uint32( uint32_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_sint32( sint32_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_uint64( uint64_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_sint64( sint64_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_float32( float32_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_float64( float64_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_char8( char8_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_char16( char16_c value )
	{
		assert( _stream != nullptr && _save_function != nullptr );
		return ( this->*_save_function )( &value, sizeof( value ) );
	}

	boolean_c data_scribe_binary_c::save_four_character_code( uint32_c value )
	{
		assert( _stream != nullptr );
		return _stream->save( &value, 4 );
	}

	boolean_c data_scribe_binary_c::save_string8( string8_c const & value )
	{
		assert( _stream != nullptr );
		sint32_c count = value.character_list.get_length() - 1;
		assert( count >= 0 && count < constants< uint16_c >::maximum() );
		if ( !save_uint16( static_cast< uint16_c >( count ) ) )
		{
			return false;
		}
		return _stream->save( value.character_list.get_internal_array(), count );
	}

	boolean_c data_scribe_binary_c::save_string16( string16_c const & value )
	{
		assert( _stream != nullptr );
		sint32_c length = value.character_list.get_length() - 1;
		assert( length >= 0 && length < constants< uint16_c >::maximum() );
		if ( !save_uint16( static_cast< uint16_c >( length ) ) )
		{
			return false;
		}
		if ( _byte_order == ops::get_native_byte_order() )
		{
			return _stream->save( value.character_list.get_internal_array(), length * 2 );
		}
		else
		{
			string16_c flipped_string = string16_c( mode_e_dynamic, value.character_list.get_internal_array(), value.character_list.get_length() );
			uint8_c * bytes = reinterpret_cast< uint8_c * >( flipped_string.character_list.get_internal_array() );
			for ( sint32_c i = 0; i < length; i++ )
			{
				uint8_c temp = bytes[ i * 2 ];
				bytes[ i * 2 ] = bytes[ i * 2 + 1 ];
				bytes[ i * 2 + 1 ] = temp;
			}
			return _stream->save( bytes, length * 2 );
		}
	}

}
