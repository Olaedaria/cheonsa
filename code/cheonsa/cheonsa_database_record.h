#pragma once

#include "cheonsa_database_types.h"
#include "cheonsa_string8.h"

namespace cheonsa
{

	// database record instance, holds a reference to the table and the data offset to this record's data in that table's record buffer.
	// these records are persistent so long as they live in the database.
	class database_record_c
	{
		friend class database_table_c;

	private:
		database_table_c * _table; // the table that owns this record.
		uint32_c _data_offset; // offset in bytes from start of the records buffer to where this record's data begins. the database will maintain this.

		void_c _set_id( uint32_c value ) const;
		void_c _set_mod_record_address( database_record_address_c const & value ) const;

		database_record_flag_e _get_flags() const;
		void_c _set_flags( database_record_flag_e value );

	public:
		database_record_c();
		~database_record_c();

		database_table_c const * get_table() const;
		database_table_c * get_table();

		uint32_c get_id() const;

		database_record_address_c get_mod_record_address() const;

		boolean_c get_is_hidden() const; // gets the state of the is hidden flag.
		void_c set_is_hidden( boolean_c value ); // sets the state of the is hidden flag. when a record hidden it means that the record is modding a previously defined record (higher up in the load order) and it wants to hide it from the game, so from the game's point of view the record won't exist. this can be used by mods to "soft" delete (patch out) content from the game.

		boolean_c get_is_deleted() const; // gets the state of the is deleted flag.
		void_c set_is_deleted( boolean_c value ); // sets the state of the is deleted flag. when a record is deleted, it still exists, but it means that it will be for real removed the next time the database is rebuilt or saved. from the game's point of view the record won't exist after it is set to be deleted.

		// returns false or asserts if field of expeted name, type, and type count does not exist.
		// gets a pointer to field data for direct memory access, the data includes the one byte field flags followed by the field data itself.
		// for data_type_e_string8 and data_type_e_string16 types, the data is an sint32_c offset into the table's string buffer.
		// for every other type the data is what you would think.
		// treat the pointer as volatile, it may become invalid if new records are added or garbage is cleaned.
		boolean_c get_field_data_pointer( string8_c const & expected_key, data_type_e expected_type, uint8_c expected_type_count, uint8_c * & field_data_pointer ) const;

		// the result is copied to field_value.
		// if expected_type is data_type_e_string8, then expected_type_count must be 1 and field_value must point to a string8_c instance to receive the result.
		// else if expected_type is data_type_e_string16, then expected_type_count must be 1 and field_value must point to a string16_c instance to receive the result.
		// else field_value must point to a buffer of appropriate size to hold the result.
		// got_default will be set to false upon return if the field value was defined.
		// got_default will be set to true upon return if the field value was not defined, and so the default value was retreived instead.
		// returns false or asserts if field of expeted name, type, and type count does not exist.
		boolean_c get_field_value( string8_c const & expected_key, data_type_e expected_type, uint8_c expected_type_count, void_c * field_value, boolean_c & got_default ) const;

		// sets a field value and sets its is defined flag to true.
		// if expected_type is data_type_e_string8, then expected_type_count must be 1 and field_value must point to a string8_c instance.
		// else if expected_type is data_type_e_string16, then expected_type_count must be 1 and field_value must point to a string16_c instance.
		// else field_value must point to a buffer of appropriate size.
		// returns false or asserts if field of expeted name, type, and type count does not exist.
		boolean_c set_field_value( string8_c const & expected_key, data_type_e expected_type, uint8_c expected_type_count, void_c const * field_value ) const;

		// sets a field value's is defined flag to false and zeros out its value.
		// returns false or asserts if field of expeted name, type, and type count does not exist.
		boolean_c undefine_field_value( string8_c const & expected_key, data_type_e expected_type, uint8_c expected_type_count ) const; // undefines the defined by name and type.

	};

}
