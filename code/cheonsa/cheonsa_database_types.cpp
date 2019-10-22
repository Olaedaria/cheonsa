#include "cheonsa_database_types.h"
#include "cheonsa_debug.h"

namespace cheonsa
{

	uint16_c database_get_data_type_size( data_type_e data_type )
	{
		switch ( data_type ) // allocate additional bytes for value data.
		{
		case data_type_e_string8:
		case data_type_e_string16:
		case data_type_e_uint32:
		case data_type_e_sint32:
		case data_type_e_float32:
			return 4;
		case data_type_e_uint8:
		case data_type_e_sint8:
			return 1;
		case data_type_e_uint16:
		case data_type_e_sint16:
			return 2;
		case data_type_e_uint64:
		case data_type_e_sint64:
		case data_type_e_float64:
			return 8;
		}
		cheonsa_assert( false );
		return 0;
	}

}
