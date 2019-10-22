#include "cheonsa_script_manager.h"
#include "cheonsa_script_instance.h"
#include "cheonsa_engine.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	script_manager_c::script_manager_c()
		: _scope_dictionary()
		, _function_dictionary()
	{
	}

	script_manager_c::~script_manager_c()
	{
	}

	boolean_c script_manager_c::start()
	{
		return true;
	}

	void_c script_manager_c::register_scope( string8_c const & scope_name, script_scope_c * scope )
	{
		assert( _scope_dictionary.contains( scope_name ) == false );
		_scope_dictionary.insert( scope_name, scope );
	}

	script_scope_c * script_manager_c::find_scope( string8_c const & scope_name )
	{
		script_scope_c * * result = _scope_dictionary.find_value_pointer( scope_name );
		return result ? *result : nullptr;
	}

	void_c script_manager_c::register_function( string8_c const & function_name, script_function_f function )
	{
		_function_dictionary.insert( function_name, function );
	}

	boolean_c script_manager_c::call_function( script_instance_c * caller, string8_c & function_name, core_list_c< script_value_c > & function_inputs, script_value_c & function_result )
	{
		script_function_f * result = _function_dictionary.find_value_pointer( function_name );
		if ( result != nullptr )
		{
			return ( *result )( caller, function_inputs, function_result );
		}
		string16_c error_message;
		error_message = L"could not find script function: \"";
		error_message += function_name;
		error_message += "\"";
		caller->_set_error( error_message );
		return false;
	}

}
