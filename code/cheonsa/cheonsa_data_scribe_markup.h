#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"
#include "cheonsa_core_list.h"
#include "cheonsa_data_stream.h"

namespace cheonsa
{

	/*

	parses markup text files that are basically XML.
	extracts and stores the parse result as a list of nodes and attributes that can be processed later by your program.
	works with either angle brackets or square brackets.
	markup needs to be well-formed, the parser isn't programmed to know how to recover from bad markup, this mainly means that every open tag needs to have a corresponding close tag.
	input is utf-8 formatted string, so special characters are supported, but you also have the option of using HTML entities.

	here's an example of what our markup looks like:
		root level text node.
		<element attribute1="value" attribute2="value">  text node content <sub-element/><!<!nested!>comment!></>
		anther root level text node.

	we supply the file data as a utf-8 string.
	the markup parser makes a local copy, then markup is parsed in-place, so that the parser can mangle its own copy:
		for text node value strings and attribute value strings, white space is trimmed from start and end of strings.
		for text node value strings and attribute value strings, new line characters are ignored and stripped.
		for text node value strings, consecutive tab and space characters in the document are condensed and converted to a single space character.
		for text node value strings and attribute value strings, HTML entities are translated into their corresponding UTF-8 encoded byte sequence.
			recognizes named entities:
				&amp; // amperstand.
				&apos; // single quote.
				&quot; // double quote.
				&gt; // close bracket, which may be '>' or ']'.
				&lt; // open bracket, which may be '<' or '['.
			recognizes decimal entities:
				&#[decimal number];
			recognizes hexadecimal entities:
				$#x[hexadecimal number];

	the differences when compared to proper XML are designed for convenience, but may be inconvenient if you are editing the files with your favorite code editor because it won't understand.
	this parser will also understand proper XML though too if you prefer to use that.
	some differeneces when compared to proper XML:
		we are not required to have <?xml ... ?> as the first line,
			it can be present but if it is then it is ignored.
		we are not required to have a single root level tag.
		we can have any number of root level nodes, and they can be any combination of text, tags, and comments.
		closing tags can simply be "</>", but you can give them a name if you want.
		closing tags are also allowed to define attributes, but there's no reason for it since you won't be able to access them.
		comments may start with "<!" or "<!--".
		comments may end with "!>" or "-->".
		comments may be nested, the root comment containing any number of other nested comments is converted to a single comment node which you can access when you walk the nodes.
	
	*/

	class data_scribe_markup_c
	{
	public:
		// an attribute is a name and value pair.
		class attribute_c
		{
		private:
			friend class data_scribe_markup_c;

			string8_c _name;
			string8_c _value;

		public:
			attribute_c();
			attribute_c( attribute_c const & ) = delete;
			attribute_c & operator = ( attribute_c const & ); // move_list_item() should be used instead.

			string8_c const & get_name() const;
			string8_c const & get_value() const;

			// replaces the assignment operator when the _attribute_heap list is resized.
			// this avoids making new copies of the heap allocated strings, so it's an optimization.
			static void_c move_list_item( attribute_c & from, attribute_c & to );

		};

		// a node represents a tag, or text, or comment.
		// tag type nodes can reference attributes.
		class node_c
		{
		public:
			enum type_e
			{
				type_e_none,
				type_e_tag, // has value (name) and references a range of attributes.
				type_e_text, // has value (text).
				type_e_comment, // has value (text).
			};

			enum tag_type_e
			{
				tag_type_e_none,
				tag_type_e_open, // open tag <tag>.
				tag_type_e_close, // close tag </tag>.
				tag_type_e_open_close, // open and close tag <tag/>.
			};

		private:
			friend class data_scribe_markup_c;

			type_e _type; // what kind of node this is
			tag_type_e _tag_type; // if this is an element node then this is the kind of element.

			string8_c _value; // if type is type_e_tag then this is the name of the tag. if type is type_e_text then this is the text.

			sint32_c _attribute_start; // the index of the first attribute within the _attribute_heap.
			sint32_c _attribute_count; // the number of concurrent entries in the _attribute_heap that are attributes of this node.

			sint32_c _index; // this node's index in the node heap.
			sint32_c _depth;

			sint32_c _mother; // the index of the mother node of this node within _node_heap. the mother node will always be a tag type of node.
			sint32_c _first_daughter; // the index of the first daughter node of this node within _node_heap, or -1 if none. one way to iterate through the daughters, data_scribe_markup_c::node_c const * daughter = this->xml->get_node( this->node_daughter ), then while ( daughter != nullptr ) { daughter = this->xml->get_node( daughter->node_sister_next ); }.
			sint32_c _daughter_count; // number of daughter nodes.
			sint32_c _next_sister; // the index of the next sister node of this node in _node_heap, or -1 if none.

			data_scribe_markup_c * _markup; // a little bloat but it lets us implement find_attribute(), find_tag(), and find_tags() on this instance.

			boolean_c _is_open; // while parsing: when true then new tags will be added as daughters, otherwise they will be added as sisters.

