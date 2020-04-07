#include "cheonsa_menu_control_file_picker.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	menu_control_file_picker_item_c::menu_control_file_picker_item_c( ops::file_system_file_information_c const & file_information )
		: menu_control_collection_item_i()
		, _path( file_information.path )
		, _creation_time( file_information.creation_time )
		, _last_write_time( file_information.last_write_time )
		, _is_folder( file_information.is_folder )
	{
		_group = _is_folder ? 0 : 1;
	}

	resource_file_texture_c * menu_control_file_picker_item_c::get_icon_texture() const
	{
		if ( _is_folder )
		{
			return engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( mode_e_static, L"menus/icon_folder.png" ) );
		}
		else
		{
			return engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( mode_e_static, L"menus/icon_file.png" ) );
		}
		return nullptr;
	}

	boolean_c menu_control_file_picker_item_c::get_value( string8_c const & key, string16_c & display_value, sint64_c & absolute_value ) const
	{
		if ( key == "name" )
		{
			display_value = ops::path_get_file_name( _path );
			absolute_value = 0;
			return true;
		}
		display_value = string16_c();
		absolute_value = 0;
		return false;
	}

	void_c menu_control_file_picker_c::_try_to_invoke_file_for_load()
	{
		if ( _mode == mode_e_load )
		{
			if ( can_load.get_subscriber_count() == 0 || can_load.invoke_with_return_value( this ) )
			{
				_result = result_e_okay;
				on_submitted.invoke( this );
			}
		}
		else
		{
			string16_c file_path = get_file_path();
			if ( ops::file_system_does_file_exist( file_path ) )
			{
				// ask if user wants to overwrite existing file.
			}
		}
	}

	void_c menu_control_file_picker_c::_handle_button_on_clicked( menu_event_information_c event_information )
	{
		if ( _muted )
		{
			return;
		}

		if ( event_information.control == _back_button )
		{
			go_back();
		}
		else if ( event_information.control == _forward_button )
		{
			go_forward();
		}
		else if ( event_information.control == _up_button )
		{
			go_up();
		}
		else if ( event_information.control == _okay_button )
		{
			_result = result_e_okay;
			on_submitted.invoke( this );
		}
		else if ( event_information.control == _cancel_button )
		{
			_result = result_e_cancel;
			on_submitted.invoke( this );
		}
	}

	void_c menu_control_file_picker_c::_handle_text_on_value_changed( menu_control_text_c * control )
	{
		if ( _muted )
		{
			return;
		}

		if ( control == _folder_path_text )
		{
			go_to_folder_path( _folder_path_text->get_plain_text_value() );
		}
		else if ( control == _file_name_text )
		{
		}
	}

	void_c menu_control_file_picker_c::_handle_file_collection_on_selected_items_changed( menu_control_collection_c * collection )
	{
		if ( _muted )
		{
			return;
		}

		if ( collection == _files_collection )
		{
			_file_name_text->clear_text_value();
			if ( _files_collection->get_selected_items().get_length() == 1 )
			{
				menu_control_file_picker_item_c * item = static_cast< menu_control_file_picker_item_c * >( _files_collection->get_selected_items()[ 0 ] );
				if ( item->_is_folder == false )
				{
					_file_name_text->set_plain_text_value( ops::path_get_file_name( item->_path ) );
				}
				if ( _mode == mode_e_load )
				{
					if ( can_load.get_subscriber_count() > 0 )
					{
						_okay_button->set_is_enabled( can_load.invoke_with_return_value( this ) );
					}
					else
					{
						_okay_button->set_is_enabled( true );
					}
				}
				else
				{
					_okay_button->set_is_enabled( true );
				}
			}
		}
	}

	void_c menu_control_file_picker_c::_handle_file_collection_on_selected_items_invoked( menu_control_collection_c * collection )
	{
		if ( _muted )
		{
			return;
		}

		if ( collection == _files_collection )
		{
			if ( _files_collection->get_selected_items().get_length() == 1 )
			{
				string16_c display_value;
				sint64_c absolute_value;
				menu_control_file_picker_item_c * item = static_cast< menu_control_file_picker_item_c * >( _files_collection->get_selected_items()[ 0 ] );
				if ( item->get_value( string8_c( mode_e_static, "name" ), display_value, absolute_value ) )
				{
#if defined( cheonsa_platform_windows )
					if ( item->_is_folder == true )
					{
						if ( ops::file_system_does_folder_exist( item->_path ) )
						{
							go_to_folder_path( item->_path );
						}
					}
					else
					{
						if ( ops::file_system_does_file_exist( item->_path ) )
						{
							_try_to_invoke_file_for_load();
						}
					}
#else
#error
#endif
				}
			}
		}
	}

	void_c menu_control_file_picker_c::_short_cut_list_remove_all()
	{
		_short_cut_list->remove_and_delete_all_items();
		_short_cut_path_list.remove_all();
	}

	void_c menu_control_file_picker_c::_short_cut_list_add( string16_c const & absolute_path, string16_c const & friendly_name )
	{
		menu_control_list_item_text_c * list_item = new menu_control_list_item_text_c();
		list_item->set_plain_text_value( friendly_name );
		_short_cut_list->give_item( list_item );
		_short_cut_path_list.insert_at_end( absolute_path );
	}

	menu_control_file_picker_c::menu_control_file_picker_c()
		: menu_control_c()
		, _frame()
		, _folder_path_text( nullptr )
		, _file_name_text( nullptr )
		, _back_button( nullptr )
		, _forward_button( nullptr )
		, _up_button( nullptr )
		, _files_collection( nullptr )
		, _history_stack()
		, _history_index( 0 )
		, _folder_path()
		, _short_cut_list()
		, _short_cut_path_list()
		, _okay_button( nullptr )
		, _cancel_button( nullptr )
		, _result( result_e_cancel )
		, _mode( mode_e_load )
		, _muted( false )
	{
		_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_frame.set_style_key( string8_c( mode_e_static, "e_field_frame" ) );
		_add_element( &_frame );

		_folder_path_text = new menu_control_text_c();
		_folder_path_text->set_name( string8_c( mode_e_static, "folder_path_text" ) );
		_folder_path_text->on_value_changed.subscribe( this, &menu_control_file_picker_c::_handle_text_on_value_changed );
		_give_control( _folder_path_text );

		_file_name_text = new menu_control_text_c();
		_file_name_text->set_name( string8_c( mode_e_static, "file_name_text" ) );
		_file_name_text->on_value_changed.subscribe( this, &menu_control_file_picker_c::_handle_text_on_value_changed );
		_give_control( _file_name_text );

		_back_button = new menu_control_button_c();
		_back_button->set_name( string8_c( mode_e_static, "back_button" ) );
		_back_button->set_plain_text_value( string16_c( mode_e_static, L"<" ) );
		_back_button->on_clicked.subscribe( this, &menu_control_file_picker_c::_handle_button_on_clicked );
		_give_control( _back_button );

		_forward_button = new menu_control_button_c();
		_forward_button->set_name( string8_c( mode_e_static, "forward_button" ) );
		_forward_button->set_plain_text_value( string16_c( mode_e_static, L">" ) );
		_forward_button->on_clicked.subscribe( this, &menu_control_file_picker_c::_handle_button_on_clicked );
		_give_control( _forward_button );

		_up_button = new menu_control_button_c();
		_up_button->set_name( string8_c( mode_e_static, "up_button" ) );
		_up_button->set_plain_text_value( string16_c( mode_e_static, L"˄" ) );
		_up_button->on_clicked.subscribe( this, &menu_control_file_picker_c::_handle_button_on_clicked );
		_give_control( _up_button );

		_short_cut_list = new menu_control_list_c();
		_short_cut_list->set_name( string8_c( mode_e_static, "short_cut_list" ) );
		_give_control( _short_cut_list );

		_files_collection = new menu_control_collection_c();
		_files_collection->set_name( string8_c( mode_e_static, "files_collection" ) );
		_files_collection->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 10.0f, 0.0f, 10.0f ) );
		_files_collection->add_column( string8_c( mode_e_static, "name" ), string16_c( mode_e_static, L"name" ), 500, menu_control_collection_c::sort_by_e_display_value, false );
		_files_collection->set_sort( string8_c( mode_e_static, "name" ), menu_control_collection_c::sort_order_e_ascending );
		_files_collection->on_selected_items_changed.subscribe( this, &menu_control_file_picker_c::_handle_file_collection_on_selected_items_changed );
		_files_collection->on_selected_items_invoked.subscribe( this, &menu_control_file_picker_c::_handle_file_collection_on_selected_items_invoked );
		_give_control( _files_collection );

		_okay_button = new menu_control_button_c();
		_okay_button->set_name( string8_c( mode_e_static, "okay_button" ) );
		_okay_button->set_plain_text_value( string16_c( mode_e_static, L"okay" ) );
		_okay_button->on_clicked.subscribe( this, &menu_control_file_picker_c::_handle_button_on_clicked );
		_give_control( _okay_button );

		_okay_button = new menu_control_button_c();
		_okay_button->set_name( string8_c( mode_e_static, "cancel_button" ) );
		_okay_button->set_plain_text_value( string16_c( mode_e_static, L"cancel" ) );
		_okay_button->on_clicked.subscribe( this, &menu_control_file_picker_c::_handle_button_on_clicked );
		_give_control( _okay_button );
	}

	menu_control_file_picker_c::mode_e menu_control_file_picker_c::get_mode() const
	{
		return _mode;
	}

	void_c menu_control_file_picker_c::set_mode( mode_e value )
	{
		_mode = value;
	}

	void_c menu_control_file_picker_c::clear_history()
	{
		_history_stack.remove_all();
		_history_index = 0;
		_back_button->set_is_enabled( false );
		_forward_button->set_is_enabled( false );
	}

	boolean_c menu_control_file_picker_c::go_to_folder_path( string16_c const & folder_path )
	{
		assert( _muted == false );
		if ( folder_path.get_length() == 0 )
		{
			return false;
		}
#if defined( cheonsa_platform_windows )
		string16_c folder_path_2 = ops::file_system_fix_path( folder_path );
		if ( folder_path_2.get_length() == 0 )
		{
			return false;
		}
		if ( folder_path_2.character_list[ folder_path_2.get_length() - 1 ] != '\\' ) // in case the user didn't include the trailing slash, let's include it for them.
		{
			folder_path_2 += '\\';
		}
		if ( ops::file_system_is_path_formatted_for_windows( folder_path_2, ops::file_system_path_type_e_dont_care ) )
		{
#endif
			if ( ops::file_system_does_folder_exist( folder_path_2 ) )
			{
				sint32_c how_many_to_remove = _history_stack.get_length() - ( _history_index + 1 );
				if ( how_many_to_remove > 0 )
				{
					_history_stack.remove_at_end( how_many_to_remove );
				}
				_history_stack.insert_at_end( folder_path_2 );
				_history_index = _history_stack.get_length() - 1;
				_muted = true;
				_back_button->set_is_enabled( _history_index > 0 );
				_forward_button->set_is_enabled( false );
				_folder_path = folder_path_2;
				_folder_path_text->set_plain_text_value( folder_path_2 );
				_file_name_text->clear_text_value();
				_muted = false;
				refresh_files_collection();
				return true;
			}
#if defined( cheonsa_platform_windows )
		}
#endif
		return false;
	}

	boolean_c menu_control_file_picker_c::go_back()
	{
		assert( _muted == false );
		if ( _history_index > 0 )
		{
			_history_index--;
			_muted = true;
			_back_button->set_is_enabled( _history_index > 0 );
			_forward_button->set_is_enabled( true );
			_folder_path = _history_stack[ _history_index ];
			_folder_path_text->set_plain_text_value( _folder_path );
			_file_name_text->clear_text_value();
			_muted = false;
			refresh_files_collection();
			return true;
		}
		return false;
	}

	boolean_c menu_control_file_picker_c::go_forward()
	{
		assert( _muted == false );
		if ( _history_index + 1 < _history_stack.get_length() )
		{
			_history_index++;
			_muted = true;
			_back_button->set_is_enabled( true );
			_forward_button->set_is_enabled( _history_index < _history_stack.get_length() - 1 );
			_folder_path = _history_stack[ _history_index ];
			_folder_path_text->set_plain_text_value( _folder_path );
			_file_name_text->clear_text_value();
			_muted = false;
			refresh_files_collection();
			return true;
		}
		return false;
	}

	boolean_c menu_control_file_picker_c::go_up()
	{
		string16_c path = ops::path_get_mother( _history_stack[ _history_index ] );
		if ( path.get_length() > 0 )
		{
			return go_to_folder_path( path );
		}
		return false;
	}

	string16_c menu_control_file_picker_c::get_folder_path() const
	{
		return _folder_path;
	}

	string16_c menu_control_file_picker_c::get_file_name() const
	{
		return _file_name_text->get_plain_text_value();
	}

	void_c menu_control_file_picker_c::set_file_name( string16_c const & value )
	{
		_file_name_text->set_plain_text_value( value );
	}

	string16_c menu_control_file_picker_c::menu_control_file_picker_c::get_file_path() const
	{
		string16_c result;
		result = _folder_path;
		result += _file_name_text->get_plain_text_value();
		return result;
	}

	void_c menu_control_file_picker_c::refresh_short_cut_list()
	{
		_short_cut_list->remove_and_delete_all_items();
		_short_cut_path_list.remove_all();

		string16_c folder_path;
		if ( ops::file_system_get_quick_access_folder_path( ops::file_system_quick_access_folder_e_desktop, folder_path ) )
		{
			_short_cut_path_list.insert_at_end( folder_path );
			menu_control_list_item_text_c * list_item = new menu_control_list_item_text_c();
			list_item->set_plain_text_value( string16_c( mode_e_static_volatile, L"Desktop" ) );
			_short_cut_list->give_item( list_item );
		}

		if ( ops::file_system_get_quick_access_folder_path( ops::file_system_quick_access_folder_e_documents, folder_path ) )
		{
			_short_cut_path_list.insert_at_end( folder_path );
			menu_control_list_item_text_c * list_item = new menu_control_list_item_text_c();
			list_item->set_plain_text_value( string16_c( mode_e_static_volatile, L"Documents" ) );
			_short_cut_list->give_item( list_item );
		}

		core_list_c< ops::file_system_file_information_c > drive_path_list;
		ops::file_system_get_drive_path_list( drive_path_list );
		for ( sint32_c i = 0; i < drive_path_list.get_length(); i++ )
		{
			_short_cut_path_list.insert_at_end( drive_path_list[ i ].path );
			menu_control_list_item_text_c * list_item = new menu_control_list_item_text_c();
			string16_c label = drive_path_list[ i ].path;
			if ( drive_path_list[ i ].label.get_length() > 0 )
			{
				label += " (";
				label += drive_path_list[ i ].label;
				label += ")";
			}
			list_item->set_plain_text_value( label );
			_short_cut_list->give_item( list_item );
		}
	}

	void_c menu_control_file_picker_c::refresh_files_collection()
	{
		_files_collection->remove_and_delete_all_items();
		string16_c folder_path = get_folder_path();
		core_list_c< ops::file_system_file_information_c > file_information_list;
		ops::file_system_get_file_information_list( file_information_list, folder_path, true, true, nullptr );
		for ( sint32_c i = 0; i < file_information_list.get_length(); i++ )
		{
			ops::file_system_file_information_c & file_information = file_information_list[ i ];
			menu_control_collection_item_i * collection_item = new menu_control_file_picker_item_c( file_information );
			_files_collection->add_item( collection_item );
		}
		_files_collection->refresh();
	}

	menu_control_file_picker_c::result_e menu_control_file_picker_c::get_result() const
	{
		return _result;
	}

}
