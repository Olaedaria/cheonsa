#include "cheonsa_script_value.h"
#include "cheonsa_debug.h"

namespace cheonsa
{

	script_value_c::script_value_c()
		: _type( type_e_none )
		, _number_value( 0.0f )
		, _string_value( nullptr )
	{
	}

	script_value_c::script_value_c( const script_value_c & other )
		: _type( type_e_none )
		, _number_value( 0.0f )
		, _string_value( nullptr )
	{
		*this = other;
	}

	script_value_c::~script_value_c()
	{
		if ( _string_value )
		{
			delete _string_value;
			_string_value = nullptr;
		}
	}

	script_value_c::type_e script_value_c::get_type() const
	{
		return _type;
	}

	void_c script_value_c::set_none_value()
	{
		_type = type_e_none;
		_number_value = 0.0f;
		if ( _string_value )
		{
			delete _string_value;
			_string_value = nullptr;
		}
	}

	float32_c script_value_c::get_number_value() const
	{
		cheonsa_assert( _type == type_e_number );
		return _number_value;
	}

	void_c script_value_c::set_number_value( float32_c value )
	{
		set_none_value();
		_type = type_e_number;
		_number_value = value;
	}

	string8_c const & script_value_c::get_string_value() const
	{
		cheonsa_assert( _type == type_e_string );
		return *_string_value;
	}

	void_c script_value_c::set_string_value( string8_c const & value )
	{
		set_none_value();
		_type = type_e_string;
		_string_value = new string8_c( value );
	}

	boolean_c script_value_c::resolve_as_boolean()
	{
		if ( _type == type_e_number )
		{
			return _number_value != 0.0f;
		}
		else if ( _type == type_e_string )
		{
			return _string_value->get_length() > 0;
		}
		return false;
	}

	void_c script_value_c::save( data_scribe_binary_property_c & scribe )
	{
		scribe.save_list_begin( "script_value_c" );
		uint8_c type = _type;
		scribe.save_uint8_buffer( "type", &type, 1 );
		if ( _type == type_e_number )
		{
			scribe.save_float32_buffer( "value", &_number_value, 1 );
		}
		else if ( _type == type_e_string )
		{
			scribe.save_string8( "value", *_string_value );
		}
		scribe.save_end();
		scribe.save_list_end( 0 );
	}

	void_c script_value_c::load( data_scribe_binary_property_c & scribe )
	{
		cheonsa_assert( scribe.loaded_check( "script_value_c", data_type_e_list, 0 ) );
		scribe.loaded_list_enter();
		while ( scribe.load() )
		{
			if ( scribe.loaded_check( "type", data_type_e_uint8, 1 ) )
			{
				_type = static_cast<type_e>( scribe.loaded_uint8_buffer()[0] );
			}
			else if ( scribe.loaded_check( "value", data_type_e_float32, 1 ) )
			{
				cheonsa_assert( _type == type_e_number );
				_number_value = scribe.loaded_float32_buffer()[0];
			}
			else if ( scribe.loaded_check( "value", data_type_e_string8, 0 ) )
			{
				cheonsa_assert( _type == type_e_string );
				cheonsa_assert( _string_value == nullptr );
				_string_value = new string8_c( scribe.loaded_string8() );
			}
		}
		scribe.loaded_list_exit();
	}

	script_value_c & script_value_c::operator = ( script_value_c const & other )
	{
		set_none_value();
		_type = other._type;
		if ( _type == type_e_number )
		{
			_number_value = other._number_value;
		}
		else if ( _type == type_e_string )
		{
			_string_value = new string8_c( *other._string_value );
		}
		return *this;
	}

}
