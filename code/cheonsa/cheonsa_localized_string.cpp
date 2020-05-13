#include "cheonsa_localized_string.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	core_linked_list_c< localized_string_c::reference_c * > localized_string_c::reference_c::_global_list;

	localized_string_c::reference_c::reference_c()
		: _global_list_node( this )
		, _key()
		, _value( nullptr )
	{
		_global_list.insert_at_end( &_global_list_node );
	}

	localized_string_c::reference_c::~reference_c()
	{
		_global_list.remove( &_global_list_node );
	}

	void_c localized_string_c::reference_c::release_value()
	{
		if ( _value )
		{
			_value = nullptr;
			on_refreshed.invoke( this );
		}
	}

	void_c localized_string_c::reference_c::resolve_value()
	{
		_value = engine.get_content_manager()->find_string( _key );
		on_refreshed.invoke( this );
	}

	void_c localized_string_c::reference_c::clear()
	{
		_key = string8_c();
		_value = nullptr;
		on_refreshed.invoke( this );
	}

	void_c localized_string_c::reference_c::set( localized_string_c const * value )
	{
		_key = value ? value->_key : string8_c();
		_value = value;
		on_refreshed.invoke( this );
	}

	string8_c const & localized_string_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c localized_string_c::reference_c::set_key( string8_c const & value )
	{
		if ( _key != value )
		{
			_key = value;
			resolve_value();
		}
	}

	localized_string_c const * localized_string_c::reference_c::get_value() const
	{
		return _value;
	}

	string8_c const & localized_string_c::get_key() const
	{
		return _key;
	}

	string8_c const & localized_string_c::get_value() const
	{
		return _value;
	}

	localized_string_c & localized_string_c::operator = ( localized_string_c const & other )
	{
		// the strings are static mode so this is cheap.
		_key = other._key;
		_value = other._value;
		return *this;
	}

}
