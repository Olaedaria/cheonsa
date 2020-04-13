#include "cheonsa_menu_control_combo.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	void_c menu_control_combo_list_item_text_c::_on_is_mouse_overed_changed()
	{
		if ( _is_mouse_overed )
		{
			assert( _mother_control != nullptr );
			menu_control_combo_list_c * mother_combo_list = dynamic_cast< menu_control_combo_list_c * >( _mother_control->get_mother_control() );
			assert( mother_combo_list );
			mother_combo_list->on_selected_item_changed_preview.invoke( this );
		}
	}

	menu_control_combo_list_item_text_c::menu_control_combo_list_item_text_c()
		: menu_control_list_item_text_i()
	{
		_name = string8_c( core_list_mode_e_static, "combo_list_item" );
	}

	void_c menu_control_combo_list_c::_on_selected_item_list_changed()
	{
		on_selected_item_changed.invoke( this );
	}

	void_c menu_control_combo_list_c::_on_is_deep_text_focused_changed()
	{
		if ( _is_deep_text_focused == false )
		{
			set_is_showed( false );
		}
		on_is_deep_text_focused_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	menu_control_combo_list_c::menu_control_combo_list_c()
		: menu_control_list_i()
	{
		_name = string8_c( core_list_mode_e_static, "combo_list" );
		_layer = menu_layer_e_popup;
		_selected_item_limit = 1;
		_vertical_size_mode = menu_size_mode_e_fit_content;
		_vertical_size_maximum = 0.0f;
		set_is_showed_immediately( false ); // start hidden.
	}

	sint32_c menu_control_combo_list_c::get_item_count() const
	{
		return _get_item_count();
	}

	menu_control_list_item_i const * menu_control_combo_list_c::get_item( sint32_c item_index ) const
	{
		return _get_item( item_index );
	}

	menu_control_list_item_i * menu_control_combo_list_c::get_item( sint32_c item_index )
	{
		return _get_item( item_index );
	}

	sint32_c menu_control_combo_list_c::give_item( menu_control_list_item_i * item, sint32_c index )
	{
		return _give_item( item, index );
	}

	menu_control_list_item_i * menu_control_combo_list_c::take_item( sint32_c item_index )
	{
		return _take_item( item_index );
	}

	void_c menu_control_combo_list_c::remove_and_delete_all_items()
	{
		_remove_and_delete_all_items();
	}

	sint32_c menu_control_combo_list_c::get_selected_item_index() const
	{
		return _get_selected_item_index();
	}

	void_c menu_control_combo_list_c::set_selected_item_index( sint32_c item_index )
	{
		_set_selected_item_index( item_index );
	}

	menu_control_list_item_i * menu_control_combo_list_c::get_selected_item() const
	{
		return _get_selected_item();
	}

	void_c menu_control_combo_list_c::set_selected_item( menu_control_list_item_i * item )
	{
		_set_selected_item( item );
	}

	void_c menu_control_combo_c::_on_clicked( input_event_c * input_event )
	{
		show_combo_list();
		menu_control_button_c::_on_clicked( input_event );
	}

	void_c menu_control_combo_c::_handle_on_selected_item_changed( menu_control_combo_list_c * combo_list )
	{
		menu_control_list_item_i const * selected_item = _combo_list->get_selected_item();
		if ( selected_item != nullptr )
		{
			menu_control_combo_list_item_text_c const * selected_combo_list_item_text = dynamic_cast< menu_control_combo_list_item_text_c const * >( selected_item );
			if ( selected_combo_list_item_text != nullptr )
			{
				_element_text.set_plain_text_value( selected_combo_list_item_text->get_plain_text_value() );
			}
			else
			{
				_element_text.clear_text_value();
			}
		}
		else
		{
			_element_text.clear_text_value();
		}
	}

	menu_control_combo_c::menu_control_combo_c()
		: menu_control_button_c()
		, _combo_list( nullptr )
	{
		_name = string8_c( core_list_mode_e_static, "combo" );

		_combo_list = new menu_control_combo_list_c();
		_combo_list->on_selected_item_changed.subscribe( this, &menu_control_combo_c::_handle_on_selected_item_changed );
		_give_control( _combo_list );

		set_style_map_key( string8_c( core_list_mode_e_static, "e_combo" ) );
	}

	void_c menu_control_combo_c::show_combo_list()
	{
		user_interface_c * user_interface = get_user_interface_root();
		assert( user_interface );
		_combo_list->set_layout_simple( user_interface->find_combo_list_pop_up_box( this, _combo_list->get_local_box().get_size(), false ) );
		_combo_list->set_is_showed_immediately( true );
		_combo_list->give_text_focus();
	}

	void_c menu_control_combo_c::hide_combo_list()
	{
		_combo_list->set_is_showed( false );
	}

	menu_control_combo_list_c const * menu_control_combo_c::get_combo_list() const
	{
		return _combo_list;
	}

	menu_control_combo_list_c * menu_control_combo_c::get_combo_list()
	{
		return _combo_list;
	}

}
