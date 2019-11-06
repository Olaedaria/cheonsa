#include "cheonsa_resource_file_menu_layout.h"

namespace cheonsa
{

	boolean_c resource_file_menu_layout_c::_load( data_stream_c * stream )
	{
		assert( stream != nullptr );
		assert( _is_loaded == false );

		_is_loaded = _markup.parse( stream );

		return _is_loaded;
	}

	void_c resource_file_menu_layout_c::_unload()
	{
		_markup.reset();
		_is_loaded = false;
	}

	resource_file_menu_layout_c::resource_file_menu_layout_c()
		: resource_file_c()
		, _markup( '<' )
	{
	}
	
	resource_file_menu_layout_c::~resource_file_menu_layout_c()
	{
	}

	data_scribe_markup_c const & resource_file_menu_layout_c::get_markup() const
	{
		return _markup;
	}

}
