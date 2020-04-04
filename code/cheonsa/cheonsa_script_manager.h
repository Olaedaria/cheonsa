﻿#pragma once

#include "cheonsa_script_types.h"

namespace cheonsa
{

	class script_manager_c
	{
	private:
		core_dictionary_c< string8_c, script_scope_c * > _scope_dictionary;
		core_dictionary_c< string8_c, script_function_f > _function_dictionary;

	public:
		script_manager_c();
		~script_manager_c();

		boolean_c start();

		void_c register_scope( string8_c const & scope_name, script_scope_c * scope );
		script_scope_c * find_scope( string8_c const & scope_name );

		void_c register_function( string8_c const & function_name, script_function_f function ); // call at startup to add a function * to the function registry.
		boolean_c call_function( script_instance_c * caller, string8_c & function_name, core_list_c< script_value_c > & function_inputs, script_value_c & function_result ); // call to call a function.

	};

}
