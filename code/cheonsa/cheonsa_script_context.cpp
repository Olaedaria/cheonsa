#include "cheonsa_script_context.h"
#include "cheonsa_script_instance.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c script_context_c::_compact_script_serial_indices()
	{
		_next_script_serial_index = 0;
		for ( sint32_c i = 0; i < _scripts.get_length(); i++ )
		{
			_scripts[ i ]->_serial_index = _next_script_serial_index++;
		}
	}

	script_context_c::script_context_c()
		: _scripts()
		, _next_script_serial_index( 0 )
	{
	}

	script_context_c::~script_context_c()
	{
		reset();
	}

	script_instance_c * script_context_c::new_script( string16_c const & file_path )
	{
		assert( _scripts.get_length() < 0x7FFFFFFF );
		if ( _next_script_serial_index >= 0x7FFFFFFF ) // compact serial indices if needed
		{
			_compact_script_serial_indices();
		}


		data_stream_file_c stream;
		string16_c absolute_file_path;
		if ( engine.get_content_manager()->resolve_absolute_file_path( file_path, absolute_file_path, true, false ) )
		{
			if ( stream.open( absolute_file_path, data_stream_mode_e_read ) )
			{
				script_instance_c * result = new script_instance_c( this );
				if ( result->load( file_path, &stream ) )
				{
					result->_serial_index = _next_script_serial_index++;
					_scripts.insert( -1, result );
					return result;
				}
				else
				{
					delete result;
				}
			}
		}

		return nullptr;
	}

	script_instance_c * script_context_c::get_script( sint32_c serial_index )
	{
		for ( sint32_c i = 0; i < _scripts.get_length(); i++ )
		{
			if ( _scripts[ i ]->_serial_index == serial_index )
			{
				return _scripts[ i ];
			}
		}
		return nullptr;
	}

	void_c script_context_c::delete_script( script_instance_c * script )
	{
		assert( script->_context == this );
		_scripts.remove_value( script );
		delete script;
	}

	void_c script_context_c::reset()
	{
		for ( sint32_c i = 0; i < _scripts.get_length(); i++ ) // delete all scripts.
		{
			delete _scripts[ i ];
		}
		_scripts.remove_all();
	}

	boolean_c script_context_c::save( data_scribe_binary_property_c & scribe )
	{
		scribe.save_list_begin( "context" );

		// save resources


		// save scripts
		scribe.save_list_begin( "scripts" );
		for ( sint32_c i = 0; i < _scripts.get_length(); i++ )
		{
			script_instance_c * script = _scripts[ i ];
			script->serial_save( scribe );
		}
		scribe.save_list_end( _scripts.get_length() );

		scribe.save_end();

		scribe.save_list_end( 0 );

		return true;
	}

	boolean_c script_context_c::load( data_scribe_binary_property_c & scribe )
	{
		assert( scribe.loaded_check( "context", data_type_e_list, 0 ) );

		reset();

		scribe.loaded_list_enter();

		while ( scribe.load() )
		{
			// load scripts
			if ( scribe.loaded_check( "scripts", data_type_e_list, 0 ) )
			{
				scribe.loaded_list_enter();
				sint32_c length = scribe.loaded_list_length();
				for ( sint32_c i = 0; i < length; i++ )
				{
					script_instance_c * script = new script_instance_c( this );
					script->serial_load( scribe );
					_scripts.insert( -1, script );
				}
				scribe.loaded_list_exit();
			}
		}

		scribe.loaded_list_exit();

		return true;
	}

}
