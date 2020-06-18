#include "cheonsa_menu_control_combo.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	void_c menu_control_combo_item_text_c::_on_clicked( input_event_c * input_event )
	{
		_mother_user_interface->reset_multi_click_detection();
		if ( get_is_actually_enabled() )
		{
			if ( input_event->get_mouse_key() == input_mouse_key_e_left )
			{
				if ( _mother_control )
				{
					menu_control_combo_c * mother_list = dynamic_cast< menu_control_combo_c * >( _mother_control );
					if ( mother_list->get_use_toggle_behavior() )
					{
						_set_is_selected( !_is_selected, true );
					}
					else
					{
						_set_is_selected( true, true );
						assert( _mother_user_interface );
						_mother_user_interface->set_text_focused( nullptr ); // this should have the effect of hiding the whole popup menu hierarchy.
					}
				}
			}
			menu_control_list_item_i::_on_clicked( input_event );
		}
	}

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

	menu_control_combo_item_text_c::menu_control_combo_item_text_c()
		: menu_control_list_item_text_i()
	{
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
			if ( !_use_toggle_behavior )
			{
				_was_just_hidden = _combo_button == next_control;
				set_is_showed( false );
			}
		}
		on_is_deep_text_focused_changed.invoke( menu_event_information_c( this, next_control, nullptr ) );
	}

	menu_control_combo_c::menu_control_combo_c()
		: menu_control_list_i()
		, _combo_button( nullptr )
		, _was_just_hidden( false )
		, _use_toggle_behavior( false )
	{
		_layer = menu_layer_e_popup;
		_selected_item_limit = 1;
		_vertical_size_mode = menu_size_mode_e_fit_content;
		_vertical_size_maximum = 0.0f;
		set_is_showed_immediately( false ); // start hidden.
	}

	menu_control_combo_c::~menu_control_combo_c()
	{
	}

	boolean_c menu_control_combo_c::get_use_toggle_behavior() const
	{
		return _use_toggle_behavior;
	}

	void_c menu_control_combo_c::set_use_toggle_behavior( boolean_c value )
	{
		_use_toggle_behavior = value;
	}

	void_c menu_control_combo_c::show_like_combo_list( menu_control_c * combo_to_spawn_pop_up_around )
	{
		assert( _mother_user_interface );
		box32x2_c new_local_box = _mother_user_interface->find_combo_list_pop_up_box( combo_to_spawn_pop_up_around, _local_box.get_size(), true );
		set_layout_simple( new_local_box );
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
		_set_selected_item( item );
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
		if ( _is_enabled )
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
		menu_control_list_item_i const * selected_item = _combo_control->get_selected_item();
		if ( selected_item )
		{
			menu_control_combo_item_text_c const * selected_combo_list_item_text = dynamic_cast< menu_control_combo_item_text_c const * >( selected_item );
			if ( selected_combo_list_item_text )
			{
				_text_element.set_plain_text_value( selected_combo_list_item_text->get_plain_text_value() );
			}
			else
			{
				_text_element.clear_text_value();
			}
		}
		else
		{
			_text_element.clear_text_value();
		}
	}

	menu_control_combo_button_c::menu_control_combo_button_c()
		: menu_control_button_c()
		, _combo_control( nullptr )
	{
		_name = string8_c( core_list_mode_e_static, "combo" );

		set_style_map_key( string8_c( core_list_mode_e_static, "e_combo" ) );
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
			_remove_supplemental_control( _combo_control );
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
			_add_supplemental_control( _combo_control );
		}
	}

}
