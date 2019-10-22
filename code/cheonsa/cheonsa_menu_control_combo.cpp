#include "cheonsa_menu_control_combo.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	menu_control_combo_list_item_c::menu_control_combo_list_item_c()
	{
		_name = string8_c( mode_e_static, "combo_list_item" );
	}

	void_c menu_control_combo_list_c::_on_deep_text_focus_lost()
	{
		hide( false );
		menu_control_c::_on_deep_text_focus_lost();
	}

	void_c menu_control_combo_list_c::_on_selection_changed()
	{
		menu_control_combo_c * combo = dynamic_cast< menu_control_combo_c * >( _mother_control );
		if ( combo != nullptr )
		{
			combo->_handle_on_selection_changed();
		}
		on_selection_changed_commit.invoke( this );
	}

	menu_control_combo_list_c::menu_control_combo_list_c()
		: menu_control_list_i()
	{
		_name = string8_c( mode_e_static, "combo_list" );
		_layer = menu_layer_e_popup;
		_set_selected_item_limit( 1 );
		_vertical_size_mode = menu_size_mode_e_fit_content;
		_vertical_size_maximum = 0.0f;
		hide_immediately(); // start hidden.
	}

	sint32_c menu_control_combo_list_c::get_selected_item_index() const
	{
		return _get_selected_item_index();
	}

	void_c menu_control_combo_list_c::set_selected_item_index( sint32_c index )
	{
		_set_selected_item_index( index );
	}

	menu_control_combo_list_item_c * menu_control_combo_list_c::get_selected_item() const
	{
		sint32_c selected_item_index = _get_selected_item_index();
		if ( selected_item_index >= 0 )
		{
			menu_control_combo_list_item_c * result = dynamic_cast< menu_control_combo_list_item_c * >( _control_list[ selected_item_index ] );
			assert( result != nullptr );
			assert( result->get_index() == selected_item_index );
			return result;
		}
		return nullptr;
	}

	void_c menu_control_combo_list_c::set_selected_item( menu_control_combo_list_item_c * value )
	{
		set_selected_item_index( value != nullptr ? value->get_index() : -1 );
	}

	sint32_c menu_control_combo_list_c::get_item_count() const
	{
		return _control_list.get_length();
	}

	menu_control_combo_list_item_c * menu_control_combo_list_c::get_item_at_index( sint32_c index )
	{
		assert( index >= 0 && index < _control_list.get_length() );
		menu_control_combo_list_item_c * result = dynamic_cast< menu_control_combo_list_item_c * >( _control_list[ index ] );
		assert( result != nullptr );
		assert( result->get_index() == index );
		return result;
	}

	void_c menu_control_combo_list_c::add_item( menu_control_combo_list_item_c * item, sint32_c index )
	{
		_list_item_holder->_add_control( item, index );
	}

	void_c menu_control_combo_list_c::remove_item( menu_control_combo_list_item_c * item )
	{
		_list_item_holder->_remove_control( item->get_index() );
	}

	void_c menu_control_combo_list_c::remove_item( sint32_c index )
	{
		_list_item_holder->_remove_control( index );
	}

	void_c menu_control_combo_list_c::remove_and_delete_all_items()
	{
		_list_item_holder->_remove_and_delete_all_controls();
	}

	void_c menu_control_combo_c::_handle_on_selection_changed()
	{
		menu_control_combo_list_item_c * combo_list_item = _combo_list->get_selected_item();
		if ( combo_list_item != nullptr )
		{
			_element_text.set_plain_text( combo_list_item->get_text_plain() );
		}
		else
		{
			_element_text.clear_text();
		}
	}

	void_c menu_control_combo_c::_on_clicked( input_event_c * input_event )
	{
		show_combo_list();
		menu_control_button_c::_on_clicked( input_event );
	}

	menu_control_combo_c::menu_control_combo_c()
		: menu_control_button_c()
		, _combo_list( nullptr )
	{
		_name = string8_c( mode_e_static, "combo" );
		_combo_list = new menu_control_combo_list_c();
		_add_control( _combo_list );
		set_style_map_key( string8_c( mode_e_static, "e_combo" ) );
	}

	void_c menu_control_combo_c::show_combo_list()
	{
		//user_interface_c * user_interface = get_user_interface();
		//assert( user_interface );
		//vector32x2_c popup_size = vector32x2_c( _combo_list->get_local_box().get_width(), _combo_list->get_local_box().get_height() );
		//box32x2_c popup_box = user_interface->find_popup_box( menu_popup_type_e_combo_list, _global_origin, _global_basis, _local_box, popup_size );
		//_combo_list->set_layout_simple( popup_box );
		//_combo_list->show_immediately();
		//_combo_list->give_text_focus();


	}

	void_c menu_control_combo_c::hide_combo_list()
	{
		_combo_list->hide( false );
	}

	menu_control_combo_list_c * menu_control_combo_c::get_combo_list() const
	{
		return _combo_list;
	}

}
