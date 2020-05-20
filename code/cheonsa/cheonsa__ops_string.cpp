#include "cheonsa__ops.h"

#include "third_party/strnatcmp.h"

namespace cheonsa
{
	namespace ops
	{

		char8_c convert_digit_to_char8( sint8_c const digit )
		{
			assert( digit >= 0 && digit <= 15 );
			static char8_c const map[] =
			{
				'0',
				'1',
				'2',
				'3',
				'4',
				'5',
				'6',
				'7',
				'8',
				'9',
				'a',
				'b',
				'c',
				'd',
				'e',
				'f'
			};
			if ( digit < 16 )
			{
				return map[ digit ];
			}
			return 0;
		}

		sint8_c convert_char8_to_digit( char8_c const character )
		{
			if ( character >= '0' && character <= '9' )
			{
				return character - '0';
			}
			else if ( character >= 'a' && character <= 'f' )
			{
				return character - 'a' + 10;
			}
			else if ( character >= 'A' && character <= 'F' )
			{
				return character - 'A' + 10;
			}
			return -1;
		}

		// does result = value * base + digit, but checks if overflow or underflow will occur.
		// if overflow or underflow would occur, caps result at int_type_c's maximum or minimum range limit and returns false.
		// otherwise, performs the operation and returns true.
		template< typename int_type_c >
		boolean_c _accumulate_digit( int_type_c & result, int_type_c value, sint8_c base, sint8_c digit )
		{
			// handle case where multiplying by base would overflow value.
			if ( value > constants< int_type_c >::maximum() / base )
			{
				result = constants< int_type_c >::maximum();
				return false;
			}
			// handle case where multiplying by base would underflow value.
			if ( value < constants< int_type_c >::minimum() / base )
			{
				result = constants< int_type_c >::minimum();
				return false;
			}
			// handle case where adding digit would overflow value.
			if ( digit > 0 && value * base > constants< int_type_c >::maximum() - digit )
			{
				result = constants< int_type_c >::maximum();
				return false;
			}
			// handle case where subtracting digit would underflow value.
			else if ( digit < 0 && value * base < constants< int_type_c >::minimum() - digit )
			{
				result = constants< int_type_c >::minimum();
				return false;
			}
			// handle ordinary case.
			result = value * base + digit;
			return true;
		}

		template< typename string_type_c, typename int_type_c >
		boolean_c _convert_string_to_int( string_type_c const & in, int_type_c & out, sint8_c base )
		{
			assert( base >= 2 && base <= 16 );

			int_type_c result = 0;
			auto const * c = in.character_list.get_internal_array();

			boolean_c negative = false;
			if ( *c == '-' )
			{
				if ( constants< int_type_c >::is_signed() == false )
				{
					out = 0;
					return true;
				}
				negative = true;
				c++;
			}

			while ( *c )
			{
				// parse digit.
				sint8_c digit = convert_char8_to_digit( *c );
				if ( digit < 0 || digit >= base )
				{
					return false;
				}

				if ( !_accumulate_digit< int_type_c >( result, result, base, negative ? -digit : digit ) )
				{
					out = result; // encountered underflow or overflow, no reason to keep going.
					return true;
				}
				c++;
			}

			out = result;

			return true;
		}

		template< typename int_type_c, typename string_type_c >
		void_c _convert_int_to_string( int_type_c in, string_type_c & out, sint8_c base )
		{
			assert( base >= 2 && base <= 16 );
			out.character_list.remove_all();
			if ( in < 0 )
			{
				out.character_list.insert( -1, '-' );
			}
			else if ( in == 0 )
			{
				out.character_list.insert( -1, '0' );
			}
			while ( in > 0 )
			{
				int_type_c digit = in % base;
				in /= base;
				out.character_list.insert( 0, convert_digit_to_char8( static_cast< sint8_c >( digit ) ) );
			}
			out.character_list.insert( -1, 0 );
		}

