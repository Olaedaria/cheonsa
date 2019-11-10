#include "cheonsa_string.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	core_linked_list_c< string_c::reference_c * > string_c::reference_c::_global_instance_list;

	void_c string_c::reference_c::_refresh_all_instances()
	{
		core_linked_list_c< string_c::reference_c * >::node_c const * list_node = string_c::reference_c::_global_instance_list.get_first();
		while ( list_node )
		{
			list_node->get_value()->refresh();
			list_node = list_node->get_next();
		}
	}

	string_c::reference_c::reference_c()
		: _global_instance_list_node( this )
		, _key()
		, _value( nullptr )
	{
		_global_instance_list.insert_at_end( &_global_instance_list_node );
	}

	string_c::reference_c::~reference_c()
	{
		_global_instance_list.remove( &_global_instance_list_node );
	}

	void_c string_c::reference_c::refresh()
	{
		_value = engine_c::get_instance()->get_content_manager()->find_string( _key );
		on_refreshed.invoke( this );
	}

	void_c string_c::reference_c::clear()
	{
		_key = string8_c();
		_value = nullptr;
		on_refreshed.invoke( this );
	}

	void_c string_c::reference_c::set( string_c const * value )
	{
		_key = value ? value->_key : string8_c();
		_value = value;
		on_refreshed.invoke( this );
	}

	string8_c const & string_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c string_c::reference_c::set_key( string8_c const & value )
	{
		if ( _key != value )
		{
			_key = value;
			refresh();
		}
	}

	string_c const * string_c::reference_c::get_value() const
	{
		return _value;
	}

	string8_c const & string_c::get_key() const
	{
		return _key;
	}

	string8_c const & string_c::get_value() const
	{
		return _value;
	}

	string_c & string_c::operator = ( string_c const & other )
	{
		// the strings are static mode so this is cheap.
		_key = other._key;
		_value = other._value;
		return *this;
	}

}
