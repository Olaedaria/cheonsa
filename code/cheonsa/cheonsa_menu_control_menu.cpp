#include "cheonsa_menu_control_menu.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	void_c menu_control_menu_list_item_c::_on_mouse_over_gained()
	{
		if ( _sub_menu )
		{
			_sub_menu->show_at( _local_box, menu_popup_type_e_right );
		}
	}

	void_c menu_control_menu_list_item_c::_on_mouse_over_lost()
	{
		if ( _sub_menu )
		{
			_sub_menu->hide( false );
		}
	}

	void_c menu_control_menu_list_item_c::_on_clicked( input_event_c * input_event )
	{
		if ( _sub_menu )
		{
			static_cast< menu_control_menu_list_c * >( _sub_menu )->show_at( _local_box, menu_popup_type_e_right );
		}
		else
		{
			//hide_popup_menu_hierarchy();
			user_interface_c * user_interface = get_user_interface();
			assert( user_interface );
			user_interface->set_text_focused( nullptr ); // this should have the effect of hiding the whole popup menu hierarchy.
		}
		menu_control_list_item_i::_on_clicked( input_event );
	}

	menu_control_menu_list_item_c::menu_control_menu_list_item_c()
		: menu_control_list_item_i()
		, _is_checkable( false )
		, _is_checked( false )
		, _sub_menu( nullptr )
	{
		set_style_map_key( string8_c( mode_e_static, "e_menu_list_item" ) );
	}

	menu_control_menu_list_c * menu_control_menu_list_item_c::get_sub_menu() const
	{
		return _sub_menu;
	}

	void_c menu_control_menu_list_item_c::give_sub_menu( menu_control_menu_list_c * menu )
	{
		assert( _sub_menu == nullptr );
		assert( menu != nullptr );
		assert( menu->get_user_interface() == nullptr );
		assert( menu->get_mother_control() == nullptr );
		_give_control( menu );
		_sub_menu = menu;
	}

	menu_control_menu_list_c * menu_control_menu_list_item_c::take_sub_menu()
	{
		assert( _sub_menu != nullptr );
		return _sub_menu;
		_sub_menu = nullptr;
	}

	//void_c menu_control_popup_menu_item_c::hide_popup_menu_hierarchy()
	//{
	//	menu_control_c * control = _mother_control;
	//	while ( control )
	//	{
	//		menu_control_popup_menu_c * popup_menu = dynamic_cast< menu_control_popup_menu_c * >( control );
	//		if ( popup_menu )
	//		{
	//			popup_menu->hide( false, false );
	//		}
	//		control = control->_mother_control;
	//	}
	//}

	void_c menu_control_menu_list_c::_on_deep_text_focus_lost()
	{
		hide( false );
		menu_control_c::_on_deep_text_focus_lost();
	}

	menu_control_menu_list_c::menu_control_menu_list_c()
		: menu_control_list_i()
	{
		_layer = menu_layer_e_popup;
		_list_item_select_mode = menu_list_item_select_mode_e_none;
		_vertical_size_mode = menu_size_mode_e_fit_content;
		_vertical_size_maximum = 0.0f;
		set_vertical_scroll_visibility_mode( menu_visibility_mode_e_automatic );
		hide_immediately();
		set_style_map_key( string8_c( mode_e_static, "e_menu_list" ) );
	}

	void_c menu_control_menu_list_c::show_at( box32x2_c const & around, menu_popup_type_e order )
	{
		_show_at( around, order );
	}

	sint32_c menu_control_menu_list_c::get_item_count() const
	{
		return _control_list.get_length();
	}

	menu_control_menu_list_item_c * menu_control_menu_list_c::get_item_at_index( sint32_c index )
	{
		assert( index >= 0 && index < _control_list.get_length() );
		menu_control_menu_list_item_c * result = dynamic_cast< menu_control_menu_list_item_c * >( _control_list[ index ] );
		assert( result != nullptr );
		assert( result->get_index() == index );
		return result;
	}

	void_c menu_control_menu_list_c::give_item_at_end( menu_control_menu_list_item_c * item )
	{
		_list_item_holder->_give_control( item );
	}

	void_c menu_control_menu_list_c::give_item_at_index( menu_control_menu_list_item_c * item, sint32_c index )
	{
		_list_item_holder->_give_control( item, index );
	}

	menu_control_menu_list_item_c * menu_control_menu_list_c::take_item( menu_control_menu_list_item_c * item )
	{
		return dynamic_cast< menu_control_menu_list_item_c * >( _list_item_holder->_take_control( item->get_index() ) );
	}

	menu_control_menu_list_item_c * menu_control_menu_list_c::take_item_at_index( sint32_c index )
	{
		return dynamic_cast< menu_control_menu_list_item_c * >( _list_item_holder->_take_control( index ) );
	}

	void_c menu_control_menu_list_c::remove_and_delete_all_items()
	{
		_list_item_holder->_remove_and_delete_all_controls();
	}

}
