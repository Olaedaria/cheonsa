#include "cheonsa_database_record.h"
#include "cheonsa_database_table.h"
#include "cheonsa_database_field_schema.h"
#include "cheonsa_database.h"
#include "cheonsa_database_stack.h"
#include "cheonsa_ops.h"
#include "cheonsa_debug.h"

namespace cheonsa
{

	database_record_c::database_record_c()
		: _table( nullptr )
		, _data_offset( 0 )
	{
	}

	database_record_c::~database_record_c()
	{
	}

	database_table_c const * database_record_c::get_table() const
	{
		return _table;
	}

	database_table_c * database_record_c::get_table()
	{
		return _table;
	}

	uint32_c database_record_c::get_id() const
	{
		database_record_header_c const * record_header = reinterpret_cast< database_record_header_c const * >( &_table->_records_buffer[ _data_offset ] );
		return record_header->id;
	}

	void_c database_record_c::set_id( uint32_c value ) const
	{
		database_record_header_c * record_header = reinterpret_cast< database_record_header_c * >( &_table->_records_buffer[ _data_offset ] );
		record_header->id = value;
	}

	database_record_address_c database_record_c::get_mod_record_address() const
	{
		database_record_header_c const * record_header = reinterpret_cast< database_record_header_c const * >( &_table->_records_buffer[ _data_offset ] );
		return record_header->mod_record_address;
	}

	void_c database_record_c::set_mod_record_address( database_record_address_c const & value ) const
	{
		database_record_header_c * record_header = reinterpret_cast< database_record_header_c * >( &_table->_records_buffer[ _data_offset ] );
		record_header->mod_record_address = value;
	}

	uint8_c database_record_c::get_flags() const
	{
		database_record_header_c const * record_header = reinterpret_cast< database_record_header_c const * >( &_table->_records_buffer[ _data_offset ] );
		return record_header->flags;
	}

	void_c database_record_c::set_flags( uint8_c value )
	{
		database_record_header_c * record_header = reinterpret_cast< database_record_header_c * >( &_table->_records_buffer[ _data_offset ] );
		record_header->flags = value;
	}

	boolean_c database_record_c::get_is_hidden() const
	{
		return ( get_flags() & database_record_flag_e_is_hidden ) != 0;
	}

	void_c database_record_c::set_is_hidden( boolean_c value )
	{
		uint8_c flags = get_flags();
		flags &= ~database_record_flag_e_is_hidden;
		flags |= value ? database_record_flag_e_is_hidden : 0;
		set_flags( flags );
	}

	boolean_c database_record_c::get_is_deleted() const
	{
		return ( get_flags() & database_record_flag_e_is_deleted ) != 0;
	}

	void_c database_record_c::set_is_deleted( boolean_c value )
	{
		uint8_c flags = get_flags();
		flags &= ~database_record_flag_e_is_deleted;
		flags |= value ? database_record_flag_e_is_deleted : 0;
		set_flags( flags );
	}

	void_c database_record_c::get_field_data_pointer( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count, uint8_c * & field_data_pointer ) const
	{
		cheonsa_assert( _table );
		cheonsa_assert( expected_type >= data_type_e_string8 && expected_type <= data_type_e_float64 );
		cheonsa_assert( expected_type_count >= 1 && expected_type_count <= 4 );
		for ( sint32_c i = 0; i < _table->_records_schema._fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _table->_records_schema._fields[ i ];
			if ( field.name == expected_name && field.type == expected_type && field.type_count == expected_type_count )
			{
				field_data_pointer = &_table->_records_buffer[ _data_offset + field._data_offset ];
				return;
			}
		}
		cheonsa_assert( false );
	}

