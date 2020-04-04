#pragma once

#include "cheonsa_resource_file.h"
#include "cheonsa_data_scribe_markup.h"

namespace cheonsa
{

	// enables a markup file to define the layout of a menu.
	class resource_file_menu_layout_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_menu_layout_c > reference_c;

	public:
		static char8_c const * get_type_static() { return "menu"; }
		virtual char8_c const * get_type() const override { return get_type_static(); }

	protected:
		data_scribe_markup_c _markup;

		virtual boolean_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_menu_layout_c();
		resource_file_menu_layout_c( resource_file_menu_layout_c const & ) = delete;
		virtual ~resource_file_menu_layout_c() override;
		data_scribe_markup_c const & get_markup() const;
		resource_file_menu_layout_c & operator = ( resource_file_menu_layout_c const & ) = delete;
	};

}
