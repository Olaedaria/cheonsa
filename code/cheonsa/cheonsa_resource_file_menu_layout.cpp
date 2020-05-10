#include "cheonsa_resource_file_menu_layout.h"

namespace cheonsa
{

	void_c resource_file_menu_layout_c::_load( data_stream_c * stream )
	{
		assert( stream != nullptr );
		assert( _is_loaded == false );

		_is_loaded = _markup.parse( stream );

		if ( !_is_loaded )
		{
			return;
		}

		on_loaded.invoke( this );
	}

	void_c resource_file_menu_layout_c::_unload()
	{
		assert( _is_loaded == true );

		on_unloaded.invoke( this );

		_is_loaded = false;

		_markup.reset();
	}

	resource_file_menu_layout_c::resource_file_menu_layout_c( string16_c const & file_path )
		: resource_file_c( file_path )
		, _markup( '<' )
	{
	}

	data_scribe_markup_c const & resource_file_menu_layout_c::get_markup() const
	{
		return _markup;
	}

}
