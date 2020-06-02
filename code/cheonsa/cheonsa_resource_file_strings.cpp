#include "cheonsa_resource_file_strings.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_data_scribe_markup.h"

namespace cheonsa
{

	resource_file_strings_c::resource_file_strings_c( string16_c const & file_path )
		: resource_file_c( file_path )
		, _byte_heap()
		, _string_heap()
		, _string_dictionary()
	{
	}

	resource_file_strings_c::~resource_file_strings_c()
	{
	}

	void_c resource_file_strings_c::_load( data_stream_c * stream )
	{
		assert( stream );
		assert( _is_loaded == false );

		// parse the stream.
		data_scribe_markup_c markup( '<' );
		if ( !markup.parse( stream ) )
		{
			return;
		}

		// look for root tag.
		data_scribe_markup_c::node_c const * strings_tag = markup.get_node( 0 )->find_tag( "strings" );
		if ( strings_tag == nullptr )
		{
			return;
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
				offsets.insert( -1, _byte_heap.get_length() ); // record start of key.
				_byte_heap.insert( -1, key_attribute->get_value().character_list.get_internal_array(), key_attribute->get_value().character_list.get_length() ); // copy key to byte buffer.
				offsets.insert( -1, _byte_heap.get_length() ); // record end of key, which is also start of value.

				// save value.
				data_scribe_markup_c::node_c const * string_text = string_tag->get_first_daughter();
				if ( string_text && string_text->get_type() == data_scribe_markup_c::node_c::type_e_text )
				{
					// text node found.
					_byte_heap.insert( -1, string_text->get_value().character_list.get_internal_array(), string_text->get_value().character_list.get_length() );
				}
				else
				{
					// no text node found.
					_byte_heap.insert( -1, '\0' ); // insert null terminator for empty string.
				}
			}
			string_tag = string_tag->get_next_sister();
		}

		// build _string_heap and _string_dictionary from _byte_heap.
		sint32_c count = offsets.get_length();
		offsets.insert( -1, _byte_heap.get_length() ); // add extra so that accessing [ i + 2 ] does not fail.
		for ( sint32_c i = 0; i < count; i += 2 )
		{
			string_c * string = _string_heap.emplace( -1, 1 );
			sint32_c key_start = offsets[ i ];
			sint32_c key_end = offsets[ i + 1 ];
			string->key.character_list.construct_mode_static_volatile_from_array( &_byte_heap.get_internal_array()[ key_start ], key_end - key_start );
			sint32_c value_start = key_end;
			sint32_c value_end = offsets[ i + 2 ];
			string->value.character_list.construct_mode_static_volatile_from_array( &_byte_heap.get_internal_array()[ value_start ], value_end - value_start );
			_string_dictionary.insert( string->key, i );
		}

		_is_loaded = true;

		on_loaded.invoke( this );
	}

	void_c resource_file_strings_c::_unload()
	{
		assert( _is_loaded == true );

		on_unloaded.invoke( this );

		_is_loaded = false;

		_string_dictionary.remove_all();
		_string_heap.remove_all();
		_byte_heap.remove_all();
	}

	boolean_c resource_file_strings_c::find_string( string8_c const & key, string8_c & result ) const
	{
		sint32_c const * string_index = _string_dictionary.find_value_pointer( key );
		if ( string_index )
		{
			result = _string_heap[ *string_index ].value;
			return true;
		}
		return false;
	}

	boolean_c resource_file_strings_c::find_string( string8_c const & key, string16_c & result ) const
	{
		sint32_c const * string_index = _string_dictionary.find_value_pointer( key );
		if ( string_index )
		{
			result = _string_heap[ *string_index ].value;
			return true;
		}
		return false;
	}

}
