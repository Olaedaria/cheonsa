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
		string16_c _file_path_relative; // this is the file path that the engine uses to reference the style file, which in turn resolves to an absolute file path.
		string16_c _file_path_absolute;
		sint64_c _file_modified_time; // keeps track of when the file was modified, so that changes can be deteced and the file can be reloaded if needed.
		core_dictionary_c< string8_c, menu_style_for_frame_c * > _frame_style_dictionary; // frame styles are applied to frame elements.
		core_dictionary_c< string8_c, menu_style_for_text_c * > _text_style_dictionary; // text styles are applied to text elements.
		core_dictionary_c< string8_c, menu_style_map_c * > _style_map_dictionary; // style maps are applied to controls.

	public:
		menu_style_file_c( string16_c const & file_path_relative );
		~menu_style_file_c();

		boolean_c refresh(); // reloads the style file referenced by _file_path_relative.

	};

}
