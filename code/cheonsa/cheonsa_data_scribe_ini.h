#pragma once

#include "cheonsa_data_stream.h"
#include "cheonsa_string8.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	// simple ini scribe, loads utf-8 documents, can parse properties, can parse categories, can parse (or rather ignore) comments, can not parse escape character sequences.
	// puts all parsed properties into a list, and unrecognized stuff it should ignore.
	class data_scribe_ini_c
	{
	private:
		struct item_c
		{
		public:
			enum type_e : uint8_c
			{
				type_e_section,
				type_e_property,
			};
			type_e type;
			char8_c const * name;
			char8_c const * value;

		};

		core_list_c< item_c > _item_list;
		string8_c _document;
		sint32_c _error_line_number; // if there was a parse error during load, then this is the number of the line that the error was encountered on.

	public:
		data_scribe_ini_c();

		// resets the document and key value pairs.
		void_c reset();

		// loads the document and parses it for key value pairs.
		// stream is a stream that is positioned at the start of the document.
		// size is the size of the document in bytes.
		// this will load the next size bytes from the stream into an internal string and parse it.
		boolean_c load( data_stream_c * stream );
		// finds the first property within section that matches key.
		boolean_c find_property_value( string8_c const & section_name, string8_c const & property_name, string8_c & result );

		// adds a category label to the end of the document.
		void_c add_section( string8_c const & section_name );
		// adds a property to the end document, so it will be under the last added section.
		void_c add_property( string8_c const & property_name, string8_c const & property_value );
		// saves the document.
		boolean_c save( data_stream_c * stream );

	};

}
