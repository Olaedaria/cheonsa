#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{
	namespace ops
	{

		/*
		for string to list (vector) conversion functions:
				convert_string8_to_*xn
			the string should contain a list of number values separated by non-number characters.
			this allows number values to be extracted even from "bad" or ugly formatting.
			the following formats are all parsable:
				"(1.0, 2, 03.0" will produce a list of 3 number values.
				"1, 2 3" will produce a list of 3 number values.
		for all list (vector) to string conversion functions:
				convert_*xn_to_string8
			the result string will be formatted like:
				"(a, b, c)"
		*/


		char8_c convert_digit_to_char8( sint8_c const digit ); // converts a number value in range 0-15 to a character value in range '0'-'9' or 'A'-'F'.
		sint8_c convert_char8_to_digit( char8_c const character ); // converts a character value in range '0'-'9' or 'a'-'f' or 'A'-'F' to a number value in range 0-15. returns -1 (which in unsigned types is actually maximum: 0xFF, 255) if character is invalid.

		boolean_c convert_string8_to_sint8( string8_c const & in, sint8_c & out ); // converts a base 10 integer string value to a sint8 value.
		void_c convert_sint8_to_string8( sint8_c in, string8_c & out ); // converts a sint8 value to a base 10 integer string value.
		boolean_c convert_string8_to_sint8xn( string8_c const & in, core_list_c< sint8_c > & out ); // converts a base 10 integer vector string value to a list of sint8 values.
		void_c convert_sint8xn_to_string8( core_list_c< sint8_c > const & in, string8_c & out ); // converts a list of sint8 values to a base 10 integer vector string value.

		boolean_c convert_string8_to_uint8( string8_c const & in, uint8_c & out ); // converts a base 10 integer string value to a uint8 value.
		void_c convert_uint8_to_string8( uint8_c in, string8_c & out ); // converts a uint8 value to a base 10 integer string value.
		boolean_c convert_string8_to_uint8xn( string8_c const & in, core_list_c< uint8_c > & out ); // converts a base 10 integer vector string value to a list of uint8 values.
		void_c convert_uint8xn_to_string8( core_list_c< uint8_c > const & in, string8_c & out ); // converts a list of uint8 values to a base 10 integer vector string value.

		boolean_c convert_string8_to_sint16( string8_c const & in, sint16_c & out ); // converts a base 10 integer string value to a sint16 value.
		void_c convert_sint16_to_string8( sint16_c in, string8_c & out ); // converts a sint16 value to a base 10 integer string value.
		boolean_c convert_string8_to_sint16xn( string8_c const & in, core_list_c< sint16_c > & out ); // converts a base 10 integer vector string value to a list of sint16 values.
		void_c convert_sint16xn_to_string8( core_list_c< sint16_c > const & in, string8_c & out ); // converts a list of sint16 values to a base 10 integer vector string value.

		boolean_c convert_string8_to_uint16( string8_c const & in, uint16_c & out ); // converts a base 10 integer string value to a uint16 value.
		void_c convert_uint16_to_string8( uint16_c in, string8_c & out ); // converts uint16 value to a base 10 integer string value.
		boolean_c convert_string8_to_uint16xn( string8_c const & in, core_list_c< uint16_c > & out ); // cnverts a base 10 integer vector string value to a list of uint16 values.
		void_c convert_uint16xn_to_string8( core_list_c< uint16_c > const & in, string8_c & out ); // converts a list of uint16 values to a base 10 integer vector string value.

		boolean_c convert_string8_to_sint32( string8_c const & in, sint32_c & out ); // converts a base 10 integer string value to a sint32 value.
		void_c convert_sint32_to_string8( sint32_c in, string8_c & out ); // converts a sint32 value to a base 10 integer string value.
		boolean_c convert_string8_to_sint32xn( string8_c const & in, core_list_c< sint32_c > & out ); // converts a base 10 integer vector string value to a list of sint32 values.
		void_c convert_sint32xn_to_string8( core_list_c< sint32_c > const & in, string8_c & out ); // converts a list of sint32 values to a base 10 integer vector string value.

		boolean_c convert_string8_to_uint32( string8_c const & in, uint32_c & out ); // converts a base 10 integer string value to a uint32 value.
		void_c convert_uint32_to_string8( uint32_c in, string8_c & out ); // converts a uint32 value to a base 10 integer string value.
		boolean_c convert_string8_to_uint32xn( string8_c const & in, core_list_c< uint32_c > & out ); // converts a base 10 integer vector string value to a list of uint32 values.
		void_c convert_uint32xn_to_string8( core_list_c< uint32_c > const & in, string8_c & out ); // converts a list of uint32 values to a base 10 integer vector string value.

		boolean_c convert_string8_to_sint64( string8_c const & in, sint64_c & out ); // converts a base 10 integer string value to a sint64 value.
		void_c convert_sint64_to_string8( sint64_c in, string8_c & out ); // converts a sint64 value to a base 10 integer string value.
		boolean_c convert_string8_to_sint64xn( string8_c const & in, core_list_c< sint64_c > & out ); // converts a base 10 integer vector string value to a list of sint64 values.
		void_c convert_sint64xn_to_string8( core_list_c< sint64_c > const & in, string8_c & out ); // converts a list of sint64 values to a base 10 integer vector string value.

		boolean_c convert_string8_to_uint64( string8_c const & in, uint64_c & out ); // converts a base 10 integer string value to a uint64 value.
		void_c convert_uint64_to_string8( uint64_c in, string8_c & out ); // converts a uint64 value to a base 10 integer string value.
		boolean_c convert_string8_to_uint64xn( string8_c const & in, core_list_c< uint64_c > & out ); // converts a base 10 integer vector string value to a list of uint64 values.
		void_c convert_uint64xn_to_string8( core_list_c< uint64_c > const & in, string8_c & out ); // converts a list of uint64 values to base 10 integer vector string value.

		boolean_c convert_hexadecimal_string8_to_uint64( string8_c const & in, uint64_c & out );
		void_c convert_uint64_to_hexadecimal_string8( uint64_c in, string8_c & out );

		boolean_c convert_string8_to_float32( string8_c const & in, float32_c & out ); // converts a base 10 decimal string value to a float32 value. at the moment, always returns true even if the string was not valid for conversion.
		void_c convert_float32_to_string8( float32_c in, string8_c & out ); // converts a float32 value to a base 10 decimal string value with a decimal precision of 4.
		boolean_c convert_string8_to_float32xn( string8_c const & in, core_list_c< float32_c > & out ); // converts a base 10 decimal vector string value to a list of float32 values.
		void_c convert_float32xn_to_string8( core_list_c< float32_c > const & in, string8_c & out ); // converts a list of float32 values to a base 10 decimal vector string value.

		boolean_c convert_string8_to_float64( string8_c const & in, float64_c & out ); // converts a base 10 decimal string value to a float64 value.
		void_c convert_float64_to_string8( float64_c const in, string8_c & out ); // converts a float64 value to a base 10 decimal string value with decimal precision of 4.
		boolean_c convert_string8_to_float64xn( string8_c const & in, core_list_c< float64_c > & out ); // converts a base 10 decimal vector string value to a list of float64 values.
		void_c convert_float64xn_to_string8( core_list_c< float64_c > const & in, string8_c & out ); // converts a list of float64 values to a base 10 decimal vector string value.

		boolean_c convert_string8_to_boolean( string8_c const & in, boolean_c & out ); // converts a string value to a boolean value. checks for "true", "false", "1", and "0".
		void_c convert_boolean_to_string8( boolean_c in, string8_c & out ); // converts a boolean value to a "true" or "false" string value.

		boolean_c convert_rgb_to_hexadecimal_string8( vector64x3_c const & rgb, string8_c & string ); // converts an rgb color to a string of hexadecimal digits.
		boolean_c convert_rgba_to_hexadecimal_string8( vector64x4_c const & rgba, string8_c & string ); // converts an rgba color to a string of hexadecimal digits.

		boolean_c convert_string8_to_rgb( string8_c const & string, vector32x3_c & rgb ); // detects and converts base 10 decimal vector string value, or hexadecimal encoded #rgb, #rgba, #rrggbb, #rrggbbaa.
		boolean_c convert_string8_to_rgb( string8_c const & string, vector64x3_c & rgb ); // detects and converts base 10 decimal vector string value, or hexadecimal encoded #rgb, #rgba, #rrggbb, #rrggbbaa.
		boolean_c convert_string8_to_rgba( string8_c const & string, vector32x4_c & rgba ); // detects and converts base 10 decimal vector string value, or hexadecimal encoded #rgb, #rgba, #rrggbb, #rrggbbaa.. if alpha value is not defined in string, then the result's alpha will be set to 1.0f.
		boolean_c convert_string8_to_rgba( string8_c const & string, vector64x4_c & rgba ); // detects and converts base 10 decimal vector string value, or hexadecimal encoded #rgb, #rgba, #rrggbb, #rrggbbaa.. if alpha value is not defined in string, then the result's alpha will be set to 1.0f.

		boolean_c char16_is_printable( char16_c a ); // returns true if a refers to any type of printable character.
		boolean_c char16_is_space( char16_c a ); // returns true if a refers to a white space type of character.
		boolean_c char16_is_latin_letter( char16_c a ); // returns true if a refers to a latin letter between 'a' and 'z' or 'A' and 'Z'.
		boolean_c char16_is_decimal_digit( char16_c a ); // returns true if a refers to a decimal digit between '0' and '9'.
		boolean_c char16_is_hexadecimal_digit( char16_c a ); // returns true if a refers to a hexadecimal digit between '0' and '9' or 'a' and 'f' or 'A' and 'F'.
		boolean_c char16_is_punctuation( char16_c a ); // returns true if a refers to any kind of punctuation character.
		boolean_c char16_is_control( char16_c a ); // returns true if a refers to any kind of control character. cheonsa's text processing is only programmed to know how to deal with '\n' (new line).

		boolean_c char8_compare( char8_c a, char8_c b, boolean_c case_sensitive );
		boolean_c char16_compare( char16_c a, char16_c b, boolean_c case_sensitive );

		char8_c char8_to_upper( char8_c a );
		char16_c char16_to_upper( char16_c a );

		char8_c char8_to_lower( char8_c a );
		char16_c char16_to_lower( char16_c a );

		void_c string8_to_upper( string8_c & string ); // converts the input string instance to upper case.
		void_c string16_to_upper( string16_c & string ); // converts the input string instance to upper case.

		void_c string8_to_lower( string8_c & string ); // converts the input string instance to lower case.
		void_c string16_to_lower( string16_c & string ); // converts the input string instance to lower case.

		// pads a base 10 integer or decimal string value with as many leading zeros as needed so that it reaches desired_whole_number_length long.
		// if the whole number part of the string value is already at least desired_whole_number_length long, then this won't do anything.
		void_c string8_pad_whole_number_with_leading_zeros( string8_c & string, sint32_c desired_whole_number_length );

		string8_c string8_trim_front( string8_c const & string );
		string16_c string16_trim_front( string16_c const & string );

		string8_c string8_trim_back( string8_c const & string );
		string16_c string16_trim_back( string16_c const & string );

		string8_c string8_trim( string8_c const & string );
		string16_c string16_trim( string16_c const & string );

		sint32_c string8_find_index_of_2( string8_c const & string, string8_c const & find ); // finds the index of the first occurrence of find in the string. returns a value >= 0 if an instance was found, -1 if not.
		sint32_c string16_find_index_of_2( string16_c const & string, string16_c const & find ); // finds the index of the first occurrence of find in the string. returns a value >= 0 if an instance was found, -1 if not.

		boolean_c string8_find_indices_of( string8_c const & string, string8_c const & find, core_list_c< sint32_c > & result ); // finds the indices of occurrences of find in the string. overlapping matches are counted, so the whole entire string of "nanan" will be a match to "nan" 2 times instead of 1.
		boolean_c string16_find_indices_of( string16_c const & string, string16_c const & find, core_list_c< sint32_c > & result ); // finds the indices of occurrences of find in the string. overlapping matches are counted, so the whole entire string of "nanan" will be a match to "nan" 2 times instead of 1.

		sint32_c string8_find_and_replace_all( string8_c const & string, string8_c const & find, string8_c const & replace, boolean_c case_sensitive, string8_c & result ); // returns the number of instances replaced.
		sint32_c string16_find_and_replace_all( string16_c const & string, string16_c const & find, string16_c const & replace, boolean_c case_sensitive, string16_c & result ); // returns the number of instances replaced.

		void_c string8_split_at_spaces( string8_c const & string, core_list_c< string8_c > & result ); // splits string into multiple strings at instances of white space, result strings will not contain white space.
		void_c string8_split_at_delimiter( string8_c const & string, string8_c const & delimiter, core_list_c< string8_c > & result ); // splits string into multiple strings at instances of delimiter. result strings will not contain delimiter.

		void_c string16_split_at_spaces( string16_c const & string, core_list_c< string16_c > & result ); // splits string into multiple strings at instances of white space, result strings will not contain white space.
		void_c string16_split_at_delimiter( string16_c const & string, string16_c const & delimiter, core_list_c< string16_c > & result ); // splits string into multiple strings at instances of delimiter. result strings will not contain delimiter.

		string8_c string8_sub_string( string8_c const & string, sint32_c start, sint32_c length );
		string16_c string16_sub_string( string16_c const & string, sint32_c start, sint32_c length );

		boolean_c string8_starts_with( string8_c const & a, string8_c const & b ); // checks if string a starts with string b.
		boolean_c string8_starts_with( string8_c const & a, string16_c const & b ); // checks if string a starts with string b, interprets b as utf-8 stream, so converts char16_c to 1 to 3 byte long utf-8 sequence then compares with a.

		boolean_c string16_starts_with( string16_c const & a, string8_c const & b ); // checks if string a starts with string b, interprets b as utf-8 stream, so reads char16_c characters one at a time from utf-8 then compares with b.
		boolean_c string16_starts_with( string16_c const & a, string16_c const & b ); // checks if string a starts with string b.

		boolean_c string8_ends_with( string8_c const & a, string8_c const & b );
		boolean_c string8_ends_with( string8_c const & a, string16_c const & b );

		boolean_c string16_ends_with( string16_c const & a, string8_c const & b );
		boolean_c string16_ends_with( string16_c const & a, string16_c const & b );

		sint32_c string8_sort_compare( string8_c const * const & a, string8_c const * const & b );

		sint32_c string8_sort_compare( string8_c const & a, string8_c const & b ); // for use with insertion sort algorithm, returns true if b is less than a, false if not.
		sint32_c string8_sort_compare_case_insensitive( string8_c const & a, string8_c const & b ); // for use with insertion sort algorithm, returns true if b is less than a, false if not.

		sint32_c string16_sort_compare( string16_c const & a, string16_c const & b ); // for use with insertion sort algorithm, returns true if b is less than a, false if not.
		sint32_c string16_sort_compare_case_insensitive( string16_c const & a, string16_c const & b ); // for use with insertion sort algorithm, returns true if b is less than a, false if not.

		boolean_c string8_compare( char8_c const * a, char8_c const * b );
		boolean_c string8_compare_case_insensitive( char8_c const * a, char8_c const * b );

		boolean_c string16_compare( char16_c const * a, char16_c const * b );
		boolean_c string16_compare_case_insensitive( char16_c const * a, char16_c const * b );

		boolean_c string16_compare( char16_c const * a, char8_c const * b ); // compares a char16_c string to a utf-8 encoded char8_c string.
		boolean_c string16_compare_case_insensitive( char16_c const * a, char8_c const * b ); // compares a char16_c string to a utf-8 encoded char8_c string.

		sint32_c string8_find_length( char8_c const * string ); // returns the length of a null terminated string in characters, not including the terminating null character.
		sint32_c string16_find_length( char16_c const * string ); // returns the length of a null terminated string in characters, not including the terminating null character.

		//sint32_c string8_find_length_safe( char8_c const * string, char8_c const * limit ); // limit points to the next character after the end of the buffer. returns -1 if limit is breached.

		boolean_c string8_is_valid_for_key( string8_c const & string );

		// converts a utf-8 encoded byte stream to a wide character.
		// in: buffer that contains a utf-8 encoded byte sequence that encodes a character code point. after return. upon return this pointer will be advanced by the number of bytes read.
		// out: the result of the decode.
		// returns: number of bytes read.
		uint8_c convert_utf8_to_char16( char8_c const * & in, char16_c & out );

		// converts a utf-8 encoded string to a wide string.
		// in: utf-8 string which may or may not be null terminated. if it is null terminated then out will also be null terminated when this function returns.
		// out: wide string buffer to append to.
		void_c append_string8_to_string16( core_list_c< char8_c > const & in, core_list_c< char16_c > & out );

		// converts a wide character to a utf-8 encoded byte stream.
		// in: the wide character to encode.
		// out: pointer to a buffer with enough room to store result, it must be at least 3 bytes. upon return, this pointer will be advanced by the number of bytes written.
		// out_end: end of buffer so that we don't write past end of buffer.
		// returns: number of bytes written.
		sint8_c convert_char16_to_utf8( char16_c in, char8_c out[ 3 ] );

		// converts wide string to a utf-8 string.
		// in: wide string which may or may not be null terminated. if it is null terminated then out will also be null terminated when this function returns.
		// out: utf-8 string buffer to append to.
		void_c append_string16_to_string8( core_list_c< char16_c > const & in, core_list_c< char8_c > & out );

	}
}
