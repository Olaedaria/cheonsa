#include "cheonsa_database_table.h"
#include "cheonsa_database_field_schema.h"
#include "cheonsa_database_record.h"
#include "cheonsa_database.h"
#include "cheonsa_database_stack.h"
#include "cheonsa__ops.h"
#include <cassert>

namespace cheonsa
{

	sint32_c absolute_value_sint32( sint32_c const & a )
	{
		return a;
	}

	uint32_c absolute_value_uint32( uint32_c const & a )
	{
		return a;
	}

	boolean_c database_table_c::_save( data_scribe_binary_c & scribe )
	{
		// save meta.
		if ( !scribe.save_string8( _name ) )
		{
			return false;
		}
		if ( !scribe.save_uint16( _id ) )
		{
			return false;
		}
		if ( !scribe.save_uint8( _flags ) )
		{
			return false;
		}
		if ( !scribe.save_uint32( _next_record_id ) )
		{
			return false;
		}

		// save strings.
		if ( !scribe.save_sint32( _string_table.get_length() ) )
		{
			return false;
		}
		if ( !scribe.get_stream()->save( _string_table.get_internal_array(), _string_table.get_internal_array_size_used() ) )
		{
			return false;
		}
		core_list_c< sint32_c > string_offsets = _string_table_map.get_value_list();
		string_offsets.quick_sort( &absolute_value_sint32, false );
		if ( !scribe.save_sint32( string_offsets.get_length() ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < string_offsets.get_length(); i++ )
		{
			if ( !scribe.save_sint32( string_offsets[ i ] ) )
			{
				return false;
			}
		}

		// save schema.
		assert( _record_schema._fields.get_length() < constants< uint16_c >::maximum() );
		if ( !scribe.save_uint16( static_cast< uint16_c >( _record_schema._fields.get_length() ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < _record_schema._fields.get_length(); i++ )
		{
			database_field_schema_c const & field_schema = _record_schema._fields[ i ];
			if ( !scribe.save_string8( field_schema.name ) )
			{
				return false;
			}
			if ( !scribe.save_uint8( static_cast< uint8_c >( field_schema.type ) ) )
			{
				return false;
			}
			if ( !scribe.save_uint8( field_schema.type_count ) )
			{
				return false;
			}
		}

		// save records.
		if ( !scribe.save_sint32( _record_map.get_length() ) )
		{
			return false;
		}
		if ( !scribe.save_sint32( _record_buffer.get_length() ) )
		{
			return false;
		}
		if ( scribe.get_byte_order() == ops::get_native_byte_order() )
		{
			if ( !scribe.get_stream()->save( _record_buffer.get_internal_array(), _record_buffer.get_internal_array_size_used() ) )
			{
				return false;
			}
		}
		else
		{
			if ( !_record_schema.save_record_buffer( scribe, _record_buffer, _record_map.get_length() ) )
			{
				return false;
			}
		}

		return true;
	}

	boolean_c database_table_c::_load( data_scribe_binary_c & scribe )
	{
		// reset.
		_name = string8_c();
		_id = 0;
		_flags = 0;
		_next_record_id = 0;
		_string_table.remove_all();
		_string_table_map.remove_all();
		_record_schema.reset();
		_record_buffer.remove_all();
		_record_map.remove_and_delete_all();
		_record_mod_map.remove_all();

		// load meta.
		core_list_c< sint32_c > strings_offsets;
		if ( !scribe.load_string8( _name ) )
		{
			goto cancel;
		}
		if ( !scribe.load_uint16( _id ) )
		{
			goto cancel;
		}
		if ( !scribe.load_uint8( _flags ) )
		{
			goto cancel;
		}
		if ( !scribe.load_uint32( _next_record_id ) )
		{
			goto cancel;
		}

		// load strings.
		sint32_c count = 0;
		if ( !scribe.load_sint32( count ) )
		{
			goto cancel;
		}
		assert( count > 0 );
		for ( sint32_c i = 0; i < count; i++ )
		{
			sint32_c string_offset = 0;
			if ( !scribe.load_sint32( string_offset ) )
			{
				goto cancel;
			}
			strings_offsets.insert_at_end( string_offset );
		}
		assert( strings_offsets[ 0 ] == 0 );
		if ( !scribe.load_sint32( count ) )
		{
			goto cancel;
		}
		assert( count > 0 );
		_string_table.set_length( count );
		scribe.get_stream()->load( _string_table.get_internal_array(), _string_table.get_internal_array_size_used() );
		assert( _string_table[ 0 ] == 0 );
		for ( sint32_c i = 0; i < strings_offsets.get_length(); i++ )
		{
			sint32_c string_offset = strings_offsets[ i ];
			string8_c string_value = string8_c( mode_e_dynamic, &_string_table[ string_offset ] );
			_string_table_map.insert( string_value, string_offset );
		}

		// load schema.
		uint16_c count2;
		if ( !scribe.load_uint16( count2 ) )
		{
			goto cancel;
		}
		assert( count2 > 0 );
		for ( sint32_c i = 0; i < count2; i++ )
		{
			string8_c field_name;
			if ( !scribe.load_string8( field_name ) )
			{
				goto cancel;
			}
			uint8_c field_type = 0;
			if ( !scribe.load_uint8( field_type ) )
			{
				goto cancel;
			}
			uint8_c field_type_count = 0;
			if ( !scribe.load_uint8( field_type_count ) )
			{
				goto cancel;
			}
			_record_schema.add_field( field_name, static_cast< data_type_e >( field_type ), field_type_count );
		}

		// load records.
		sint32_c records_count = 0;
		if ( !scribe.load_sint32( records_count ) )
		{
			goto cancel;
		}
		sint32_c records_buffer_length = 0;
		if ( !scribe.load_sint32( records_buffer_length ) )
		{
			goto cancel;
		}
		_record_buffer.set_length( records_buffer_length );
		if ( scribe.get_byte_order() == ops::get_native_byte_order() )
		{
			if ( !scribe.get_stream()->load( _record_buffer.get_internal_array(), _record_buffer.get_internal_array_size_used() ) )
			{
				goto cancel;
			}
		}
		else
		{
			if ( !_record_schema.load_record_buffer( scribe, _record_buffer, records_count ) )
			{
				goto cancel;
			}
		}
		assert( _record_schema.get_size() * records_count == records_buffer_length );
		uint8_c const * records_buffer_data = _record_buffer.get_internal_array();
		sint32_c record_offset = 0;
		sint32_c record_size = _record_schema.get_size();
		for ( sint32_c i = 0; i < records_count; i++ )
		{
			database_record_c * record = new database_record_c();
			record->_table = this;
			record->_data_offset = record_offset;
			database_record_header_c const * record_header = reinterpret_cast< database_record_header_c const * >( &records_buffer_data[ record_offset ] );
			_record_map.insert( record_header->id, record );
			if ( record_header->mod_record_address.is_set() )
			{
				_record_mod_map.insert( record_header->mod_record_address, record );
			}
			record_offset += record_size;
		}

		return true;

	cancel:
		_name = string8_c();
		_id = 0;
		_flags = 0;
		_next_record_id = 0;
		_string_table.remove_all();
		_string_table_map.remove_all();
		_record_schema.reset();
		_record_buffer.remove_all();
		_record_map.remove_and_delete_all();
		_record_mod_map.remove_all();

		return false;
	}

	sint32_c database_table_c::_add_string8( string8_c const & value )
	{
		sint32_c * buffer_offset_pointer = _string_table_map.find_value_pointer( value );
		if ( buffer_offset_pointer )
		{
			return *buffer_offset_pointer;
		}
		sint32_c strings_buffer_offset = _string_table.get_length();
		char8_c * strings_buffer = _string_table.emplace_range_at_end( value.character_list.get_length() );
		ops::memory_copy( value.character_list.get_internal_array(), strings_buffer, value.character_list.get_length() );
		_string_table_map.insert( string8_c( mode_e_dynamic, value.character_list.get_internal_array(), value.character_list.get_length() ), strings_buffer_offset );
		return strings_buffer_offset;
	}

	string8_c database_table_c::_get_string8( sint32_c offset ) const
	{
		return string8_c( mode_e_static_volatile, &_string_table[ offset ] );
	}

	sint32_c database_table_c::_add_string16( string16_c const & value )
	{
		return _add_string8( string8_c( value ) );
	}

	string16_c database_table_c::_get_string16( sint32_c offset ) const
	{
		return string16_c( &_string_table[ offset ] );
	}

	database_table_c::database_table_c()
		: _database( nullptr )
		, _name()
		, _id( 0 )
		, _flags( 0 )
		, _next_record_id( 0 )
		, _string_table()
		, _string_table_map()
		, _record_schema()
		, _record_buffer()
		, _record_map()
		, _record_mod_map()
	{
	}

	string8_c const & database_table_c::get_name() const
	{
		return _name;
	}

	void_c database_table_c::set_name( string8_c const & value )
	{
		_name = value;
	}

	uint16_c database_table_c::get_id() const
	{
		return _id;
	}

	void_c database_table_c::set_id( uint16_c value )
	{
		_id = value;
	}

	void_c database_table_c::reset()
	{
		_string_table.remove_all();
		_string_table_map.remove_all();
		_record_schema.reset();
		_record_buffer.remove_all();
		_record_map.remove_all();
		_record_mod_map.remove_all();
	}

	database_record_schema_c const & database_table_c::get_records_schema() const
	{
		return _record_schema;
	}

	void_c database_table_c::set_records_schema( database_record_schema_c const & value )
	{
		if ( value._fields.get_length() == 0 )
		{
			// just delete everything in this table.
			_string_table.remove_all();
			_string_table_map.remove_all();
			_record_schema = value;
			_record_buffer.remove_all();
			_record_map.remove_all();
			_record_mod_map.remove_all();
			return;
		}

		// defines how to remap fields from the old format to the new one.
		struct remap_c
		{
			database_field_schema_c const * from; // may be nullptr.
			database_field_schema_c const * to; // will never be nullptr.
		};

		// reformat|migrate existing data if needed.
		if ( ( _record_schema._fields.get_length() > 0 ) && ( _record_schema != value ) )
		{
			// build remap rules, which tells us how to shift field data around from the old schema format to the new one.
			core_list_c< remap_c > remaps;
			remaps.set_length( value._fields.get_length() );
			for ( sint32_c i = 0; i < value._fields.get_length(); i++ )
			{
				remap_c * remap = &remaps[ i ];
				remap->to = &value._fields[ i ];
				remap->from = _record_schema.get_field( remap->to->name, remap->to->type, remap->to->type_count );
			}

			// copy existing buffers to temporaries so we can rebuild new buffers in their place.
			core_list_c< char8_c > old_string_table;
			_string_table.transfer_to( old_string_table );
			_string_table_map.remove_all();
			_add_string8( string8_c() ); // add default empty string.
			database_record_schema_c old_records_schema;
			old_records_schema = _record_schema;
			_record_schema = value;
			core_list_c< uint8_c > old_records_buffer;
			_record_buffer.transfer_to( old_records_buffer );
			core_dictionary_c< uint32_c, database_record_c * > old_records_map;
			_record_map.transfer_to( old_records_map );
			_record_mod_map.remove_all();
			// go through each old record and determine if it needs to be converted or deleted.
			sint32_c new_record_size = _record_schema.get_size();
			core_list_c< uint32_c > record_ids = old_records_map.get_key_list();
			record_ids.quick_sort( &absolute_value_uint32, false );
			for ( sint32_c i = 0; i < record_ids.get_length(); i++ )
			{
				uint32_c record_id = record_ids[ i ];
				database_record_c * record = old_records_map.find_value( record_id );
				uint8_c const * old_record_data = &old_records_buffer[ record->_data_offset ];
				database_record_header_c const * old_record_header = reinterpret_cast< database_record_header_c const * >( old_record_data );
				if ( ( old_record_header->flags & database_record_flag_e_is_deleted ) != 0 )
				{
					sint32_c new_record_offset = _record_buffer.get_length();
					uint8_c * new_record_data = _record_buffer.emplace_range_at_end( new_record_size );
					ops::memory_copy( old_record_data, new_record_data, sizeof( database_record_header_c ) );
					for ( sint32_c j = 0; j < remaps.get_length(); j++ )
					{
						remap_c const * remap = &remaps[ j ];
						if ( remap->from )
						{
							// remapped field.
							database_field_schema_c const & old_field_schema = old_records_schema._fields[ remap->from->_index ];
							if ( old_field_schema.type == data_type_e_string8 || old_field_schema.type == data_type_e_string16 )
							{
								// add new string.
								new_record_data[ remap->to->_data_offset ] = static_cast< uint8_c >( database_field_flag_e_is_defined );
								*reinterpret_cast< sint32_c * >( &new_record_data[ remap->to->_data_offset + 1 ] ) = _add_string8( string8_c( mode_e_static, &old_string_table[ *reinterpret_cast< sint32_c const * >( old_record_data[ remap->from->_data_offset + 1 ] ) ] ) );
							}
							else
							{
								// straight copy.
								ops::memory_copy( &old_record_data[ remap->from->_data_offset ], &new_record_data[ remap->to->_data_offset ], remap->from->_data_size );
							}
						}
						else
						{
							// new field.
							// default to undefined.
							ops::memory_zero( &new_record_data[ remap->to->_data_offset ], remap->to->_data_size );
						}
					}
					record->_data_offset = new_record_offset;
					new_record_offset += new_record_size;
					_record_map.insert( record_id, record );
					if ( old_record_header->mod_record_address.is_set() )
					{
						_record_mod_map.insert( old_record_header->mod_record_address, record );
					}
				}
				else
				{
					delete record;
				}
			}
		}
	}

	database_record_c const * database_table_c::get_record_with_id( uint32_c id ) const
	{
		return _record_map.find_value_else_nullptr( id );
	}

	database_record_c * database_table_c::get_record_with_id( uint32_c id )
	{
		return _record_map.find_value_else_nullptr( id );
	}

	database_record_c const * database_table_c::get_record_with_mod_record_address( database_record_address_c const & mod_record_address ) const
	{
		return _record_mod_map.find_value_else_nullptr( mod_record_address );
	}

	database_record_c * database_table_c::get_record_with_mod_record_address( database_record_address_c const & mod_record_address )
	{
		return _record_mod_map.find_value_else_nullptr( mod_record_address );
	}

	database_record_c * database_table_c::add_record( database_record_address_c mod_record_address )
	{
		// find an unused id.
		// this linear search can take very long.
		uint32_c record_id = _next_record_id;
		uint32_c i = 0;
		while ( _record_map.contains( record_id ) )
		{
			assert( i != 0xFFFFFFFF );
			i++;
			record_id++;
		}
		_next_record_id = record_id + 1;

		// add the record.
		database_record_c * record = new database_record_c();
		record->_table = this;
		record->_data_offset = _record_buffer.get_length();
		sint32_c record_size = _record_schema.get_size();
		uint8_c * record_data = _record_buffer.emplace_range_at_end( record_size );
		ops::memory_zero( record_data, record_size );
		database_record_header_c * record_header = reinterpret_cast< database_record_header_c * >( record_data );
		record_header->id = record_id;
		record_header->mod_record_address = mod_record_address;
		_record_map.insert( record_id, record );
		if ( mod_record_address.is_set() )
		{
			_record_mod_map.insert( mod_record_address, record );
		}

		// invoke on_modified event.
		database_on_modified_info_c on_modified_info;
		on_modified_info.operation = database_operation_e_record_added;
		on_modified_info.mod_record_address.database_id = _database->get_id();
		on_modified_info.mod_record_address.table_id = _id;
		_database->on_modified.invoke( on_modified_info );
		if ( _database->get_database_stack() )
		{
			_database->get_database_stack()->on_modified.invoke( on_modified_info );
		}

		return record;
	}

	void_c database_table_c::remove_record_with_id( uint32_c id )
	{
		// remove and delete the record.
		database_record_c * record = _record_map.find_value_else_nullptr( id );
		assert( record );
		_record_map.remove( id );
		database_record_address_c mod_record_address = record->get_mod_record_address();
		if ( mod_record_address.is_set() )
		{
			_record_mod_map.remove( mod_record_address );
		}
		delete record;

		// invoke on_modified event.
		database_on_modified_info_c on_modified_info;
		on_modified_info.operation = database_operation_e_record_removed;
		on_modified_info.mod_record_address.database_id = _database->get_id();
		on_modified_info.mod_record_address.table_id = _id;
		_database->on_modified.invoke( on_modified_info );
		if ( _database->get_database_stack() )
		{
			_database->get_database_stack()->on_modified.invoke( on_modified_info );
		}
	}

	void_c database_table_c::rebuild()
	{
		// defer processing to set_records_schema() because it has the side effect of rebuilding the buffers.
		// maybe not optimal.
		set_records_schema( _record_schema );
	}

}
