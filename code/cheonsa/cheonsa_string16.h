#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	class string8_c;

	// wide string, 16-bit characters, null terminated.
	// 16-bit strings are used for file paths.
	// 16-bit strings are used for user facing text.
	class string16_c
	{
	public:
		core_list_c< char16_c > character_list; // null terminated list of characters.

	public:
		explicit string16_c(); // constructs an empty static mode string that contains just the null terminator.
		explicit string16_c( string8_c const & other ); // constructs dynamic mode copy of other string. other string may be utf-8 encoded.
		         string16_c( string16_c const & other ); // constructs copy of other string. if other string is static mode, then this string will also be a static mode wrapper. otherwise if other string is dynamic or static volatile, then this string will be a dynamic mode copy.
		explicit string16_c( char8_c const * other ); // constructs dynamic mode copy of other string. other string must be null terminated. other string may be utf-8 encoded.
		explicit string16_c( char16_c const * other, core_list_mode_e mode = core_list_mode_e_dynamic, sint32_c other_length_with_null_terminator = 0 ); // constructs dynamic mode copy of other string. other string must be null terminated.

		boolean_c is_set() const; // returns true if character_list.get_length() > 1.

		sint32_c get_length() const; // gets the effective length of the string, not including the terminating null character.

		string16_c & operator = ( char8_c const * other ); // makes this string a dynamic mode copy of other string. other string may be utf-8 encoded.
		string16_c & operator = ( string8_c const & other ); // makes this string a dynamic mode ocpy of other string. other string may be utf-8 encoded.
		string16_c & operator = ( char16_c const * other ); // makes this string a dynamic mode copy of other string.
		string16_c & operator = ( string16_c const & other ); // makes this string a dynamic mode copy of other string.

		string16_c & operator += ( char8_c const other ); // appends a character to this string instance. converts this string instance to dynamic mode.
		string16_c & operator += ( char16_c const other ); // appends a character to this string instance. converts this string instance to dynamic mode.
		string16_c & operator += ( char8_c const * other ); // appends another string to this string instance. converts this string instance to dynamic mode.
		string16_c & operator += ( char16_c const * other ); // appends another string to this string instance. converts this string instance to dynamic mode.
		string16_c & operator += ( string8_c const & other ); // appends another string to this string instance. converts this string instance to dynamic mode.
		string16_c & operator += ( string16_c const & other ); // appends another string to this string instance. converts this string instance to dynamic mode.

		boolean_c operator == ( char8_c const * other ) const;
		boolean_c operator == ( string8_c const & other ) const;
		boolean_c operator == ( char16_c const * other ) const;
		boolean_c operator == ( string16_c const & other ) const;

		boolean_c operator != ( char8_c const * other ) const;
		boolean_c operator != ( char16_c const * other ) const;
		boolean_c operator != ( string8_c const & other ) const;
		boolean_c operator != ( string16_c const & other ) const;

	};

}
