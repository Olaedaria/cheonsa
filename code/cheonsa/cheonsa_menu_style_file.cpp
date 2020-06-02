#include "cheonsa_menu_style_file.h"
#include "cheonsa__ops.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_style_file_c::_load( data_stream_c * stream )
	{
		assert( stream );
		assert( _is_loaded == false );

		data_scribe_markup_c markup( '<' );
		if ( !markup.parse( stream ) )
		{
			return;
		}

		// unload current data.
		_color_style_dictionary.remove_and_delete_all();
		_frame_style_dictionary.remove_and_delete_all();
		_text_style_dictionary.remove_and_delete_all();
		_style_map_dictionary.remove_and_delete_all();


		data_scribe_markup_c::attribute_c const * attribute = nullptr;
		data_scribe_markup_c::node_c const * node = markup.get_node( 0 )->find_tag( "styles" );
		if ( node == nullptr )
		{
			return;
		}

		// load new data.
		data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
		while ( sub_node )
		{
			if ( sub_node->get_value() == "color" )
			{
				menu_color_style_c * color_style = new menu_color_style_c();
				color_style->load( sub_node );
				if ( color_style->key.get_length() > 0 )
				{
					if ( _color_style_dictionary.contains( color_style->key ) )
					{
						delete _color_style_dictionary.find_value( color_style->key );
					}
					_color_style_dictionary.insert( color_style->key, color_style );
				}
			}
			else if ( sub_node->get_value() == "frame" )
			{
				menu_frame_style_c * frame_style = new menu_frame_style_c();
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
				menu_text_style_c * text_style = new menu_text_style_c();
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
				if ( style_map->get_key().get_length() > 0 )
				{
					if ( _style_map_dictionary.contains( style_map->get_key() ) )
					{
						delete _style_map_dictionary.find_value( style_map->get_key() );
					}
					_style_map_dictionary.insert( style_map->get_key(), style_map );
				}
				else
				{
					delete style_map;
				}
			}

			sub_node = sub_node->get_next_sister();
		}

		_is_loaded = true;

		on_loaded.invoke( this );
	}

	void_c menu_style_file_c::_unload()
	{
		assert( _is_loaded == true );

		on_unloaded.invoke( this );

		_is_loaded = false;

		_color_style_dictionary.remove_and_delete_all();
		_frame_style_dictionary.remove_and_delete_all();
		_text_style_dictionary.remove_and_delete_all();
		_style_map_dictionary.remove_and_delete_all();
	}

	menu_style_file_c::menu_style_file_c( string16_c const & file_path )
		: resource_file_c( file_path )
		, _color_style_dictionary()
		, _frame_style_dictionary()
		, _text_style_dictionary()
		, _style_map_dictionary()
	{
	}

	menu_style_file_c::~menu_style_file_c()
	{
		if ( _is_loaded )
		{
			_unload();
		}
	}

	menu_color_style_c const * menu_style_file_c::find_color_style( string8_c const & key ) const
	{
		return _color_style_dictionary.find_value_else_nullptr( key );
	}

	menu_frame_style_c const * menu_style_file_c::find_frame_style( string8_c const & key ) const
	{
		return _frame_style_dictionary.find_value_else_nullptr( key );
	}

	menu_text_style_c const * menu_style_file_c::find_text_style( string8_c const & key ) const
	{
		return _text_style_dictionary.find_value_else_nullptr( key );
	}

	menu_style_map_c const * menu_style_file_c::find_style_map( string8_c const & key ) const
	{
		return _style_map_dictionary.find_value_else_nullptr( key );
	}

}
