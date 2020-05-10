#include "cheonsa_data_scribe_binary.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	static const sint32_c flip_buffer_size = 8;

	boolean_c data_scribe_binary_c::_load_straight( void_c * const data, sint32_c size )
	{
		assert( _stream != nullptr && size > 0 && size <= flip_buffer_size );
		boolean_c result = _stream->load( data, size );
		_encountered_error |= !result;
		return result;
	}

	boolean_c data_scribe_binary_c::_save_straight( void_c const * const data, sint32_c size )
	{
		assert( _stream != nullptr && size > 0 && size <= flip_buffer_size );
		boolean_c result = _stream->save( data, size );
		_encountered_error |= !result;
		return result;
	}

	boolean_c data_scribe_binary_c::_load_flipped( void_c * const data, sint32_c size )
	{
		assert( _stream != nullptr && size > 0 && size <= flip_buffer_size );
		uint8_c * data_bytes = reinterpret_cast< uint8_c * >( data );
		uint8_c data_bytes_copy[ flip_buffer_size ];
		if ( !_stream->load( data_bytes_copy, size ) )
		{
			_encountered_error = true;
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
		boolean_c result = _stream->save( data_bytes_copy, size );
		_encountered_error |= !result;
		return result;
	}

	data_scribe_binary_c::data_scribe_binary_c()
		: _stream( nullptr )
		, _byte_order( ops::get_native_byte_order() )
		, _load_function( &data_scribe_binary_c::_load_straight )
		, _save_function( &data_scribe_binary_c::_save_straight )
		, _encountered_error( false )
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
		boolean_c result = _stream->load( value.character_list.get_internal_array(), length );
		_encountered_error |= !result;
		return result;
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
			_encountered_error = true;
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
		boolean_c result = _stream->save( &value, 4 );
		_encountered_error |= !result;
		return result;
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
		boolean_c result = _stream->save( value.character_list.get_internal_array(), count );
		_encountered_error |= !result;
		return result;
	}

	boolean_c data_scribe_binary_c::save_string16( string16_c const & value )
	{
		assert( _stream != nullptr );
		sint32_c length = value.character_list.get_length() - 1;
		assert( length >= 0 && length < constants< uint16_c >::maximum() );
		if ( !save_uint16( static_cast< uint16_c >( length ) ) )
		{
			_encountered_error = true;
			return false;
		}
		if ( _byte_order == ops::get_native_byte_order() )
		{
			boolean_c result = _stream->save( value.character_list.get_internal_array(), length * 2 );
			_encountered_error |= !result;
			return result;
		}
		else
		{
			string16_c flipped_string = string16_c( core_list_mode_e_dynamic, value.character_list.get_internal_array(), value.character_list.get_length() );
			uint8_c * bytes = reinterpret_cast< uint8_c * >( flipped_string.character_list.get_internal_array() );
			for ( sint32_c i = 0; i < length; i++ )
			{
				uint8_c temp = bytes[ i * 2 ];
				bytes[ i * 2 ] = bytes[ i * 2 + 1 ];
				bytes[ i * 2 + 1 ] = temp;
			}
			boolean_c result = _stream->save( bytes, length * 2 );
			_encountered_error |= !result;
			return result;
		}
	}

	boolean_c data_scribe_binary_c::encountered_error() const
	{
		return _encountered_error;
	}

	void_c data_scribe_binary_c::clear_error()
	{
		_encountered_error = false;
	}

}
