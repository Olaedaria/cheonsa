#include "cheonsa_string_file.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_data_scribe_markup.h"

namespace cheonsa
{

	string_file_c::string_file_c()
		: _string_dictionary()
	{
	}

	string_file_c::~string_file_c()
	{
	}

	boolean_c string_file_c::load_from_xml( string16_c const & absolute_file_path )
	{
		unload();

		// try to open the file.
		data_stream_file_c stream; // we don't need to close it because its destructor will close it when it goes out of scope.
		if ( !stream.open( absolute_file_path, data_stream_mode_e_read ) )
		{
			// this usually means that the file doesn't exist.
			// this isn't a big deal though, it could be by design.
			return false;
		}

		// parse the stream.
		data_scribe_markup_c markup( '<' );
		if ( !markup.parse( &stream ) )
		{
			return false;
		}

		// look for root tag.
		data_scribe_markup_c::node_c const * strings_tag = markup.get_node( 0 )->find_tag( "strings" );
		if ( strings_tag == nullptr )
		{
			return false;
		}

		// extract string keys and values, build _byte_heap.
		core_list_c< sint32_c > offsets; // we record offsets to keys and values in the string buffer. sint32_c can index a range up to 2GB, it should be enough.
		data_scribe_markup_c::node_c const * string_tag = strings_tag->get_first_daughter();
		while ( string_tag )
		{
			if ( string_tag->get_type() == data_scribe_markup_c::node_c::type_e_tag && string_tag->get_value() == "string" )
			{
				data_scribe_markup_c::attribute_c const * key_attribute = string_tag->find_attribute( "key" );
				if ( key_attribute == nullptr || key_attribute->get_value().get_length() == 0 )
				{
					continue;
				}

				// save key.
				offsets.insert_at_end( _byte_heap.get_length() ); // record start of key.
				_byte_heap.insert_at_end( key_attribute->get_value().character_list.get_internal_array(), key_attribute->get_value().character_list.get_length() ); // copy key to byte buffer.
				offsets.insert_at_end( _byte_heap.get_length() ); // record end of key, which is also start of value.

				// save value.
				data_scribe_markup_c::node_c const * string_text = string_tag->get_first_daughter();
				if ( string_text && string_text->get_type() == data_scribe_markup_c::node_c::type_e_text )
				{
					// text node found.
					_byte_heap.insert_at_end( string_text->get_value().character_list.get_internal_array(), string_text->get_value().character_list.get_length() );
				}
				else
				{
					// no text node found.
					_byte_heap.insert_at_end( '\0' ); // insert null terminator for empty string.
				}
			}
			string_tag = string_tag->get_next_sister();
		}

		// build _string_heap and _string_dictionary from _byte_heap.
		sint32_c count = offsets.get_length();
		offsets.insert_at_end( _byte_heap.get_length() ); // add extra so that accessing [ i + 2 ] does not fail.
		for ( sint32_c i = 0; i < count; i += 2 )
		{
			string_c * string = _string_heap.emplace_at_end();
			sint32_c key_start = offsets[ i ];
			sint32_c key_end = offsets[ i + 1 ];
			string->_key = string8_c( mode_e_static, &_byte_heap.get_internal_array()[ key_start ], key_end - key_start );
			sint32_c value_start = key_end;
			sint32_c value_end = offsets[ i + 2 ];
			string->_value = string8_c( mode_e_static, &_byte_heap.get_internal_array()[ value_start ], value_end - value_start );
			_string_dictionary.insert( string->_key, i );
		}

		return true;
	}

	void_c string_file_c::unload()
	{
		_string_dictionary.remove_all();
	}

	string_c const * string_file_c::find_string( string8_c const & key ) const
	{
		sint32_c const * string_index = _string_dictionary.find_value_pointer( key );
		if ( string_index )
		{
			return &_string_heap[ *string_index ];
		}
		return nullptr;
	}

}
