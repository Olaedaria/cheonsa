#pragma once

#include "cheonsa_database_types.h"
#include "cheonsa_database_field_schema.h"
#include "cheonsa_data_scribe_binary.h"
#include "cheonsa_core_list.h"
#include "cheonsa_string8.h"


namespace cheonsa
{

	// defines the format of fields in a record.
	// holds a list of field schemas.
	// schemas are saved and loaded with the database tables, but the game has to set these each time for the tables when they are created or loaded.
	// in this way, the game can update (in a backward|forward compatible way) the data format of each table after they are loaded, and can then work with the data in a more straight forward way because it can be programmed to assume value types during run time without needing to check them each time.
	class database_record_schema_c
	{
	private:
		friend class database_record_c;
		friend class database_table_c;

		core_list_c< database_field_schema_c > _fields; // the field schemas that make up this record.

	public:
		database_record_schema_c();
		database_record_schema_c( database_record_schema_c const & ) = delete;

		void_c reset(); // resets and unseals this schema so that fields can be added to it again.
		core_list_c< database_field_schema_c > const & get_fields() const;
		database_field_schema_c * add_field( string8_c const & name, data_type_e type, uint8_c type_count ); // add field schema, without reflection info.
		database_field_schema_c const * get_field( string8_c const & name ) const; // returns the field that matches name, otherwise returns nullptr.
		database_field_schema_c const * get_field( string8_c const & name, data_type_e type, uint8_c type_count ) const; // returns the field that matches name, type, and type_count, otherwise returns nullptr.
		uint16_c get_size() const; // gets the size in bytes of a record that is in the format defined by this schema.

		boolean_c save_record_buffer( data_scribe_binary_c & scribe, core_list_c< uint8_c > const & record_buffer, sint32_c record_count ); // saves a records buffer that is formatted with this schema, does byte order flipping as needed.
		boolean_c load_record_buffer( data_scribe_binary_c & scribe, core_list_c< uint8_c > & record_buffer, sint32_c record_count ); // loads a records buffer that is formatted with this schema, does byte order flipping as needed.

		database_record_schema_c & operator = ( database_record_schema_c const & other );

		boolean_c operator == ( database_record_schema_c const & other ) const; // returns true if this schema is basically equivalent to the other one. only compares name, type, and type_count of each field, does not compare reflection_info or _index, _data_offset, and _data_size.
		boolean_c operator != ( database_record_schema_c const & other ) const;

	};

}
