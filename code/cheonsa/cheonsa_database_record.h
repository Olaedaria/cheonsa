#pragma once

#include "cheonsa_database_types.h"
#include "cheonsa_string8.h"

namespace cheonsa
{

	// database record instance, holds a reference to the table and the data offset to this record's data in that table's record buffer.
	// these records are persistent so long as they live in the database.
	class database_record_c
	{
	private:
		friend class database_table_c;

		database_table_c * _table; // the table that owns this record.
		uint32_c _data_offset; // offset in bytes from start of the records buffer to where this record's data begins. the database will maintain this.

		void_c set_id( uint32_c value ) const;
		void_c set_mod_record_address( database_record_address_c const & value ) const;

	public:
		database_record_c();
		~database_record_c();

		database_table_c const * get_table() const;
		database_table_c * get_table();

		uint32_c get_id() const;

		database_record_address_c get_mod_record_address() const;

		uint8_c get_flags() const;
		void_c set_flags( uint8_c value );

		boolean_c get_is_hidden() const; // gets the state of the is hidden flag.
		void_c set_is_hidden( boolean_c value ); // sets the state of the is hidden flag. when a record hidden it means that the record is modding a previously defined record and it wants to hide it from the game, so from the game's point of view the record won't exist.

		boolean_c get_is_deleted() const; // gets the state of the is deleted flag.
		void_c set_is_deleted( boolean_c value ); // sets the state of the is deleted flag. when a record is deleted it means that it will be purged the next time the database is rebuilt or saved, but in the mean time the record will still exist, but from the game's point of view the record won't exist.

		// gets a pointer to field data for direct memory access, the data includes one byte of flags followed by the data itself.
		// for data_type_e_string8 and data_type_e_string16 types, the data is an sint32_c offset into the table's string buffer.
		// for every other type the data is what you would think.
		// treat the pointer as volatile, it may become invalid if new records are added or garbage is cleaned.
		// asserts if the field does not exist.
		void_c get_field_data_pointer( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count, uint8_c * & field_data_pointer ) const;

		// returns true if value was defined, false if not.
		// the result is copied to field_value.
		// if expected_type is data_type_e_string8, then expected_type_count must be 1 and field_value must point to a string8_c instance to receive the result.
		// else if expected_type is data_type_e_string16, then expected_type_count must be 1 and field_value must point to a string16_c instance to receive the result.
		// else field_value must point to a buffer of appropriate size to hold the result
		// asserts if the field does not exist.
		boolean_c get_field_value( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count, void_c * field_value, boolean_c if_not_defined_then_get_default ) const;

		// sets a field value and sets its is defined flag to true.
		// if expected_type is data_type_e_string8, then expected_type_count must be 1 and field_value must point to a string8_c instance.
		// else if expected_type is data_type_e_string16, then expected_type_count must be 1 and field_value must point to a string16_c instance.
		// else field_value must point to a buffer of appropriate size.
		// asserts if the field does not exist.
		void_c set_field_value( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count, void_c const * field_value ) const;

		// sets a field value's is defined flag to false and zeros out its value.
		void_c undefine_field_value( string8_c const & expected_name, data_type_e expected_type, uint8_c expected_type_count ) const; // undefines the defined by name and type.

	};

}