		public:
			node_c();
			node_c( node_c const & ) = delete;
			node_c & operator = ( node_c const & ); // move_list_item() should be used instead.

			void_c reset( data_scribe_markup_c * markup );

			node_c const * get_node( sint32_c index ) const; // gets the daughter at index, where index is relative to this node's local daughter list, so in the range of 0 and node_daughter_count - 1.

			attribute_c const * find_attribute( char8_c const * name ) const;

			node_c const * find_tag( char8_c const * name ) const;
			void_c find_tags( char8_c const * name, core_list_c< node_c const * > & result ) const;

			type_e get_type() const;
			tag_type_e get_tag_type() const;
			string8_c const & get_value() const;

			sint32_c get_depth() const;
			node_c const * get_mother() const;
			node_c const * get_first_daughter() const;
			sint32_c get_daughter_count() const;
			node_c const * get_daughter_at_index( sint32_c index ) const;
			node_c const * get_next_sister() const;

			data_scribe_markup_c const * get_markup() const;

			// replaces the assignment operator when the _attribute_heap list is resized.
			// this transfers ownership of strings, which avoids making new value copies, so it's an optimization.
			static void_c move_list_item( node_c & from, node_c & to );

		};

	private:
		char8_c _bracket_open; // will be '<' or '['.
		char8_c _bracket_close; // will be '>' or ']'.

		core_list_c< char8_c > _text_with_mark_up; // this contains a copy of the original document. when the document is parsed, some characters will be replaced with null characters to terminate names, values, texts, etc. so this copy will become mangled.
		char8_c * _current; // pointer to current position of parser in _text_with_mark_up.
		sint32_c _current_line; // current line number of parser within the file.
		sint32_c _current_column; // current column of parser within the file. because tabs count 1 character but can count from 1 to 4 columns.
		sint32_c _current_character; // current character in the line. because tabs count as 1 character but can count from 1 to 4 columns.
		sint32_c _tab_size; // defaults to 4.
		string16_c _error_message; // if there was an error while parsing, an explanation should be found here.

		core_list_c< node_c > _node_heap;
		core_list_c< attribute_c > _attribute_heap;

		void_c _advance( sint32_c amount );
		void_c _skip_space();

		boolean_c _parse(); // parses what's in _text_with_mark_up buffer.
		boolean_c _parse_text( node_c * node ); // parses text until '<' or '\0' is encountered.
		boolean_c _parse_tag( node_c * node ); // parses name and attributes until '>' or '/>' is encountered.
		boolean_c _parse_comment( node_c * node ); // parses text until last '!>' is encountered (nested comments are allowed). call when _current is lined up at start of '<!'.
		boolean_c _parse_generic_name( string8_c & out );
		boolean_c _parse_generic_quoted_value( string8_c & out );

		// processes a string in-place, storing the output result in the same buffer as the input.
		// given a utf-8 string defined by start and end, strips leading and trailing white space, condenses consecutive white space characters into a single space character, translates HTML entities to utf-8 byte sequences.
		// upon return, end will be set to the next byte after the end of the string, such that end - start is the length of the string.
		// end will need to be set to a null character later in order to properly finialize the string.
		void_c _process_string( char8_c * start, char8_c * & end );

		boolean_c _is_mark_up_character( char16_c character );
		boolean_c _is_name_character( char16_c character );
		boolean_c _is_space_character( char16_c character );

	public:
		// bracket type must be either '<' or '['.
		data_scribe_markup_c( char8_c bracket_type );
		~data_scribe_markup_c();

		void_c reset();

		boolean_c parse( string8_c const & document_string ); // document is null-terminated utf-8 string.
		boolean_c parse( data_stream_c * document_stream ); // document is a non-null-terminated utf-8 text file.

		// gets a node in the internal node heap at index.
		// returns nullptr if index is out of bounds of internal node heap.
		// the node at index 0 is the virtual root node, created to contain any number of document level root nodes.
		// the node at index 1 is the first document level root node, if the document contained any. note that strict xml requires that there is always only ever one of these, but this markup parser doesn't impose that restriction. also note that any <? ?> tag at the top of the document would have been skipped by this markup parser, and so would not be stored here or anywhere.
		// the node heap is a flat list, so it doesn't exactly reflect the nested structure of the parsed document.
		node_c const * get_node( sint32_c index ) const;

		void_c get_error_message( string16_c & message, sint32_c & line, sint32_c & column, sint32_c & character ) const; // if parse returned false, then this can be used to get more details about the error.

	public:
		// this function is specifically designed to parse only the first tag in a stream.
		// this is useful when the file format is designed to put meta data in the first tag, which may be the only thing that the program is interested in scanning before it decides if it wants to load and parse the whole file.
		// if no open bracket is found in the first x bytes, then it assumes that the file is incorrect format and gives up the search early and returns false.
		// returns true if a tag was found, then the result result tag node can be accessed with get_node( 0 ).
		boolean_c parse_first_tag( data_stream_c * stream );

	};

}
