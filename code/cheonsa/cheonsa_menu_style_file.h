#pragma once

#include "cheonsa_menu_types.h"

namespace cheonsa
{

	// during run time, the engine and game may request to load any number of style files.
	// style files are typically only loaded once at start up, but we support hot loading too so if the engine or game wants to load additional style files during run time then it may.
	// style files may also be re-loaded during run time in the case that the developer is making live edits to the files.
	// all controls affected by style file reloads 
	class menu_style_file_c
	{
	private:
		friend class menu_style_manager_c;

		string16_c _relative_file_path; // this is the file path that the engine uses to reference the style file, which in turn resolves to an absolute file path.
		string16_c _absolute_file_path; // _relative_file_path expanded to an absolute file path using the content manager's resolve_file_path() method.
		sint64_c _last_write_time; // keeps track of when the file was modified, so that changes can be deteced and the file can be reloaded if needed.
		core_dictionary_c< string8_c, menu_color_style_c * > _color_style_dictionary;
		core_dictionary_c< string8_c, menu_frame_style_c * > _frame_style_dictionary;
		core_dictionary_c< string8_c, menu_text_style_c * > _text_style_dictionary;
		core_dictionary_c< string8_c, menu_style_map_c * > _style_map_dictionary;

		boolean_c _refresh(); // reloads the style file referenced by _relative_file_path.

	public:
		menu_style_file_c( string16_c const & relative_file_path );
		~menu_style_file_c();

		menu_color_style_c const * find_color_style( string8_c const & key ) const;
		menu_frame_style_c const * find_frame_style( string8_c const & key ) const;
		menu_text_style_c const * find_text_style( string8_c const & key ) const;
		menu_style_map_c const * find_style_map( string8_c const & key ) const;

	};

}
