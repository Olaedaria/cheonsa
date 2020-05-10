#pragma once

#include "cheonsa_database_types.h"
#include "cheonsa_string8.h"

namespace cheonsa
{

	// describes the format of a field within a record, its offset and size in bytes, and how it interacts with reflection system.
	class database_field_schema_c
	{
		friend class database_record_schema_c;
		friend class database_record_c;
		friend class database_table_c;
		friend class database_stack_record_c;

	private:
		database_record_schema_c * _record_schema; // the record schema that owns this field.
		string8_c _key; // unique name to identify this field by.
		data_type_e _type; // the base type.
		uint8_c _type_count; // a value between 1 and 4 that describes the number of elements. should always be 1 for string and list types.

		// used with property inspector user interface to tell it how data can be edited.
		// this information has to be defined by the game when it configures its database expectations, it is not saved|loaded with the database file.
		struct reflection_information_c
		{
			string8_c description; // description of this field.

			data_view_e view; // what kind of special data editor to use if any.
			boolean_c is_readable; // if true then this field will be visible to and readable by reflection system.
			boolean_c is_writable; // if true then this field will be writeable to by reflection system.

			// how this is interpreted depends on the data type.
			// if view is file path, then this is a vertical slash separated list of file extensions to filter ".txt|.log".
			// if view is enum, then this is a string that references the enum's type name as registered with the reflection system, this lets the reflection system generate a combo box with appropriate options.
			string8_c additional_options;

			union
			{
				struct
				{
					sint32_c enum_default;
				};
				struct
				{
					sint32_c sint32_default[ 4 ];
					sint32_c sint32_minimum;
					sint32_c sint32_maximum;
				};
				struct
				{
					sint64_c sint64_default[ 4 ];
					sint64_c sint64_minimum;
					sint64_c sint64_maximum;
				};
				struct
				{
					float32_c float32_default[ 4 ];
					float32_c float32_minimum;
					float32_c float32_maximum;
				};
				struct
				{
					float64_c float64_default[ 4 ];
					float64_c float64_minimum;
					float64_c float64_maximum;
				};
				struct
				{
					sint32_c string_character_limit;
				};
			} defaults_and_limits;

			reflection_information_c();

			reflection_information_c & operator = ( reflection_information_c const & other );

		};

		reflection_information_c _reflection_information; // defines default values (which are used when sampling fields that don't define a value) and also how data interacts with reflection system. reflection info is not saved with the schema when the database is saved, it must be (re)defined by the game each time the database is created or loaded.

		uint16_c _index; // this field's index within the record schema.
		uint16_c _data_offset; // offset in bytes relative to record's data offset to where this field's data begins. this points to the one byte field header, which is followed by the field value.
		uint16_c _data_size; // size in bytes of this field's data in records buffer, this includes the field header and the field value.

	public:
		database_field_schema_c();

		database_field_schema_c & operator = ( database_field_schema_c const & other ); // copies everything so that this class can work by value in a core_list_c.

		boolean_c operator == ( database_field_schema_c const & other ) const = delete;
		boolean_c operator != ( database_field_schema_c const & other ) const = delete;

	};

}
