#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	class string8_c;

	// wide string, 16-bit characters, null terminated.
	// 16-bit strings are used for file paths.
	// 16-bit strings are used for user facing text.
	// this string can operate in two modes:
	// static: the string16_c instance is a light weight wrapper around a string literal value. a benefit to this is that copying by value is very low overhead. a draw back to this is that the string value can not be manipulated.
	// dynamic: the string16_c instance manages its own heap memory dynamically to hold the string value. a benefit to this is that the string value can be manipulated. a draw back to this is that copying by value will mean creating a new copy on the heap.
	class string16_c
	{
	public:
		core_list_c< char16_c > character_list; // null terminated list of characters.

	public:
		explicit string16_c(); // constructs an empty static mode string that contains just the terminating null character.
		explicit string16_c( string8_c const & other ); // constructs dynamic mode copy of other string, decodes from utf-8.
		         string16_c( string16_c const & other ); // constructs copy of other string, converts mode if needed.
		explicit string16_c( char8_c const * other ); // constructs dynamic mode copy of other string, decodes from utf-8.
		explicit string16_c( char8_c const * other, sint32_c other_length ); // constructs dynamic mode copy of other string, decodes from utf-8. other_length is length of other string including the terminating null character.
		explicit string16_c( mode_e mode, char16_c const * other ); // constructs static or dynamic mode copy of other string.
		explicit string16_c( mode_e mode, char16_c const * other, sint32_c other_length_with_null_terminator ); // constructs static or dynamic mode copy of other string. other_length is length of other string including the terminating null character.

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
