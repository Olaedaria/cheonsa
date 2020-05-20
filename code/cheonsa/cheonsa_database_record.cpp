#include "cheonsa_database_record.h"
#include "cheonsa_database_table.h"
#include "cheonsa_database_field_schema.h"
#include "cheonsa_database.h"
#include "cheonsa_database_stack.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	void_c database_record_c::_set_id( uint32_c value ) const
	{
		database_record_header_c * record_header = reinterpret_cast< database_record_header_c * >( &_table->_record_buffer[ _data_offset ] );
		record_header->id = value;
	}

	void_c database_record_c::_set_mod_record_address( database_record_address_c const & value ) const
	{
		database_record_header_c * record_header = reinterpret_cast< database_record_header_c * >( &_table->_record_buffer[ _data_offset ] );
		record_header->mod_record_address = value;
	}

	database_record_flag_e database_record_c::_get_flags() const
	{
		database_record_header_c const * record_header = reinterpret_cast< database_record_header_c const * >( &_table->_record_buffer[ _data_offset ] );
		return static_cast< database_record_flag_e >( record_header->flags );
	}

	void_c database_record_c::_set_flags( database_record_flag_e value )
	{
		database_record_header_c * record_header = reinterpret_cast< database_record_header_c * >( &_table->_record_buffer[ _data_offset ] );
		record_header->flags = static_cast< uint8_c >( value );
	}

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
		database_record_header_c const * record_header = reinterpret_cast< database_record_header_c const * >( &_table->_record_buffer[ _data_offset ] );
		return record_header->id;
	}

	database_record_address_c database_record_c::get_mod_record_address() const
	{
		database_record_header_c const * record_header = reinterpret_cast< database_record_header_c const * >( &_table->_record_buffer[ _data_offset ] );
		return record_header->mod_record_address;
	}

	boolean_c database_record_c::get_is_hidden() const
	{
		return ( _get_flags() & database_record_flag_e_is_hidden ) != 0;
	}

	void_c database_record_c::set_is_hidden( boolean_c value )
	{
		uint8_c flags = _get_flags();

		if ( !!( flags & database_record_flag_e_is_hidden ) != value )
		{
			flags &= ~database_record_flag_e_is_hidden;
			flags |= value ? database_record_flag_e_is_hidden : 0;
			_set_flags( static_cast< database_record_flag_e >( flags ) );

			// invoke on_modified event.
			database_on_modified_info_c on_modified_info;
			on_modified_info.mod_record_address = get_mod_record_address();
			on_modified_info.record = this;
			on_modified_info.operation = value ? database_operation_e_record_removed : database_operation_e_record_added;
			on_modified_info.field_key_list;
			_table->_database->on_modified.invoke( on_modified_info );
			if ( _table->_database->get_database_stack() )
			{
				_table->_database->get_database_stack()->on_modified.invoke( on_modified_info );
			}
		}
	}

	boolean_c database_record_c::get_is_deleted() const
	{
		return ( _get_flags() & database_record_flag_e_is_deleted ) != 0;
	}

	void_c database_record_c::set_is_deleted( boolean_c value )
	{
		uint8_c flags = _get_flags();

		if ( !!( flags & database_record_flag_e_is_deleted ) != value )
		{
			flags &= ~database_record_flag_e_is_deleted;
			flags |= value ? database_record_flag_e_is_deleted : 0;
			_set_flags( static_cast< database_record_flag_e >( flags ) );

			// invoke on_modified event.
			database_on_modified_info_c on_modified_info;
			on_modified_info.mod_record_address = get_mod_record_address();
			on_modified_info.record = this;
			on_modified_info.operation = value ? database_operation_e_record_removed : database_operation_e_record_added;
			on_modified_info.field_key_list;
			_table->_database->on_modified.invoke( on_modified_info );
			if ( _table->_database->get_database_stack() )
			{
				_table->_database->get_database_stack()->on_modified.invoke( on_modified_info );
			}
		}
	}

	boolean_c database_record_c::get_field_data_pointer( string8_c const & expected_key, data_type_e expected_type, uint8_c expected_type_count, uint8_c * & field_data_pointer ) const
	{
		assert( _table );
		assert( expected_type >= data_type_e_string8 && expected_type <= data_type_e_float64 );
		assert( expected_type_count >= 1 && expected_type_count <= 4 );
		for ( sint32_c i = 0; i < _table->_record_schema._fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _table->_record_schema._fields[ i ];
			if ( field._key == expected_key && field._type == expected_type && field._type_count == expected_type_count )
			{
				field_data_pointer = &_table->_record_buffer[ _data_offset + field._data_offset ];
				return true;
			}
		}
		assert( false );
		return false;
	}

	boolean_c database_record_c::get_field_value( string8_c const & expected_key, data_type_e expected_type, uint8_c expected_type_count, void_c * field_value, boolean_c & got_default ) const
	{
		assert( field_value );
		assert( _table );
		assert( expected_type >= data_type_e_string8 && expected_type <= data_type_e_float64 );
		assert( expected_type_count >= 1 && expected_type_count <= 4 );
		for ( sint32_c i = 0; i < _table->_record_schema._fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _table->_record_schema._fields[ i ];
			if ( field._key == expected_key && field._type == expected_type && field._type_count == expected_type_count )
			{
				uint8_c * field_data = &_table->_record_buffer[ _data_offset + field._data_offset ];
				database_field_header_c const * field_header = reinterpret_cast< database_field_header_c const * >( field_data );
				if ( field_header->flags & database_field_flag_e_is_defined )
				{
					if ( expected_type == data_type_e_string8 )
					{
						assert( expected_type_count == 1 );
						*reinterpret_cast< string8_c * >( field_value ) = _table->_get_string8( *reinterpret_cast< sint32_c const * >( &field_data[ sizeof( database_field_header_c ) ] ) );
					}
					else if ( expected_type == data_type_e_string16 )
					{
						assert( expected_type_count == 1 );
						*reinterpret_cast< string16_c * >( field_value ) = _table->_get_string16( *reinterpret_cast< sint32_c const * >( &field_data[ sizeof( database_field_header_c ) ] ) );
					}
					else
					{
						assert( 1 + ( database_get_data_type_size( expected_type ) * expected_type_count ) == field._data_size );
						ops::memory_copy( &field_data[ sizeof( database_field_header_c ) ], field_value, field._data_size - 1 );
					}
					got_default = false;
					return true;
				}
				else
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
						ops::memory_copy( &field._reflection_information.defaults_and_limits, field_value, field._data_size - 1 );
					}
					got_default = true;
					return true;
				}
			}
		}
		assert( false );
		return false;
	}

	boolean_c database_record_c::set_field_value( string8_c const & expected_key, data_type_e expected_type, uint8_c expected_type_count, void_c const * field_value ) const
	{
		assert( _table );
		assert( expected_type >= data_type_e_string8 && expected_type <= data_type_e_float64 );
		assert( expected_type_count >= 1 && expected_type_count <= 4 );
		for ( sint32_c i = 0; i < _table->_record_schema._fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _table->_record_schema._fields[ i ];
			if ( field._key == expected_key && field._type == expected_type && field._type_count == expected_type_count )
			{
				// update the field value.
				uint8_c * field_data = &_table->_record_buffer[ _data_offset + field._data_offset ];
				database_field_header_c * field_header = reinterpret_cast< database_field_header_c * >( field_data );
				field_header->flags |= database_field_flag_e_is_defined;
				if ( expected_type == data_type_e_string8 )
				{
					assert( expected_type_count == 1 );
					*reinterpret_cast< sint32_c * >( &field_data[ sizeof( database_field_header_c ) ] ) = _table->_add_string8( *reinterpret_cast< string8_c const * >( field_value ) );
				}
				else if ( expected_type == data_type_e_string16 )
				{
					assert( expected_type_count == 1 );
					*reinterpret_cast< sint32_c * >( &field_data[ sizeof( database_field_header_c ) ] ) = _table->_add_string16( *reinterpret_cast< string16_c const * >( field_value ) );
				}
				else
				{
					assert( field._data_size - 1 == database_get_data_type_size( expected_type ) * expected_type_count );
					ops::memory_copy( field_value, &field_data[ 1 ], field._data_size - 1 );
				}

				// invoke on_modified event.
				database_on_modified_info_c on_modified_info;
				on_modified_info.mod_record_address = get_mod_record_address();
				on_modified_info.record = this;
				on_modified_info.operation = database_operation_e_record_modified;
				on_modified_info.field_key_list = expected_key;
				_table->_database->on_modified.invoke( on_modified_info );
				if ( _table->_database->get_database_stack() )
				{
					_table->_database->get_database_stack()->on_modified.invoke( on_modified_info );
				}
				return true;
			}
		}
		assert( false );
		return false;
	}

	boolean_c database_record_c::undefine_field_value( string8_c const & expected_key, data_type_e expected_type, uint8_c expected_type_count ) const
	{
		assert( _table );
		assert( expected_type >= data_type_e_string8 && expected_type <= data_type_e_float64 );
		assert( expected_type_count >= 1 && expected_type_count <= 4 );
		for ( sint32_c i = 0; i < _table->_record_schema._fields.get_length(); i++ )
		{
			database_field_schema_c const & field = _table->_record_schema._fields[ i ];
			if ( field._key == expected_key && field._type == expected_type && field._type_count == expected_type_count )
			{
				// clear the field value.
				uint8_c * field_data = &_table->_record_buffer[ _data_offset + field._data_offset ];
				database_field_header_c * field_header = reinterpret_cast< database_field_header_c * >( field_data );
				field_header->flags &= ~database_field_flag_e_is_defined;
				field_data++;
				ops::memory_zero( field_data, field._data_size - 1 );

				// invoke on_modified event.
				database_on_modified_info_c on_modified_info;
				on_modified_info.mod_record_address = get_mod_record_address();
				on_modified_info.record = this;
				on_modified_info.operation = database_operation_e_record_modified;
				on_modified_info.field_key_list = expected_key;
				_table->_database->on_modified.invoke( on_modified_info );
				if ( _table->_database->get_database_stack() )
				{
					_table->_database->get_database_stack()->on_modified.invoke( on_modified_info );
				}
				return true;
			}
		}
		assert( false );
		return false;
	}

}
