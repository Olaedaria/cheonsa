#include "cheonsa_resource_file.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	void_c resource_file_c::_load( data_stream_c * stream )
	{
	}

	void_c resource_file_c::_unload()
	{
	}

	resource_file_c::resource_file_c( string16_c const & file_path )
		: _file_path( file_path )
		, _file_path_absolute()
		, _file_last_write_time( 0 )
		, _search_engine_data( true )
		, _search_game_data( true )
		, _is_loaded( false )
		, _reference_count( 0 )
	{
	}

	resource_file_c::~resource_file_c()
	{
		assert( _is_loaded == false );
		if ( _is_loaded )
		{
			_unload();
		}
	}

	boolean_c resource_file_c::get_search_game_data() const
	{
		return _search_game_data;
	}

	void_c resource_file_c::set_search_game_data( boolean_c value )
	{
		_search_game_data = value;
	}

	boolean_c resource_file_c::get_search_engine_data() const
	{
		return _search_engine_data;
	}

	void_c resource_file_c::set_search_engine_data( boolean_c value )
	{
		_search_engine_data = value;
	}

	boolean_c resource_file_c::get_is_loaded() const
	{
		return _is_loaded;
	}

	string16_c const & resource_file_c::get_file_path() const
	{
		return _file_path;
	}

	string16_c const & resource_file_c::get_file_path_absolute() const
	{
		return _file_path_absolute;
	}

	sint64_c resource_file_c::get_file_last_write_time() const
	{
		return _file_last_write_time;
	}

	void_c resource_file_c::add_reference()
	{
		_reference_count++;
	}

	void_c resource_file_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
	}

	void_c resource_file_c::refresh()
	{
		string16_c file_path_absolute = _file_path_absolute;
		sint64_c file_last_write_time = _file_last_write_time;
		if ( !engine.get_content_manager()->resolve_absolute_file_path( _file_path, file_path_absolute, _search_engine_data, _search_game_data ) )
		{
			return;
		}

		if ( !ops::file_system_get_file_or_folder_last_write_time( file_path_absolute, file_last_write_time ) )
		{
			return;
		}

		if ( _file_path_absolute != file_path_absolute || _file_last_write_time != file_last_write_time )
		{
			data_stream_file_c file_stream;
			if ( file_stream.open( file_path_absolute, data_stream_mode_e_read ) )
			{
				if ( _is_loaded )
				{
					_unload();
					assert( _is_loaded == false );
				}
				_file_last_write_time = 0;
				_load( &file_stream );
				if ( _is_loaded )
				{
					_file_path_absolute = file_path_absolute;
					_file_last_write_time = file_last_write_time;
				}
			}
		}
	}

}
