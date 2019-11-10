#include "cheonsa_menu_style_file.h"
#include "cheonsa_menu_style_for_frame.h"
#include "cheonsa_menu_style_for_text.h"
#include "cheonsa_menu_style_map.h"
#include "cheonsa__ops.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	menu_style_file_c::menu_style_file_c( string16_c const & file_path_relative )
		: _file_path_relative( file_path_relative )
		, _file_path_absolute()
		, _file_modified_time( 0 )
		, _frame_style_dictionary()
		, _text_style_dictionary()
		, _style_map_dictionary()
	{
	}

	menu_style_file_c::~menu_style_file_c()
	{
		core_dictionary_c< string8_c, menu_style_for_frame_c * >::iterator_c frame_style_iterator = _frame_style_dictionary.get_iterator();
		while ( frame_style_iterator.next() )
		{
			delete frame_style_iterator.get_value();
		}

		core_dictionary_c< string8_c, menu_style_for_text_c * >::iterator_c text_style_iterator = _text_style_dictionary.get_iterator();
		while ( text_style_iterator.next() )
		{
			delete text_style_iterator.get_value();
		}

		core_dictionary_c< string8_c, menu_style_map_c * >::iterator_c style_map_iterator = _style_map_dictionary.get_iterator();
		while ( style_map_iterator.next() )
		{
			delete style_map_iterator.get_value();
		}
	}

	boolean_c menu_style_file_c::refresh()
	{
		// try to open new data.
		assert( engine_c::get_instance()->get_content_manager() != nullptr );
		if ( !engine_c::get_instance()->get_content_manager()->resolve_file_path( _file_path_relative, _file_path_absolute ) )
		{
			return false;
		}

		sint64_c file_modified_time = 0;
		if ( !ops::data_get_file_or_folder_modified_time( _file_path_absolute, file_modified_time ) )
		{
			return false;
		}

		if ( file_modified_time == _file_modified_time )
		{
			return false;
		}

		data_stream_file_c stream;
		if ( !stream.open( _file_path_absolute, data_stream_mode_e_read ) )
		{
			return false;
		}

		data_scribe_markup_c markup( '<' );
		if ( !markup.parse( &stream ) )
		{
			return false;
		}

		// unload current data.
		_frame_style_dictionary.remove_and_delete_all();
		_text_style_dictionary.remove_and_delete_all();
		_style_map_dictionary.remove_and_delete_all();

		// load new data.
		data_scribe_markup_c::node_c const * virtual_root_node = markup.get_node( 0 );
		data_scribe_markup_c::node_c const * node = virtual_root_node->get_first_daughter();
		while ( node )
		{
			if ( node->get_value() == "styles" )
			{
				data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
				while ( sub_node )
				{
					if ( sub_node->get_value() == "frame" )
					{
						menu_style_for_frame_c * frame_style = new menu_style_for_frame_c();
						frame_style->load( sub_node );
						if ( frame_style->key.get_length() > 0 )
						{
							if ( _frame_style_dictionary.contains( frame_style->key ) )
							{
								delete _frame_style_dictionary.find_value( frame_style->key );
							}
							_frame_style_dictionary.insert( frame_style->key, frame_style );
						}
						else
						{
							delete frame_style;
						}
					}
					else if ( sub_node->get_value() == "text" )
					{
						menu_style_for_text_c * text_style = new menu_style_for_text_c();
						text_style->load( sub_node );
						if ( text_style->key.get_length() > 0 )
						{
							if ( _text_style_dictionary.contains( text_style->key ) )
							{
								delete _text_style_dictionary.find_value( text_style->key );
							}
							_text_style_dictionary.insert( text_style->key, text_style );
						}
						else
						{
							delete text_style;
						}
					}
					else if ( sub_node->get_value() == "map" )
					{
						menu_style_map_c * style_map = new menu_style_map_c();
						style_map->load( sub_node );
						if ( style_map->key.get_length() > 0 )
						{
							if ( _style_map_dictionary.contains( style_map->key ) )
							{
								delete _style_map_dictionary.find_value( style_map->key );
							}
							_style_map_dictionary.insert( style_map->key, style_map );
						}
						else
						{
							delete style_map;
						}
					}
					sub_node = sub_node->get_next_sister();
				}
			}
			node = node->get_next_sister();
		}

		return true;
	}

}
