#include "cheonsa_data_scribe_binary_property.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	data_scribe_binary_property_c::data_scribe_binary_property_c()
		: _scribe_binary( nullptr )
		, _loaded_key()
		, _loaded_type( 0 )
		, _loaded_type_count( 0 )
		, _loaded_value_buffer()
		, _loaded_value_list_length( 0 )
		, _loaded_value_list_size( 0 )
		, _loaded_value_string8()
		, _loaded_value_string16()
		, _list_instance_stack()
	{
	}

	void_c data_scribe_binary_property_c::open( data_scribe_binary_c * scribe_binary )
	{
		assert( _scribe_binary == nullptr );
		assert( scribe_binary != nullptr );
		_scribe_binary = scribe_binary;
	}

	void_c data_scribe_binary_property_c::close()
	{
		assert( _scribe_binary != nullptr );
		_scribe_binary = nullptr;
		_loaded_key = "";
		_loaded_type = 0;
		_loaded_type_count = 0;
		_loaded_value_list_length = 0;
		_loaded_value_list_size = 0;
		_loaded_value_string8 = "";
		_loaded_value_string16 = L"";
		_list_instance_stack.remove_all();
	}

	data_scribe_binary_c * data_scribe_binary_property_c::get_scribe_binary() const
	{
		return _scribe_binary;
	}

	boolean_c data_scribe_binary_property_c::load()
	{
		assert( _scribe_binary );

		if ( _list_instance_stack.get_length() && !_list_instance_stack[ _list_instance_stack.get_length() - 1 ].handled )
		{
			// skip to end of core_list_c to get to next property.
			if ( !_scribe_binary->get_stream()->set_position( _list_instance_stack[ _list_instance_stack.get_length() - 1 ].offset ) )
			{
				return false;
			}
			_list_instance_stack.remove( -1, 1 );
		}

		// check if done.
		if ( _scribe_binary->get_stream()->get_position() == _scribe_binary->get_stream()->get_size() )
		{
			return false;
		}

		// load type.
		_loaded_type;
		if ( !_scribe_binary->load_uint8( _loaded_type ) )
		{
			return false;
		}
		assert( _loaded_type >= data_type_e_list && _loaded_type <= data_type_e_float64 );

		// load type count.
		_loaded_type_count = 1;
		if ( _loaded_type >= data_type_e_uint8 && _loaded_type <= data_type_e_float64 )
		{
			if ( !_scribe_binary->load_uint8( _loaded_type_count ) )
			{
				return false;
			}
			assert( _loaded_type_count >= 1 && _loaded_type_count <= maximum_atomic_length );
		}

		// load key.
		string8_c loaded_key;
		if ( !_scribe_binary->load_string8( loaded_key ) )
		{
			_loaded_key = loaded_key;
			return false;
		}

		// load value.
		if ( _loaded_type >= data_type_e_uint8 && _loaded_type <= data_type_e_float64 )
		{
			sint32_c element_size = ops::get_data_type_size( static_cast< data_type_e >( _loaded_type ) );
			uint8_c * element_buffer = reinterpret_cast< uint8_c * >( _loaded_value_buffer );
			for ( sint32_c i = 0; i < _loaded_type_count; i++ )
			{
				if ( !_scribe_binary->load_generic( element_buffer, element_size ) )
				{
					return false;
				}
				element_buffer += element_size;
			}
		}
		else if ( _loaded_type == data_type_e_list )
		{
			if ( !_scribe_binary->load_sint32( _loaded_value_list_length ) )
			{
				return false;
			}
			if ( !_scribe_binary->load_sint32( _loaded_value_list_size ) )
			{
				return false;
			}
			_list_instance_stack.insert( -1, list_instance_c( false, _scribe_binary->get_stream()->get_position() + _loaded_value_list_size ) );
		}
		else if ( _loaded_type == data_type_e_string8 )
		{
			if ( !_scribe_binary->load_string8( _loaded_value_string8 ) )
			{
				return false;
			}
		}
		else if ( _loaded_type == data_type_e_string16 )
		{
			if ( !_scribe_binary->load_string16( _loaded_value_string16 ) )
			{
				return false;
			}
		}

		return true;
	}

	boolean_c data_scribe_binary_property_c::loaded_check( char8_c const * expected_key, data_type_e expected_type, uint8_c expected_length )
	{
		return ( _loaded_type == expected_type ) && ( _loaded_type_count == expected_length ) && ( _loaded_key == expected_key );
	}

	string16_c const & data_scribe_binary_property_c::loaded_key()
	{
		return _loaded_key;
	}

	data_type_e data_scribe_binary_property_c::loaded_type()
	{
		return static_cast< data_type_e >( _loaded_type );
	}

	uint8_c data_scribe_binary_property_c::loaded_type_length()
	{
		return _loaded_type_count;
	}

	void_c data_scribe_binary_property_c::loaded_list_enter()
	{
		_list_instance_stack[ _list_instance_stack.get_length() - 1 ].handled = true;
	}

	void_c data_scribe_binary_property_c::loaded_list_exit()
	{
		_list_instance_stack[ _list_instance_stack.get_length() - 1 ].handled = false;
	}

	sint32_c data_scribe_binary_property_c::loaded_list_length()
	{
		assert( _loaded_type == data_type_e_list );
		return _loaded_value_list_length;
	}

	sint32_c data_scribe_binary_property_c::loaded_list_size()
	{
		assert( _loaded_type == data_type_e_list );
		return _loaded_value_list_size;
	}

	string8_c const & data_scribe_binary_property_c::loaded_string8() const
	{
		assert( _loaded_type == data_type_e_string8 );
		return _loaded_value_string8;
	}

	string16_c const & data_scribe_binary_property_c::loaded_string16() const
	{
		assert( _loaded_type == data_type_e_string16 );
		return _loaded_value_string16;
	}

	uint8_c data_scribe_binary_property_c::loaded_uint8() const
	{
		assert( _loaded_type == data_type_e_uint8 && _loaded_type_count == 1 );
		return reinterpret_cast< uint8_c const * >( _loaded_value_buffer )[ 0 ];
	}

	sint8_c data_scribe_binary_property_c::loaded_sint8() const
	{
		assert( _loaded_type == data_type_e_sint8 && _loaded_type_count == 1 );
		return reinterpret_cast< sint8_c const * >( _loaded_value_buffer )[ 0 ];
	}

	uint16_c data_scribe_binary_property_c::loaded_uint16() const
	{
		assert( _loaded_type == data_type_e_uint16 && _loaded_type_count == 1 );
		return reinterpret_cast< uint16_c const * >( _loaded_value_buffer )[ 0 ];
	}

	sint16_c data_scribe_binary_property_c::loaded_sint16() const
	{
		assert( _loaded_type == data_type_e_sint16 && _loaded_type_count == 1 );
		return reinterpret_cast< sint16_c const * >( _loaded_value_buffer )[ 0 ];
	}

	uint32_c data_scribe_binary_property_c::loaded_uint32() const
	{
		assert( _loaded_type == data_type_e_uint32 && _loaded_type_count == 1 );
		return reinterpret_cast< uint32_c const * >( _loaded_value_buffer )[ 0 ];
	}

	sint32_c data_scribe_binary_property_c::loaded_sint32() const
	{
		assert( _loaded_type == data_type_e_sint32 && _loaded_type_count == 1 );
		return reinterpret_cast< sint32_c const * >( _loaded_value_buffer )[ 0 ];
	}

	uint64_c data_scribe_binary_property_c::loaded_uint64() const
	{
		assert( _loaded_type == data_type_e_uint64 && _loaded_type_count == 1 );
		return reinterpret_cast< uint64_c const * >( _loaded_value_buffer )[ 0 ];
	}

	sint64_c data_scribe_binary_property_c::loaded_sint64() const
	{
		assert( _loaded_type == data_type_e_sint64 && _loaded_type_count == 1 );
		return reinterpret_cast< sint64_c const * >( _loaded_value_buffer )[ 0 ];
	}

	float32_c data_scribe_binary_property_c::loaded_float32() const
	{
		assert( _loaded_type == data_type_e_float32 && _loaded_type_count == 1 );
		return reinterpret_cast< float32_c const * >( _loaded_value_buffer )[ 0 ];
	}

	float64_c data_scribe_binary_property_c::loaded_float64() const
	{
		assert( _loaded_type == data_type_e_float64 && _loaded_type_count == 1 );
		return reinterpret_cast< float64_c const * >( _loaded_value_buffer )[ 0 ];
	}

	uint8_c const * data_scribe_binary_property_c::loaded_uint8_buffer() const
	{
		assert( _loaded_type == data_type_e_uint8 );
		return reinterpret_cast< uint8_c const * >( _loaded_value_buffer );
	}

	sint8_c const * data_scribe_binary_property_c::loaded_sint8_buffer() const
	{
		assert( _loaded_type == data_type_e_sint8 );
		return reinterpret_cast< sint8_c const * >( _loaded_value_buffer );
	}

	uint16_c const * data_scribe_binary_property_c::loaded_uint16_buffer() const
	{
		assert( _loaded_type == data_type_e_uint16 );
		return reinterpret_cast< uint16_c const * >( _loaded_value_buffer );
	}

	sint16_c const * data_scribe_binary_property_c::loaded_sint16_buffer() const
	{
		assert( _loaded_type == data_type_e_sint16 );
		return reinterpret_cast< sint16_c const * >( _loaded_value_buffer );
	}

	uint32_c const * data_scribe_binary_property_c::loaded_uint32_buffer() const
	{
		assert( _loaded_type == data_type_e_uint32 );
		return reinterpret_cast< uint32_c const * >( _loaded_value_buffer );
	}

	sint32_c const * data_scribe_binary_property_c::loaded_sint32_buffer() const
	{
		assert( _loaded_type == data_type_e_sint32 );
		return reinterpret_cast< sint32_c const * >( _loaded_value_buffer );
	}

	uint64_c const * data_scribe_binary_property_c::loaded_uint64_buffer() const
	{
		assert( _loaded_type == data_type_e_uint64 );
		return reinterpret_cast< uint64_c const * >( _loaded_value_buffer );
	}

	sint64_c const * data_scribe_binary_property_c::loaded_sint64_buffer() const
	{
		assert( _loaded_type == data_type_e_sint64 );
		return reinterpret_cast< sint64_c const * >( _loaded_value_buffer );
	}

	float32_c const * data_scribe_binary_property_c::loaded_float32_buffer() const
	{
		assert( _loaded_type == data_type_e_float32 );
		return reinterpret_cast< float32_c const * >( _loaded_value_buffer );
	}

	float64_c const * data_scribe_binary_property_c::loaded_float64_buffer() const
	{
		assert( _loaded_type == data_type_e_float64 );
		return reinterpret_cast< float64_c const * >( _loaded_value_buffer );
	}

	boolean_c data_scribe_binary_property_c::save_list_begin( char8_c const * key )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_list ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint32( 0 ) ) // number of items in the list, we will write the final value when save_list_end() is called.
		{
			return false;
		}
		if ( !_scribe_binary->save_uint32( 0 ) ) // size in bytes of the list, we will write the final value when save_list_end() is called.
		{
			return false;
		}
		_list_instance_stack.insert( -1, list_instance_c( true, _scribe_binary->get_stream()->get_position() ) );
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_list_end( sint32_c length )
	{
		assert( _scribe_binary );
		uint32_c list_offset = _list_instance_stack[ _list_instance_stack.get_length() - 1 ].offset;
		_list_instance_stack.remove( -1, 1 );
		uint32_c current_offset = _scribe_binary->get_stream()->get_position();
		if ( !_scribe_binary->get_stream()->set_position( list_offset - 8 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_sint32( length ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_sint32( current_offset - list_offset ) )
		{
			return false;
		}
		if ( !_scribe_binary->get_stream()->set_position( current_offset ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_string8( char8_c const * key, string8_c const & value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( data_type_e_string8 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_string16( char8_c const * key, string16_c const & value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( data_type_e_string16 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string16( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_uint8( char8_c const * key, uint8_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_uint8 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_sint8( char8_c const * key, sint8_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_sint8 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_sint8( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_uint16( char8_c const * key, uint16_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_uint16 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint16( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_sint16( char8_c const * key, sint16_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_sint16 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_sint16( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_uint32( char8_c const * key, uint32_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_uint32 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint32( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_sint32( char8_c const * key, sint32_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_sint32 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_sint32( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_uint64( char8_c const * key, uint64_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_uint64 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint64( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_sint64( char8_c const * key, sint64_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_uint64 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_sint64( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_float32( char8_c const * key, float32_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_float32 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_float32( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_float64( char8_c const * key, float64_c value )
	{
		assert( _scribe_binary );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_float64 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( 1 ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_float64( value ) )
		{
			return false;
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_uint8_buffer( char8_c const * key, uint8_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_uint8 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_uint8( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_sint8_buffer( char8_c const * key, sint8_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_sint8 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_sint8( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_uint16_buffer( char8_c const * key, uint16_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_uint16 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_uint16( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_sint16_buffer( char8_c const * key, sint16_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_sint16 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_sint16( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_uint32_buffer( char8_c const * key, uint32_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_uint32 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_uint32( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_sint32_buffer( char8_c const * key, sint32_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_sint32 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_sint32( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_uint64_buffer( char8_c const * key, uint64_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_uint64 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_uint64( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_sint64_buffer( char8_c const * key, sint64_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_sint64 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_sint64( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_float32_buffer( char8_c const * key, float32_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_float32 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_float32( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_float64_buffer( char8_c const * key, float64_c const * value, sint32_c length )
	{
		assert( _scribe_binary );
		assert( length > 0 && length < maximum_atomic_length );
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( data_type_e_float64 ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_uint8( static_cast< uint8_c >( length ) ) )
		{
			return false;
		}
		if ( !_scribe_binary->save_string8( string8_c( core_list_mode_e_static, key ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < length; i++ )
		{
			if ( !_scribe_binary->save_float64( value[ i ] ) )
			{
				return false;
			}
		}
		return true;
	}

	boolean_c data_scribe_binary_property_c::save_end()
	{
		assert( _scribe_binary );
		return _scribe_binary->save_uint8( 0 );
	}

}
