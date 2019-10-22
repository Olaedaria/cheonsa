#pragma once

#include "cheonsa___build.h"
#include "third_party/half.hpp"

namespace cheonsa
{

	typedef void void_c;
	typedef char char8_c;
	typedef wchar_t char16_c;
	typedef signed char sint8_c;
	typedef unsigned char uint8_c;
	typedef signed short int sint16_c;
	typedef unsigned short int uint16_c;
	typedef signed long int sint32_c;
	typedef unsigned long int uint32_c;
	typedef signed long long int sint64_c;
	typedef unsigned long long int uint64_c;
	typedef half_float::half float16_c;
	typedef float float32_c;
	typedef double float64_c;
	typedef bool boolean_c;

#if defined( cheonsa_32_bit )
	typedef sint32_c sint_native_c;
	typedef uint32_c uint_native_c;
#elif defined( cheonsa_64_bit )
	typedef sint64_c sint_native_c;
	typedef uint64_c uint_native_c;
#else
	#error can't determine if compiling for 32 bit or 64 bit.
#endif

	// using templates, because it's actually more convenient when writing templatized code.
	// in particular, i need this when writing templatized string to int and int to string functions.
	template< typename number_type_c > 
	struct constants
	{
	};

	template<>
	struct constants< sint8_c >
	{
		static inline boolean_c is_signed() { return true; }
		static inline sint8_c minimum() { return -127; }
		static inline sint8_c maximum() { return  127; }
	};

	template<>
	struct constants< uint8_c >
	{
		static inline boolean_c is_signed() { return false; }
		static inline uint8_c minimum() { return 0; }
		static inline uint8_c maximum() { return 255; }
	};

	template<>
	struct constants< sint16_c >
	{
		static inline boolean_c is_signed() { return true; }
		static inline sint16_c minimum() { return -32767; }
		static inline sint16_c maximum() { return  32767; }
	};

	template<>
	struct constants< uint16_c >
	{
		static inline boolean_c is_signed() { return false; }
		static inline uint16_c minimum() { return 0; }
		static inline uint16_c maximum() { return 65535; }
	};

	template<>
	struct constants< sint32_c >
	{
		static inline boolean_c is_signed() { return true; }
		static inline sint32_c minimum() { return -2147483647l; }
		static inline sint32_c maximum() { return  2147483647l; }
	};

	template<>
	struct constants< uint32_c >
	{
		static inline boolean_c is_signed() { return false; }
		static inline uint32_c minimum() { return 0ul; }
		static inline uint32_c maximum() { return 4294967295ul; }
	};

	template<>
	struct constants< sint64_c >
	{
		static inline boolean_c is_signed() { return true; }
		static inline sint64_c minimum() { return -9223372036854775807ll; }
		static inline sint64_c maximum() { return  9223372036854775807ll; }
	};

	template<>
	struct constants< uint64_c >
	{
		static inline boolean_c is_signed() { return false; }
		static inline uint64_c minimum() { return 0ull; }
		static inline uint64_c maximum() { return 18446744073709551615ull; }
	};

	template<>
	struct constants< float32_c >
	{
		static inline boolean_c is_signed() { return true; }
		static inline float32_c minimum() { return 0.0f; } // minimum positive value. unlike two's compliment ints, floats use a signed bit, so rather than returning the largest negative number here (as we do with signed int types), we return 0.
		static inline float32_c maximum() { return 3.402823466e+38f; } // maximum positive value. negate this value for the largest negative value.
		static inline float32_c epsilon() { return 1.192092896e-07f; } // smallest positive number x, such that x + 1.0 is not equal to 1.0.
		static inline float32_c smallest() { return 1.175494351e-38f; } // smallest positive value.
		static inline float32_c pi() { return 3.141592653589793238462643383279f; }
		static inline float32_c degrees_to_radians() { return pi() / 180.0f; }
		static inline float32_c radians_to_degrees() { return 180.0f / pi(); }
		static inline float32_c value_near_zero() { return 1e-20f; }
		static inline float32_c division_near_zero() { return 1e-15f; }
		static inline float32_c angle_near_zero() { return 1e-7f; } // angle is in radians.
	};

	template<>
	struct constants< float64_c >
	{
		static inline boolean_c is_signed() { return true; }
		static inline float64_c minimum() { return 0.0; } // minimum positive value. unlike two's compliment ints, floats use a signed bit, so rather than returning the largest negative number here (as we do with signed int types), we return 0.
		static inline float64_c maximum() { return 1.7976931348623158e+308; } // maximum positive value.
		static inline float64_c epsilon() { return 2.2204460492503131e-016; } // smallest positive number x, such that x + 1.0 is not equal to 1.0.
		static inline float64_c smallest() { return 2.2250738585072014e-308; } // smallest positive value.
		static inline float64_c pi() { return 3.141592653589793238462643383279; }
		static inline float64_c square_root_of_2() { return 1.41421356237309504880; }
		static inline float64_c degrees_to_radians() { return pi() / 180.0; }
		static inline float64_c radians_to_degrees() { return 180.0f / pi(); }
		static inline float64_c value_near_zero() { return 1e-20; }
		static inline float64_c division_near_zero() { return 1e-15; }
		static inline float64_c angle_near_zero() { return 1e-7; }
	};

	// four character codes are frequently used as file [type|format] [signatures|magic numbers].
	// four character codes are always supposed to be saved and loaded in big endian byte order (even if the format of other parts of the file are not big endian), so that they can be read by humans who open the file in a hex editor.
	struct four_character_code_c
	{
		union
		{
			char8_c characters[ 4 ]; // the four character code, in the correct order.
			uint32_c uint32; // equivalent uint32 value in native endianness, same as reinterpret_cast< uint32_c * >( this ).
		};
		inline four_character_code_c()
		{
			characters[ 0 ] = 0;
			characters[ 1 ] = 0;
			characters[ 2 ] = 0;
			characters[ 3 ] = 0;
		}
		inline four_character_code_c( char8_c const * four_character_string )
		{
			characters[ 0 ] = four_character_string[ 0 ];
			characters[ 1 ] = four_character_string[ 1 ];
			characters[ 2 ] = four_character_string[ 2 ];
			characters[ 3 ] = four_character_string[ 3 ];
		}
		inline boolean_c operator == ( four_character_code_c const & other ) const
		{
			return uint32 == other.uint32;
		}
		inline boolean_c operator != ( four_character_code_c const & other ) const
		{
			return uint32 != other.uint32;
		}
		inline four_character_code_c get_flipped() const
		{
			four_character_code_c result;
			result.characters[ 0 ] = characters[ 3 ];
			result.characters[ 1 ] = characters[ 2 ];
			result.characters[ 2 ] = characters[ 1 ];
			result.characters[ 3 ] = characters[ 0 ];
			return result;
		}
	};

}
