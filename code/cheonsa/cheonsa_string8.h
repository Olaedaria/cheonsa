#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	class string16_c;

	// narrow string, 8-bit characters, ascii, utf-8, null terminated.
	// 8-bit strings are used to hold keys (unique identifiers).
	// 8-bit strings are used to convert between string values and numeric value types (in the ops namespace).
	class string8_c
	{
	public:
		core_list_c< char8_c > character_list; // null terminated list of characters.

	public:
		explicit string8_c(); // constructs an empty static mode string that contains just the terminating null character.
		         string8_c( string8_c const & other ); // constructs copy of other string, preserves mode.
		explicit string8_c( string16_c const & other ); // constructs dynamic mode copy of other string, encodes as utf-8.
		explicit string8_c( char8_c const * other, core_list_mode_e mode = core_list_mode_e_dynamic, sint32_c other_length_with_null_terminator = 0 ); // constructs static or dynamic mode copy of other string. other string must be null terminated. other string may be utf-8 encoded. if other_length_with_null_terminator == 0 then length will be calculated automatically, otherwise if you already know the length then you can pass it in (to avoid recalculation).
		explicit string8_c( char16_c const * other ); // constructs dynamic mode copy of other string, encodes as utf-8. other string must be null terminated.

		boolean_c is_set() const; // returns true if character_list.get_length() > 1.

		sint32_c get_length() const; // gets the effective length of the string, not including the terminating null character.

		string8_c & operator = ( char8_c const * other ); // makes this string a dynamic mode copy of other string.
		string8_c & operator = ( string8_c const & other ); // makes this string a dynamic mode copy of other string.
		string8_c & operator = ( char16_c const * other ); // makes this string a dynamic mode copy of other string. uses utf-8 encoding to pack the char16s into char8s. if the char16s are in the ascii range then this will result in an ascii string.
		string8_c & operator = ( string16_c const & other ); // makes this string a dynamic mode copy of other string. uses utf-8 encoding to pack the char16s into char8s. if the char16s are in the ascii range then this will result in an ascii string.

		string8_c & operator += ( char8_c other );
		string8_c & operator += ( char16_c other );
		string8_c & operator += ( char8_c const * other );
		string8_c & operator += ( char16_c const * other );
		string8_c & operator += ( string8_c const & other );
		string8_c & operator += ( string16_c const & other );

		boolean_c operator == ( char8_c const * other ) const;
		boolean_c operator == ( string8_c const & other ) const;
		boolean_c operator == ( char16_c const * other ) const;
		boolean_c operator == ( string16_c const & other ) const;

		boolean_c operator != ( char8_c const * other ) const;
		boolean_c operator != ( string8_c const & other ) const;
		boolean_c operator != ( char16_c const * other ) const;
		boolean_c operator != ( string16_c const & other ) const;

	};

}
