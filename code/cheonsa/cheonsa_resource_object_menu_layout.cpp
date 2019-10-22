#include "cheonsa_resource_object_menu_layout.h"

namespace cheonsa
{

	boolean_c resource_object_menu_layout_c::_load( data_stream_c * stream )
	{
		_is_loaded = _markup.parse( stream );
		return _is_loaded;
	}

	void_c resource_object_menu_layout_c::_unload()
	{
		_markup.reset();
		_is_loaded = false;
	}

	resource_object_menu_layout_c::resource_object_menu_layout_c()
		: resource_object_c()
		, _markup( '<' )
	{
	}
	
	resource_object_menu_layout_c::~resource_object_menu_layout_c()
	{
	}

	data_scribe_markup_c const & resource_object_menu_layout_c::get_markup() const
	{
		return _markup;
	}

}
