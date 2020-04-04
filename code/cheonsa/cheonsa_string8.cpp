#include "cheonsa_string8.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	string8_c::string8_c()
		: character_list( mode_e_static, "", 1 )
	{
	}

	string8_c::string8_c( string8_c const & other )
		: character_list()
	{
		*this = other;
	}

	string8_c::string8_c( char16_c const * other )
		: character_list( mode_e_static, nullptr, 0 )
	{
		assert( other != nullptr );
		*this = other;
	}

	string8_c::string8_c( string16_c const & other )
		: character_list( mode_e_static, nullptr, 0 )
	{
		*this = other;
	}

	string8_c::string8_c( mode_e mode, char8_c const * other )
		: character_list( mode, other, ops::string8_find_length( other ) + 1 )
	{
	}

	string8_c::string8_c( mode_e mode, char8_c const * other, sint32_c other_length_with_null_terminator )
		: character_list( mode, other, other_length_with_null_terminator )
	{
		assert( other[ other_length_with_null_terminator - 1 ] == 0 );
	}

	sint32_c string8_c::get_length() const
	{
		assert( character_list.get_length() > 0 );
		assert( character_list[ character_list.get_length() - 1 ] == 0 );
		return character_list.get_length() - 1;
	}

	//void_c string8_c::construct_mode_static()
	//{
	//	character_list.construct_mode_static_from_array( "", 1 );
	//}

	//void_c string8_c::construct_mode_static_from_array( char8_c const * other )
	//{
	//	character_list.construct_mode_static_from_array( other, ops::string8_find_length( other ) + 1 );
	//}

	//void_c string8_c::construct_mode_dynamic()
	//{
	//	character_list.construct_mode_dynamic( 1 ); // this will behave like character_list.construct_mode_dynamic_but_keep_allocated() if character_list is already allocated to allocation length >= 1, otherwise it will reallocate it.
	//	character_list[ 0 ] = 0;
	//}

	string8_c & string8_c::operator = ( char8_c const * other )
	{
		character_list.construct_mode_dynamic_from_array( other, ops::string8_find_length( other ) + 1 );
		return *this;
	}

	string8_c & string8_c::operator = ( string8_c const & other )
	{
		character_list = other.character_list;
		return *this;
	}

	string8_c & string8_c::operator = ( char16_c const * other )
	{
		character_list.remove_all();
		ops::convert_string16_to_string8( core_list_c< char16_c >( mode_e_static, other, ops::string16_find_length( other ) + 1 ), character_list );
		return *this;
	}

	string8_c & string8_c::operator = ( string16_c const & other )
	{
		character_list.remove_all();
		ops::convert_string16_to_string8( other.character_list, character_list );
		return *this;
	}

	string8_c & string8_c::operator += ( char8_c other )
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

	string8_c & string8_c::operator += ( char16_c other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		char8_c buffer[ 4 ];
		char8_c * buffer_pointer = buffer;
		sint8_c bytes = ops::convert_char16_to_utf8( other, buffer_pointer );
		buffer[ bytes ] = 0;
		character_list.insert_at_end( buffer, bytes + 1 );
		return *this;
	}

	string8_c & string8_c::operator += ( char8_c const * other )
	{
		assert( other != nullptr );
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		character_list.insert_at_end( other, ops::string8_find_length( other ) + 1 );
		return *this;
	}

	string8_c & string8_c::operator += ( char16_c const * other )
	{
		assert( other != nullptr );
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		ops::convert_string16_to_string8( core_list_c< char16_c >( mode_e_static, other, ops::string16_find_length( other ) + 1 ), character_list );
		return *this;
	}

	string8_c & string8_c::operator += ( string8_c const & other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		character_list.insert_at_end( other.character_list.get_internal_array(), other.character_list.get_length() );
		return *this;
	}

	string8_c & string8_c::operator += ( string16_c const & other )
	{
		if ( character_list.get_mode_is_static() )
		{
			character_list.convert_from_static_to_dynamic();
		}
		character_list.remove_at_end();
		ops::convert_string16_to_string8( other.character_list, character_list );
		return *this;
	}

	boolean_c string8_c::operator == ( char8_c const * other ) const
	{
		return ops::string8_compare( character_list.get_internal_array(), other );
	}

	boolean_c string8_c::operator == ( string8_c const & other ) const
	{
		if ( character_list.get_length() != other.character_list.get_length() )
		{
			return false;
		}
		return ops::string8_compare( character_list.get_internal_array(), other.character_list.get_internal_array() );
	}

	boolean_c string8_c::operator == ( char16_c const * other ) const
	{
		return ops::string16_compare( other, character_list.get_internal_array() );
	}

	boolean_c string8_c::operator == ( string16_c const & other ) const
	{
		return operator == ( other.character_list.get_internal_array() );
	}

	boolean_c string8_c::operator != ( char8_c const * other ) const
	{
		return ! operator == ( other );
	}

	boolean_c string8_c::operator != ( string8_c const & other ) const
	{
		return ! operator == ( other );
	}

	boolean_c string8_c::operator != ( char16_c const * other ) const
	{
		return ! operator == ( other );
	}

	boolean_c string8_c::operator != ( string16_c const & other ) const
	{
		return ! operator == ( other );
	}

}
