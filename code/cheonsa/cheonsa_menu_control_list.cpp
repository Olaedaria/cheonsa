#include "cheonsa_menu_control_list.h"
#include "cheonsa_user_interface.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_control_list_item_separator_c::menu_control_list_item_separator_c()
		: menu_control_list_item_separator_i()
	{
		set_style_map_key( string8_c( "e_list_item_separator", core_list_mode_e_static ) );
		set_color_theme( engine.get_menu_style_manager()->get_internal_field_color_theme() );
	}

	menu_control_list_item_separator_c::~menu_control_list_item_separator_c()
	{
	}

	menu_control_list_c * menu_control_list_item_separator_c::get_list() const
	{
		menu_control_list_c * result = nullptr;
		if ( _mother_control )
		{
			assert( _mother_control->get_mother_control() );
			result = dynamic_cast< menu_control_list_c * >( _mother_control->get_mother_control() );
			assert( result );
		}
		return result;
	}

	void_c menu_control_list_item_text_c::_on_is_selected_changed()
	{
		if ( _style_map_reference.get_key().is_set() )
		{
			if ( _is_selected )
			{
				if ( !ops::string8_ends_with( _style_map_reference.get_key(), string8_c( "_selected", core_list_mode_e_static ) ) )
				{
					string8_c new_key = _style_map_reference.get_key();
					new_key += "_selected";
					_style_map_reference.set_key( new_key );
				}
			}
			else
			{
				if ( ops::string8_ends_with( _style_map_reference.get_key(), string8_c( "_selected", core_list_mode_e_static ) ) )
				{
					string8_c new_key = ops::string8_sub_string( _style_map_reference.get_key(), 0, _style_map_reference.get_key().get_length() - 9 );
					_style_map_reference.set_key( new_key );
				}
			}
		}
	}

	void_c menu_control_list_item_text_c::_on_clicked( input_event_c * input_event )
	{
		if ( get_is_actually_enabled() )
		{
			assert( _mother_control );
			menu_control_list_i * mother_list = dynamic_cast< menu_control_list_i * >( _mother_control->get_mother_control() );
			assert( mother_list );
			if ( _can_be_toggled )
			{
				_set_is_selected( !_is_selected, true );
			}
			else
			{
				if ( input_event->get_modifier_flags() == input_modifier_flag_e_ctrl )
				{
					_set_is_selected( !_is_selected, true );
				}
				else
				{
					_set_is_selected( true, false );
				}
			}
			on_clicked.invoke( menu_event_information_c( this, nullptr, input_event ) );
		}
		else
		{
			_mother_user_interface->reset_multi_click_detection();
		}
	}

	void_c menu_control_list_item_text_c::_on_multi_clicked( input_event_c * input_event )
	{
		if ( get_is_actually_enabled() )
		{
			if ( input_event->get_menu_multi_click_count() >= 2 )
			{
				_mother_user_interface->reset_multi_click_detection();
			}
			on_multi_clicked.invoke( menu_event_information_c( this, nullptr, input_event ) );
		}
		else
		{
			_mother_user_interface->reset_multi_click_detection();
		}
	}

	menu_control_list_item_text_c::menu_control_list_item_text_c()
		: menu_control_list_item_text_i()
	{
		set_style_map_key( string8_c( "e_list_item", core_list_mode_e_static ) );
		set_color_theme( engine.get_menu_style_manager()->get_internal_field_color_theme() );
	}

	menu_control_list_item_text_c::~menu_control_list_item_text_c()
	{
	}

	menu_control_list_c * menu_control_list_item_text_c::get_list() const
	{
		menu_control_list_c * result = nullptr;
		if ( _mother_control )
		{
			assert( _mother_control->get_mother_control() );
			result = dynamic_cast< menu_control_list_c * >( _mother_control->get_mother_control() );
			assert( result );
		}
		return result;
	}

	void_c menu_control_list_item_text_c::set_is_selected( boolean_c is_selected, boolean_c try_to_multi_select )
	{
		_set_is_selected( is_selected, try_to_multi_select );
	}

	void_c menu_control_list_c::_on_selected_item_list_changed()
	{
		on_selected_item_list_changed.invoke( this );
	}

	menu_control_list_c::menu_control_list_c()
		: menu_control_list_i()
	{
		_name = string8_c( "list", core_list_mode_e_static );
		set_style_map_key( string8_c( "e_list", core_list_mode_e_static ) );
	}

	menu_control_list_c::~menu_control_list_c()
	{
	}

	sint32_c menu_control_list_c::get_selected_item_limit() const
	{
		return _selected_item_limit;
	}

	void_c menu_control_list_c::set_selected_item_limit( sint32_c value )
	{
		_set_selected_item_limit( value );
	}

	sint32_c menu_control_list_c::get_item_count() const
	{
		return _client->get_daughter_control_list().get_length();
	}

	menu_control_list_item_i const * menu_control_list_c::get_item_at_index( sint32_c item_index ) const
	{
		return static_cast< menu_control_list_item_i const * >( _client->get_daughter_control_list().get_at_index( item_index )->get_value() );
	}

	menu_control_list_item_i * menu_control_list_c::get_item_at_index( sint32_c item_index )
	{
		return static_cast< menu_control_list_item_i * >( _client->get_daughter_control_list().get_at_index( item_index )->get_value() );
	}

	void_c menu_control_list_c::add_item( menu_control_list_item_i * item, sint32_c index )
	{
		assert( dynamic_cast< menu_control_list_item_separator_c * >( item ) || dynamic_cast< menu_control_list_item_text_c * >( item ) );
		_add_item( item, index );
	}

	void_c menu_control_list_c::remove_item( menu_control_list_item_i * item )
	{
		_remove_item( item );
	}

	void_c menu_control_list_c::remove_all_items()
	{
		_remove_all_items();
	}

	menu_control_list_item_i * menu_control_list_c::get_selected_item() const
	{
		return _get_selected_item();
	}

	void_c menu_control_list_c::set_selected_item( menu_control_list_item_i * item )
	{
		menu_control_list_item_text_c * list_item = dynamic_cast< menu_control_list_item_text_c * >( item );
		if ( list_item )
		{
			list_item->set_is_selected( true, false );
		}
	}

	sint32_c menu_control_list_c::get_selected_item_index() const
	{
		return _get_selected_item_index();
	}

	void_c menu_control_list_c::set_selected_item_index( sint32_c item_index )
	{
		_set_selected_item_index( item_index );
	}

	core_list_c< menu_control_list_item_i * > const & menu_control_list_c::get_selected_item_list() const
	{
		return _selected_item_list;
	}

	sint32_c list_item_compare_function( menu_control_list_item_text_c * const & a, menu_control_list_item_text_c * const & b )
	{
		return ops::string16_sort_compare( a->get_internal_plain_text_value(), b->get_internal_plain_text_value() );
	}

	void_c menu_control_list_c::sort_items_by_plain_text_value( boolean_c invert )
	{
		core_list_c< menu_control_list_item_text_c * > list_item_list;
		core_linked_list_c< menu_control_c * >::node_c * daughter_node = _client->get_daughter_control_list().get_first();
		while ( daughter_node )
		{
			list_item_list.insert( -1, static_cast< menu_control_list_item_text_c * >( daughter_node->get_value() ) );
			daughter_node = daughter_node->get_next();
		}
		list_item_list.quick_sort_2( &list_item_compare_function, false );

		_client->_get_daughter_control_list().remove_all();
		for ( sint32_c i = 0; i < list_item_list.get_length(); i++ )
		{
			_client->_get_daughter_control_list().insert_at_end( &list_item_list[ i ]->_get_daughter_control_list_node() );
		}
		_item_origins_are_dirty = true;
		_layout_item_origins();
	}

}

