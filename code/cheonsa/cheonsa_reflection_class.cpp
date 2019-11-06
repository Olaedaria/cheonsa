#include "cheonsa_reflection_class.h"
#include "cheonsa_reflection_property.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	reflection_class_c::reflection_class_c( reflection_manager_c * reflection_manager, string8_c const & name, string8_c const & description, reflection_class_c * base_class )
		: _reflection_manager( reflection_manager )
		, _name( name )
		, _description( description )
		, _base_class( nullptr )
		, _property_list()
	{
	}

	reflection_class_c::~reflection_class_c()
	{
		for ( sint32_c i = 0; i < _property_list.get_length(); i++ )
		{
			delete _property_list[ i ];
		}
	}

	string8_c const & reflection_class_c::get_name() const
	{
		return _name;
	}

	string8_c const & reflection_class_c::get_description() const
	{
		return _description;
	}

	reflection_class_c * reflection_class_c::get_base_class() const
	{
		return _base_class;
	}

	reflection_property_c * reflection_class_c::initialize_property( string8_c const & name, string8_c const & description )
	{
		assert( ops::string8_is_valid_for_key( name ) );
		reflection_property_c * new_property = new reflection_property_c( this, name, description );
		_property_list.insert_at_end( new_property );
		return new_property;
	}

	sint32_c reflection_class_c::get_property_count() const
	{
		return _property_list.get_length();
	}

	reflection_property_c const * reflection_class_c::get_property( sint32_c index ) const
	{
		return _property_list[ index ];
	}

	reflection_property_c const * reflection_class_c::get_property_with_name( string8_c const & name ) const
	{
		for ( sint32_c i = 0; i < _property_list.get_length(); i++ )
		{
			if ( _property_list[ i ]->_name == name )
			{
				return _property_list[ i ];
			}
		}
		return nullptr;
	}

}
