#pragma once

#include "cheonsa_database_types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_event.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// holds tables, which in turn hold records, which in turn hold fields and values.
	// when databases are loaded their data is in native [ byte order | endianness ].
	// when databases are saved their data may be converted to match _endianness if it is non-native [ byte order | endianness ].
	// this isn't as bloated or as robust as sqlite or whatever.
	class database_c
	{
	public:
		static inline uint32_c get_file_signature_static() { return fourcc( "chdb" ); }

	private:
		friend class database_stack_c;

		database_stack_c * _database_stack; // if this database is in a stack, so that we can invoke on_modified events at the stack level.
		string16_c _file_path; // absolute path to database file.
		//byte_order_e _byte_order; // endianness of file when it is saved or when it was loaded. when data is loaded it is always converted to native endianness.
		string8_c _name; // just a name to describe this database. it's not used to reference this database.
		uint16_c _id; // used to uniquely identify this database. for the master game database, may be set to something deliberate. for user-made mods, may be set to something randomly generated so that it avoids collision with other user-made mods.
		uint8_c _flags; // interpret as database_flag_e.
		core_list_c< uint16_c > _dependency_ids; // list of ids that this database is dependent on, in the case that this database is a mod.
		core_list_c< database_table_c * > _tables; // the tables in this database.

	public:
		database_c( string16_c const & file_path );

		database_stack_c const * get_database_stack() const;

		string16_c const & get_file_path() const; // gets absolute file path of where file was loaded from or saved to.
		void_c set_file_path( string16_c const & value ); // sets absolute file path of where to load file from or save file to.

		//byte_order_e get_byte_order() const; // the endianness that was loaded from the file.
		//void_c set_byte_order( byte_order_e value ); // defines the endianness to save the file with.

		string8_c const & get_name() const;
		void_c set_name( string8_c const & value );

		uint16_c get_id() const;
		void_c set_id( uint16_c value );

		uint8_c get_flags() const;
		void_c set_flags( uint8_c value );

		database_table_c * add_table( uint16_c id ); // adds a new table to the database. you will have to call initialize_schema() on the table before you can use it.
		database_table_c const * get_table( uint16_c id ) const;
		database_table_c * get_table( uint16_c id ); // gets a table in the database.
		void_c remove_table( uint16_c id ); // deletes a table from the database.

		// looks up a record given its record address and if it exists then returns a pointer to it otherwise returns nullptr.
		// this does not take into account if the record is hidden or is deleted.
		database_record_c const * get_record( database_record_address_c const & record_address ) const;

		void_c rebuild(); // rebuilds all the tables in this database, which cleans garbage and compacts them.

		boolean_c save(); // saves this database as a file at _file_path.
		boolean_c load(); // loads this database from a file at _file_path.

		core_event_c< void_c, database_on_modified_info_c const & > on_modified; // is invoked each time a record is added, modified, or removed.

	};

}
