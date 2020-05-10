#pragma once

#include "cheonsa_database_types.h"
#include "cheonsa_core_event.h"

namespace cheonsa
{

	// holds a list of databases.
	// the first database is the master, the rest are mods, this order is also called the load order.
	// equivalent tables across different databases must have the same id and record schema in order for merged record lookup.
	// the intention for this was to create a data system that could do bethesda-like mods.
	class database_stack_c
	{
		friend class database_stack_record_c;

	private:
		core_list_c< database_c * > _database_stack; // the first one is the master, the rest are mods which may add or delete records, and may modify field values.

	public:
		database_stack_c();

		void_c add_database( database_c * database );

		// for each database in the stack, looks up the record at the given address (which may or may not be nullptr) and adds it to the record_stack result.
		// returns true if record is found in one or more of the databases.
		// returns false if record is not found in any of the databases, or if the record was found but flagged as deleted by any mod.
		boolean_c get_record_stack( database_record_address_c const & record_address, database_stack_record_c & record_stack );

		core_event_c< void_c, database_on_modified_info_c const & > on_modified; // is invoked each time a record in any of the databases is added, modified, or removed.

	};

}
