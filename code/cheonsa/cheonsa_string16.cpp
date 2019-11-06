#include "cheonsa_string16.h"
#include "cheonsa_string8.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	string16_c::string16_c()
		: character_list( mode_e_static, L"", 1 )
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
		: character_list( mode_e_static, nullptr, 0 )
	{
		assert( other != nullptr );
		*this = other;
	}

	string16_c::string16_c( char8_c const * other, sint32_c other_length )
		: character_list( mode_e_dynamic, nullptr, 0 )
	{
		assert( other );
		assert( other_length >= 0 && other[ other_length ] == 0 );
		ops::convert_string8_to_string16( core_list_c< char8_c >( mode_e_static, other, other_length + 1 ), character_list );
	}

	string16_c::string16_c( mode_e mode, char16_c const * other )
		: character_list( mode, other, ops::string16_find_length( other ) + 1 )
	{
	}

	string16_c::string16_c( mode_e mode, char16_c const * other, sint32_c other_length_with_null_terminator )
		: character_list( mode, other, other_length_with_null_terminator )
	{
		assert( other[ other_length_with_null_terminator - 1 ] == 0 );
	}

	boolean_c string16_c::is_set() const
	{
		return character_list.get_length() > 1;
	}

	sint32_c string16_c::get_length() const
	{
		return character_list.get_length() - 1;
	}

	//void_c string16_c::construct_mode_static()
	//{
	//	character_list.construct_mode_static_from_array( L"", 1 );
	//}

	//void_c string16_c::construct_mode_static_from_array( char16_c const * other )
	//{
	//	character_list.construct_mode_static_from_array( other, ops::string16_find_length( other ) + 1 );
	//}

	//void_c string16_c::construct_mode_dynamic()
	//{
	//	character_list.construct_mode_dynamic( 1 ); // this will behave like character_list.construct_mode_dynamic_but_keep_allocated() if character_list is already allocated to allocation length >= 1, otherwise it will reallocate it.
	//	character_list[ 0 ] = 0;
	//}

	string16_c & string16_c::operator = ( char8_c const * other )
	{
		character_list.remove_all();
		ops::convert_string8_to_string16( core_list_c< char8_c >( mode_e_static, other, ops::string8_find_length( other ) + 1 ), character_list );
		return *this;
	}

	string16_c & string16_c::operator = ( string8_c const & other )
	{
		character_list.remove_all();
		ops::convert_string8_to_string16( other.character_list, character_list );
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
		character_list.remove_at_end();
		character_list.insert_at_end( other );
		character_list.insert_at_end( 0 );
		return *this;
	}

	string16_c & string16_c::operator += ( char16_c const other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		character_list.insert_at_end( other );
		character_list.insert_at_end( 0 );
		return *this;
	}

	string16_c & string16_c::operator += ( char8_c const * other )
	{
		assert( other != nullptr );
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		ops::convert_string8_to_string16( core_list_c< char8_c >( mode_e_static, other, ops::string8_find_length( other ) + 1 ), character_list );
		return *this;
	}

	string16_c & string16_c::operator += ( char16_c const * other )
	{
		assert( other != nullptr );
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		character_list.insert_range_at_end( other, ops::string16_find_length( other ) + 1 );
		return *this;
	}

	string16_c & string16_c::operator += ( string8_c const & other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		ops::convert_string8_to_string16( other.character_list, character_list );
		return *this;
	}

	string16_c & string16_c::operator += ( string16_c const & other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		character_list.insert_range_at_end( other.character_list.get_internal_array(), other.character_list.get_length() );
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

	//void_c string16_c::transfer_ownership( string16_c & in, string16_c & out )
	//{
	//	in.character_list.transfer_to( out.character_list );
	//}

}
