#pragma once

#include "cheonsa_script_types.h"

namespace cheonsa
{

	// pass by value type.
	class script_value_c
	{
	public:
		enum type_e
		{
			type_e_none,
			type_e_number,
			type_e_string,
		};

	private:
		//script_context_c * _context; // the heap that this value belongs to.
		//sint32_c _reference_count; // number of references to this value.
		//uint32_c _serial_index; // serial index of this value when it is saved to a stream, so that references can reaquire a * to this value instance after being loaded.
		type_e _type; // this value's type.
		float32_c _number_value;
		string8_c * _string_value;

	public:
		script_value_c();
		script_value_c( const script_value_c & other );
		~script_value_c();

		type_e get_type() const;

		void_c set_none_value();

		float32_c get_number_value() const;
		void_c set_number_value( float32_c value );

		string8_c const & get_string_value() const;
		void_c set_string_value( string8_c const & value );

		boolean_c resolve_as_boolean();

		void_c save( data_scribe_binary_property_c & scribe );
		void_c load( data_scribe_binary_property_c & scribe );

		script_value_c & operator = ( script_value_c const & other );

	};

}
