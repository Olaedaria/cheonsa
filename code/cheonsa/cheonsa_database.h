#pragma once

#include "cheonsa_database_types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_event.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// holds tables, which in turn hold records, which in turn hold fields and values.
	// this isn't as bloated or as robust as sqlite or whatever.
	// all databases are in memory all the time.
	class database_c
	{
		friend class database_stack_c;

	private:
		database_stack_c * _database_stack; // will be set if this database is in a stack, so that we can invoke the stack's on_modified event when we are modified.
		string16_c _file_path_absolute; // absolute file path of database file.
		string8_c _name; // just a friendly name for the human designer to use to describe this database. it's not used by code to identify this database.
		uint16_c _id; // used to uniquely identify this database. for the master game database, may be set to something deliberate. for user-made mods, may be set to something randomly generated so that it avoids collision with other user-made mods.
		uint8_c _flags; // interpret as database_flag_e.
		core_list_c< uint16_c > _dependency_ids; // list of ids that this database is dependent on, in the case that this database is a mod.
		core_list_c< database_table_c * > _tables; // the tables in this database.

	public:
		database_c( string16_c const & file_path );

		database_stack_c const * get_database_stack() const;

		string16_c const & get_file_path_absolute() const; // gets absolute file path of where file was loaded from or saved to.
		void_c set_file_path_absolute( string16_c const & value ); // sets absolute file path of where to load file from or save file to.

		string8_c const & get_name() const;
		void_c set_name( string8_c const & value );

		uint16_c get_id() const;
		void_c set_id( uint16_c value );

		database_flag_e get_flags() const;
		void_c set_flags( database_flag_e value );

		database_table_c * add_table( uint16_c id ); // adds a new table to the database. you will have to call initialize_schema() on the table before you can use it.
		database_table_c const * get_table( uint16_c id ) const;
		database_table_c * get_table( uint16_c id ); // gets a table in the database.
		void_c remove_table( uint16_c id ); // deletes a table from the database.

		// looks up a record given its record address and if it exists then returns a pointer to it otherwise returns nullptr.
		// this does not take into account if the record is hidden or is deleted.
		database_record_c const * get_record( database_record_address_c const & record_address ) const;

		void_c rebuild(); // rebuilds all the tables in this database, which cleans garbage and compacts them.

		boolean_c save( byte_order_e byte_order ); // saves this database as a file at _file_path.
		boolean_c load(); // loads this database from a file at _file_path.

		core_event_c< void_c, database_on_modified_info_c const & > on_modified; // is invoked each time a record is added, modified, or removed.

	};

}
