#pragma once

#include "cheonsa_types.h"
#include "cheonsa_data_types.h"

namespace cheonsa
{

	// databases can work in one of two different modes.
	enum database_flag_e
	{
		database_flag_e_none = 0x00,
		// if schema is modified and new fields are added, then their values will be defined and set to the default defined by the scehma.
		database_flag_e_master = 0x01,
		// if schema is modified and new fields are added, then their values will be left undefined.
		database_flag_e_mod = 0x02,
	};

	// each field data is prefixed with one byte for flags.
	enum database_field_flag_e
	{
		database_field_flag_e_none = 0x00,

		// if true then this field has a value defined.
		database_field_flag_e_is_defined = 0x01,
	};

	// each record data is prefixed with one byte for flags.
	enum database_record_flag_e
	{
		database_record_flag_e_none = 0x00,

		// if true then this record is a mod for a record in another database higher up in the load order and it wants to make the record appear as non-existent to the game.
		// the record will still exist in the database(s) but the game will just not get a result when it tries to look it up.
		// in this way mods can "delete" content that was defined previously.
		database_record_flag_e_is_hidden = 0x01,

		// if true then this record should be considered as deleted and it will be pruned the next time the table is rebuilt.
		database_record_flag_e_is_deleted = 0x02,
	};

#pragma pack( push, 1 )
	// 8 bytes, used to address|reference records of a specific known origin.
	struct database_record_address_c
	{
		uint16_c database_id;
		uint16_c table_id;
		uint32_c record_id;
		inline database_record_address_c()
			: database_id( 0 )
			, table_id( 0 )
			, record_id( 0 )
		{
		}
		inline boolean_c is_set() const
		{
			return ( database_id != 0 ) || ( table_id != 0 ) || ( record_id != 0 );
		}
		inline boolean_c operator == ( database_record_address_c const & other ) const
		{
			return ( database_id == other.database_id ) && ( table_id == other.table_id ) && ( record_id == other.record_id );
		}
	};
	static_assert( sizeof( database_record_address_c ) == 8, "size of database_record_address_c is wrong." );

	// 16 bytes, each record value entry is prefixed by this structure.
	struct database_record_header_c
	{
		uint32_c id;
		database_record_address_c mod_record_address;
		uint8_c flags; // interpret as database_record_flag_e.
		uint8_c padding[ 3 ];
	};
	static_assert( sizeof( database_record_header_c ) == 16, "size of database_record_header_c is wrong." );
#pragma pack( pop )

	enum database_operation_e
	{
		// a new record was added.
		// the game usually doesn't need to do anything to respond to this situation.
		// but in the case that we are in the editor and the user is undoing or undeleting a record, then the game should reinstance any previously deinstanced objects|states that were instanced from this data.
		database_operation_e_record_added,
		// a one or more fields in a record were modified.
		// the game should gracefully reinstance any objects|states that it had instanced from this data.
		database_operation_e_record_modified,
		// a record is about to be deleted.
		// the game should gracefully deinstance any objects|states that it had instanced from this data.
		database_operation_e_record_removed,
	};

	class database_record_c;
	class database_record_schema_c;
	class database_field_schema_c;
	class database_table_c;
	class database_c;
	class database_stack_record_c;
	class database_stack_c;

	// any time databases are modified in the database stack, the game has an opportunity to adapt to those changes during run time.
	struct database_on_modified_info_c
	{
		//database_stack_c * database_stack; // is set if event is generated by database stack.
		database_record_address_c mod_record_address; // lets the game respond to when the record is deleted.
		database_record_c const * record; // the record that is being operated on.
		database_operation_e operation; // the operation that happened (added and updated) or is about to happen (removed).
		string8_c field_key_list; // a vertical slash separated list of field keys with values that were modified. this enables the game respond to changes in the most optimal (minimal) way that it can.
	};

	uint16_c database_get_data_type_size( data_type_e data_type );

}
