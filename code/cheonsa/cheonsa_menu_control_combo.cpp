#include "cheonsa_menu_control_combo.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	void_c menu_control_combo_item_text_c::_on_is_mouse_overed_changed()
	{
		if ( _is_mouse_overed )
		{
			assert( _mother_control );
			menu_control_combo_c * mother_combo_list = dynamic_cast< menu_control_combo_c * >( _mother_control->get_mother_control() );
			assert( mother_combo_list );
			mother_combo_list->on_selected_item_changed_preview.invoke( this );
		}
	}

	void_c menu_control_combo_item_text_c::_on_clicked( input_event_c * input_event )
	{
		_mother_user_interface->reset_multi_click_detection();
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
					_set_is_selected( true, true );
				}
				else
				{
					_set_is_selected( true, false );
					_mother_user_interface->set_text_focused( nullptr ); // this should hide the pop up.
				}
			}
			on_clicked.invoke( menu_event_information_c( this, nullptr, input_event ) );
		}
	}

	menu_control_combo_item_text_c::menu_control_combo_item_text_c()
		: menu_control_list_item_text_i()
	{
		set_non_client_type( menu_non_client_type_e_client );
	}

	menu_control_combo_item_text_c::~menu_control_combo_item_text_c()
	{
	}

	void_c menu_control_combo_item_text_c::select()
	{
		_set_is_selected( true, false );
	}

	void_c menu_control_combo_c::_on_selected_item_list_changed()
	{
		on_selected_item_changed.invoke( this );
	}

	void_c menu_control_combo_c::_handle_user_interface_local_box_changed()
	{
		if ( _combo_button && _is_showed )
		{
			show_like_combo_list( _combo_button );
		}
	}

	void_c menu_control_combo_c::_on_is_deep_text_focused_changed( menu_control_c * next_control )
	{
		if ( _is_deep_text_focused == false )
		{
			_was_just_hidden = _combo_button == next_control;
			set_is_showed( false );
		}
		on_is_deep_text_focused_changed.invoke( menu_event_information_c( this, next_control, nullptr ) );
	}

	menu_control_combo_c::menu_control_combo_c()
		: menu_control_list_i()
		, _combo_button( nullptr )
		, _was_just_hidden( false )
	{
		_layer = menu_layer_e_popup;
		_selected_item_limit = 1;
		_vertical_size_mode = menu_size_mode_e_fit_content;
		_vertical_size_maximum = 0.0f;
		set_is_showed_immediately( false ); // start hidden.
		set_non_client_type( menu_non_client_type_e_client );
	}

	menu_control_combo_c::~menu_control_combo_c()
	{
	}

	void_c menu_control_combo_c::show_like_combo_list( menu_control_c * combo_button_to_spawn_pop_up_around )
	{
		assert( _mother_user_interface );
		box32x2_c pop_up_box;
		pop_up_box.minimum.a = 0.0f;
		pop_up_box.maximum.a = combo_button_to_spawn_pop_up_around->get_local_box().get_width();
		pop_up_box.minimum.b = 0.0f;
		pop_up_box.maximum.b = 1000.0f;
		set_layout_simple( pop_up_box );

		box32x2_c new_local_box = _mother_user_interface->find_combo_list_pop_up_box( combo_button_to_spawn_pop_up_around, _local_box.get_size(), true );
		set_layout_simple( new_local_box );
		//new_local_box = _mother_user_interface->find_combo_list_pop_up_box( combo_button_to_spawn_pop_up_around, _local_box.get_size(), true );
		//set_layout_simple( new_local_box );
		set_is_showed_immediately( true );
		bring_to_front();
		give_text_focus();
	}

	sint32_c menu_control_combo_c::get_item_count() const
	{
		return _client->get_daughter_control_list().get_length();
	}

	menu_control_list_item_i const * menu_control_combo_c::get_item_at_index( sint32_c item_index ) const
	{
		return static_cast< menu_control_list_item_i const * >( _client->get_daughter_control_list().get_at_index( item_index )->get_value() );
	}

	menu_control_list_item_i * menu_control_combo_c::get_item_at_index( sint32_c item_index )
	{
		return static_cast< menu_control_list_item_i * >( _client->get_daughter_control_list().get_at_index( item_index )->get_value() );
	}

	void_c menu_control_combo_c::add_item( menu_control_list_item_i * item, sint32_c index )
	{
		assert( dynamic_cast< menu_control_combo_item_text_c * >( item ) );
		_add_item( item, index );
	}

	void_c menu_control_combo_c::remove_item( menu_control_list_item_i * item )
	{
		_remove_item( item );
	}

	void_c menu_control_combo_c::remove_all_items()
	{
		_remove_all_items();
	}

	menu_control_list_item_i * menu_control_combo_c::get_selected_item() const
	{
		return _get_selected_item();
	}

	void_c menu_control_combo_c::set_selected_item( menu_control_list_item_i * item )
	{
		menu_control_combo_item_text_c * combo_item = dynamic_cast< menu_control_combo_item_text_c * >( item );
		if ( combo_item )
		{
			combo_item->select();
		}
	}

	sint32_c menu_control_combo_c::get_selected_item_count() const
	{
		return _selected_item_list.get_length();
	}

	menu_control_list_item_i const * menu_control_combo_c::get_selected_item_at_index( sint32_c index ) const
	{
		return _selected_item_list[ index ];
	}

	menu_control_list_item_i * menu_control_combo_c::get_selected_item_at_index( sint32_c index )
	{
		return _selected_item_list[ index ];
	}

	sint32_c menu_control_combo_c::get_selected_item_index() const
	{
		return _get_selected_item_index();
	}

	void_c menu_control_combo_c::set_selected_item_index( sint32_c item_index )
	{
		_set_selected_item_index( item_index );
	}

	sint32_c menu_control_combo_c::get_selected_item_limit() const
	{
		return _selected_item_limit;
	}

	void_c menu_control_combo_c::set_selected_item_limit( sint32_c value )
	{
		_set_selected_item_limit( value );
	}

	void_c menu_control_combo_button_c::_on_clicked( input_event_c * input_event )
	{
		_mother_user_interface->reset_multi_click_detection();
		if ( get_is_actually_enabled() )
		{
			if ( _combo_control && input_event->get_mouse_key() == input_mouse_key_e_left )
			{
				if ( _combo_control->_was_just_hidden )
				{
					_combo_control->_was_just_hidden = false; // flop.
				}
				else if ( _combo_control->get_is_showed() == false )
				{
					_combo_control->show_like_combo_list( this );
				}
			}
			on_clicked.invoke( menu_event_information_c( this, nullptr, input_event ) );
		}
	}

	void_c menu_control_combo_button_c::_handle_on_selected_item_changed( menu_control_combo_c * combo_list )
	{
		_update_plain_text_value();
	}

	void_c menu_control_combo_button_c::_update_plain_text_value()
	{
		string16_c plain_text_value;
		menu_control_combo_c * combo_control = dynamic_cast< menu_control_combo_c * >( _combo_control );
		if ( combo_control )
		{
			// walk the whole item list (rather than just the selected item list) so that string building is done in the same order that the items are inserted into the list rather than in the order that the items are selected.
			for ( sint32_c i = 0; i < combo_control->get_item_count(); i++ )
			{
				menu_control_combo_item_text_c * combo_item_text = dynamic_cast< menu_control_combo_item_text_c * >( combo_control->get_item_at_index( i ) );
				if ( combo_item_text && combo_item_text->get_is_selected() )
				{
					if ( plain_text_value.is_set() )
					{
						plain_text_value += ", ";
					}
					plain_text_value += combo_item_text->get_plain_text_value();
				}
			}
		}
		set_plain_text_value( plain_text_value );
	}

	menu_control_combo_button_c::menu_control_combo_button_c()
		: menu_control_button_c()
		, _combo_control( nullptr )
	{
		_name = string8_c( "combo", core_list_mode_e_static );

		set_style_map_key( string8_c( "e_combo", core_list_mode_e_static ) );
	}

	menu_control_combo_button_c::~menu_control_combo_button_c()
	{
	}

	menu_control_combo_c const * menu_control_combo_button_c::get_combo_control() const
	{
		return _combo_control;
	}

	menu_control_combo_c * menu_control_combo_button_c::get_combo_control()
	{
		return _combo_control;
	}

	void_c menu_control_combo_button_c::set_combo_control( menu_control_combo_c * value )
	{
		if ( _combo_control )
		{
			remove_supplemental_daughter_control( _combo_control );
			_combo_control->_combo_button = nullptr;
			_combo_control->on_selected_item_changed.unsubscribe( this, &menu_control_combo_button_c::_handle_on_selected_item_changed );
		}
		_combo_control  = value;
		if ( _combo_control )
		{
			assert( _combo_control->_combo_button == nullptr );
			_combo_control->set_is_showed_immediately( false );
			_combo_control->_combo_button = this;
			_combo_control->on_selected_item_changed.subscribe( this, &menu_control_combo_button_c::_handle_on_selected_item_changed );
			add_supplemental_daughter_control( _combo_control );
		}
		_update_plain_text_value();
	}

}
