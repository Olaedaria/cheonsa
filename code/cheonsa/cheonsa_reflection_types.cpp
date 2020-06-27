#include "cheonsa_reflection_types.h"
#include "cheonsa_reflection_property.h"
#include "cheonsa_reflection_object.h"
#include "cheonsa_reflection_class.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	reflection_value_container_c::reflection_value_container_c()
		: uint64{}
		, string8()
		, string16()
	{
	}

	reflection_value_container_c & reflection_value_container_c::operator = ( reflection_value_container_c const & other )
	{
		for ( sint32_c i = 0; i < 4; i++ )
		{
			uint64[ i ] = other.uint64[ i ];
		}
		string8 = other.string8;
		string16 = other.string16;
		return *this;
	}

	reflection_property_value_changed_information_c::reflection_property_value_changed_information_c()
		: reflection_object( nullptr )
		, reflection_property( nullptr )
		, before_value()
		, after_value()
	{
	}

	reflection_property_value_changed_information_c::reflection_property_value_changed_information_c( reflection_property_value_changed_information_c const & other )
		: reflection_object( other.reflection_object )
		, reflection_property( other.reflection_property )
		, before_value( other.before_value )
		, after_value( other.after_value )
	{
	}

	reflection_property_value_changed_information_c & reflection_property_value_changed_information_c::operator = ( reflection_property_value_changed_information_c const & other )
	{
		reflection_object = other.reflection_object;
		reflection_property = other.reflection_property;
		before_value = other.before_value;
		after_value = other.after_value;
		return *this;
	}

	boolean_c reflection_get_object_property_value( reflection_object_c * object, reflection_property_c const * property, reflection_value_container_c & value )
	{
		assert( object && property );
		assert( object->_reflection_class == property->_reflection_class );
		if ( property->_type == data_type_e_string8 )
		{
			assert( property->_accessors._value_getter );
			return property->_accessors._value_getter( object, &value.string8, property->_type, property->_type_count );
		}
		else if ( property->_type == data_type_e_string16 )
		{
			assert( property->_accessors._value_getter );
			return property->_accessors._value_getter( object, &value.string16, property->_type, property->_type_count );
		}
		else if ( property->_type >= data_type_e_uint8 && property->_type <= data_type_e_float64 )
		{
			assert( property->_accessors._value_getter );
			return property->_accessors._value_getter( object, &value, property->_type, property->_type_count );
		}
		return false;
	}

	boolean_c reflection_set_object_property_value( reflection_object_c * object, reflection_property_c const * property, reflection_value_container_c const & value )
	{
		assert( object && property );
		assert( object->_reflection_class == property->_reflection_class );
		assert( property->_type >= data_type_e_string8 && property->_type <= data_type_e_float64 );
		assert( property->_accessors._value_setter );
		if ( property->_type == data_type_e_string8 )
		{
			return property->_accessors._value_setter( object, &value.string8, property->_type, property->_type_count );
		}
		else if ( property->_type == data_type_e_string16 )
		{
			return property->_accessors._value_setter( object, &value.string16, property->_type, property->_type_count );
		}
		else
		{
			return property->_accessors._value_setter( object, &value, property->_type, property->_type_count );
		}
	}

	string16_c reflection_convert_value_to_string16( reflection_property_c const * property, reflection_value_container_c const & value )
	{
		assert( property );
		//reflection_value_container_c value_b;
		//reflection_value_container_c & value = value_a ? *value_a : value_b;
		string8_c value_as_string8;
		string16_c value_as_string16;
		if ( property->_view == data_view_e_color )
		{
			vector64x4_c value_as_color;
			value_as_color.a = 1.0;
			value_as_color.b = 1.0;
			value_as_color.c = 1.0;
			value_as_color.d = 1.0;
			if ( property->_type == data_type_e_uint32 )
			{
				assert( property->_type_count == 1 );
				value_as_color.a = static_cast< float64_c >( ( value.uint32[ 0 ] >> 24 ) & 0x000000FF ) / 255.0;
				value_as_color.b = static_cast< float64_c >( ( value.uint32[ 0 ] >> 16 ) & 0x000000FF ) / 255.0;
				value_as_color.c = static_cast< float64_c >( ( value.uint32[ 0 ] >>  8 ) & 0x000000FF ) / 255.0;
				value_as_color.d = static_cast< float64_c >( ( value.uint32[ 0 ]       ) & 0x000000FF ) / 255.0;
				value_as_string8 = "#";
				string8_c hex;
				ops::convert_rgba_to_hexadecimal_string8( value_as_color, hex );
				value_as_string8 += hex;
				value_as_string16 = value_as_string8;
				return value_as_string16;
			}
			else if ( property->_type == data_type_e_uint8 )
			{
				assert( property->_type_count == 3 || property->_type_count == 4 );
				value_as_color.a = static_cast< float64_c >( value.uint8[ 0 ] ) / 255.0;
				value_as_color.b = static_cast< float64_c >( value.uint8[ 1 ] ) / 255.0;
				value_as_color.c = static_cast< float64_c >( value.uint8[ 2 ] ) / 255.0;
				value_as_string8 = "#";
				string8_c hex;
				if ( property->_type_count == 4 )
				{
					value_as_color.d = static_cast< float64_c >( value.uint8[ 3 ] ) / 255.0;
					ops::convert_rgba_to_hexadecimal_string8( value_as_color, hex );
				}
				else
				{
					ops::convert_rgb_to_hexadecimal_string8( vector64x3_c( value_as_color.as_array() ), hex );
				}
				value_as_string8 += hex;
				value_as_string16 = value_as_string8;
				return value_as_string16;
			}
			else if ( property->_type == data_type_e_uint16 )
			{
				assert( property->_type_count == 3 || property->_type_count == 4 );
				value_as_color.a = static_cast< float64_c >( value.uint16[ 0 ] ) / 65535.0;
				value_as_color.b = static_cast< float64_c >( value.uint16[ 1 ] ) / 65535.0;
				value_as_color.c = static_cast< float64_c >( value.uint16[ 2 ] ) / 65535.0;
				if ( property->_type_count == 4 )
				{
					value_as_color.d = static_cast< float64_c >( value.uint16[ 3 ] ) / 65535.0;
				}
			}
			else if ( property->_type == data_type_e_float32 )
			{
				assert( property->_type_count == 3 || property->_type_count == 4 );
				value_as_color.a = static_cast< float64_c >( value.float32[ 0 ] );
				value_as_color.b = static_cast< float64_c >( value.float32[ 1 ] );
				value_as_color.c = static_cast< float64_c >( value.float32[ 2 ] );
				if ( property->_type_count == 4 )
				{
					value_as_color.d = static_cast< float64_c >( value.float32[ 0 ] );
				}
			}
			else if ( property->_type == data_type_e_float64 )
			{
				assert( property->_type_count == 3 || property->_type_count == 4 );
				value_as_color.a = value.float64[ 0 ];
				value_as_color.b = value.float64[ 1 ];
				value_as_color.c = value.float64[ 2 ];
				if ( property->_type_count == 4 )
				{
					value_as_color.d = value.float64[ 3 ];
				}
			}
			else
			{
				assert( false );
			}
			if ( property->_type_count == 3 )
			{
				ops::convert_float64xn_to_string8( core_list_c< float64_c >( core_list_mode_e_static, value_as_color.as_array(), 3 ), value_as_string8 );
			}
			else if ( property->_type_count == 4 )
			{
				ops::convert_float64xn_to_string8( core_list_c< float64_c >( core_list_mode_e_static, value_as_color.as_array(), 4 ), value_as_string8 );
			}
			else
			{
				assert( false );
			}
			value_as_string16 = value_as_string8;
		}
		else if ( property->_view == data_view_e_euler_angles )
		{
			vector32x3_c value_as_euler_angles;
			if ( property->_type == data_type_e_float32 )
			{
				if ( property->_type_count == 3 )
				{
					value_as_euler_angles.a = value.float32[ 0 ];
					value_as_euler_angles.b = value.float32[ 1 ];
					value_as_euler_angles.c = value.float32[ 2 ];
				}
				else if ( property->_type_count == 4 )
				{
					quaternion32_c value_as_quaternion;
					value_as_quaternion.a = value.float32[ 0 ];
					value_as_quaternion.b = value.float32[ 1 ];
					value_as_quaternion.c = value.float32[ 2 ];
					value_as_quaternion.d = value.float32[ 3 ];
					value_as_euler_angles = ops::euler_angles_from_rotation_quaternion32( quaternion32_c( value.float32 ) );
				}
				else
				{
					assert( false );
				}
			}
			else if ( property->_type == data_type_e_float64 )
			{
				if ( property->_type_count == 3 )
				{
					value_as_euler_angles.a = static_cast< float32_c >( value.float64[ 0 ] );
					value_as_euler_angles.b = static_cast< float32_c >( value.float64[ 1 ] );
					value_as_euler_angles.c = static_cast< float32_c >( value.float64[ 2 ] );
				}
				else if ( property->_type_count == 4 )
				{
					quaternion32_c value_as_quaternion;
					value_as_quaternion.a = static_cast< float32_c >( value.float64[ 0 ] );
					value_as_quaternion.b = static_cast< float32_c >( value.float64[ 1 ] );
					value_as_quaternion.c = static_cast< float32_c >( value.float64[ 2 ] );
					value_as_quaternion.d = static_cast< float32_c >( value.float64[ 3 ] );
					value_as_euler_angles = ops::euler_angles_from_rotation_quaternion32( value_as_quaternion );
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
			ops::convert_float32xn_to_string8( core_list_c< float32_c >( core_list_mode_e_static, value_as_euler_angles.as_array(), 3 ), value_as_string8 );
			value_as_string16 = value_as_string8;
		}
		else
		{
			if ( property->_type == data_type_e_string8 )
			{
				assert( property->_type_count == 1 );
				value_as_string16 = value.string8;
			}
			else if ( property->_type == data_type_e_string16 )
			{
				assert( property->_type_count == 1 );
				value_as_string16 = value.string16;
			}
			else if ( property->_type == data_type_e_uint8 )
			{
				ops::convert_uint8xn_to_string8( core_list_c< uint8_c >( core_list_mode_e_static, value.uint8, property->_type_count ), value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( property->_type == data_type_e_sint8 )
			{
				ops::convert_sint8xn_to_string8( core_list_c< sint8_c >( core_list_mode_e_static, value.sint8, property->_type_count ), value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( property->_type == data_type_e_uint16 )
			{
				ops::convert_uint16xn_to_string8( core_list_c< uint16_c >( core_list_mode_e_static, value.uint16, property->_type_count ), value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( property->_type == data_type_e_sint16 )
			{
				ops::convert_sint16xn_to_string8( core_list_c< sint16_c >( core_list_mode_e_static, value.sint16, property->_type_count ), value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( property->_type == data_type_e_uint32 )
			{
				ops::convert_uint32xn_to_string8( core_list_c< uint32_c >( core_list_mode_e_static, value.uint32, property->_type_count ), value_as_string8 );
			}
			else if ( property->_type == data_type_e_sint32 )
			{
				ops::convert_sint32xn_to_string8( core_list_c< sint32_c >( core_list_mode_e_static, value.sint32, property->_type_count ), value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( property->_type == data_type_e_uint64 )
			{
				ops::convert_uint64xn_to_string8( core_list_c< uint64_c >( core_list_mode_e_static, value.uint64, property->_type_count ), value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( property->_type == data_type_e_sint64 )
			{
				ops::convert_sint64xn_to_string8( core_list_c< sint64_c >( core_list_mode_e_static, value.sint64, property->_type_count ), value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( property->_type == data_type_e_float32 )
			{
				ops::convert_float32xn_to_string8( core_list_c< float32_c >( core_list_mode_e_static, value.float32, property->_type_count ), value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else if ( property->_type == data_type_e_float64 )
			{
				ops::convert_float64xn_to_string8( core_list_c< float64_c >( core_list_mode_e_static, value.float64, property->_type_count ), value_as_string8 );
				value_as_string16 = value_as_string8;
			}
			else
			{
				assert( false );
			}
		}
		return value_as_string16;
	}

	boolean_c reflection_convert_string16_to_value( reflection_property_c const * property, reflection_value_container_c & value, string16_c const & value_as_string16 )
	{
		assert( property );
		string8_c value_as_string8;
		if ( property->_view == data_view_e_color )
		{
			value_as_string8 = value_as_string16;
			value_as_string8 = ops::string8_trim( value_as_string8 );
			vector64x4_c value_as_color;
			if ( !ops::convert_string8_to_rgba( value_as_string8, value_as_color ) )
			{
				return false;
			}
			if ( property->_type == data_type_e_uint32 )
			{
				assert( property->_type_count == 1 );
				value.uint32[ 0 ] =
					( static_cast< uint32_c >( ops::math_saturate( value_as_color.a ) * 255.0 ) << 24 ) |
					( static_cast< uint32_c >( ops::math_saturate( value_as_color.b ) * 255.0 ) << 16 ) |
					( static_cast< uint32_c >( ops::math_saturate( value_as_color.c ) * 255.0 ) << 8 ) |
					( static_cast< uint32_c >( ops::math_saturate( value_as_color.d ) * 255.0 ) );
			}
			else if ( property->_type == data_type_e_uint8 )
			{
				assert( property->_type_count == 3 || property->_type_count == 4 );
				value.uint8[ 0 ] = static_cast< uint8_c >( ops::math_saturate( value_as_color.a ) * 255 );
				value.uint8[ 1 ] = static_cast< uint8_c >( ops::math_saturate( value_as_color.b ) * 255 );
				value.uint8[ 2 ] = static_cast< uint8_c >( ops::math_saturate( value_as_color.c ) * 255 );
				value.uint8[ 3 ] = static_cast< uint8_c >( ops::math_saturate( value_as_color.d ) * 255 ); // this element may not be used but it's okay to set it anyway.
			}
			else if ( property->_type == data_type_e_uint16 )
			{
				assert( property->_type_count == 3 || property->_type_count == 4 );
				value.uint16[ 0 ] = static_cast< uint16_c >( ops::math_saturate( value_as_color.a ) * 65535.0 );
				value.uint16[ 1 ] = static_cast< uint16_c >( ops::math_saturate( value_as_color.b ) * 65535.0 );
				value.uint16[ 2 ] = static_cast< uint16_c >( ops::math_saturate( value_as_color.c ) * 65535.0 );
				value.uint16[ 3 ] = static_cast< uint16_c >( ops::math_saturate( value_as_color.d ) * 65535.0 ); // this element may not be used but it's okay to set it anyway.
			}
			else if ( property->_type == data_type_e_float32 )
			{
				assert( property->_type_count == 3 || property->_type_count == 4 );
				value.float32[ 0 ] = static_cast< float32_c >( value_as_color.a );
				value.float32[ 1 ] = static_cast< float32_c >( value_as_color.b );
				value.float32[ 2 ] = static_cast< float32_c >( value_as_color.c );
				value.float32[ 3 ] = static_cast< float32_c >( value_as_color.d ); // this element may not be used but it's okay to set it anyway.
			}
			else if ( property->_type == data_type_e_float64 )
			{
				assert( property->_type_count == 3 || property->_type_count == 4 );
				value.float64[ 0 ] = value_as_color.a;
				value.float64[ 1 ] = value_as_color.b;
				value.float64[ 2 ] = value_as_color.c;
				value.float64[ 3 ] = value_as_color.d; // this element may not be used but it's okay to set it anyway.
			}
			else
			{
				assert( false );
			}
		}
		else if ( property->_view == data_view_e_euler_angles )
		{
			vector32x3_c value_as_euler_angles;
			value_as_string8 = value_as_string16;
			if ( !ops::convert_string8_to_float32xn( value_as_string8, core_list_c< float32_c >( core_list_mode_e_static, value_as_euler_angles.as_array(), 3 ) ) )
			{
				return false;
			}
			if ( property->_type == data_type_e_float32 )
			{
				if ( property->_type_count == 3 )
				{
					value.float32[ 0 ] = value_as_euler_angles.a;
					value.float32[ 1 ] = value_as_euler_angles.b;
					value.float32[ 2 ] = value_as_euler_angles.c;
				}
				else if ( property->_type_count == 4 )
				{
					quaternion32_c value_as_quaternion = ops::rotation_quaternion32_from_euler_angles( vector32x3_c( value_as_euler_angles ) );
					value.float32[ 0 ] = value_as_quaternion.a;
					value.float32[ 1 ] = value_as_quaternion.b;
					value.float32[ 2 ] = value_as_quaternion.c;
					value.float32[ 3 ] = value_as_quaternion.d;
				}
				else
				{
					assert( false );
				}
			}
			else if ( property->_type == data_type_e_float64 )
			{
				if ( property->_type_count == 3 )
				{
					value.float64[ 0 ] = static_cast< float64_c >( value_as_euler_angles.a );
					value.float64[ 1 ] = static_cast< float64_c >( value_as_euler_angles.b );
					value.float64[ 2 ] = static_cast< float64_c >( value_as_euler_angles.c );
				}
				else if ( property->_type_count == 4 )
				{
					quaternion32_c quaternion = ops::rotation_quaternion32_from_euler_angles( vector32x3_c( value_as_euler_angles ) );
					value.float64[ 0 ] = static_cast< float64_c >( quaternion.a );
					value.float64[ 1 ] = static_cast< float64_c >( quaternion.b );
					value.float64[ 2 ] = static_cast< float64_c >( quaternion.c );
					value.float64[ 3 ] = static_cast< float64_c >( quaternion.d );
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
			if ( property->_type == data_type_e_string8 )
			{
				value.string8 = value_as_string16;
			}
			else if ( property->_type == data_type_e_string16 )
			{
				value.string16 = value_as_string16;
			}
			else if ( property->_type == data_type_e_uint8 )
			{
				if ( !ops::convert_string8_to_uint8xn( value_as_string8, core_list_c< uint8_c >( core_list_mode_e_static, value.uint8, property->_type_count ) ) )
				{
					return false;
				}
			}
			else if ( property->_type == data_type_e_sint8 )
			{
				if ( !ops::convert_string8_to_sint8xn( value_as_string8, core_list_c< sint8_c >( core_list_mode_e_static, value.sint8, property->_type_count ) ) )
				{
					return false;
				}
			}
			else if ( property->_type == data_type_e_uint16 )
			{
				if ( !ops::convert_string8_to_uint16xn( value_as_string8, core_list_c< uint16_c >( core_list_mode_e_static, value.uint16, property->_type_count ) ) )
				{
					return false;
				}
			}
			else if ( property->_type == data_type_e_sint16 )
			{
				if ( !ops::convert_string8_to_sint16xn( value_as_string8, core_list_c< sint16_c >( core_list_mode_e_static, value.sint16, property->_type_count ) ) )
				{
					return false;
				}
			}
			else if ( property->_type == data_type_e_uint32 )
			{
				if ( !ops::convert_string8_to_uint32xn( value_as_string8, core_list_c< uint32_c >( core_list_mode_e_static, value.uint32, property->_type_count ) ) )
				{
					return false;
				}
			}
			else if ( property->_type == data_type_e_sint32 )
			{
				if ( !ops::convert_string8_to_sint32xn( value_as_string8, core_list_c< sint32_c >( core_list_mode_e_static, value.sint32, property->_type_count ) ) )
				{
					return false;
				}
			}
			else if ( property->_type == data_type_e_uint64 )
			{
				if ( !ops::convert_string8_to_uint64xn( value_as_string8, core_list_c< uint64_c >( core_list_mode_e_static, value.uint64, property->_type_count ) ) )
				{
					return false;
				}
			}
			else if ( property->_type == data_type_e_sint64 )
			{
				if ( !ops::convert_string8_to_sint64xn( value_as_string8, core_list_c< sint64_c >( core_list_mode_e_static, value.sint64, property->_type_count ) ) )
				{
					return false;
				}
			}
			else if ( property->_type == data_type_e_float32 )
			{
				if ( !ops::convert_string8_to_float32xn( value_as_string8, core_list_c< float32_c >( core_list_mode_e_static, value.float32, property->_type_count ) ) )
				{
					return false;
				}
			}
			else if ( property->_type == data_type_e_float64 )
			{
				if ( !ops::convert_string8_to_float64xn( value_as_string8, core_list_c< float64_c >( core_list_mode_e_static, value.float64, property->_type_count ) ) )
				{
					return false;
				}
			}
			else
			{
				assert( false );
			}
		}
		return true;
	}

	vector64x4_c reflection_convert_value_to_color( reflection_property_c const * property, reflection_value_container_c const & value )
	{
		assert( property );
		assert( property->_view == data_view_e_color );
		vector64x4_c value_as_color;
		value_as_color.d = 1.0;
		if ( property->_type == data_type_e_uint32 )
		{
			assert( property->_type_count == 1 );
			value_as_color.a = static_cast< float64_c >( ( value.uint32[ 0 ] >> 24 ) & 0x000000FF ) / 255.0;
			value_as_color.b = static_cast< float64_c >( ( value.uint32[ 0 ] >> 16 ) & 0x000000FF ) / 255.0;
			value_as_color.c = static_cast< float64_c >( ( value.uint32[ 0 ] >>  8 ) & 0x000000FF ) / 255.0;
			value_as_color.d = static_cast< float64_c >( ( value.uint32[ 0 ]       ) & 0x000000FF ) / 255.0;
		}
		else if ( property->_type == data_type_e_uint8 )
		{
			assert( property->_type_count == 3 || property->_type_count == 4 );
			value_as_color.a = static_cast< float64_c >( value.uint8[ 0 ] ) / 255.0;
			value_as_color.b = static_cast< float64_c >( value.uint8[ 1 ] ) / 255.0;
			value_as_color.c = static_cast< float64_c >( value.uint8[ 2 ] ) / 255.0;
			if ( property->_type_count == 4 )
			{
				value_as_color.d = static_cast< float64_c >( value.uint8[ 3 ] ) / 255.0;
			}
		}
		else if ( property->_type == data_type_e_uint16 )
		{
			assert( property->_type_count == 3 || property->_type_count == 4 );
			value_as_color.a = static_cast< float64_c >( value.uint16[ 0 ] ) / 65535.0;
			value_as_color.b = static_cast< float64_c >( value.uint16[ 1 ] ) / 65535.0;
			value_as_color.c = static_cast< float64_c >( value.uint16[ 2 ] ) / 65535.0;
			if ( property->_type_count == 4 )
			{
				value_as_color.d = static_cast< float64_c >( value.uint16[ 3 ] ) / 65535.0;
			}
		}
		else if ( property->_type == data_type_e_float32 )
		{
			assert( property->_type_count == 3 || property->_type_count == 4 );
			value_as_color.a = static_cast< float64_c >( value.float32[ 0 ] );
			value_as_color.b = static_cast< float64_c >( value.float32[ 1 ] );
			value_as_color.c = static_cast< float64_c >( value.float32[ 2 ] );
			if ( property->_type_count == 4 )
			{
				value_as_color.d = static_cast< float64_c >( value.float32[ 3 ] );
			}
		}
		else if ( property->_type == data_type_e_float64 )
		{
			assert( property->_type_count == 3 || property->_type_count == 4 );
			value_as_color.a = value.float64[ 0 ];
			value_as_color.b = value.float64[ 1 ];
			value_as_color.c = value.float64[ 2 ];
			if ( property->_type_count == 4 )
			{
				value_as_color.d = value.float64[ 3 ];
			}
		}
		else
		{
			assert( false );
		}
		return value_as_color;
	}

	void_c reflection_convert_color_to_value( reflection_property_c const * property, reflection_value_container_c & value, vector64x4_c const & value_as_color )
	{
		assert( property );
		assert( property->_view == data_view_e_color );
		if ( property->_type == data_type_e_uint32 )
		{
			assert( property->_type_count == 1 );
			value.uint32[ 0 ] =
				( static_cast< uint32_c >( ops::math_saturate( value_as_color.a ) * 255.0 ) << 24 ) |
				( static_cast< uint32_c >( ops::math_saturate( value_as_color.b ) * 255.0 ) << 16 ) |
				( static_cast< uint32_c >( ops::math_saturate( value_as_color.c ) * 255.0 ) << 8 ) |
				( static_cast< uint32_c >( ops::math_saturate( value_as_color.d ) * 255.0 ) );
		}
		else if ( property->_type == data_type_e_uint8 )
		{
			assert( property->_type_count == 3 || property->_type_count == 4 );
			value.uint8[ 0 ] = static_cast< uint8_c >( ops::math_saturate( value_as_color.a ) * 255 );
			value.uint8[ 1 ] = static_cast< uint8_c >( ops::math_saturate( value_as_color.b ) * 255 );
			value.uint8[ 2 ] = static_cast< uint8_c >( ops::math_saturate( value_as_color.c ) * 255 );
			value.uint8[ 3 ] = static_cast< uint8_c >( ops::math_saturate( value_as_color.d ) * 255 ); // this element may not be used but it's okay to set it anyway.
		}
		else if ( property->_type == data_type_e_uint16 )
		{
			assert( property->_type_count == 3 || property->_type_count == 4 );
			value.uint16[ 0 ] = static_cast< uint16_c >( ops::math_saturate( value_as_color.a ) * 65535.0 );
			value.uint16[ 1 ] = static_cast< uint16_c >( ops::math_saturate( value_as_color.b ) * 65535.0 );
			value.uint16[ 2 ] = static_cast< uint16_c >( ops::math_saturate( value_as_color.c ) * 65535.0 );
			value.uint16[ 3 ] = static_cast< uint16_c >( ops::math_saturate( value_as_color.d ) * 65535.0 ); // this element may not be used but it's okay to set it anyway.
		}
		else if ( property->_type == data_type_e_float32 )
		{
			assert( property->_type_count == 3 || property->_type_count == 4 );
			value.float32[ 0 ] = static_cast< float32_c >( value_as_color.a );
			value.float32[ 1 ] = static_cast< float32_c >( value_as_color.b );
			value.float32[ 2 ] = static_cast< float32_c >( value_as_color.c );
			value.float32[ 3 ] = static_cast< float32_c >( value_as_color.d );
		}
		else if ( property->_type == data_type_e_float64 )
		{
			assert( property->_type_count == 3 || property->_type_count == 4 );
			value.float64[ 0 ] = value_as_color.a;
			value.float64[ 1 ] = value_as_color.b;
			value.float64[ 2 ] = value_as_color.c;
			value.float64[ 3 ] = value_as_color.d;
		}
		else
		{
			assert( false );
		}
	}

	vector32x3_c reflection_convert_value_to_euler_angles( reflection_property_c const * property, reflection_value_container_c const & value )
	{
		assert( property );
		assert( property->_view == data_view_e_euler_angles );
		vector32x3_c value_as_euler_angles;
		if ( property->_type == data_type_e_float32 )
		{
			if ( property->_type_count == 3 )
			{
				value_as_euler_angles.a = value.float32[ 0 ];
				value_as_euler_angles.b = value.float32[ 1 ];
				value_as_euler_angles.c = value.float32[ 2 ];
			}
			else if ( property->_type_count == 4 )
			{
				quaternion32_c value_as_quaternion;
				value_as_quaternion.a = value.float32[ 0 ];
				value_as_quaternion.b = value.float32[ 1 ];
				value_as_quaternion.c = value.float32[ 2 ];
				value_as_quaternion.d = value.float32[ 3 ];
				value_as_euler_angles = ops::euler_angles_from_rotation_quaternion32( value_as_quaternion );
			}
			else
			{
				assert( false );
			}
		}
		else if ( property->_type == data_type_e_float64 )
		{
			if ( property->_type_count == 3 )
			{
				value_as_euler_angles.a = static_cast< float32_c >( value.float64[ 0 ] );
				value_as_euler_angles.b = static_cast< float32_c >( value.float64[ 1 ] );
				value_as_euler_angles.c = static_cast< float32_c >( value.float64[ 2 ] );
			}
			else if ( property->_type_count == 4 )
			{
				quaternion32_c value_as_quaternion;
				value_as_quaternion.a = static_cast< float32_c >( value.float64[ 0 ] );
				value_as_quaternion.b = static_cast< float32_c >( value.float64[ 1 ] );
				value_as_quaternion.c = static_cast< float32_c >( value.float64[ 2 ] );
				value_as_quaternion.d = static_cast< float32_c >( value.float64[ 3 ] );
				value_as_euler_angles = ops::euler_angles_from_rotation_quaternion32( value_as_quaternion );
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
		return value_as_euler_angles;
	}

	void_c reflection_convert_euler_angles_to_value( reflection_property_c const * property, reflection_value_container_c & value, vector32x3_c const & value_as_euler_angles )
	{
		assert( property );
		assert( property->_view == data_view_e_euler_angles );
		if ( property->_type == data_type_e_float32 )
		{
			if ( property->_type_count == 3 )
			{
				value.float32[ 0 ] = value_as_euler_angles.a;
				value.float32[ 1 ] = value_as_euler_angles.b;
				value.float32[ 2 ] = value_as_euler_angles.c;
			}
			else if ( property->_type_count == 4 )
			{
				quaternion32_c value_as_quaternion = ops::rotation_quaternion32_from_euler_angles( value_as_euler_angles );
				value.float32[ 0 ] = value_as_quaternion.a;
				value.float32[ 1 ] = value_as_quaternion.b;
				value.float32[ 2 ] = value_as_quaternion.c;
				value.float32[ 3 ] = value_as_quaternion.d;
			}
			else
			{
				assert( false );
			}
		}
		else if ( property->_type == data_type_e_float64 )
		{
			if ( property->_type_count == 3 )
			{
				value.float64[ 0 ] = static_cast< float64_c >( value_as_euler_angles.a );
				value.float64[ 1 ] = static_cast< float64_c >( value_as_euler_angles.b );
				value.float64[ 2 ] = static_cast< float64_c >( value_as_euler_angles.c );
			}
			else if ( property->_type_count == 4 )
			{
				quaternion32_c value_as_quaternion = ops::rotation_quaternion32_from_euler_angles( value_as_euler_angles );
				value.float64[ 0 ] = static_cast< float64_c >( value_as_quaternion.a );
				value.float64[ 1 ] = static_cast< float64_c >( value_as_quaternion.b );
				value.float64[ 2 ] = static_cast< float64_c >( value_as_quaternion.c );
				value.float64[ 3 ] = static_cast< float64_c >( value_as_quaternion.d );
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

	boolean_c reflection_compare_values( reflection_property_c const * property, reflection_value_container_c const & a, reflection_value_container_c const & b )
	{
		assert( property );
		assert( property->_type_count >= 1 && property->_type_count <= 4 );
		if ( property->_type == data_type_e_uint8 || property->_type == data_type_e_sint8 )
		{
			for ( sint32_c i = 0; i < property->_type_count; i++ )
			{
				if ( a.uint8[ i ] != b.uint8[ i ] )
				{
					return false;
				}
			}
		}
		else if ( property->_type == data_type_e_uint16 || property->_type == data_type_e_sint16 )
		{
			for ( sint32_c i = 0; i < property->_type_count; i++ )
			{
				if ( a.uint16[ i ] != b.uint16[ i ] )
				{
					return false;
				}
			}
		}
		else if ( property->_type == data_type_e_uint32 || property->_type == data_type_e_sint32 || property->_type == data_type_e_float32 )
		{
			for ( sint32_c i = 0; i < property->_type_count; i++ )
			{
				if ( a.uint32[ i ] != b.uint32[ i ] )
				{
					return false;
				}
			}
		}
		else if ( property->_type == data_type_e_uint64 || property->_type == data_type_e_sint64 || property->_type == data_type_e_float64 )
		{
			for ( sint32_c i = 0; i < property->_type_count; i++ )
			{
				if ( a.uint64[ i ] != b.uint64[ i ] )
				{
					return false;
				}
			}
		}
		else if ( property->_type == data_type_e_string8 )
		{
			assert( property->_type_count == 1 );
			if ( a.string8 != b.string8 )
			{
				return false;
			}
		}
		else if ( property->_type == data_type_e_string16 )
		{
			assert( property->_type_count == 1 );
			if ( a.string16 != b.string16 )
			{
				return false;
			}
		}
		else
		{
			assert( false );
		}
		return true;
	}

}