	boolean_c database_record_c::get_field_value( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count, void_c * field_value, boolean_c if_not_defined_then_get_default ) const
	{
		cheonsa_assert( field_value );
		cheonsa_assert( _table );
		cheonsa_assert( expected_type >= data_type_e_string8 && expected_type <= data_type_e_float64 );
		cheonsa_assert( expected_type_count >= 1 && expected_type_count <= 4 );
		for ( sint32_c i = 0; i < _table->_records_schema._fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _table->_records_schema._fields[ i ];
			if ( field.name == expected_name && field.type == expected_type && field.type_count == expected_type_count )
			{
				uint8_c * field_data_pointer = &_table->_records_buffer[ _data_offset + field._data_offset ];
				uint8_c field_flags = *field_data_pointer;
				field_data_pointer++;
				if ( field_flags & database_field_flag_e_is_defined )
				{
					if ( expected_type == data_type_e_string8 )
					{
						cheonsa_assert( expected_type_count == 1 );
						*reinterpret_cast< string8_c * >( field_value ) = _table->get_string8( *reinterpret_cast< sint32_c const * >( field_data_pointer ) );
					}
					else if ( expected_type == data_type_e_string16 )
					{
						cheonsa_assert( expected_type_count == 1 );
						*reinterpret_cast< string16_c * >( field_value ) = _table->get_string16( *reinterpret_cast< sint32_c const * >( field_data_pointer ) );
					}
					else
					{
						cheonsa_assert( field._data_size - 1 == database_get_data_type_size( expected_type ) * expected_type_count );
						ops::memory_copy( field_data_pointer, field_value, field._data_size - 1 );
					}
					return true;
				}
				else
				{
					if ( if_not_defined_then_get_default )
					{
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
							ops::memory_copy( &field.reflection_info.defaults_and_limits, field_value, field._data_size - 1 );
						}
					}
					return false;
				}
			}
		}
		cheonsa_assert( false );
		return false; // should be urneachable.
	}

	void_c database_record_c::set_field_value( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count, void_c const * field_value ) const
	{
		cheonsa_assert( _table );
		cheonsa_assert( expected_type >= data_type_e_string8 && expected_type <= data_type_e_float64 );
		cheonsa_assert( expected_type_count >= 1 && expected_type_count <= 4 );
		for ( sint32_c i = 0; i < _table->_records_schema._fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _table->_records_schema._fields[ i ];
			if ( field.name == expected_name && field.type == expected_type && field.type_count == expected_type_count )
			{
				// update the field value.
				uint8_c * field_data_pointer = &_table->_records_buffer[ _data_offset + field._data_offset ];
				*field_data_pointer |= database_field_flag_e_is_defined; // first byte is flags.
				field_data_pointer++; // advance past flags.
				if ( expected_type == data_type_e_string8 )
				{
					cheonsa_assert( expected_type_count == 1 );
					*reinterpret_cast< sint32_c * >( field_data_pointer ) = _table->add_string8( *reinterpret_cast< string8_c const * >( field_value ) );
				}
				else if ( expected_type == data_type_e_string16 )
				{
					cheonsa_assert( expected_type_count == 1 );
					*reinterpret_cast< sint32_c * >( field_data_pointer ) = _table->add_string16( *reinterpret_cast< string16_c const * >( field_value ) );
				}
				else
				{
					cheonsa_assert( field._data_size - 1 == database_get_data_type_size( expected_type ) * expected_type_count );
					ops::memory_copy( field_value, field_data_pointer, field._data_size - 1 );
				}

				// invoke on_modified event.
				database_on_modified_info_c on_modified_info;
				on_modified_info.mod_record_address = get_mod_record_address();
				on_modified_info.record = this;
				on_modified_info.operation = database_operation_e_record_modified;
				on_modified_info.fields = expected_name;
				_table->_database->on_modified.invoke( on_modified_info );
				if ( _table->_database->get_database_stack() )
				{
					_table->_database->get_database_stack()->on_modified.invoke( on_modified_info );
				}
				return;
			}
		}
		cheonsa_assert( false );
	}

	void_c database_record_c::undefine_field_value( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count ) const
	{
		cheonsa_assert( _table );
		cheonsa_assert( expected_type >= data_type_e_string8 && expected_type <= data_type_e_float64 );
		cheonsa_assert( expected_type_count >= 1 && expected_type_count <= 4 );
		for ( sint32_c i = 0; i < _table->_records_schema._fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _table->_records_schema._fields[ i ];
			if ( field.name == expected_name && field.type == expected_type && field.type_count == expected_type_count )
			{
				// clear the field value.
				uint8_c * field_data = &_table->_records_buffer[ _data_offset + field._data_offset ];
				*field_data &= ~database_field_flag_e_is_defined;
				field_data++;
				ops::memory_zero( field_data, field._data_size - 1 );

				// invoke on_modified event.
				database_on_modified_info_c on_modified_info;
				on_modified_info.mod_record_address = get_mod_record_address();
				on_modified_info.record = this;
				on_modified_info.operation = database_operation_e_record_modified;
				on_modified_info.fields = expected_name;
				_table->_database->on_modified.invoke( on_modified_info );
				if ( _table->_database->get_database_stack() )
				{
					_table->_database->get_database_stack()->on_modified.invoke( on_modified_info );
				}
			}
		}
		cheonsa_assert( false );
	}

}
