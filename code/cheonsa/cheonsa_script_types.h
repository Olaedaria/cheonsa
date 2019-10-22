#pragma once

#include "cheonsa_base_types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_scribe_binary_property.h"

namespace cheonsa
{

	class script_value_c;
	class script_instance_c;
	class script_context_c;
	class script_scope_c;

	//void_c script_scope_register( char8_c const * scope_name, script_scope_c * scope );
	//script_scope_c * script_scope_get( string16_c const & scope_name );

	typedef boolean_c (*script_function_f)( script_instance_c *, core_list_c< script_value_c > &, script_value_c & ); // function * type. all functions that are added to this function registry need to have this template: static EScriptInstructionResult function_name( ScriptVariable& Arguments );

	//void_c script_function_register( char8_c const * function_name, script_function_f function ); // call at startup to add a function * to the function registry.
	//boolean_c script_function_call( script_instance_c * caller, string16_c & function_name, core_list_c< script_value_c > & function_inputs, script_value_c & function_result ); // call to call a function.
	//void_c script_function_release();

}
