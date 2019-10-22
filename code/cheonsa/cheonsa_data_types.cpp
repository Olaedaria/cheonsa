#include "cheonsa_data_types.h"
#include "cheonsa_base_types.h"

namespace cheonsa
{

	sint32_c data_get_type_size( data_type_e type )
	{
		switch( type )
		{
		case data_type_e_uint8:
		case data_type_e_sint8:
			return 1;
		case data_type_e_uint16:
		case data_type_e_sint16:
			return 2;
		case data_type_e_uint32:
		case data_type_e_sint32:
			return 4;
		case data_type_e_uint64:
		case data_type_e_sint64:
			return 8;
		case data_type_e_float32:
			return 4;
		case data_type_e_float64:
			return 8;
		}
		return 0;
	}

	static union { uint16_c i; uint8_c c[ 2 ]; } const bint = { 0x0001 }; // the shortest-handest c i have ever seened.

	data_endianness_e _native_endianness = ( bint.c[ 0 ] == 0x00 ? data_endianness_e_big : data_endianness_e_little );

	data_endianness_e data_get_native_endianness()
	{
		return _native_endianness;
	}

}
