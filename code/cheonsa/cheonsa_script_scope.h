#pragma once

#include "cheonsa_script_types.h"

namespace cheonsa
{

	class script_scope_c
	{
	private:
		core_dictionary_c< string8_c, script_value_c * > _variables; // maps variable names to variable values.

	public:
		script_scope_c();
		~script_scope_c();

		script_value_c * get_variable( string8_c const & name ); // gets variable with name.
		script_value_c * add_or_get_variable( string8_c const & name ); // adds a new variable or gets an existing variable with name.
		void_c remove_variable( string8_c const & name ); // removes an existing variable with name.

		void_c save( data_scribe_binary_property_c & scribe, char8_c const * key );
		void_c load( data_scribe_binary_property_c & scribe, char8_c const * key );

	};

}
