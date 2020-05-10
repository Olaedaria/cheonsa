#pragma once

#include "cheonsa_menu_types.h"
#include "cheonsa_resource_file.h"

namespace cheonsa
{

	// during run time, the engine and game may request to load any number of style files.
	// this class inherits from resource file for convenience, but these instances are managed by the menu style manager rather than the resource manager.
	// the menu style manager can reload these files during run time.
	class menu_style_file_c
		: public resource_file_c
	{
	private:
		friend class menu_style_manager_c;

		core_dictionary_c< string8_c, menu_color_style_c * > _color_style_dictionary;
		core_dictionary_c< string8_c, menu_frame_style_c * > _frame_style_dictionary;
		core_dictionary_c< string8_c, menu_text_style_c * > _text_style_dictionary;
		core_dictionary_c< string8_c, menu_style_map_c * > _style_map_dictionary;

		virtual void_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		menu_style_file_c( string16_c const & file_path );
		~menu_style_file_c();

		menu_color_style_c const * find_color_style( string8_c const & key ) const;
		menu_frame_style_c const * find_frame_style( string8_c const & key ) const;
		menu_text_style_c const * find_text_style( string8_c const & key ) const;
		menu_style_map_c const * find_style_map( string8_c const & key ) const;

	};

}
