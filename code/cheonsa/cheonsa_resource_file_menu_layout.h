#pragma once

#include "cheonsa__types.h"
#include "cheonsa_resource_file.h"
#include "cheonsa_data_scribe_markup.h"

namespace cheonsa
{

	// can load ".menu.layout.xml" files.
	// (file extension is a suggestion, you may deviate from it).
	// a menu layout file defines the layout and appearance (style map assignments) of a hierarchy of menu controls.
	// it does not define any of the contents or logic of the menu controls, all of that is hard-coded and is not data-driven.
	class resource_file_menu_layout_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_menu_layout_c > reference_c;

	public:
		static char8_c const * get_resource_file_type_static() { return "menu"; }
		virtual char8_c const * get_resource_file_type() const override { return get_resource_file_type_static(); }

	protected:
		data_scribe_markup_c _markup;

		virtual void_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_menu_layout_c( string16_c const & file_path );

		data_scribe_markup_c const & get_markup() const;
		resource_file_menu_layout_c & operator = ( resource_file_menu_layout_c const & ) = delete;
	};

}
