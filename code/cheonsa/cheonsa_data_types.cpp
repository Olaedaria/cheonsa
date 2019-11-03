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

	data_endianness_e data_get_native_endianness()
	{
		const union { uint32_c u; uint8_c c[ 4 ]; } probe = { 1 };
		return probe.c[ 0 ] != 0 ? data_endianness_e_little : data_endianness_e_big;
	}

}
