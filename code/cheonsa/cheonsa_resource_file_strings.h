#pragma once

#include "cheonsa_resource_file.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_dictionary.h"

namespace cheonsa
{

	// loads strings from an xml file (typically called "text.strings.xml").
	// references to the strings can be acquired using their unique key.
	// the way it is designed at this time, the strings are read only, the only way to modify them is to modify the source file and then reload the whole thing, which may not be ideal if we want to implement some kind of automatic wysiwyg editor.
	class resource_file_strings_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_strings_c > reference_c;

	private:
		class string_c
		{
		public:
			string8_c key;
			string8_c value;

		};

		core_list_c< char8_c > _byte_heap; // all of the string keys and values in a flat buffer, each one is utf-8 encoded and null terminated.
		core_list_c< string_c > _string_heap; // references strings in the _byte_heap.
		core_dictionary_c< string8_c, sint32_c > _string_dictionary; // references strings in the _string_heap.

		virtual void_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_strings_c( string16_c const & file_path );
		~resource_file_strings_c();

		// looks up a string in this string file.
		// utf-8 version.
		boolean_c find_string( string8_c const & key, string8_c & result ) const;

		// looks up a string in this string file.
		boolean_c find_string( string8_c const & key, string16_c & result ) const;

	};

}
