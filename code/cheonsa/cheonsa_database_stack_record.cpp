#include "cheonsa_database_stack_record.h"
#include "cheonsa_database_stack.h"
#include "cheonsa_database_record.h"
#include "cheonsa_database_table.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	database_stack_record_c::database_stack_record_c()
		: _database_stack( nullptr )
		, _record_schema()
		, _record_stack()
	{
	}

	database_stack_c const * database_stack_record_c::get_database_stack() const
	{
		return _database_stack;
	}

	boolean_c database_stack_record_c::sample_field_value( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count, void_c * field_value ) const
	{
		database_record_c const * last_record = nullptr;
		uint8_c * last_field_data_pointer = nullptr;
		for ( sint32_c i = 0; i < _record_stack.get_length(); i++ )
		{
			database_record_c const * record = _record_stack[ i ];
			if ( record != nullptr )
			{
				assert( !record->get_is_hidden() );
				uint8_c * field_data_pointer = nullptr;
				record->get_field_data_pointer( expected_name, expected_type, expected_type_count, field_data_pointer );
				uint8_c field_flags = *field_data_pointer;
				if ( ( field_flags & database_field_flag_e_is_defined ) != 0 )
				{
					last_record = record;
					last_field_data_pointer = field_data_pointer + 1;
				}
			}
		}
		if ( last_record )
		{
			// value was defined.
			if ( expected_type == data_type_e_string8 )
			{
				*reinterpret_cast< string8_c * >( field_value ) = last_record->get_table()->_get_string8( *reinterpret_cast< sint32_c * >( last_field_data_pointer ) );
			}
			else if ( expected_type == data_type_e_string16 )
			{
				*reinterpret_cast< string16_c * >( field_value ) = last_record->get_table()->_get_string16( *reinterpret_cast< sint32_c * >( last_field_data_pointer ) );
			}
			else
			{
				ops::memory_copy( last_field_data_pointer, field_value, database_get_data_type_size( expected_type ) * expected_type_count );
			}
			return true;
		}
		else
		{
			// value was not defined, use default.
			if ( expected_type == data_type_e_string8 )
			{
				*reinterpret_cast< string8_c * >( field_value ) = string8_c();
			}
			else if ( expected_type == data_type_e_string16 )
			{
				*reinterpret_cast< string16_c * >( field_value ) = string16_c();
			}
			else
			{
				assert( _record_schema );
				database_field_schema_c const * field = _record_schema->get_field( expected_name, expected_type, expected_type_count );
				ops::memory_copy( &field->reflection_info.defaults_and_limits, field_value, field->_data_offset - 1 );
			}
			return false;
		}
	}

}
