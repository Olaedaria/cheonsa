#include "cheonsa_script_scope.h"
#include "cheonsa_script_context.h"
#include "cheonsa_script_value.h"
#include "cheonsa_ops.h"
#include <cassert>

namespace cheonsa
{

	script_scope_c::script_scope_c()
		: _variables()
	{
	}

	script_scope_c::~script_scope_c()
	{
		auto iterator = _variables.get_iterator();
		while ( iterator.next() )
		{
			delete iterator.get_value();
		}
		_variables.remove_all();
	}

	script_value_c * script_scope_c::get_variable( string8_c const & name )
	{
		script_value_c * * result = _variables.find_value_pointer( name );
		if ( result )
		{
			return *result;
		}
		return nullptr;
	}

	script_value_c * script_scope_c::add_or_get_variable( string8_c const & name )
	{
		script_value_c * * result = _variables.find_value_pointer( name );
		if ( result )
		{
			return *result;
		}
		script_value_c * NewResult = new script_value_c();
		_variables.insert( name, NewResult );
		return NewResult;
	}

	void_c script_scope_c::remove_variable( string8_c const & name )
	{
		script_value_c * * result = _variables.find_value_pointer( name );
		if ( result )
		{
			delete * result;
			_variables.remove( name );
		}
	}

	void_c script_scope_c::save( data_scribe_binary_property_c & scribe, char8_c const * key )
	{
		scribe.save_list_begin( key );

		scribe.save_list_begin( "variables" );
		core_dictionary_c< string8_c, script_value_c * >::iterator_c iterator = _variables.get_iterator();
		while ( iterator.next() )
		{
			scribe.save_string8( "name", iterator.get_key() );
			iterator.get_value()->save( scribe );
		}
		scribe.save_list_end( _variables.get_length() );

		scribe.save_list_end( 0 );
	}

	void_c script_scope_c::load( data_scribe_binary_property_c & scribe, char8_c const * key )
	{
		assert( scribe.loaded_check( key, data_type_e_list, 0 ) );
		scribe.loaded_list_enter();

		while ( scribe.load() )
		{
			if ( scribe.loaded_check( "variables", data_type_e_list, 0 ) )
			{
				scribe.loaded_list_enter();
				sint32_c count = scribe.loaded_list_length();
				for ( sint32_c i = 0; i < count; i++ )
				{
					assert( scribe.load() );
					assert( scribe.loaded_check( "name", data_type_e_string8, 0 ) );
					string8_c variable_name = scribe.loaded_string8();
					assert( scribe.load() );
					script_value_c * variable_value = new script_value_c();
					variable_value->load( scribe );
					_variables.insert( variable_name, variable_value );
				}
				scribe.loaded_list_exit();
			}
		}

		scribe.loaded_list_exit();
	}

}
