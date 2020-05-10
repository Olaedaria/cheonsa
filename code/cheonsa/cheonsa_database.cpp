#include "cheonsa_database.h"
#include "cheonsa_database_table.h"
#include "cheonsa__ops.h"
#include "cheonsa_data_stream_file.h"
#include <cassert>

namespace cheonsa
{

	database_c::database_c( string16_c const & file_path )
		: _database_stack( nullptr )
		, _file_path_absolute( file_path )
		//, _endianness( ops::get_native_byte_order() )
		, _name()
		, _id( 0 )
		, _flags( database_flag_e_none )
		, _dependency_ids()
		, _tables()
	{
	}

	database_stack_c const * database_c::get_database_stack() const
	{
		return _database_stack;
	}

	string16_c const & database_c::get_file_path_absolute() const
	{
		return _file_path_absolute;
	}

	void_c database_c::set_file_path_absolute( string16_c const & value )
	{
		_file_path_absolute = value;
	}

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

	database_flag_e database_c::get_flags() const
	{
		return static_cast< database_flag_e >( _flags );
	}

	void_c database_c::set_flags( database_flag_e value )
	{
		_flags = value;
	}

	database_table_c * database_c::add_table( uint16_c id )
	{
		database_table_c * table = new database_table_c();
		table->_database = this;
		table->_id = id;
		_tables.insert( -1, table );
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
				_tables.remove( i, 1 );
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

	boolean_c database_c::save( byte_order_e byte_order )
	{
		assert( _flags != 0 ); // make sure at least master or mod flag is set.
		assert( ( ( _flags & database_flag_e_master ) != 0 ) != ( ( _flags & database_flag_e_mod ) != 0 ) ); // make sure only one flag is set.

		data_stream_file_c stream;
		if ( !stream.open( _file_path_absolute, data_stream_mode_e_write ) )
		{
			return false;
		}

		// save signature.
		stream.save( byte_order == byte_order_e_little ? "chdb" : "CHDB", 4 );

		// save rest of file.
		data_scribe_binary_c scribe;
		scribe.set_stream( &stream );
		scribe.set_byte_order( byte_order );

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

		assert( _dependency_ids.get_length() < constants< uint16_c >::maximum() );
		if ( !scribe.save_uint16( static_cast< uint16_c >( _dependency_ids.get_length() ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < _dependency_ids.get_length(); i++ )
		{
			if ( !scribe.save_uint16( _dependency_ids[ i ] ) )
			{
				return false;
			}
		}
		assert( _tables.get_length() < constants< uint16_c >::maximum() );
		if ( !scribe.save_uint16( static_cast< uint16_c >( _tables.get_length() ) ) )
		{
			return false;
		}
		for ( sint32_c i = 0; i < _tables.get_length(); i++ )
		{
			database_table_c * table = _tables[ i ];
			table->rebuild(); // clean garbage, compact, remove dead data.
			if ( !table->_save( scribe ) )
			{
				return false;
			}
		}

		return true;
	}

	boolean_c database_c::load()
	{
		_name = string8_c();
		_id = 0;
		_flags = database_flag_e_none;
		_dependency_ids.remove_all();
		_tables.remove_and_delete_all();

		data_stream_file_c stream;
		if ( !stream.open( _file_path_absolute, data_stream_mode_e_read ) )
		{
			return false;
		}

		// load signature and endianness.
		char8_c signature[ 4 ] = {};
		if ( !stream.load( signature, 4 ) )
		{
			return false;
		}
		data_scribe_binary_c scribe;
		scribe.set_stream( &stream );
		if ( ops::memory_compare( signature, "chdb", 4 ) )
		{
			scribe.set_byte_order( byte_order_e_little );
		}
		else if ( ops::memory_compare( signature, "CHDB", 4 ) )
		{
			scribe.set_byte_order( byte_order_e_big );
		}
		else
		{
			return false;
		}

		// load rest of file.
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
		uint16_c dependency_ids_length = 0;
		if ( !scribe.load_uint16( dependency_ids_length ) )
		{
			goto cancel;
		}
		for ( uint16_c i = 0; i < dependency_ids_length; i++ )
		{
			uint16_c dependency_id = 0;
			if ( !scribe.load_uint16( dependency_id ) )
			{
				goto cancel;
			}
			_dependency_ids.insert( -1, dependency_id );
		}
		uint16_c tables_length = 0;
		if ( !scribe.load_uint16( tables_length ) )
		{
			goto cancel;
		}
		for ( uint16_c i = 0; i < tables_length; i++ )
		{
			database_table_c * table = new database_table_c();
			table->_database = this;
			_tables.insert( -1, table );
			if ( !table->_load( scribe ) )
			{
				goto cancel;
			}
		}

		return true;

	cancel:
		_name = string8_c();
		_id = 0;
		_flags = database_flag_e_none;
		_dependency_ids.remove_all();
		_tables.remove_and_delete_all();
		return false;
	}

}
