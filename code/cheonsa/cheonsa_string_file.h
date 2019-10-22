#pragma once

#include "cheonsa_core_list.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_string.h"

namespace cheonsa
{

	// loads strings from an xml file.
	// references to the strings can be acquired using their unique key.
	// the way it is designed at this time, the strings are read only, the only way to modify them is to modify the source file and then reload the whole thing, which may not be ideal if we want to implement some kind of automatic wysiwyg editor.
	class string_file_c
	{
	private:
		core_list_c< char8_c > _byte_heap; // all of the string keys and values in a flat buffer, each one is utf-8 encoded and null terminated.
		core_list_c< string_c > _string_heap; // references strings in the _byte_heap.
		core_dictionary_c< string8_c, sint32_c > _string_dictionary; // references strings in the _string_heap.

	public:
		string_file_c();
		~string_file_c();

		boolean_c load_from_xml( string16_c const & file_path_absolute );
		void_c unload();

		string_c const * find_string( string8_c const & key ) const; // looks up a string in this string file.

	};

}
