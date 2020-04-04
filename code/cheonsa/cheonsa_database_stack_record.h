#pragma once

#include "cheonsa_database_types.h"

namespace cheonsa
{

	// holds the result of a database stack record look up.
	// allows users to sample data that is merged from all of the databases in the stack.
	// this is how mods are applied.
	class database_stack_record_c
	{
	private:
		friend class database_stack_c;

		database_stack_c * _database_stack; // the database stack that spawned this stack record.
		database_record_schema_c const * _record_schema; // a pointer to the schema in the first database for the table that maps to the records in this record stack. this is the record schema that holds the default field value definitions that we will fall back to using if a field value is not defined by a record.
		core_list_c< database_record_c const * > _record_stack; // one record for each database in the database stack. elements will be nullptr if that database didn't define this record.

	public:
		database_stack_record_c();

		database_stack_c const * get_database_stack() const;

		// goes through each record in the stack and checks if the field value is defined.
		// if the field value is defined then it sets field_value to a copy of it for the last (highest in the stack) time it was defined.
		// if the field value is not defined then it sets field_value to a copy of the default field value defined by the schema.
		// returns true if the value was defined, and field_value was set to a copy of it.
		// returns false if the value was not defined, and field_value was set to default.
		// if expected_type is data_type_e_string8, then field_value must point to a string8_c instance to hold the result.
		// else if expected_type is data_type_e_string16, then field_value must point to a string16_c instance to hold the result.
		// else field_value must point to a buffer of appropriate size.
		// asserts if the field does not exist or the value is never defined.
		boolean_c sample_field_value( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count, void_c * field_value ) const;

	};

}
