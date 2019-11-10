#include "cheonsa_database.h"
#include "cheonsa_database_table.h"
#include "cheonsa__ops.h"
#include "cheonsa_data_stream_file.h"
#include <cassert>

namespace cheonsa
{

	database_c::database_c( string16_c const & file_path )
		: _database_stack( nullptr )
		, _file_path( file_path )
		//, _endianness( ops::get_native_byte_order() )
		, _name()
		, _id( 0 )
		, _flags( 0 )
		, _dependency_ids()
		, _tables()
	{
	}

	database_stack_c const * database_c::get_database_stack() const
	{
		return _database_stack;
	}

	string16_c const & database_c::get_file_path() const
	{
		return _file_path;
	}

	void_c database_c::set_file_path( string16_c const & value )
	{
		_file_path = value;
	}

	//byte_order_e database_c::get_byte_order() const
	//{
	//	return _byte_order;
	//}

	//void_c database_c::set_byte_order( byte_order_e value )
	//{
	//	_byte_order = value;
	//}

	string8_c const & database_c::get_name() const
	{
		return _name;
	}

	void_c database_c::set_name( string8_c const & value )
	{
		_name = value;
	}

	uint16_c database_c::get_id() const
	{
		return _id;
	}

	void_c database_c::set_id( uint16_c value )
	{
		_id = value;
	}

	uint8_c database_c::get_flags() const
	{
		return _flags;
	}

	void_c database_c::set_flags( uint8_c value )
	{
		_flags = value;
	}

	database_table_c * database_c::add_table( uint16_c id )
	{
		database_table_c * table = new database_table_c();
		table->_database = this;
		table->_id = id;
		_tables.insert_at_end( table );
		return table;
	}

	database_table_c const * database_c::get_table( uint16_c id ) const
	{
		for ( sint32_c i = 0; i < _tables.get_length(); i++ )
		{
			database_table_c * table = _tables[ i ];
			if ( table->_id == id )
			{
				return table;
			}
		}
		return nullptr;
	}

	database_table_c * database_c::get_table( uint16_c id )
	{
		for ( sint32_c i = 0; i < _tables.get_length(); i++ )
		{
			database_table_c * table = _tables[ i ];
			if ( table->_id == id )
			{
				return table;
			}
		}
		return nullptr;
	}

	void_c database_c::remove_table( uint16_c id )
	{
		for ( sint32_c i = 0; i < _tables.get_length(); i++ )
		{
			database_table_c * table = _tables[ i ];
			if ( table->_id == id )
			{
				_tables.remove_at_index( i );
				delete table;
			}
		}
	}

	database_record_c const * database_c::get_record( database_record_address_c const & record_address ) const
	{
		database_table_c const * table = get_table( record_address.table_id );
		if ( table )
		{
			return table->get_record_with_mod_record_address( record_address );
		}
		return nullptr;
	}

	void_c database_c::rebuild()
	{
		for ( sint32_c i = 0; i < _tables.get_length(); i++ )
		{
			_tables[ i ]->rebuild();
		}
	}

	boolean_c database_c::save()
	{
		assert( _flags != 0 ); // make sure at least master or mod flag is set.
		assert( ( ( _flags & database_flag_e_master ) != 0 ) != ( ( _flags & database_flag_e_mod ) != 0 ) ); // make sure only one flag is set.

		data_stream_file_c stream;
		if ( !stream.open( _file_path, data_stream_mode_e_write ) )
		{
			return false;
		}

		// save signature.
		data_scribe_binary_c scribe;
		scribe.set_stream( &stream );
		scribe.save_four_character_code( get_file_signature_static() );

		// save rest of file.
		scribe.save_string8( _name );
		scribe.save_uint16( _id );
		scribe.save_uint8( _flags );
		assert( _dependency_ids.get_length() < constants< uint16_c >::maximum() );
		scribe.save_uint16( static_cast< uint16_c >( _dependency_ids.get_length() ) );
		for ( sint32_c i = 0; i < _dependency_ids.get_length(); i++ )
		{
			scribe.save_uint16( _dependency_ids[ i ] );
		}
		assert( _tables.get_length() < constants< uint16_c >::maximum() );
		scribe.save_uint16( static_cast< uint16_c >( _tables.get_length() ) );
		for ( sint32_c i = 0; i < _tables.get_length(); i++ )
		{
			database_table_c * table = _tables[ i ];
			table->rebuild(); // clean garbage, compact, remove dead data.
			table->save( scribe );
		}

		return true;
	}

	boolean_c database_c::load()
	{
		_name = string8_c();
		_id = 0;
		_dependency_ids.remove_all();
		_tables.remove_and_delete_all();

		data_stream_file_c stream;
		if ( !stream.open( _file_path, data_stream_mode_e_read ) )
		{
			return false;
		}

		data_scribe_binary_c scribe;
		scribe.set_stream( &stream );

		// load signature and endianness.
		uint32_c signature = scribe.load_four_character_code();
		if ( signature != get_file_signature_static() )
		{
			return false;
		}

		// load rest of file.
		_name = scribe.load_string8();
		_id = scribe.load_uint16();
		_flags = scribe.load_uint8();
		uint16_c dependency_ids_length = scribe.load_uint16();
		for ( uint16_c i = 0; i < dependency_ids_length; i++ )
		{
			_dependency_ids.insert_at_end( scribe.load_uint16() );
		}
		uint16_c tables_length = scribe.load_uint16();
		for ( uint16_c i = 0; i < tables_length; i++ )
		{
			database_table_c * table = new database_table_c();
			table->_database = this;
			_tables.insert_at_end( table );
			table->load( scribe );
		}

		return true;
	}

}
