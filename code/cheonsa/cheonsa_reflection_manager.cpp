#include "cheonsa_reflection_manager.h"
#include "cheonsa_reflection_property.h"
#include "cheonsa_reflection_enumeration.h"
#include "cheonsa_reflection_class.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"
#include <cassert>

namespace cheonsa
{

	reflection_manager_c::reflection_manager_c()
		: _enumeration_list()
		, _class_list()
	{
	}

	reflection_manager_c::~reflection_manager_c()
	{
		_enumeration_list.remove_and_delete_all();
		_class_list.remove_and_delete_all();
	}

	boolean_c reflection_manager_c::start()
	{
		return true;
	}

	reflection_enumeration_c * reflection_manager_c::_add_enumeration( string8_c const & name, string8_c const & description )
	{
		assert( find_enumeration( name ) == nullptr );
		reflection_enumeration_c * new_enumeration = new reflection_enumeration_c( this, name, description );
		_enumeration_list.insert( -1, new_enumeration );
		return new_enumeration;
	}

	reflection_enumeration_c * reflection_manager_c::find_enumeration( string8_c const & name )
	{
		for ( sint32_c i = 0; i < _enumeration_list.get_length(); i++ )
		{
			if ( _enumeration_list[ i ]->get_name() == name )
			{
				return _enumeration_list[ i ];
			}
		}
		return nullptr;
	}

	reflection_class_c * reflection_manager_c::_add_class( string8_c const & name, string8_c const & description, reflection_class_c * base_class )
	{
		assert( find_class( name ) == nullptr );

		reflection_class_c * new_class = new reflection_class_c( this, name, description, base_class );
		_class_list.insert( -1, new_class );

		// get chain of base classes.
		core_list_c< reflection_class_c * > class_list;
		reflection_class_c * new_class_walker = new_class;
		while ( new_class_walker->get_base_class() )
		{
			new_class_walker = new_class_walker->get_base_class();
			class_list.insert( -1, new_class_walker );
		}

		// clone properties from base classes to this class.
		for ( sint32_c i = class_list.get_length() - 1; i >= 0; i-- )
		{
			new_class_walker = class_list[ i ];
			for ( sint32_c i = 0; i < new_class_walker->get_property_count(); i++ )
			{
				reflection_property_c const * property = new_class_walker->get_property( i );
				reflection_property_c * new_property = new reflection_property_c( new_class, property->_name, property->_description );
				new_property->initialize_clone( property );
			}
		}

		return new_class;
	}

	reflection_class_c * reflection_manager_c::find_class( string8_c const & name )
	{
		for ( sint32_c i = 0; i < _class_list.get_length(); i++ )
		{
			if ( _class_list[ i ]->get_name() == name )
			{
				return _class_list[ i ];
			}
		}
		return nullptr;
	}

	

}
