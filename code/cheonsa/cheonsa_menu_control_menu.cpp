#include "cheonsa_menu_control_menu.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	menu_control_menu_list_item_separator_c::menu_control_menu_list_item_separator_c()
		: menu_control_list_item_separator_i()
	{
	}

	void_c menu_control_menu_list_item_text_c::_on_is_mouse_overed_changed()
	{
		if ( _is_mouse_overed )
		{
			_element_highlighted_frame.set_is_showed( true );
			//if ( _sub_menu )
			//{
			//	_sub_menu->show_like_sub_menu( this );
			//}
		}
		else
		{
			_element_highlighted_frame.set_is_showed( false );
			//if ( _sub_menu )
			//{
			//	_sub_menu->set_is_showed( false );
			//}
		}
		on_is_mouse_overed_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	void_c menu_control_menu_list_item_text_c::_on_clicked( input_event_c * input_event )
	{
		if ( _sub_menu )
		{
			static_cast< menu_control_menu_list_c * >( _sub_menu )->show_like_sub_menu( this );
		}
		else if ( _can_be_checked )
		{
			_is_checked = !_is_checked;
		}
		else
		{
			user_interface_c * user_interface = get_user_interface_root();
			assert( user_interface );
			user_interface->set_text_focused( nullptr ); // this should have the effect of hiding the whole popup menu hierarchy.
		}
		menu_control_list_item_i::_on_clicked( input_event );
	}

	menu_control_menu_list_item_text_c::menu_control_menu_list_item_text_c()
		: menu_control_list_item_text_i()
		, _can_be_checked( false )
		, _is_checked( false )
		, _sub_menu( nullptr )
	{
		set_style_map_key( string8_c( core_list_mode_e_static, "e_menu_list_item" ) );
	}

	boolean_c menu_control_menu_list_item_text_c::get_can_be_checked() const
	{
		return _can_be_checked;
	}

	void_c menu_control_menu_list_item_text_c::set_can_be_checked( boolean_c value )
	{
		_can_be_checked = value;
		if ( _can_be_checked == false )
		{
			_is_checked = false;
			on_is_checked_changed.invoke( this );
		}
	}

	boolean_c menu_control_menu_list_item_text_c::get_is_checked() const
	{
		return _is_checked;
	}

	void_c menu_control_menu_list_item_text_c::set_is_checked( boolean_c value )
	{
		assert( _can_be_checked );
		if ( _is_checked != value )
		{
			_is_checked = value;
			on_is_checked_changed.invoke( this );
		}
	}

	menu_control_menu_list_c * menu_control_menu_list_item_text_c::get_sub_menu() const
	{
		return _sub_menu;
	}

	void_c menu_control_menu_list_item_text_c::give_sub_menu( menu_control_menu_list_c * menu )
	{
		assert( _sub_menu == nullptr );
		assert( menu != nullptr );
		assert( menu->get_user_interface_root() == nullptr );
		assert( menu->get_mother_control() == nullptr );
		_give_control( menu );
		_sub_menu = menu;
	}

	menu_control_menu_list_c * menu_control_menu_list_item_text_c::take_sub_menu()
	{
		assert( _sub_menu != nullptr );
		return _sub_menu;
		_sub_menu = nullptr;
	}

	void_c menu_control_menu_list_c::_on_is_deep_text_focused_changed()
	{
		if ( _is_deep_text_focused == false )
		{
			set_is_showed( false );
		}
		on_is_deep_text_focused_changed.invoke( menu_event_information_c( this, nullptr ) );
	}

	menu_control_menu_list_c::menu_control_menu_list_c()
		: menu_control_list_i()
	{
		_layer = menu_layer_e_popup;
		_selected_item_limit = 0;
		_vertical_size_mode = menu_size_mode_e_fit_content;
		_vertical_size_maximum = 0.0f;
		set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e_automatic );
		set_is_showed_immediately( false );
		set_style_map_key( string8_c( core_list_mode_e_static, "e_menu_list" ) );
	}

	void_c menu_control_menu_list_c::show_like_context_menu( vector32x2_c const & screen_space_point_to_spawn_pop_up_around )
	{
		user_interface_c * user_interface = get_user_interface_root();
		assert( user_interface );
		box32x2_c new_local_box = user_interface->find_context_menu_pop_up_box( screen_space_point_to_spawn_pop_up_around, _local_box.get_size() );
		set_layout_simple( new_local_box );
		set_is_showed_immediately( true );
		bring_to_front();
		give_text_focus();
	}

	void_c menu_control_menu_list_c::show_like_sub_menu( menu_control_c * menu_item_to_spawn_pop_up_around )
	{
		user_interface_c * user_interface = get_user_interface_root();
		assert( user_interface );
		box32x2_c new_local_box = user_interface->find_sub_menu_pop_up_box( menu_item_to_spawn_pop_up_around, _local_box.get_size(), true );
		set_layout_simple( new_local_box );
		set_is_showed_immediately( true );
		bring_to_front();
		give_text_focus();
	}

	void_c menu_control_menu_list_c::show_like_combo_list( menu_control_c * combo_to_spawn_pop_up_around )
	{
		user_interface_c * user_interface = get_user_interface_root();
		assert( user_interface );
		box32x2_c new_local_box = user_interface->find_combo_list_pop_up_box( combo_to_spawn_pop_up_around, _local_box.get_size(), true );
		set_layout_simple( new_local_box );
		set_is_showed_immediately( true );
		bring_to_front();
		give_text_focus();
	}

	sint32_c menu_control_menu_list_c::get_item_count() const
	{
		return _get_item_count();
	}

	menu_control_list_item_i * menu_control_menu_list_c::get_item( sint32_c item_index)
	{
		return _get_item( item_index );
	}

	sint32_c menu_control_menu_list_c::give_item( menu_control_list_item_i * item, sint32_c index )
	{
		assert( dynamic_cast< menu_control_menu_list_item_separator_c * >( item ) != nullptr || dynamic_cast< menu_control_menu_list_item_text_c * >( item ) != nullptr );
		return _give_item( item, index );
	}

	menu_control_list_item_i * menu_control_menu_list_c::take_item( sint32_c item_index )
	{
		return _take_item( item_index );
	}

	void_c menu_control_menu_list_c::remove_and_delete_all_items()
	{
		_remove_and_delete_all_items();
	}

}
