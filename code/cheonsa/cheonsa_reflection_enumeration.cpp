#include "cheonsa_reflection_enumeration.h"

namespace cheonsa
{

	reflection_enumeration_c::value_c::value_c( string8_c const & name, string8_c const & description, sint32_c value )
		: _name( name )
		, _description( description )
		, _value( value )
	{
	}

	string8_c const & reflection_enumeration_c::value_c::get_name() const
	{
		return _name;
	}

	string8_c const & reflection_enumeration_c::value_c::get_description() const
	{
		return _description;
	}

	sint32_c reflection_enumeration_c::value_c::get_value() const
	{
		return _value;
	}

	reflection_enumeration_c::reflection_enumeration_c( reflection_manager_c * reflection_manager, string8_c const & name, string8_c const & description )
		: _reflection_manager( reflection_manager )
		, _name( name )
		, _description( description )
		, _is_for_bit_flags( false )
		, _value_list()
	{
	}

	reflection_enumeration_c::~reflection_enumeration_c()
	{
		for ( sint32_c i = 0; i < _value_list.get_length(); i++ )
		{
			delete _value_list[ i ];
		}
	}

	string8_c const & reflection_enumeration_c::get_name() const
	{
		return _name;
	}

	string8_c const & reflection_enumeration_c::get_description() const
	{
		return _description;
	}

	void_c reflection_enumeration_c::initialize_for_bit_flags()
	{
		_is_for_bit_flags = true;
	}

	void_c reflection_enumeration_c::initialize_value( string8_c const & name, string8_c const & description, sint32_c value )
	{
		value_c * new_value = new value_c( name, description, value );
		_value_list.insert_at_end( new_value );
	}

	sint32_c reflection_enumeration_c::get_value_count() const
	{
		return _value_list.get_length();
	}

	reflection_enumeration_c::value_c const * reflection_enumeration_c::get_value( sint32_c index ) const
	{
		return _value_list[ index ];
	}

	boolean_c reflection_enumeration_c::find_name_with_value( string8_c & name, uint32_c value ) const
	{
		for ( sint32_c i = 0; i < _value_list.get_length(); i++ )
		{
			if ( _value_list[ i ]->get_value() == value )
			{
				name = _value_list[ i ]->get_name();
				return true;
			}
		}
		return false;
	}

	boolean_c reflection_enumeration_c::find_name_with_index( string8_c & name, sint32_c index ) const
	{
		name = _value_list[ index ]->get_name();
		return true;
	}

	boolean_c reflection_enumeration_c::find_value_with_name( uint32_c & value, string8_c const & name ) const
	{
		for ( sint32_c i = 0; i < _value_list.get_length(); i++ )
		{
			if ( _value_list[ i ]->get_name() == name )
			{
				value = _value_list[ i ]->get_value();
				return true;
			}
		}
		return false;
	}

	boolean_c reflection_enumeration_c::find_value_with_index( uint32_c & value, sint32_c index ) const
	{
		value = _value_list[ index ]->get_value();
		return true;
	}

	boolean_c reflection_enumeration_c::find_index_with_name( sint32_c & index, string8_c const & name ) const
	{
		for ( sint32_c i = 0; i < _value_list.get_length(); i++ )
		{
			if ( _value_list[ i ]->get_name() == name )
			{
				index = i;
				return true;
			}
		}
		return false;
	}

	boolean_c reflection_enumeration_c::find_index_with_value( sint32_c & index, uint32_c value ) const
	{
		index = -1;
		for ( sint32_c i = 0; i < _value_list.get_length(); i++ )
		{
			if ( _value_list[ i ]->get_value() == value )
			{
				index = i;
				return true;
			}
		}
		return false;
	}

	boolean_c reflection_enumeration_c::contains_value( uint32_c value ) const
	{
		if ( _is_for_bit_flags == false )
		{
			for ( sint32_c i = 0; i < _value_list.get_length(); i++ )
			{
				if ( _value_list[ i ]->get_value() == value )
				{
					return true;
				}
			}
			return false;
		}
		// not gonna bother verifying at the moment.
		return true;
	}

}
