#include "cheonsa_menu_control_menu.h"
#include "cheonsa_user_interface.h"

namespace cheonsa
{

	menu_control_menu_list_item_separator_c::menu_control_menu_list_item_separator_c( string8_c const & name )
		: menu_control_list_item_separator_i( name )
	{
	}

	menu_control_menu_list_item_separator_c::~menu_control_menu_list_item_separator_c()
	{
	}

	menu_control_menu_list_item_separator_c * menu_control_menu_list_item_separator_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_menu_list_item_separator_c( name );
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
		if ( _is_enabled )
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
				assert( _user_interface );
				_user_interface->set_text_focused( nullptr ); // this should have the effect of hiding the whole popup menu hierarchy.
			}
			menu_control_list_item_i::_on_clicked( input_event );
		}
	}

	menu_control_menu_list_item_text_c::menu_control_menu_list_item_text_c( string8_c const & name )
		: menu_control_list_item_text_i( name )
		, _can_be_checked( false )
		, _is_checked( false )
		, _sub_menu( nullptr )
	{
		set_style_map_key( string8_c( core_list_mode_e_static, "e_menu_list_item" ) );
	}

	menu_control_menu_list_item_text_c::~menu_control_menu_list_item_text_c()
	{
	}

	menu_control_menu_list_item_text_c * menu_control_menu_list_item_text_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_menu_list_item_text_c( name );
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
		assert( menu->get_user_interface() == nullptr );
		assert( menu->get_mother_control() == nullptr );
		add_daughter_control( menu );
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

	menu_control_menu_list_c::menu_control_menu_list_c( string8_c const & name )
		: menu_control_list_i( name )
	{
		_layer = menu_layer_e_popup;
		_selected_item_limit = 0;
		_vertical_size_mode = menu_size_mode_e_fit_content;
		_vertical_size_maximum = 0.0f;
		set_is_showed_immediately( false );
		set_style_map_key( string8_c( core_list_mode_e_static, "e_menu_list" ) );
	}

	menu_control_menu_list_c::~menu_control_menu_list_c()
	{
	}

	menu_control_menu_list_c * menu_control_menu_list_c::make_new_instance( string8_c const & name )
	{
		return new menu_control_menu_list_c( name );
	}

	void_c menu_control_menu_list_c::show_like_context_menu( vector32x2_c const & screen_space_point_to_spawn_pop_up_around )
	{
		assert( _user_interface );
		box32x2_c new_local_box = _user_interface->find_context_menu_pop_up_box( screen_space_point_to_spawn_pop_up_around, _local_box.get_size() );
		set_layout_simple( new_local_box );
		set_is_showed_immediately( true );
		bring_to_front();
		give_text_focus();
	}

	void_c menu_control_menu_list_c::show_like_sub_menu( menu_control_c * menu_item_to_spawn_pop_up_around )
	{
		assert( _user_interface );
		box32x2_c new_local_box = _user_interface->find_sub_menu_pop_up_box( menu_item_to_spawn_pop_up_around, _local_box.get_size(), true );
		set_layout_simple( new_local_box );
		set_is_showed_immediately( true );
		bring_to_front();
		give_text_focus();
	}

	void_c menu_control_menu_list_c::show_like_combo_list( menu_control_c * combo_to_spawn_pop_up_around )
	{
		assert( _user_interface );
		box32x2_c new_local_box = _user_interface->find_combo_list_pop_up_box( combo_to_spawn_pop_up_around, _local_box.get_size(), true );
		set_layout_simple( new_local_box );
		set_is_showed_immediately( true );
		bring_to_front();
		give_text_focus();
	}

	sint32_c menu_control_menu_list_c::get_item_count() const
	{
		return _client->get_daughter_control_list().get_length();
	}

	menu_control_list_item_i const * menu_control_menu_list_c::get_item_at_index( sint32_c item_index ) const
	{
		return static_cast< menu_control_list_item_i const * >( _client->get_daughter_control_list().get_at_index( item_index )->get_value() );
	}

	menu_control_list_item_i * menu_control_menu_list_c::get_item_at_index( sint32_c item_index )
	{
		return static_cast< menu_control_list_item_i * >( _client->get_daughter_control_list().get_at_index( item_index )->get_value() );
	}

	void_c menu_control_menu_list_c::add_item( menu_control_list_item_i * item, sint32_c index )
	{
		assert( dynamic_cast< menu_control_menu_list_item_separator_c * >( item ) != nullptr || dynamic_cast< menu_control_menu_list_item_text_c * >( item ) != nullptr );
		_add_item( item, index );
	}

	void_c menu_control_menu_list_c::remove_item( menu_control_list_item_i * item )
	{
		_remove_item( item );
	}

	void_c menu_control_menu_list_c::remove_all_items()
	{
		_remove_all_items();
	}

}
