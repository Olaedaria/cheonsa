#include "cheonsa_string16.h"
#include "cheonsa_string8.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	string16_c::string16_c()
		: character_list( core_list_mode_e_static, L"", 1 )
	{
	}

	string16_c::string16_c( string8_c const & other )
		: character_list()
	{
		*this = other;
	}

	string16_c::string16_c( string16_c const & other )
		: character_list()
	{
		*this = other;
	}

	string16_c::string16_c( char8_c const * other )
		: character_list( core_list_mode_e_static, nullptr, 0 )
	{
		assert( other );
		*this = other;
	}

	string16_c::string16_c( char16_c const * other, core_list_mode_e mode, sint32_c other_length_with_null_terminator )
		: character_list( mode, other, other_length_with_null_terminator > 0 ? other_length_with_null_terminator : ops::string16_find_length( other ) + 1 )
	{
	}

	boolean_c string16_c::is_set() const
	{
		return character_list.get_length() > 1;
	}

	sint32_c string16_c::get_length() const
	{
		return character_list.get_length() - 1;
	}

	string16_c & string16_c::operator = ( char8_c const * other )
	{
		character_list.remove_all();
		ops::append_string8_to_string16( core_list_c< char8_c >( core_list_mode_e_static, other, ops::string8_find_length( other ) + 1 ), character_list );
		return *this;
	}

	string16_c & string16_c::operator = ( string8_c const & other )
	{
		character_list.remove_all();
		ops::append_string8_to_string16( other.character_list, character_list );
		return *this;
	}

	string16_c & string16_c::operator = ( char16_c const * other )
	{
		character_list.construct_mode_dynamic_from_array( other, ops::string16_find_length( other ) + 1 );
		return *this;
	}

	string16_c & string16_c::operator = ( string16_c const & other )
	{
		character_list = other.character_list;
		return *this;
	}

	string16_c & string16_c::operator += ( char8_c const other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list[ character_list.get_length() - 1 ] = other;
		character_list.insert( -1, '\0' );
		return *this;
	}

	string16_c & string16_c::operator += ( char16_c const other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list[ character_list.get_length() - 1 ] = other;
		character_list.insert( -1, '\0' );
		return *this;
	}

	string16_c & string16_c::operator += ( char8_c const * other )
	{
		assert( other );
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove( -1, 1 );
		ops::append_string8_to_string16( core_list_c< char8_c >( core_list_mode_e_static, other, ops::string8_find_length( other ) + 1 ), character_list );
		return *this;
	}

	string16_c & string16_c::operator += ( char16_c const * other )
	{
		assert( other );
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove( -1, 1 );
		character_list.insert( -1, other, ops::string16_find_length( other ) + 1 );
		return *this;
	}

	string16_c & string16_c::operator += ( string8_c const & other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove( -1, 1 );
		ops::append_string8_to_string16( other.character_list, character_list );
		return *this;
	}

	string16_c & string16_c::operator += ( string16_c const & other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove( -1, 1 );
		character_list.insert( -1, other.character_list.get_internal_array(), other.character_list.get_length() );
		return *this;
	}

	boolean_c string16_c::operator == ( char8_c const * other ) const
	{
		return ops::string16_compare( character_list.get_internal_array(), other );
	}
	
	boolean_c string16_c::operator == ( string8_c const & other ) const
	{
		return ops::string16_compare( character_list.get_internal_array(), other.character_list.get_internal_array() );
	}

	boolean_c string16_c::operator == ( char16_c const * other ) const
	{
		return ops::string16_compare( character_list.get_internal_array(), other );
	}

	boolean_c string16_c::operator == ( string16_c const & other ) const
	{
		if ( character_list.get_length() != other.character_list.get_length() )
		{
			return false;
		}
		return ops::string16_compare( character_list.get_internal_array(), other.character_list.get_internal_array() );
	}

	boolean_c string16_c::operator != ( char8_c const * other ) const
	{
		return ! operator == ( other );
	}

	boolean_c string16_c::operator != ( char16_c const * other ) const
	{
		return ! operator == ( other );
	}

	boolean_c string16_c::operator != ( string8_c const & other ) const
	{
		return ! operator == ( other );
	}

	boolean_c string16_c::operator != ( string16_c const & other ) const
	{
		return ! operator == ( other );
	}

}
