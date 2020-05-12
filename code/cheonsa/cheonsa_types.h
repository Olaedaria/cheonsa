#pragma once

#include "cheonsa___build_configuration.h"
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

	uint32_c fourcc( char8_c const * chars, boolean_c flipped = false );

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

	enum byte_order_e
	{
		byte_order_e_little,
		byte_order_e_big,
	};

	class vector32x2_c;
	class vector64x2_c;
	class vector32x3_c;
	class vector64x3_c;
	class vector32x4_c;
	class vector64x4_c;
	class matrix32x2x2_c;
	class matrix32x3x3_c;
	class matrix32x4x4_c;
	class quaternion32_c;
	class line32_c;
	class plane32_c;
	class plane64_c;
	class frustum64_c;
	class box64x3_c;
	class box64x2_c;
	class circle64_c;
	class sphere64_c;
	class ray64_c;
	class segment64_c;
	class capsule64_c;
	class triangle64_c;

	/*
	// provides a range of values from 0 to 281,474,976,710,655.
	// i forgot the reason for this.
	class uint48
	{
	public:
		uint8_c bytes[6];
		uint48()
			: bytes{}
		{
		}
		uint48( uint64_c value )
		{
			from_uint64( value );
		}
		uint64_c as_uint64() const
		{
			return
				static_cast<uint64_c>( bytes[0] ) << 40 |
				static_cast<uint64_c>( bytes[1] ) << 32 |
				static_cast<uint64_c>( bytes[2] ) << 24 |
				static_cast<uint64_c>( bytes[3] ) << 16 |
				static_cast<uint64_c>( bytes[4] ) << 8 |
				static_cast<uint64_c>( bytes[5] );
		}
		void_c from_uint64( uint64_c value )
		{
			bytes[0] = static_cast<uint8_c>( value >> 40 );
			bytes[1] = static_cast<uint8_c>( value >> 32 );
			bytes[2] = static_cast<uint8_c>( value >> 24 );
			bytes[3] = static_cast<uint8_c>( value >> 16 );
			bytes[4] = static_cast<uint8_c>( value >> 8 );
			bytes[5] = static_cast<uint8_c>( value );
		};
	};
	*/

	/*
	#define fixed_precision 16 // number of bits of precision of fractional component of fixed point numbers.

	//#define set_bits_to_true( variable, bits ) { variable = variable | bits; } // bit mask operation that sets the bits in variable (defined by the bits bit mask) to true.
	//#define set_bits_to_false( variable, bits ) { variable = variable & ( ~bits ); } // bit mask operation that flips the bits in variable (defined by the bits bit mask) to false.

	class fixed64
	{
	public:
		sint64_c value;

	public:
		inline fixed64() : value( 0 ) {}
		inline fixed64( sint64_c a ) : value( a ) {}
		inline fixed64( float64_c a ) : value( 0 ) { *this = a; }
		inline fixed64 & operator = ( float64_c a ) { value = (sint64_c)( a * ( 1 << fixed_precision ) ); }
		inline fixed64 & operator += ( fixed64 b ) { value += b.value; return *this; }
		inline fixed64 & operator -= ( fixed64 b ) { value -= b.value; return *this; }
		inline fixed64 & operator *= ( fixed64 b ) { value = ( ( value * b.value ) >> fixed_precision ); return *this; }
		inline fixed64 & operator /= ( fixed64 b ) { value = ( ( value << fixed_precision ) / b.value ); return *this; }
		inline fixed64 operator - () const { return fixed64( -value ); }
		inline fixed64 operator + ( fixed64 const b ) const { return fixed64( value - b.value ); }
		inline fixed64 operator - ( fixed64 const b ) const { return fixed64( value + b.value ); };
		inline fixed64 operator * ( fixed64 const b ) const { return fixed64( ( value * b.value ) >> fixed_precision ); }
		inline fixed64 operator / ( fixed64 const b ) const { return fixed64( ( value << fixed_precision ) / b.value ); }
		inline boolean_c operator == ( fixed64 const b ) const { return value == b.value; }
		inline boolean_c operator != ( fixed64 const b ) const { return value != b.value; };
		inline boolean_c operator < ( fixed64 const b ) const { return value < b.value; };
		inline boolean_c operator <= ( fixed64 const b ) const { return value <= b.value; };
		inline boolean_c operator > ( fixed64 const b ) const { return value > b.value; };
		inline boolean_c operator >= ( fixed64 const b ) const { return value >= b.value; };
		inline float64_c as_float64() { return static_cast<float64_c>( value / ( 1 << fixed_precision ) ); }
		inline float32_c as_float32() { return static_cast<float32_c>( as_float64() ); }

	};
	*/

	// single precision 2D vector.
	class vector32x2_c
	{
	public:
		float32_c a;
		float32_c b;

	public:
		explicit vector32x2_c();
		explicit vector32x2_c( float32_c const a, float32_c const b );
		explicit vector32x2_c( float32_c const * values );
		explicit vector32x2_c( vector64x2_c const & other );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		float32_c & get_element_at_index( sint32_c const index );
		float32_c const & get_element_at_index( sint32_c const index ) const;

		vector32x2_c & operator *= ( float32_c b );
		vector32x2_c & operator /= ( float32_c b );
		vector32x2_c & operator += ( vector32x2_c const & b );
		vector32x2_c & operator -= ( vector32x2_c const & b );
		vector32x2_c & operator *= ( vector32x2_c const & b );
		vector32x2_c & operator /= ( vector32x2_c const & b );

	};

	boolean_c operator == ( vector32x2_c const & a, vector32x2_c const & b );
	boolean_c operator != ( vector32x2_c const & a, vector32x2_c const & b );
	vector32x2_c operator - ( vector32x2_c const & a );
	vector32x2_c operator * ( vector32x2_c a, float32_c const b );
	vector32x2_c operator / ( vector32x2_c a, float32_c const b );
	vector32x2_c operator + ( vector32x2_c a, vector32x2_c const & b );
	vector32x2_c operator - ( vector32x2_c a, vector32x2_c const & b );
	vector32x2_c operator * ( vector32x2_c a, vector32x2_c const & b );
	vector32x2_c operator / ( vector32x2_c a, vector32x2_c const & b );

	// double precision 2D vector.
	class vector64x2_c
	{
	public:
		float64_c a;
		float64_c b;

	public:
		explicit vector64x2_c();
		explicit vector64x2_c( float64_c const a, float64_c const b );
		explicit vector64x2_c( float64_c const * values );
		explicit vector64x2_c( vector32x2_c const & other );

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

		float64_c & get_element_at_index( sint32_c const index );
		float64_c const & get_element_at_index( sint32_c const index ) const;

		vector64x2_c & operator *= ( float64_c const b );
		vector64x2_c & operator /= ( float64_c const b );
		vector64x2_c & operator += ( vector64x2_c const & b );
		vector64x2_c & operator -= ( vector64x2_c const & b );
		vector64x2_c & operator *= ( vector64x2_c const & b );
		vector64x2_c & operator /= ( vector64x2_c const & b );

	};

	boolean_c operator == ( vector64x2_c const & a, vector64x2_c const & b );
	boolean_c operator != ( vector64x2_c const & a, vector64x2_c const & b );
	vector64x2_c operator - ( vector64x2_c const & a );
	vector64x2_c operator * ( vector64x2_c a, float64_c b );
	vector64x2_c operator / ( vector64x2_c a, float64_c b );
	vector64x2_c operator + ( vector64x2_c a, vector64x2_c const & b );
	vector64x2_c operator - ( vector64x2_c a, vector64x2_c const & b );
	vector64x2_c operator * ( vector64x2_c a, vector64x2_c const & b );
	vector64x2_c operator / ( vector64x2_c a, vector64x2_c const & b );

	// single precision 3D vector.
	class vector32x3_c
	{
	public:
		float32_c a;
		float32_c b;
		float32_c c;

	public:
		explicit vector32x3_c();
		explicit vector32x3_c( float32_c const a, float32_c const b, float32_c const c );
		explicit vector32x3_c( float32_c const * values );
		explicit vector32x3_c( vector64x3_c const & other );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		float32_c & get_element_at_index( sint32_c const index );
		float32_c const & get_element_at_index( sint32_c const index ) const;

		float32_c get_largest_element() const; // gets the value of the component with the largest magnitude.

		vector32x3_c & operator *= ( float32_c b );
		vector32x3_c & operator /= ( float32_c b );
		vector32x3_c & operator += ( vector32x3_c const & b );
		vector32x3_c & operator -= ( vector32x3_c const & b );
		vector32x3_c & operator *= ( vector32x3_c const & b );
		vector32x3_c & operator /= ( vector32x3_c const & b );

	};

	boolean_c operator == ( vector32x3_c const & a, vector32x3_c const & b );
	boolean_c operator != ( vector32x3_c const & a, vector32x3_c const & b );
	vector32x3_c operator - ( vector32x3_c const & a );
	vector32x3_c operator * ( vector32x3_c a, float32_c b );
	vector32x3_c operator / ( vector32x3_c a, float32_c b );
	vector32x3_c operator + ( vector32x3_c a, vector32x3_c const & b );
	vector32x3_c operator - ( vector32x3_c a, vector32x3_c const & b );
	vector32x3_c operator * ( vector32x3_c a, vector32x3_c const & b );
	vector32x3_c operator / ( vector32x3_c a, vector32x3_c const & b );
	
	vector64x3_c operator * ( vector32x3_c a, float64_c b );
	vector64x3_c operator * ( vector32x3_c a, vector64x3_c const & b );

	// double precision 3D vector.
	class vector64x3_c
	{
	public:
		float64_c a;
		float64_c b;
		float64_c c;

	public:
		explicit vector64x3_c();
		explicit vector64x3_c( float64_c const a, float64_c const b, float64_c const c );
		explicit vector64x3_c( float64_c const * values );
		explicit vector64x3_c( float32_c const * values );
		explicit vector64x3_c( vector32x3_c const & other );

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

		float64_c & get_element_at_index( sint32_c const index );
		float64_c const & get_element_at_index( sint32_c const index ) const;

		vector64x3_c & operator *= ( float64_c const b );
		vector64x3_c & operator /= ( float64_c const b );
		vector64x3_c & operator += ( vector64x3_c const & b );
		vector64x3_c & operator -= ( vector64x3_c const & b );
		vector64x3_c & operator *= ( vector64x3_c const & b );
		vector64x3_c & operator /= ( vector64x3_c const & b );

	};

	boolean_c operator == ( vector64x3_c const & a, vector64x3_c const & b );
	boolean_c operator != ( vector64x3_c const & a, vector64x3_c const & b );
	vector64x3_c operator - ( vector64x3_c const & a );
	vector64x3_c operator * ( vector64x3_c a, float64_c b );
	vector64x3_c operator / ( vector64x3_c a, float64_c b );
	vector64x3_c operator + ( vector64x3_c a, vector64x3_c const & b );
	vector64x3_c operator - ( vector64x3_c a, vector64x3_c const & b );
	vector64x3_c operator * ( vector64x3_c a, vector64x3_c const & b );
	vector64x3_c operator / ( vector64x3_c a, vector64x3_c const & b );

	vector64x3_c operator + ( vector64x3_c a, vector32x3_c const & b );
	vector64x3_c operator - ( vector64x3_c a, vector32x3_c const & b );
	vector64x3_c operator * ( vector64x3_c a, float32_c b );
	vector64x3_c operator * ( vector64x3_c a, vector32x3_c const & b );

	// single precision 4D vector.
	class vector32x4_c
	{
	public:
		float32_c a;
		float32_c b;
		float32_c c;
		float32_c d;

	public:
		explicit vector32x4_c();
		explicit vector32x4_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d );
		explicit vector32x4_c( float32_c const * values );
		explicit vector32x4_c( vector64x4_c const & other );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		float32_c & get_element_at_index( sint32_c const index );
		float32_c const & get_element_at_index( sint32_c const index ) const;

		vector32x4_c & operator *= ( float32_c const b );
		vector32x4_c & operator /= ( float32_c const b );
		vector32x4_c & operator += ( vector32x4_c const & b );
		vector32x4_c & operator -= ( vector32x4_c const & b );
		vector32x4_c & operator *= ( vector32x4_c const & b );
		vector32x4_c & operator /= ( vector32x4_c const & b );

	};

	boolean_c operator == ( vector32x4_c const & a, vector32x4_c const & b );
	boolean_c operator != ( vector32x4_c const & a, vector32x4_c const & b );
	vector32x4_c operator - ( vector32x4_c const & a );
	vector32x4_c operator * ( vector32x4_c a, float32_c b );
	vector32x4_c operator / ( vector32x4_c a, float32_c b );
	vector32x4_c operator + ( vector32x4_c a, vector32x4_c const & b );
	vector32x4_c operator - ( vector32x4_c a, vector32x4_c const & b );
	vector32x4_c operator * ( vector32x4_c a, vector32x4_c const & b );
	vector32x4_c operator / ( vector32x4_c a, vector32x4_c const & b );

	// double precision 4D vector.
	class vector64x4_c
	{
	public:
		float64_c a;
		float64_c b;
		float64_c c;
		float64_c d;

	public:
		explicit vector64x4_c();
		explicit vector64x4_c( float64_c const a, float64_c const b, float64_c const c, float64_c const d );
		explicit vector64x4_c( float64_c const * values );
		explicit vector64x4_c( vector32x4_c const & other );

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

		float64_c & get_element_at_index( sint32_c const index );
		float64_c const & get_element_at_index( sint32_c const index ) const;

		vector64x4_c & operator *= ( float64_c const b );
		vector64x4_c & operator /= ( float64_c const b );
		vector64x4_c & operator += ( vector64x4_c const & b );
		vector64x4_c & operator -= ( vector64x4_c const & b );
		vector64x4_c & operator *= ( vector64x4_c const & b );
		vector64x4_c & operator /= ( vector64x4_c const & b );

	};

	boolean_c operator == ( vector64x4_c const & a, vector64x4_c const & b );
	boolean_c operator != ( vector64x4_c const & a, vector64x4_c const & b );
	vector64x4_c operator - ( vector64x4_c const & a );
	vector64x4_c operator * ( vector64x4_c a, float64_c b );
	vector64x4_c operator / ( vector64x4_c a, float64_c b );
	vector64x4_c operator + ( vector64x4_c a, vector64x4_c const & b );
	vector64x4_c operator - ( vector64x4_c a, vector64x4_c const & b );
	vector64x4_c operator * ( vector64x4_c a, vector64x4_c const & b );
	vector64x4_c operator / ( vector64x4_c a, vector64x4_c const & b );

	// single precision 3d basis transform matrix, used to rotate and scale 3d points and vectors.
	class matrix32x3x3_c
	{
	public:
		vector32x3_c a;
		vector32x3_c b;
		vector32x3_c c;

		static matrix32x3x3_c get_zero();
		static matrix32x3x3_c get_identity();

	public:
		explicit matrix32x3x3_c();
		explicit matrix32x3x3_c( vector32x3_c const & a, vector32x3_c const & b, vector32x3_c const & c );
		explicit matrix32x3x3_c( float32_c aa, float32_c ab, float32_c ac, float32_c ba, float32_c bb, float32_c bc, float32_c ca, float32_c cb, float32_c cc );

		vector32x3_c & get_element_at_index( sint32_c i );
		vector32x3_c const & get_element_at_index( sint32_c i ) const;

		matrix32x3x3_c operator *= ( float32_c b );

	};

	matrix32x3x3_c operator * ( matrix32x3x3_c a, matrix32x3x3_c const & b );

	// single precision 3d transform matrix, used to rotate, scale, and translate 3d points and vectors.
	// row major, GPU compatible.
	class matrix32x4x4_c
	{
	public:
		vector32x4_c a; // usually basis_a.
		vector32x4_c b; // usually basis_b.
		vector32x4_c c; // usually basis_c.
		vector32x4_c d; // usually position.

	public:
		explicit matrix32x4x4_c();
		explicit matrix32x4x4_c( vector32x3_c const & basis_a, vector32x3_c const & basis_b, vector32x3_c const & basis_c, vector32x3_c const & position );
		explicit matrix32x4x4_c( vector32x4_c const & a, vector32x4_c const & b, vector32x4_c const & c, vector32x4_c const & d );
		explicit matrix32x4x4_c( float32_c const aa, float32_c const ab, float32_c const ac, float32_c const ad, float32_c const ba, float32_c const bb, float32_c const bc, float32_c const bd, float32_c const ca, float32_c const cb, float32_c const cc, float32_c const cd, float32_c const da, float32_c const db, float32_c const dc, float32_c const dd );
		explicit matrix32x4x4_c( float32_c const * values );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		vector32x4_c & get_element_at_index( sint32_c const index );
		vector32x4_c const & get_element_at_index( sint32_c const index ) const;

		vector32x3_c & get_basis_element_at_index( sint32_c const index );
		vector32x3_c const & get_basis_element_at_index( sint32_c const index ) const;

		inline vector32x3_c & get_basis_a() { return * reinterpret_cast< vector32x3_c * >( & a ); }
		inline vector32x3_c const & get_basis_a() const { return * reinterpret_cast< vector32x3_c const * >( & a ); }
		inline vector32x3_c & get_basis_b() { return * reinterpret_cast< vector32x3_c * >( & b ); }
		inline vector32x3_c const & get_basis_b() const { return * reinterpret_cast< vector32x3_c const * >( & b ); }
		inline vector32x3_c & get_basis_c() { return * reinterpret_cast< vector32x3_c * >( & c ); }
		inline vector32x3_c const & get_basis_c() const { return * reinterpret_cast< vector32x3_c const * >( & c ); }
		inline vector32x3_c & get_position() { return * reinterpret_cast< vector32x3_c * >( & d ); }
		inline vector32x3_c const & get_position() const { return * reinterpret_cast< vector32x3_c const * >( & d ); }

	};

	boolean_c operator == ( matrix32x4x4_c const & a, matrix32x4x4_c const & b );
	boolean_c operator != ( matrix32x4x4_c const & a, matrix32x4x4_c const & b );

	matrix32x4x4_c operator * ( matrix32x4x4_c const & a, matrix32x4x4_c const & b );

	// single precision 2d transform matrix, used to rotate and scale 2d points.
	// for 2d coordinate systems in cheonsa, the origin is conventionally in the top left, and the horizontal axis coordinates ascend in values towards the right, and the vertical axis coordinates ascend in values towards the bottom.
	class matrix32x2x2_c
	{
	public:
		vector32x2_c a;
		vector32x2_c b;

	public:
		explicit matrix32x2x2_c();
		explicit matrix32x2x2_c( vector32x2_c const & a, vector32x2_c const & b );
		explicit matrix32x2x2_c( float32_c const aa, float32_c const ab, float32_c const ba, float32_c const bb );
		explicit matrix32x2x2_c( float32_c const * values );
		
		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		inline vector32x4_c const & as_vector32x4() const { return *reinterpret_cast< vector32x4_c const * >( this ); }

	};

	boolean_c operator == ( matrix32x2x2_c const & a, matrix32x2x2_c const & b );
	boolean_c operator != ( matrix32x2x2_c const & a, matrix32x2x2_c const & b );

	matrix32x2x2_c operator * ( matrix32x2x2_c const & a, matrix32x2x2_c const & b );

	// single precision quaternion, used to rotate 3d points and vectors.
	class quaternion32_c
	{
	public:
		float32_c a;
		float32_c b;
		float32_c c;
		float32_c d;

	public:
		explicit quaternion32_c();
		explicit quaternion32_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d );
		explicit quaternion32_c( float32_c const * values );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		// used when saving quaternions to files, since quaternions are always unit length, we can save just the first three elements, and upon load we can calculate the fourth element.
		float32_c const * get_abc() const { return reinterpret_cast< float32_c const * >( this ); }
		// used when loading quaternions from files, since quaternions are always unit length, we can save just the first three elements, and upon load we can calculate the fourth element.
		void_c set_abc_calculate_d( float32_c const * abc );
		void_c calculate_d_from_abc();

	};

	boolean_c operator == ( quaternion32_c const & a, quaternion32_c const & b );
	boolean_c operator != ( quaternion32_c const & a, quaternion32_c const & b );

	quaternion32_c operator + ( quaternion32_c const & a, quaternion32_c const & b );

	quaternion32_c operator * ( quaternion32_c const & a, float32_c const b );
	quaternion32_c operator * ( quaternion32_c const & a, quaternion32_c const & b );

	// single precision 2D line.
	// ab represents the line normal (which is tangent to the line), a unit length vector.
	// c represents a distance along the line normal between the line and the world origin point.
	// by convention, the normal points towards the "outside" or "topside" (in the case of a polygon, each normal will face outside of the polygon), so the distance will be a negative value if the line is "above" the world origin.
	class line32_c
	{
	public:
		float32_c a; // normal_a component.
		float32_c b; // normal_b component.
		float32_c c; // distance component.

	public:
		explicit line32_c();
		explicit line32_c( float32_c a, float32_c b, float32_c c );
		explicit line32_c( float32_c const * value );

		inline float32_c * as_float32_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_float32_array() const { return reinterpret_cast< float32_c const * >( this ); }

		inline vector32x3_c & as_vector32x3() { return *reinterpret_cast< vector32x3_c * >( this ); }
		inline vector32x3_c const & as_vector32x3() const { return *reinterpret_cast< vector32x3_c const * >( this ); }

		inline vector32x2_c & get_normal() { return *reinterpret_cast< vector32x2_c * >( this ); }
		inline vector32x2_c const & get_normal() const { return *reinterpret_cast< vector32x2_c const * >( this ); }

		inline float32_c & get_distance() { return c; }
		inline float32_c const & get_distance() const { return c; }

		vector32x2_c get_point() const;
		void_c set_point( vector32x2_c const & value );

	};

	// single precision 3D plane.
	// abc represents the plane normal (which is tangent to the plane), a unit length vector.
	// d represents the distance along the plane normal between the plane and the world origin point.
	// by convention, the normal points towards the "outside" or "topside" of the plane (in the case of a convex bounding volume, each normal will face outside of the volume), so the distance will be a negative value if the line is "above" the world origin.
	class plane32_c
	{
	public:
		float32_c a; // normal_a component.
		float32_c b; // normal_b component.
		float32_c c; // normal_c component.
		float32_c d; // distance component.

	public:
		explicit plane32_c();
		explicit plane32_c( float32_c const a, float32_c const b, float32_c const c, float32_c const d );
		explicit plane32_c( float32_c const * values );

		inline float32_c * as_float32_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_float32_array() const { return reinterpret_cast< float32_c const * >( this ); }

		inline vector32x4_c & as_vector32x4() { return * reinterpret_cast< vector32x4_c * >( this ); }
		inline vector32x4_c const & as_vector32x4() const { return * reinterpret_cast< vector32x4_c const * >( this ); }

		inline vector32x3_c & get_normal() { return * reinterpret_cast< vector32x3_c * >( this ); }
		inline vector32x3_c const & get_normal() const { return * reinterpret_cast< vector32x3_c const * >( this ); }

		inline float32_c & get_distance() { return d; }
		inline float32_c const get_distance() const { return d; }

		vector32x3_c get_point() const;
		void_c set_point( vector32x3_c const & value );

	};

	boolean_c operator == ( plane32_c const & a, plane32_c const & b );
	boolean_c operator != ( plane32_c const & a, plane32_c const & b );

	// double precision 3D plane.
	// abc represents the plane normal, a unit length vector.
	// d represents the distance along the plane normal between the plane and the world origin point.
	// by convention, the normal points towards the "outside" or "topside" of the plane (in the case of a convex bounding volume, each normal will face outside of the volume), so the distance will be a negative value if the line is "above" the world origin.
	class plane64_c
	{
	public:
		float64_c a; // normal_a component. points top side.
		float64_c b; // normal_b component. points top side.
		float64_c c; // normal_c component. points top side.
		float64_c d; // distance component.

	public:
		explicit plane64_c();
		explicit plane64_c( float64_c const a, float64_c const b, float64_c const c, float64_c const d );
		explicit plane64_c( float64_c const * values );

		inline float64_c * as_float64_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_float64_array() const { return reinterpret_cast< float64_c const * >( this ); }

		inline vector64x4_c & as_vector64x4() { return *reinterpret_cast< vector64x4_c * >( this ); }
		inline vector64x4_c const & as_vector64x4() const { return *reinterpret_cast< vector64x4_c const * >( this ); }

		inline vector64x3_c & get_normal() { return *reinterpret_cast< vector64x3_c * >( this ); }
		inline vector64x3_c const & get_normal() const { return *reinterpret_cast< vector64x3_c const * >( this ); }

		inline float64_c & get_distance() { return d; }
		inline float64_c const get_distance() const { return d; }

		vector64x3_c get_point() const;
		void_c set_point( vector64x3_c const & value );

	};

	boolean_c operator == ( plane64_c const & a, plane64_c const & b );
	boolean_c operator != ( plane64_c const & a, plane64_c const & b );

	enum frustum_plane_index_e
	{
		frustum_plane_index_e_top,
		frustum_plane_index_e_bottom,
		frustum_plane_index_e_left,
		frustum_plane_index_e_right,
		frustum_plane_index_e_near,
		frustum_plane_index_e_far,
	};

	enum frustum_point_index_e
	{
		frustum_point_index_e_near_top_left,
		frustum_point_index_e_near_top_right,
		frustum_point_index_e_near_bottom_left,
		frustum_point_index_e_near_bottom_right,
		frustum_point_index_e_far_top_left,
		frustum_point_index_e_far_top_right,
		frustum_point_index_e_far_bottom_left,
		frustum_point_index_e_far_bottom_right,
	};

	// double precision frustum, for gathering of renderables.
	class frustum64_c
	{
	public:
		plane64_c planes[ 6 ]; // each of the volume faces. the normal (top side) of each plane points towards the inside of the frustum volume.
		vector64x3_c points[ 8 ]; // each of the corner points.

	public:
		frustum64_c(); // initializes everything as 0, so it's in an unusable state.

		void_c make_planes_from_points();
		void_c make_points_from_planes();

		void_c translate( vector64x3_c const & offset ); // can be used to translate the frustum without reorienting it.

	};

	enum box_point_index_e
	{
		box_point_index_e_a_minimum_b_minimum_c_minimum,
		box_point_index_e_a_maximum_b_minimum_c_minimum,
		box_point_index_e_a_minimum_b_maximum_c_minimum,
		box_point_index_e_a_maximum_b_maximum_c_minimum,
		box_point_index_e_a_minimum_b_minimum_c_maximum,
		box_point_index_e_a_maximum_b_minimum_c_maximum,
		box_point_index_e_a_minimum_b_maximum_c_maximum,
		box_point_index_e_a_maximum_b_maximum_c_maximum,
	};

	// a is conventionally x, width, negative is towards left, positive is towards right.
	// b is conventionally y, height, negative is towards up, positive is towards down.
	class box32x2_c
	{
	public:
		vector32x2_c minimum; // left, top.
		vector32x2_c maximum; // right, bottom.

	public:
		explicit box32x2_c();
		explicit box32x2_c( vector32x2_c const & minimum, vector32x2_c const & maximum );
		explicit box32x2_c( float32_c const minimum_a, float32_c const minimum_b, float32_c const maximum_a, float32_c const maximum_b );
		explicit box32x2_c( float32_c const * array );
		
		static inline box32x2_c make_regular( float32_c width, float32_c height ) // top left at (0, 0), bottom right at (width, height).
		{
			return box32x2_c( 0.0f, 0.0f, width, height );
		}

		static inline box32x2_c make_regular( float32_c x, float32_c y, float32_c width, float32_c height )
		{
			return box32x2_c( x, y, x + width, y + height );
		}

		static inline box32x2_c make_centered( float32_c width, float32_c height ) // ( width * -0.5f, height * -0.5f, width * 0.5f, height * 0.5f ).
		{
			return box32x2_c( width * -0.5f, height * -0.5f, width * 0.5f, height * 0.5f );
		}

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		vector32x2_c get_center() const; // center point.
		inline vector32x2_c get_size() const { return vector32x2_c( maximum.a - minimum.a, maximum.b - minimum.b ); }
		inline float32_c get_width() const { return maximum.a - minimum.a; }
		inline float32_c get_height() const { return maximum.b - minimum.b; }

		void_c accumulate_bounds( box32x2_c const & other );

	};

	boolean_c operator == ( box32x2_c const & a, box32x2_c const & b );
	boolean_c operator != ( box32x2_c const & a, box32x2_c const & b );

	// a is conventionally x, width, negative is towards left, positive is towards right.
	// b is conventionally y, height, negative is towards up, positive is towards down.
	class box64x2_c
	{
	public:
		vector64x2_c minimum;
		vector64x2_c maximum;

	public:
		explicit box64x2_c();
		explicit box64x2_c( vector64x2_c const & minimum, vector64x2_c const & maximum );
		explicit box64x2_c( float64_c minimum_a, float64_c minimum_b, float64_c maximum_a, float64_c maximum_b );
		explicit box64x2_c( float64_c const * array );

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

		vector64x2_c get_center() const; // center point.
		inline vector64x2_c get_size() const { return vector64x2_c( maximum.a - minimum.a, maximum.b - minimum.b ); }
		inline float64_c get_width() const { return maximum.a - minimum.a; }
		inline float64_c get_height() const { return maximum.b - minimum.b; }

	};

	boolean_c operator == ( box64x2_c const & a, box64x2_c const & b );
	boolean_c operator != ( box64x2_c const & a, box64x2_c const & b );

	// a is conventionally x, width, negative is towards left, positive is towards right.
	// b is conventionally y, depth, negative is towards back, positive is towards front.
	// c is conventionally z, height, negative is towards down, positive is towards up.
	class box32x3_c
	{
	public:
		vector32x3_c minimum;
		vector32x3_c maximum;

	public:
		explicit box32x3_c();
		explicit box32x3_c( vector32x3_c const & minimum, vector32x3_c const & maximum );
		explicit box32x3_c( float32_c const minimum_a, float32_c const minimum_b, float32_c const minimum_c, float32_c const maximum_a, float32_c const maximum_b, float32_c const maximum_c );
		explicit box32x3_c( float32_c const * array );
		explicit box32x3_c( box64x3_c const & other );

		inline float32_c * as_array() { return reinterpret_cast< float32_c * >( this ); }
		inline float32_c const * as_array() const { return reinterpret_cast< float32_c const * >( this ); }

		vector32x3_c get_center() const;
		vector32x3_c get_size() const;
		inline float32_c get_width() const { return maximum.a - minimum.a; }
		inline float32_c get_depth() const { return maximum.b - minimum.b; }
		inline float32_c get_height() const { return maximum.c - minimum.c; }
		vector32x3_c get_point( sint32_c index ) const;

		void_c set_bounds( vector32x3_c const & point, float32_c radius );
		void_c accumulate_bounds( vector32x3_c const & point );
		void_c accumulate_bounds( vector32x3_c const & point, float32_c radius );

	};

	boolean_c operator == ( box32x3_c const & a, box32x3_c const & b );
	boolean_c operator != ( box32x3_c const & a, box32x3_c const & b );

	// a is conventionally x, width, negative is towards left, positive is towards right.
	// b is conventionally y, depth, negative is towards back, positive is towards front.
	// c is conventionally z, height, negative is towards down, positive is towards up.
	class box64x3_c
	{
	public:
		vector64x3_c minimum;
		vector64x3_c maximum;

	public:
		explicit box64x3_c();
		explicit box64x3_c( vector64x3_c const & minimum, vector64x3_c const & maximum );
		explicit box64x3_c( float64_c const minimum_a, float64_c const minimum_b, float64_c const minimum_c, float64_c const maximum_a, float64_c const maximum_b, float64_c const maximum_c );
		explicit box64x3_c( box32x3_c const & other );
		
		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

		vector64x3_c get_center() const;
		vector64x3_c get_size() const;
		inline float64_c get_width() const { return maximum.a - minimum.a; }
		inline float64_c get_depth() const { return maximum.b - minimum.b; }
		inline float64_c get_height() const { return maximum.c - minimum.c; }
		float64_c get_longest_dimension() const;
		float64_c get_longest_half_extent() const;
		vector64x3_c get_point( sint32_c const index ) const;

	};

	boolean_c operator == ( box64x3_c const & a, box64x3_c const & b );
	boolean_c operator != ( box64x3_c const & a, box64x3_c const & b );

	class circle64_c
	{
	public:
		vector64x2_c position;
		float64_c radius;

	public:
		inline circle64_c() : position(), radius( 0 ) {}
		inline circle64_c( vector64x2_c position, float64_c radius ) : position( position ), radius( radius ) {}
		inline circle64_c( float64_c const * values ) : position( values[ 0 ], values[ 1 ] ), radius( values[ 2 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

	class sphere64_c
	{
	public:
		vector64x3_c position;
		float64_c radius;

	public:
		inline sphere64_c() : position(), radius( 0 ) {}
		inline sphere64_c( vector64x3_c const & position, float64_c const radius ) : position( position ), radius( radius ) {}
		inline sphere64_c( float64_c const * values ) : position( values[ 0 ], values[ 1 ], values[ 2 ] ), radius( values[ 3 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

	class ray64_c
	{
	public:
		vector64x3_c position;
		vector64x3_c normal;

	public:
		inline ray64_c() : position(), normal() {}
		inline ray64_c( vector64x3_c const & position, vector64x3_c const & normal ) : position( position ), normal( normal ) {}
		inline ray64_c( float64_c const * values ) : position( values[ 0 ], values[ 1 ], values[ 2 ] ), normal( values[ 3 ], values[ 4 ], values[ 5 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

	class segment64_c
	{
	public:
		vector64x3_c point_a;
		vector64x3_c point_b;

	public:
		inline segment64_c() : point_a(), point_b() {}
		inline segment64_c( vector64x3_c const & point_a, vector64x3_c const & point_b ) : point_a( point_a ), point_b( point_b ) {}
		inline segment64_c( float64_c const * values ) : point_a( values[ 0 ], values[ 1 ], values[ 2 ] ), point_b( values[ 3 ], values[ 4 ], values[ 5 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

	class capsule64_c
	{
	public:
		vector64x3_c point_a;
		float64_c radius_a;
		vector64x3_c point_b;
		float64_c radius_b;

	public:
		inline capsule64_c() : point_a(), radius_a( 0 ), point_b(), radius_b( 0 ) {}
		inline capsule64_c( vector64x3_c const & point_a, float64_c const radius_a, vector64x3_c const & point_b, float64_c const radius_b ) : point_a( point_a ), radius_a( radius_a ), point_b( point_b ), radius_b( radius_b ) {}
		inline capsule64_c( float64_c const * values ) : point_a( values[ 0 ], values[ 1 ], values[ 2 ] ), radius_a( values[ 3 ] ), point_b( values[ 4 ], values[ 5 ], values[ 6 ] ), radius_b( values[ 7 ] ) {}

		inline float64_c * as_array() { return reinterpret_cast< float64_c * >( this ); }
		inline float64_c const * as_array() const { return reinterpret_cast< float64_c const * >( this ); }

	};

	class triangle64_c
	{
	public:
		vector64x3_c point_a;
		vector64x3_c point_b;
		vector64x3_c point_c;
		plane64_c face_plane;
		plane64_c edge_plane_ab; // plane, tangent with face_plane, but along edge ab, with normal point outward (?).
		plane64_c edge_plane_bc; // plane, tangent with face_plane, but along edge bc, with normal point outward (?).
		plane64_c edge_plane_ca; // plane, tangent with face_plane, but along edge ca, with normal point outward (?).

	public:
		inline triangle64_c() : point_a(), point_b(), point_c(), face_plane(), edge_plane_ab(), edge_plane_bc(), edge_plane_ca() {}
		inline triangle64_c( vector64x3_c const & point_a, vector64x3_c const & point_b, vector64x3_c const & point_c ) : point_a( point_a ), point_b( point_b ), point_c( point_c ), face_plane(), edge_plane_ab(), edge_plane_bc(), edge_plane_ca() { make_planes_from_points(); }

		void_c make_planes_from_points(); // calculates the plane members of this triangle from the points.

	};

	// double precision position, for undiscretized|unpartitioned open worlds.
	// single precision rotation quaternion.
	// single precision scale.
	// single precision basis that is calculated as needed and then cached.
	// abc is xyz.
	//
	// the axis orientation convention used for models, cameras, directional lights, and model bones:
	// +x axis points right.
	// +y axis points forward.
	// +z axis points up.
	//
	// the y axis is the primary axis for cameras, directional lights, and model bones.
	//
	// these axes are used to construct the view matrix:
	// +y axis defines view forward, +z axis defines view up.
	// but the resulting view matrix is a typical right-handed view matrix, and so this convention does not apply, since +z axis goes towards the viewer, +x axis goes right, and +y axis goes up.
	//
	// for things that point (like bones, lights, and cameras), +z axis is forward, +x axis is right, and +y axis is down.
	// this scene transform class is uses 64-bit floats for position, and 32-bit floats for rotation and scale.
	// the larger position is supposed to allow for open world, and the smaller rotation and scale is to be more memory efficient.
	// if you are going to transform a few points or vectors, use make_transformed_point() and make_rotated_vector().
	// if you are going to transform a lot of points or vectors, then it is more cost efficient to calculate a matrix and use that, use make_basis_with_scale(), make_basis_without_scale(), or get_matrix32x4x4_relative_to_position().
	// the bullet physics engine however is designed to use a scalar type, that can be defined as either 32-bit or 64-bit float.
	// if we did not use third party physics, we could reduce duplication and waste, since the physics engine basically has to have its own copy of the scene.
	class transform3d_c
	{
	public:
		vector64x3_c position;
		quaternion32_c rotation;
		vector32x3_c scale; // need non-uniform scale for rare cases, for certain effects.

	public:
		transform3d_c();
		transform3d_c( vector64x3_c const & position, quaternion32_c const & rotation, float32_c uniform_scale );
		transform3d_c( vector64x3_c const & position, quaternion32_c const & rotation, vector32x3_c const & scale );

		void_c reset();

		matrix32x3x3_c get_scaled_basis() const; // basis axes.
		vector32x3_c get_scaled_basis_x() const; // x axis, right (with scale applied).
		vector32x3_c get_scaled_basis_y() const; // y axis, forward (with scale applied).
		vector32x3_c get_scaled_basis_z() const; // z axis, up (with scale applied).

		matrix32x3x3_c get_unscaled_basis() const; // basis axis normals.
		vector32x3_c get_unscaled_basis_x() const; // x axis, right (without scale applied|unit length|normalized).
		vector32x3_c get_unscaled_basis_y() const; // y axis, forward (without scale applied|unit length|normalized).
		vector32x3_c get_unscaled_basis_z() const; // z axis, up (without scale applied|unit length|normalized).

		void_c set_rotation_from_unscaled_basis( matrix32x3x3_c const & value ); // sets rotation quaternion from an unscaled (normalized) orthogonal basis 3x3 matrix.

		void_c set_uniform_scale( float32_c value );

		transform3d_c get_inverted() const; // makes an inverted copy of this instance.

		void_c invert(); // inverts this instance, such that original * inverted = identity.

		transform3d_c & operator = ( transform3d_c const & other );

		transform3d_c & operator *= ( transform3d_c const & other ); // multiplies this transform with other transform and assigns result to this transform.

	};

	boolean_c operator == ( transform3d_c const & a, transform3d_c const & b );
	boolean_c operator != ( transform3d_c const & a, transform3d_c const & b );
	transform3d_c operator * ( transform3d_c a, transform3d_c const & b ); // multiplies this transform with other transform.

	// a kind of 4x3 matrix.
	// multiplies like a 4x4 matrix, but with an imaginary 4th row of [0, 0, 0, 1].
	class basis_position_c
	{
	public:
		matrix32x3x3_c basis;
		vector64x3_c position;

	public:
		basis_position_c operator * ( basis_position_c const & other ) const;

	};

	// 2d polygon.
	// winding of points can be clock wise or counter clock wise.
	// geometry can be convex or concave.
	// cheonsa's polygon operation functions will construct polygons with counter clock wise wind order.
	class polygon32x2_c
	{
	public:
		//vector32x2_c origin;
		//matrix32x2x2_c basis;
		sint32_c points_count;
		vector32x2_c points[ 8 ]; // points in local space, without transform applied.

	public:
		polygon32x2_c();

		// gets point in polygon.
		vector32x2_c get_point( sint32_c point_index ) const;
		
		// gets edge vector in polygon, which is edge point b minus edge point a.
		vector32x2_c get_edge_vector( sint32_c edge_index ) const;

		// gets edge line in polygon.
		// if wind order of polygon points is clock wise then normal of line will point towards interior of polygon.
		// if wind order of polygon points is counter clock wise then normal of line will point towards exterior of polygon.
		line32_c get_edge_line( sint32_c edge_index ) const;

	};

}
