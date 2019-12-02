#pragma once

#include "cheonsa_database_types.h"
#include "cheonsa_database_record_schema.h"
#include "cheonsa_data_scribe_binary.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_core_list.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// database table holds records.
	// records can be gathered with simple queries.
	// fields in those records can be read or written.
	// writing new string values will be a bit wasteful, old unreferenced strings will hang around until garbage is collected.
	class database_table_c
	{
	private:
		friend class database_c;
		friend class database_record_c;
		friend class database_stack_record_c;

		database_c * _database;

		string8_c _name; // name to describe this table for convenience. it should not used to reference this table, because names might change between builds.
		uint16_c _id; // static id used to identify this table to the game. in the case that this table contains records to modify table records in a higher in the load order database, then this id should be the same as the id of the original table.
		uint8_c _flags; // not used for anything at the moment.
		uint32_c _next_record_id; // next id to try to use as id for next new record that is added, which is likely to be unallocated.

		core_list_c< char8_c > _string_table; // all of the strings utf-8 encoded and null terminated one after the other.
		core_dictionary_c< string8_c, sint32_c > _string_table_map; // maps a string value to a byte offset within the string table so that duplicate strings can be reused.

		database_record_schema_c _record_schema; // defines the format of records in this table.
		core_list_c< uint8_c > _record_buffer; // holds all of the records in this table.
		core_dictionary_c< uint32_c, database_record_c * > _record_map; // maps a record id to a record instance. it could be more efficient if we instead map to the record offset, and then defer the newing of the record instance to when the game asks for the record.
		core_dictionary_c< database_record_address_c, database_record_c * > _record_mod_map; // maps a mod record address to a record instance.

		boolean_c _save( data_scribe_binary_c & scribe );
		boolean_c _load( data_scribe_binary_c & scribe );

		sint32_c _add_string8( string8_c const & value ); // if the string is not yet in the buffer then adds it, then returns its offset.
		string8_c _get_string8( sint32_c offset ) const; // returns a static volatile reference of string at offset.

		sint32_c _add_string16( string16_c const & value ); // utf-8 encodes the string, if the string is not yet in the buffer then adds it, then returns its offset.
		string16_c _get_string16( sint32_c offset ) const; // returns a new dynamic copy of string at offset.

	public:
		database_table_c();

		string8_c const & get_name() const;
		void_c set_name( string8_c const & value ); // will assert if a table of the given name already exists in the database.

		uint16_c get_id() const;
		void_c set_id( uint16_c value ); // don't change this while the game is already dependent on it. just change it during initial set up.

		void_c reset(); // removes all strings and records, but keeps name and id.

		database_record_schema_c const & get_records_schema() const;

		// sets the record schema.
		// if schema is already set and records are already present, then tries to migrate the existing data to the new schema.
		// has a dual purpose, it also rebuilds internal strings and records buffers.
		// the game should always call this on loaded tables as a way to update the data to the game's current build|version if needed, since trying to access fields that don't match expected name|type|type_count will assert.
		void_c set_records_schema( database_record_schema_c const & value );

		database_record_c * add_record( database_record_address_c mod_record_address = database_record_address_c() ); // adds a new record to the table. the new record will be automatically allocated and assigned a unique id.
		database_record_c const * get_record_with_id( uint32_c id ) const; // gets record with the given id if it exists or returns nullptr.
		database_record_c * get_record_with_id( uint32_c id ); // gets record with the given id if it exists or returns nullptr.
		database_record_c const * get_record_with_mod_record_address( database_record_address_c const & mod_record_address ) const;
		database_record_c * get_record_with_mod_record_address( database_record_address_c const & mod_record_address );
		void_c remove_record_with_id( uint32_c id ); // removes record with the given record id.

		void_c rebuild(); // rebuilds internal strings and records buffers, removes unused strings and records.

	};

}