		template< typename string_type_c, typename int_type_c >
		boolean_c _convert_string_to_intxn( string_type_c const & in, core_list_c< int_type_c > & out )
		{
			if ( out.get_mode_is_dynamic() )
			{
				out.remove_all();
			}
			string_type_c element_string;
			int_type_c element_value;
			sint32_c element_index = 0;
			auto const * c = in.character_list.get_internal_array();
			while ( *c != 0 )
			{
				if ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
				{
					do
					{
						element_string += *c;
						c++;
					} while ( ( *c != 0 ) && ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' ) );
					element_value = 0;
					_convert_string_to_int< string_type_c, int_type_c >( element_string, element_value, 10 );
					element_string = string_type_c();
					if ( out.get_mode_is_dynamic() )
					{
						// append element to dynamic mode list.
						out.insert( -1, element_value );
					}
					else
					{
						if ( element_index < out.get_length() )
						{
							// set element in static mode list.
							out[ element_index ] = element_value;
						}
						else
						{
							// out of bounds of static mode list.
							return false;
						}
					}
					element_index++;
				}
				c++;
			}
			return true;
		}

		template< typename int_type_c, typename string_type_c >
		void_c _convert_intxn_to_string( core_list_c< int_type_c > const & in, string_type_c & out, char8_c const * brackets = nullptr )
		{
			out = string_type_c();
			if ( in.get_length() > 1 && brackets )
			{
				out += brackets[ 0 ];
			}
			for ( sint32_c element_index = 0; element_index < in.get_length(); element_index++ )
			{
				if ( element_index > 0 )
				{
					out += ", ";
				}
				string_type_c element_string;
				_convert_int_to_string< int_type_c, string_type_c >( in[ element_index ], element_string, 10 );
				out += element_string;
			}
			if ( in.get_length() > 1 && brackets )
			{
				out += brackets[ 1 ];
			}
		}

		boolean_c convert_string8_to_sint8( string8_c const & in, sint8_c & out )
		{
			return _convert_string_to_int< string8_c, sint8_c >( in, out, 10 );
		}

		void_c convert_sint8_to_string8( sint8_c in, string8_c & out )
		{
			_convert_int_to_string< sint8_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_sint8xn( string8_c const & in, core_list_c< sint8_c > & out )
		{
			return _convert_string_to_intxn< string8_c, sint8_c >( in, out );
		}

		void_c convert_sint8xn_to_string8( core_list_c< sint8_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< sint8_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_uint8( string8_c const & in, uint8_c & out )
		{
			return _convert_string_to_int< string8_c, uint8_c >( in, out, 10 );
		}

		void_c convert_uint8_to_string8( uint8_c in, string8_c & out )
		{
			_convert_int_to_string< uint8_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_uint8xn( string8_c const & in, core_list_c< uint8_c > & out )
		{
			return _convert_string_to_intxn< string8_c, uint8_c >( in, out );
		}

		void_c convert_uint8xn_to_string8( core_list_c< uint8_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< uint8_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_sint16( string8_c const & in, sint16_c & out )
		{
			return _convert_string_to_int< string8_c, sint16_c >( in, out, 10 );
		}

		void_c convert_sint16_to_string8( sint16_c in, string8_c & out )
		{
			_convert_int_to_string< sint16_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_sint16xn( string8_c const & in, core_list_c< sint16_c > & out )
		{
			return _convert_string_to_intxn< string8_c, sint16_c >( in, out );
		}

		void_c convert_sint16xn_to_string8( core_list_c< sint16_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< sint16_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_uint16( string8_c const & in, uint16_c & out )
		{
			return _convert_string_to_int< string8_c, uint16_c >( in, out, 10 );
		}

		void_c convert_uint16_to_string8( uint16_c in, string8_c & out )
		{
			_convert_int_to_string< uint16_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_uint16xn( string8_c const & in, core_list_c< uint16_c > & out )
		{
			return _convert_string_to_intxn< string8_c, uint16_c >( in, out );
		}

		void_c convert_uint16xn_to_string8( core_list_c< uint16_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< uint16_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_sint32( string8_c const & in, sint32_c & out )
		{
			return _convert_string_to_int< string8_c, sint32_c >( in, out, 10 );
		}

		void_c convert_sint32_to_string8( sint32_c in, string8_c & out )
		{
			_convert_int_to_string< sint32_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_sint32xn( string8_c const & in, core_list_c< sint32_c > & out )
		{
			return _convert_string_to_intxn< string8_c, sint32_c >( in, out );
		}

		void_c convert_sint32xn_to_string8( core_list_c< sint32_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< sint32_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_uint32( string8_c const & in, uint32_c & out )
		{
			return _convert_string_to_int< string8_c, uint32_c >( in, out, 10 );
		}

		void_c convert_uint32_to_string8( uint32_c in, string8_c & out )
		{
			_convert_int_to_string< uint32_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_uint32xn( string8_c const & in, core_list_c< uint32_c > & out )
		{
			return _convert_string_to_intxn< string8_c, uint32_c >( in, out );
		}

		void_c convert_uint32xn_to_string8( core_list_c< uint32_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< uint32_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_sint64( string8_c const & in, sint64_c & out )
		{
			return _convert_string_to_int< string8_c, sint64_c >( in, out, 10 );
		}

		void_c convert_sint64_to_string8( sint64_c in, string8_c & out )
		{
			_convert_int_to_string< sint64_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_sint64xn( string8_c const & in, core_list_c< sint64_c > & out )
		{
			return _convert_string_to_intxn< string8_c, sint64_c >( in, out );
		}

		void_c convert_sint64xn_to_string8( core_list_c< sint64_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< sint64_c, string8_c >( in, out );
		}

		boolean_c convert_string8_to_uint64( string8_c const & in, uint64_c & out )
		{
			return _convert_string_to_int< string8_c, uint64_c >( in, out, 10 );
		}

		void_c convert_uint64_to_string8( uint64_c in, string8_c & out )
		{
			_convert_int_to_string< uint64_c, string8_c >( in, out, 10 );
		}

		boolean_c convert_string8_to_uint64xn( string8_c const & in, core_list_c< uint64_c > & out )
		{
			return _convert_string_to_intxn< string8_c, uint64_c >( in, out );
		}

		void_c convert_uint64xn_to_string8( core_list_c< uint64_c > const & in, string8_c & out )
		{
			_convert_intxn_to_string< uint64_c, string8_c >( in, out );
		}

		boolean_c convert_hexadecimal_string8_to_uint64( string8_c const & in, uint64_c & out )
		{
			if ( in.get_length() > 16 )
			{
				return false;
			}
			uint64_c result = 0;
			sint32_c in_count = in.get_length();
			for ( sint32_c i = 0; i < in_count; i++ )
			{
				uint8_c digit = convert_char8_to_digit( in.character_list.get_internal_array()[ i ] );
				if ( digit == -1 )
				{
					return false;
				}
				if ( i > 0 )
				{
					result <<= 4;
				}
				result |= digit;
			}
			out = result;
			return true;
		}

		void_c convert_uint64_to_hexadecimal_string8( uint64_c in, string8_c & out )
		{
			out.character_list.construct_mode_dynamic( 17 );
			for ( sint32_c i = 0; i < 16; i++ )
			{
				out.character_list[ 16 - i ] = convert_digit_to_char8( static_cast< uint8_c >( in & 0x0F ) );
				in >>= 4;
			}
			out.character_list[ 16 ] = 0;
		}

		boolean_c convert_string8_to_float32( string8_c const & in, float32_c & out )
		{
			out = static_cast< float32_c >( atof( in.character_list.get_internal_array() ) );
			//out = static_cast< float32_c >( wcstod( in.character_list.get_array(), nullptr ) );
			return true;
		}

		void_c convert_float32_to_string8( float32_c in, string8_c & out )
		{
			char8_c buffer[ 32 ];
			if ( snprintf( buffer, 32, "%.4f", static_cast< float64_c >( in ) ) >= 0 )
			{
				out = buffer;
			}
		}

		boolean_c convert_string8_to_float32xn( string8_c const & in, core_list_c< float32_c > & out )
		{
			if ( out.get_mode_is_dynamic() )
			{
				out.remove_all();
			}
			else
			{
				assert( out.get_length() > 0 );
			}
			string8_c element_string;
			float32_c element_value = 0.0;
			sint32_c element_index = 0; // for writing to fixed length out.
			char8_c const * c = in.character_list.get_internal_array();
			while ( ( *c != 0 ) )
			{
				if ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
				{
					element_string = string8_c();
					while ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
					{
						element_string += *c;
						c++;
					}
					convert_string8_to_float32( element_string, element_value );
					if ( out.get_mode_is_dynamic() )
					{
						out.insert( -1, element_value );
					}
					else
					{
						if ( element_index < out.get_length() )
						{
							out[ element_index ] = element_value;
							element_index++;
							if ( element_index == out.get_length() )
							{
								return true;
							}
						}
					}
				}
				else
				{
					c++;
				}
			}
			return out.get_mode_is_dynamic();
		}

		void_c convert_float32xn_to_string8( core_list_c< float32_c > const & in, string8_c & out )
		{
			out = in.get_length() > 1 ? "(" : "";
			for ( sint32_c i = 0; i < in.get_length(); i++ )
			{
				if ( i > 0 )
				{
					out += ", ";
				}
				string8_c element_string;
				convert_float32_to_string8( in[ i ], element_string );
				out += element_string;
			}
			if ( in.get_length() > 1 )
			{
				out += ")";
			}
		}

		boolean_c convert_string8_to_float64( string8_c const & in, float64_c & out )
		{
			out = atof( in.character_list.get_internal_array() );
			return true;
		}

		void_c convert_float64_to_string8( float64_c const in, string8_c & out )
		{
			char8_c buffer[ 32 ];
			if ( snprintf( buffer, 32, "%.4f", in ) >= 0 )
			{
				out = buffer;
			}
		}

		boolean_c convert_string8_to_float64xn( string8_c const & in, core_list_c< float64_c > & out )
		{
			if ( out.get_mode_is_dynamic() )
			{
				out.remove_all();
			}
			else
			{
				assert( out.get_length() > 0 );
			}
			string8_c element_string;
			float64_c element_value = 0.0;
			sint32_c element_index = 0; // for writing to fixed length out.
			char8_c const * c = in.character_list.get_internal_array();
			while ( ( *c != 0 ) )
			{
				if ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
				{
					element_string = string8_c();
					while ( char16_is_decimal_digit( *c ) || *c == '.' || *c == '-' )
					{
						element_string += *c;
						c++;
					}
					convert_string8_to_float64( element_string, element_value );
					if ( out.get_mode_is_dynamic() )
					{
						out.insert( -1, element_value );
					}
					else
					{
						if ( element_index < out.get_length() )
						{
							out[ element_index ] = element_value;
							element_index++;
							if ( element_index == out.get_length() )
							{
								return true;
							}
						}
					}
				}
				else
				{
					c++;
				}
			}
			return out.get_mode_is_dynamic();
		}

		void_c convert_float64xn_to_string8( core_list_c< float64_c > const & in, string8_c & out )
		{
			out = in.get_length() > 1 ? "(" : "";
			for ( sint32_c i = 0; i < in.get_length(); i++ )
			{
				if ( i > 0 )
				{
					out += ", ";
				}
				string8_c element_string;
				convert_float64_to_string8( in[ i ], element_string );
				out += element_string;
			}
			if ( in.get_length() > 1 )
			{
				out += ")";
			}
		}

		boolean_c convert_string8_to_boolean( string8_c const & in, boolean_c & out )
		{
			if ( in == "true" || in == "1" )
			{
				out = true;
				return true;
			}
			else if ( in == "false" || in == "0" )
			{
				out = false;
				return true;
			}
			return false;
		}

		void_c convert_boolean_to_string8( boolean_c in, string8_c & out )
		{
			if ( in )
			{
				out = string8_c( core_list_mode_e_static, "true" );
			}
			else
			{
				out = string8_c( core_list_mode_e_static, "false" );
			}
		}

		boolean_c convert_rgb_to_hexadecimal_string8( vector64x3_c const & rgb, string8_c & string )
		{
			uint8_c r = static_cast< uint8_c >( math_saturate( rgb.a ) * 255.0 );
			uint8_c g = static_cast< uint8_c >( math_saturate( rgb.b ) * 255.0 );
			uint8_c b = static_cast< uint8_c >( math_saturate( rgb.c ) * 255.0 );
			string.character_list.construct_mode_dynamic( 7 );
			string.character_list[ 0 ] = convert_digit_to_char8( ( r >> 4 ) & 0x0f );
			string.character_list[ 1 ] = convert_digit_to_char8( r & 0x0f );
			string.character_list[ 2 ] = convert_digit_to_char8( ( g >> 4 ) & 0x0f );
			string.character_list[ 3 ] = convert_digit_to_char8( g & 0x0f );
			string.character_list[ 4 ] = convert_digit_to_char8( ( b >> 4 ) & 0x0f );
			string.character_list[ 5 ] = convert_digit_to_char8( b & 0x0f );
			string.character_list[ 6 ] = 0;
			return true;
		}

		boolean_c convert_rgba_to_hexadecimal_string8( vector64x4_c const & rgba, string8_c & string )
		{
			uint8_c r = static_cast< uint8_c >( math_saturate( rgba.a ) * 255.0 );
			uint8_c g = static_cast< uint8_c >( math_saturate( rgba.b ) * 255.0 );
			uint8_c b = static_cast< uint8_c >( math_saturate( rgba.c ) * 255.0 );
			uint8_c a = static_cast< uint8_c >( math_saturate( rgba.d ) * 255.0 );
			string.character_list.construct_mode_dynamic( 9 );
			string.character_list[ 0 ] = convert_digit_to_char8( ( r >> 4 ) & 0x0f );
			string.character_list[ 1 ] = convert_digit_to_char8( r & 0x0f );
			string.character_list[ 2 ] = convert_digit_to_char8( ( g >> 4 ) & 0x0f );
			string.character_list[ 3 ] = convert_digit_to_char8( g & 0x0f );
			string.character_list[ 4 ] = convert_digit_to_char8( ( b >> 4 ) & 0x0f );
			string.character_list[ 5 ] = convert_digit_to_char8( b & 0x0f );
			string.character_list[ 6 ] = convert_digit_to_char8( ( a >> 4 ) & 0x0f );
			string.character_list[ 7 ] = convert_digit_to_char8( a & 0x0f );
			string.character_list[ 8 ] = 0;
			return true;
		}

		boolean_c convert_string8_to_rgb( string8_c const & string, vector32x3_c & rgb )
		{
			vector32x4_c rgba;
			if ( convert_string8_to_rgba( string, rgba ) )
			{
				rgb = vector32x3_c( rgba.a, rgba.b, rgba.c );
				return true;
			}
			return false;
		}

		boolean_c convert_string8_to_rgb( string8_c const & string, vector64x3_c & rgb )
		{
			vector64x4_c rgba;
			if ( convert_string8_to_rgba( string, rgba ) )
			{
				rgb = vector64x3_c( rgba.a, rgba.b, rgba.c );
				return true;
			}
			return false;
		}

		boolean_c convert_string8_to_rgba( string8_c const & string, vector32x4_c & rgba )
		{
			vector64x4_c t;
			if ( convert_string8_to_rgba( string, t ) )
			{
				rgba = vector32x4_c( t );
				return true;
			}
			return false;
		}

		boolean_c convert_string8_to_rgba( string8_c const & string, vector64x4_c & rgba )
		{
			boolean_c hex = false;
			sint32_c hex_start = 0;
			if ( string8_starts_with( string.character_list.get_internal_array(), "#" ) )
			{
				hex = true;
				hex_start = 1;
			}
			else if ( string8_starts_with( string.character_list.get_internal_array(), "0x" ) )
			{
				hex = true;
				hex_start = 2;
			}
			else if ( string.get_length() == 3 || string.get_length() == 4 || string.get_length() == 6 || string.get_length() == 8 )
			{
				hex = true;
				for ( sint32_c i = 0; i < string.get_length(); i++ )
				{
					if ( !char16_is_hexadecimal_digit( string.character_list[ i ] ) )
					{
						hex = false;
						break;
					}
				}
			}
			if ( hex )
			{
				string8_c color_string = string8_sub_string( string, hex_start, string.get_length() - hex_start );

				uint32_c color_value = 0xFFFFFFFF;
				sint32_c r = ( color_value >> 24 ) & 0xFF;
				sint32_c g = ( color_value >> 16 ) & 0xFF;
				sint32_c b = ( color_value >> 8 ) & 0xFF;
				sint32_c a = ( color_value ) & 0xFF;
				if ( color_string.get_length() == 3 ) // RGB
				{
					r = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 0 ] ) ) << 4;
					g = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 1 ] ) ) << 4;
					b = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 2 ] ) ) << 4;
				}
				else if ( color_string.get_length() == 4 ) // RGBA
				{
					r = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 0 ] ) ) << 4;
					g = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 1 ] ) ) << 4;
					b = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 2 ] ) ) << 4;
					a = convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 3 ] ) ) << 4;
				}
				else if ( color_string.get_length() == 6 ) // RRGGBB
				{
					r = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 0 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 1 ] ) ) );
					g = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 2 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 3 ] ) ) );
					b = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 4 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 5 ] ) ) );
				}
				else if ( color_string.get_length() == 8 ) // RRGGBBAA
				{
					r = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 0 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 1 ] ) ) );
					g = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 2 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 3 ] ) ) );
					b = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 4 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 5 ] ) ) );
					a = ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 6 ] ) ) << 4 ) | ( convert_char8_to_digit( static_cast< char8_c >( color_string.character_list[ 7 ] ) ) );
				}
				rgba.a = static_cast< float32_c >( r ) / 255.0f;
				rgba.b = static_cast< float32_c >( g ) / 255.0f;
				rgba.c = static_cast< float32_c >( b ) / 255.0f;
				rgba.d = static_cast< float32_c >( a ) / 255.0f;
				return true;
			}
			else
			{
				core_list_c< float64_c > result;
				assert( result.get_mode_is_dynamic() );
				if ( convert_string8_to_float64xn( string, result ) )
				{
					if ( result.get_length() >= 3 )
					{
						rgba.a = result[ 0 ];
						rgba.b = result[ 1 ];
						rgba.c = result[ 2 ];
						rgba.d = result.get_length() >= 4 ? result[ 3 ] : 1.0;
						return true;
					}
				}
			}
			return false;
		}

		boolean_c char16_is_printable( char16_c a )
		{
			return !( ( a >= 0 && a <= 31 ) || ( a == 127 ) );
		}

		boolean_c char16_is_space( char16_c a )
		{
			return ( a == ' ' ) || ( a == '\t' );
		}

		boolean_c char16_is_latin_letter( char16_c a )
		{
			return ( ( a >= 'a' ) && ( a <= 'z' ) ) || ( ( a >= 'A' && a <= 'Z' ) );
		}

		boolean_c char16_is_decimal_digit( char16_c a )
		{
			return a >= '0' && a <= '9';
		}

		boolean_c char16_is_hexadecimal_digit( char16_c a )
		{
			return ( a >= '0' && a <= '9' ) || ( a >= 'a' && a <= 'f' ) || ( a >= 'A' && a <= 'F' );
		}

		boolean_c char16_is_punctuation( char16_c a )
		{
			return
				( a >= 0x0021 && a <= 0x002F ) ||
				( a >= 0x003A && a <= 0x0040 ) ||
				( a >= 0x005B && a <= 0x0060 ) ||
				( a >= 0x007B && a <= 0x007E ) ||
				( a >= 0x3000 && a <= 0x303F ); // "CJK Symbols and Punctuation"
		}

		boolean_c char16_is_control( char16_c a )
		{
			return iscntrl( a ) != 0;
		}

		boolean_c char8_compare( char8_c a, char8_c b, boolean_c case_sensitive )
		{
			return char16_compare( a, b, case_sensitive );
		}

		boolean_c char16_compare( char16_c a, char16_c b, boolean_c case_sensitive )
		{
			if ( !case_sensitive )
			{
				a = char16_to_lower( a );
				b = char16_to_lower( b );
			}
			return a == b;
		}

		char8_c char8_to_upper( char8_c a )
		{
			if ( a >= 'a' && a <= 'z' )
			{
				return a - ( 'a' - 'A' );
			}
			return a;
		}

		char16_c char16_to_upper( char16_c a )
		{
			if ( a >= 'a' && a <= 'z' )
			{
				return a - ( 'a' - 'A' );
			}
			return a;
		}

		char8_c char8_to_lower( char8_c a )
		{
			if ( a >= 'A' && a <= 'Z' )
			{
				return a + ( 'a' - 'A' );
			}
			return a;
		}

		char16_c char16_to_lower( char16_c a )
		{
			if ( a >= 'A' && a <= 'Z' )
			{
				return a + ( 'a' - 'A' );
			}
			return a;
		}

		void_c string8_to_upper( string8_c & string )
		{
			char8_c * c = string.character_list.get_internal_array();
			while ( *c )
			{
				*c = char8_to_upper( *c );
				c++;
			}
		}

		void_c string16_to_upper( string16_c & string )
		{
			char16_c * c = string.character_list.get_internal_array();
			while ( *c )
			{
				*c = char16_to_upper( *c );
				c++;
			}
		}

		void_c string8_to_lower( string8_c & string )
		{
			char8_c * c = string.character_list.get_internal_array();
			while ( *c )
			{
				*c = char8_to_lower( *c );
				c++;
			}
		}

		void_c string16_to_lower( string16_c & string )
		{
			char16_c * c = string.character_list.get_internal_array();
			while ( *c )
			{
				*c = char16_to_lower( *c );
				c++;
			}
		}

		void_c string8_pad_with_leading_zeros( string8_c & string, sint32_c desired_length )
		{
			sint32_c how_many = desired_length - string.get_length();
			if ( how_many > 0 )
			{
				string.character_list.emplace( 0, how_many );
				char8_c * c = string.character_list.get_internal_array();
				for ( sint32_c i = 0; i < how_many; i++ )
				{
					c[ i ] = 0;
				}
			}
		}

		template< typename string_type_c >
		string_type_c _string_trim_front( string_type_c const & string )
		{
			string_type_c result;
			sint32_c length = string.get_length();
			sint32_c trim_front_count = 0;
			for ( sint32_c i = 0; i < length; i++ )
			{
				auto b = string.character_list[ i ];
				if ( char16_is_space( b ) )
				{
					trim_front_count++;
				}
				else
				{
					break;
				}
			}
			result = _string_sub_string< string_type_c >( string, trim_front_count, length - trim_front_count );
			return result;
		}

		template< typename string_type_c >
		string_type_c _string_trim_back( string_type_c const & string )
		{
			string_type_c result;
			sint32_c length = string.get_length();
			sint32_c trim_end_length = 0;
			for ( sint32_c i = length - 1; i >= 0; i-- )
			{
				auto b = string.character_list[ i ];
				if ( char16_is_space( b ) )
				{
					trim_end_length++;
				}
				else
				{
					break;
				}
			}
			result = _string_sub_string< string_type_c >( result, 0, length - trim_end_length );
			return result;
		}

		template< typename string_type_c >
		string_type_c _string_trim( string_type_c const & string )
		{
			string_type_c result;
			sint32_c start = 0;
			sint32_c end = result.get_length();
			while ( start <= end )
			{
				auto b = string.character_list[ start ];
				if ( char16_is_space( b ) )
				{
					start++;
				}
				else
				{
					break;
				}
			}
			while ( end > start )
			{
				auto b = string.character_list[ end ];
				if ( char16_is_space( b ) )
				{
					end--;
				}
				else
				{
					break;
				}
			}
			result = string;
			result.character_list.remove( -1, end - start );
			result.character_list.remove( 0, start );
			return result;
		}

		string8_c string8_trim_front( string8_c const & string )
		{
			return _string_trim_front< string8_c >( string );
		}

		string8_c string8_trim_back( string8_c const & string )
		{
			return _string_trim_back< string8_c >( string );
		}

		string8_c string8_trim( string8_c const & string )
		{
			return _string_trim< string8_c >( string );
		}

		string16_c string16_trim_front( string16_c const & string )
		{
			return _string_trim_front< string16_c >( string );
		}

		string16_c string16_trim_back( string16_c const & string )
		{
			return _string_trim_back< string16_c >( string );
		}

		string16_c string16_trim( string16_c const & string )
		{
			return _string_trim< string16_c >( string );
		}

		template< typename string_type_c >
		boolean_c _string_find_index_of( string_type_c const & string, string_type_c const & find, sint32_c & result )
		{
			result = -1;
			sint32_c i = 0; // current position in string.
			sint32_c j = 0; // current position in needle.
			auto const * a = string.character_list.get_internal_array();
			auto const * b = find.character_list.get_internal_array();
			while ( a[ i ] )
			{
				j = 0;
				while ( b[ j ] )
				{
					if ( a[ i + j ] == b[ j ] )
					{
						j++;
						if ( b[ j ] == 0 )
						{
							result = i;
							return true;
						}
					}
					else
					{
						break;
					}
				}
				i++;
			}
			return false;
		}

		boolean_c string8_find_index_of( string8_c const & string, string8_c const & find, sint32_c & result )
		{
			return _string_find_index_of< string8_c >( string, find, result );
		}

		boolean_c string16_find_index_of( string16_c const & string, string16_c const & find, sint32_c & result )
		{
			return _string_find_index_of< string16_c >( string, find, result );
		}

		template< typename string_type_c >
		boolean_c _string_find_indices_of( string_type_c const & string, string_type_c const & find, core_list_c< sint32_c > & result )
		{
			result.remove_all();
			sint32_c i = 0; // current position in string.
			sint32_c j = 0; // current position in needle.
			auto const * a = string.character_list.get_internal_array();
			auto const * b = find.character_list.get_internal_array();
			while ( a[ i ] )
			{
				j = 0;
				while ( a[ i + j ] == b[ j ] )
				{
					j++;
					if ( b[ j ] == 0 )
					{
						result.insert( -1, i );
						i += j;
						break;
					}
				}
				i++;
			}
			return result.get_length() != 0;
		}

		boolean_c string8_find_indices_of( string8_c const & string, string8_c const & find, core_list_c< sint32_c > & result )
		{
			return _string_find_indices_of< string8_c >( string, find, result );
		}

		boolean_c string16_find_indices_of( string16_c const & string, string16_c const & find, core_list_c< sint32_c > & result )
		{
			return _string_find_indices_of< string16_c >( string, find, result );
		}

		template< typename string_type_c >
		string_type_c _string_find_and_replace_all( string_type_c const & string, string_type_c const & find, string_type_c const & replace, boolean_c case_sensitive )
		{
			assert( find.get_length() > 0 );
			string_type_c result;
			result.character_list.remove_all();
			auto const * s = string.character_list.get_internal_array();
			auto const * f = find.character_list.get_internal_array();
			auto const * r = replace.character_list.get_internal_array();
			sint32_c i = 0; // current position in string.
			while ( s[ i ] )
			{
				sint32_c j = 0; // current position in needle, and current offset relative to current position in string.
				while ( char16_compare( s[ i + j ], f[ j ], case_sensitive ) )
				{
					j++;
					if ( f[ j ] == 0 )
					{
						sint32_c k = 0;
						while ( r[ k ] )
						{
							result.character_list.insert( -1, r[ k ] );
							k++;
						}
						i += j;
						goto replaced;
					}
				}
				result.character_list.insert( -1, s[ i ] );
				i++;
			replaced:
				continue;
			}
			result.character_list.insert( -1, 0 );
			return result;
		}

		string8_c string8_find_and_replace_all( string8_c const & string, string8_c const & find, string8_c const & replace, boolean_c case_sensitive )
		{
			return _string_find_and_replace_all< string8_c >( string, find, replace, case_sensitive );
		}

		string16_c string16_find_and_replace_all( string16_c const & string, string16_c const & find, string16_c const & replace, boolean_c case_sensitive )
		{
			return _string_find_and_replace_all< string16_c >( string, find, replace, case_sensitive );
		}

		template< typename string_type_c >
		void_c _string_split_at_spaces( string_type_c const & string, core_list_c< string_type_c > & result )
		{
			result.remove_all();
			sint32_c sub_string_start = 0;
			sint32_c sub_string_count = 0;
			sint32_c length = string.get_length();
			for ( sint32_c i = 0; i < length; i++ )
			{
				if ( char16_is_space( string.character_list[ i ] ) )
				{
					if ( sub_string_count > 0 )
					{
						result.insert( -1, _string_sub_string< string_type_c >( string, sub_string_start, sub_string_count ) );
					}
					sub_string_count = 0;
				}
				else
				{
					if ( sub_string_count == 0 )
					{
						sub_string_start = i;
					}
					sub_string_count++;
				}
			}
			if ( sub_string_count > 0 )
			{
				result.insert( -1, _string_sub_string< string_type_c >( string, sub_string_start, sub_string_count ) );
			}
		}

		template< typename string_type_c >
		void_c _string_split_at_delimiter( string_type_c const & string, string_type_c const & delimiter, core_list_c< string_type_c > & result )
		{
			result.remove_all();
			if ( delimiter.get_length() >= string.get_length() )
			{
				result.insert( -1, string );
			}
			else
			{
				sint32_c sub_string_start = 0;
				sint32_c sub_string_count = 0;
				sint32_c j = 0;
				sint32_c length = string.get_length();
				for ( sint32_c i = 0; i < length; i++ )
				{
					if ( delimiter.character_list[ j ] == string.character_list[ i ] )
					{
						j++;
						sub_string_count++;
						if ( j == delimiter.get_length() )
						{
							sub_string_count -= j;
							if ( sub_string_count > 0 )
							{
								result.insert( -1, _string_sub_string< string_type_c >( string, sub_string_start, sub_string_count ) );
							}
							j = 0;
							sub_string_start = i + 1;
							sub_string_count = 0;
						}
					}
					else
					{
						j = 0;
						sub_string_count++;
					}
				}
				if ( sub_string_count > 0 )
				{
					result.insert( -1, _string_sub_string< string_type_c >( string, sub_string_start, sub_string_count ) );
				}
			}
		}

		void_c string8_split_at_spaces( string8_c const & string, core_list_c< string8_c > & result )
		{
			_string_split_at_spaces< string8_c >( string, result );
		}

		void_c string8_split_at_delimiter( string8_c const & string, string8_c const & delimiter, core_list_c< string8_c > & result )
		{
			_string_split_at_delimiter< string8_c >( string, delimiter, result );
		}

		void_c string16_split_at_spaces( string16_c const & string, core_list_c< string16_c > & result )
		{
			_string_split_at_spaces< string16_c >( string, result );
		}

		void_c string16_split_at_delimiter( string16_c const & string, string16_c const & delimiter, core_list_c< string16_c > & result )
		{
			_string_split_at_delimiter< string16_c >( string, delimiter, result );
		}

		template< typename string_type_c >
		string_type_c _string_sub_string( string_type_c const & string, sint32_c start, sint32_c length )
		{
			assert( start >= 0 && length >= 0 && start + length <= string.get_length() );
			string_type_c result;
			result.character_list.construct_mode_dynamic_from_array( &string.character_list.get_internal_array()[ start ], length );
			result.character_list.insert( -1, 0 );
			return result;
		}

		string8_c string8_sub_string( string8_c const & string, sint32_c start, sint32_c length )
		{
			return _string_sub_string< string8_c >( string, start, length );
		}

		string16_c string16_sub_string( string16_c const & string, sint32_c start, sint32_c length )
		{
			return _string_sub_string< string16_c >( string, start, length );
		}


		template< typename char_type_c >
		boolean_c _string_starts_with( char_type_c const * a, char_type_c const * b )
		{
			while ( *a && *b )
			{
				if ( *a != *b )
				{
					return false;
				}
				a++;
				b++;
			}
			return *b == 0;
		}


		boolean_c string8_starts_with( char8_c const * a, char8_c const * b )
		{
			return _string_starts_with< char8_c >( a, b );
		}

		boolean_c string8_starts_with( char8_c const * a, char16_c const * b )
		{
			char8_c c_buffer[ 3 ];
			while ( *a && *b )
			{
				char8_c * c_buffer_pointer = c_buffer;
				sint8_c byte_count = convert_char16_to_utf8( *b, c_buffer_pointer );
				for ( sint8_c i = 0; i < byte_count; i++ )
				{
					if ( a[ i ] != c_buffer[ i ] )
					{
						return false;
					}
				}
				a += byte_count;
				b += 1;
			}
			return *b == 0;// || *a == *b; // *b will == 0 if pass, *a will == *b if both are 0 which is also pass.
		}

		boolean_c string16_starts_with( char16_c const * a, char8_c const * b )
		{
			while ( *a && *b )
			{
				char16_c c;
				convert_utf8_to_char16( b, c );
				if ( *a != c )
				{
					return false;
				}
				a += 1;
			}
			return *b == 0;// || *a == *b; // *b will == 0 if pass, *a will == *b if both are 0 which is also pass.
		}

		boolean_c string16_starts_with( char16_c const * a, char16_c const * b )
		{
			return _string_starts_with< char16_c >( a, b );
		}

		template< typename char_type_c >
		boolean_c _string_ends_with( char_type_c const * a, char_type_c const * b )
		{
			sint32_c a_length = _string_find_length< char_type_c >( a );
			sint32_c b_length = _string_find_length< char_type_c >( b );
			sint32_c a_index = a_length - 1;
			sint32_c b_index = b_length - 1;
			while ( a_index >= 0 && b_index >= 0 )
			{
				if ( a[ a_index ] != b[ b_index ] )
				{
					return false;
				}
				a_index--;
				b_index--;
			}
			return b_index == -1 || a_index == b_index; // b_index will == -1 if pass, a_index will == b_index if both are -1, which is also pass.
		}

		boolean_c string8_ends_with( char8_c const * a, char8_c const * b )
		{
			return _string_ends_with< char8_c >( a, b );
		}

		boolean_c string8_ends_with( char8_c const * a, char16_c const * b )
		{
			sint32_c a_length = string8_find_length( a );
			sint32_c b_length = string16_find_length( b );

			sint32_c a_index = a_length - 1;
			sint32_c b_index = b_length - 1;

			char8_c b_buffer[ 3 ];
			while ( a_index >= 0 && b_index >= 0 )
			{
				char8_c * b_buffer_pointer = b_buffer;
				sint8_c b_bytes_length = convert_char16_to_utf8( b[ b_index ], b_buffer_pointer );
				sint8_c b_bytes_index = b_bytes_length - 1;
				while ( a_index >= 0 && b_bytes_index >= 0 )
				{
					if ( a[ a_index ] != b_buffer[ b_bytes_index ] )
					{
						return false;
					}
					a_index -= 1;
					b_bytes_index -= 1;
				}
				b_index -= 1;
			}
			return b_index == -1 || a_index == b_index; // b_index will == -1 if pass, a_index will == b_index if both are -1, which is also pass.
		}

		boolean_c string16_ends_with( char16_c const * a, char8_c const * b )
		{
			0x80; // 1 byte sequence: first byte signature mask, 1 bit, for 1 byte sequence.

			0xC0; // 2 byte sequence: first byte signature, 3 bits.
			0xE0; // 2 byte sequence: first byte signature mask, 3 bits.
			
			0xE0; // 3 byte sequence: first byte signature, 4 bits.
			0xF0; // 3 byte sequence: first byte sitnature bit mask, 4 bits.

			0x80; // 2 and 3 byte sequences: subsequent byte signature, 2 bits.
			0xC0; // 2 and 3 byte sequences: subsequent byte sitnature bit mask, 2 bits.

			sint32_c a_length = string16_find_length( a );
			sint32_c b_length = string8_find_length( b );

			sint32_c a_index = a_length - 1;
			sint32_c b_index = b_length - 1;

			while ( a_index >= 0 && b_index >= 0 )
			{
				char16_c b_character = 0;
				char8_c const * b_buffer = nullptr;
				if ( ( b[ b_index ] & 0x80 ) == 0x00 )
				{
					// 1 byte sequence.
					b_buffer = &b[ b_index ];
					b_index -= 1;
				}
				else if ( ( b[ b_index ] & 0xC0 ) == 0x80 )
				{
					if ( ( b_index >= 1 ) && ( b[ b_index - 1 ] & 0xE0 ) == 0xC0 )
					{
						// 2 byte sequence
						b_buffer = &b[ b_index - 1 ];
						b_index -= 2;
					}
					else if ( ( b_index >= 1 ) && ( b[ b_index - 1 ] & 0xC0 ) == 0x80 )
					{
						if ( ( b_index >= 2 ) && ( b[ b_index - 2 ] & 0xF0 ) == 0xE0 )
						{
							// 3 byte sequence.
							b_buffer = &b[ b_index - 2 ];
							b_index -= 3;
						}
						else
						{
							assert( false );
						}
					}
					else
					{
						assert( false );
					}
				}
				else
				{
					assert( false );
				}
				convert_utf8_to_char16( b_buffer, b_character );
				if ( a[ a_index ] != b_character )
				{
					return false;
				}
				a_index -= 1;
			}
			return b_index == -1 || a_index == b_index; // b_index will == -1 if pass, a_index will == b_index if both are -1, which is also pass.
		}

		boolean_c string16_ends_with( char16_c const * a, char16_c const * b )
		{
			return _string_ends_with< char16_c >( a, b );
		}

		boolean_c string8_sort_compare( string8_c const & a, string8_c const & b )
		{
			return strnatcmp::strnatcmp0< char8_c >( a.character_list.get_internal_array(), b.character_list.get_internal_array(), false ) > 0;
		}

		boolean_c string8_sort_compare_case_insensitive( string8_c const & a, string8_c const & b )
		{
			return strnatcmp::strnatcmp0< char8_c >( a.character_list.get_internal_array(), b.character_list.get_internal_array(), true ) > 0;
		}

		boolean_c string16_sort_compare( string16_c const & a, string16_c const & b )
		{
			return strnatcmp::strnatcmp0< char16_c >( a.character_list.get_internal_array(), b.character_list.get_internal_array(), false ) > 0;
		}

		boolean_c string16_sort_compare_case_insensitive( string16_c const & a, string16_c const & b )
		{
			return strnatcmp::strnatcmp0< char16_c >( a.character_list.get_internal_array(), b.character_list.get_internal_array(), true ) > 0;
		}

		boolean_c string8_compare( char8_c const * a, char8_c const * b )
		{
			assert( a != nullptr && b != nullptr );

			if ( a == b )
			{
				return true;
			}
			while ( *a && *b )
			{
				if ( *a != *b )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;
		}

		boolean_c string8_compare_case_insensitive( char8_c const * a, char8_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			if ( a == b )
			{
				return true;
			}
			while ( *a && *b )
			{
				if ( char8_to_lower( *a ) != char8_to_lower( *b ) )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;
		}

		boolean_c string16_compare( char16_c const * a, char16_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			if ( a == b )
			{
				return true;
			}
			while ( *a && *b )
			{
				if ( *a != *b )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;
		}

		boolean_c string16_compare_case_insensitive( char16_c const * a, char16_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			if ( a == b )
			{
				return true;
			}
			while ( *a && *b )
			{
				if ( char16_to_lower( *a ) != char16_to_lower( *b ) )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;;
		}

		boolean_c string16_compare( char16_c const * a, char8_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			while ( *a != 0 && *b != 0 )
			{
				char16_c character;
				convert_utf8_to_char16( b, character );
				if ( *a != character )
				{
					return false;
				}
				a++;
			}
			return *a == *b;
		}

		boolean_c string16_compare_case_insensitive( char16_c const * a, char8_c const * b )
		{
			assert( a != nullptr && b != nullptr );
			while ( *a != 0 && *b != 0 )
			{
				if ( char16_to_lower( *a ) != char16_to_lower( *b ) )
				{
					return false;
				}
				a++;
				b++;
			}
			return *a == *b;
		}

		template< typename char_type_c >
		sint32_c _string_find_length( char_type_c const * string )
		{
			assert( string );
			char_type_c const * string_start = string;
			while ( *string != 0 )
			{
				string++;
			}
			return static_cast< sint32_c >( string - string_start );
		}

		sint32_c string8_find_length( char8_c const * string )
		{
			return _string_find_length< char8_c >( string );
		}

		sint32_c string16_find_length( char16_c const * string )
		{
			return _string_find_length< char16_c >( string );
		}

		boolean_c string8_is_valid_for_key( string8_c const & string )
		{
			sint32_c string_length = string.get_length();
			for ( sint32_c i = 0; i < string_length; i++ )
			{
				char8_c character = string.character_list[ i ];
				if ( !( ( character >= 'a' && character <= 'z' ) || ( character >= 'A' && character <= 'Z' ) || ( character >= '0' && character <= '9' ) || ( character == '_' || character == '-' || character == '.' || character == '[' || character == ']' ) ) )
				{
					// multi-byte utf-8 encodings characters will reach here too.
					return false;
				}
			}
			return true;
		}

		uint8_c convert_utf8_to_char16( char8_c const * & in, char16_c & out )
		{
			assert( in != nullptr );
			out = 0;
			if ( ( in[ 0 ] & 0x80 ) == 0x00 )
			{
				// out 7 bits
				// in 1 byte
				// 0bbbbbbb
				assert( in[ 0 ] != 0 );
				out |= static_cast< char16_c >( in[ 0 ] );
				in += 1;
				return 1;
			}
			else if ( ( in[0] & 0xE0 ) == 0xC0 )
			{
				// out 11 bits
				// in 2 bytes
				// 110bbbbb 10bbbbbb
				assert( in[ 0 ] != 0 );
				assert( in[ 1 ] != 0 );
				out |= static_cast<char16_c>( in[ 0 ] & 0x1F ) << 6;
				out |= static_cast<char16_c>( in[ 1 ] & 0x3F );
				in += 2;
				return 2;
			}
			else if ( ( in[0] & 0xF0 ) == 0xE0 )
			{
				// out 16 bits
				// in 3 bytes
				// 1110bbbb 10bbbbbb 10bbbbbb
				assert( in[ 0 ] != 0 );
				assert( in[ 1 ] != 0 );
				assert( in[ 2 ] != 0 );
				out |= static_cast<char16_c>( in[ 0 ] & 0x0F ) << 12;
				out |= static_cast<char16_c>( in[ 1 ] & 0x3F ) << 6;
				out |= static_cast<char16_c>( in[ 2 ] & 0x3F );
				in += 3;
				return 3;
			}
			assert( false );
			return 0;
		}

		void_c append_string8_to_string16( core_list_c< char8_c > const & in, core_list_c< char16_c > & out )
		{
			assert( out.get_mode_is_dynamic() == true );

			boolean_c has_null_terminator = false;
			if ( in.get_length() > 0 )
			{
				has_null_terminator = in[ in.get_length() - 1 ] == '\0';
			}

			char8_c const * byte = in.get_internal_array();
			char8_c const * byte_end = in.get_internal_array() + in.get_length();
			while ( byte < byte_end && *byte != '\0' )
			{
				char16_c character;
				convert_utf8_to_char16( byte, character );
				out.insert( -1, character );
			}

			if ( has_null_terminator )
			{
				out.insert( -1, '\0' );
			}
		}

		sint8_c convert_char16_to_utf8( char16_c in, char8_c out[ 3 ] )
		{
			if ( in < 0x80 )
			{
				// in 7 bits
				// out 1 byte
				// 0bbbbbbb
				out[ 0 ] = static_cast< char8_c >( in );
				out += 1;
				return 1;
			}
			else if ( in < 0x800 )
			{
				// in 11 bits
				// out 2 bytes
				// 110bbbbb 10bbbbbb
				out[ 1 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
				out[ 0 ] = static_cast< char8_c >( in | 0xC0 );
				out += 2;
				return 2;
			}
			else if ( in < 0x10000 )
			{
				// in 16 bits
				// out 3 bytes
				// 1110bbbb 10bbbbbb 10bbbbbb
				out[ 2 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
				out[ 1 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
				out[ 0 ] = static_cast< char8_c >( in | 0xE0 );
				out += 3;
				return 3;
			}
			//else if ( in < 0x110000 )
			//{
			//	out[ 3 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
			//	out[ 2 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
			//	out[ 1 ] = static_cast< char8_c >( ( in | 0x80 ) & 0xBF ); in >>= 6;
			//	out[ 0 ] = static_cast< char8_c >( in | 0xF0 );
			//	out += 4;
			//}
			assert( false );
			return 0;
		}

		void_c append_string16_to_string8( core_list_c< char16_c > const & in, core_list_c< char8_c > & out )
		{
			assert( out.get_mode_is_dynamic() == true );

			boolean_c has_null_terminator = false;
			if ( in.get_length() > 0 )
			{
				has_null_terminator = in[ in.get_length() - 1 ] == '\0';
			}

			char16_c const * char16 = in.get_internal_array();
			char16_c const * char16_end = in.get_internal_array() + in.get_length();
			char8_c char8_buffer_base[ 4 ];
			while ( char16 < char16_end && *char16 != '\0' )
			{
				char8_c * char8_buffer = char8_buffer_base;
				sint8_c bytes_count = convert_char16_to_utf8( *char16, char8_buffer );
				for ( sint8_c i = 0; i < bytes_count; i++ )
				{
					out.insert( -1, char8_buffer_base[ i ] );
				}
				char16++;
			}

			if ( has_null_terminator )
			{
				out.insert( -1, '\0' );
			}
		}

	}
}
