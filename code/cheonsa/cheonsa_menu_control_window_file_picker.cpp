#include "cheonsa_menu_control_window_file_picker.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	vector32x2_c menu_control_window_file_picker_c::default_size = vector32x2_c( 1000, 600 );

	menu_control_window_file_picker_c::item_c::item_c( ops::file_system_file_information_c const & file_information )
		: item_i()
		, _path( file_information.path )
		, _creation_time( file_information.creation_time )
		, _last_write_time( file_information.last_write_time )
		, _is_folder( file_information.is_folder )
	{
		_group = _is_folder ? 0 : 1;
	}

	resource_file_texture_c * menu_control_window_file_picker_c::item_c::get_icon_texture() const
	{
		if ( _is_folder )
		{
			return engine.get_resource_manager()->load_texture( string16_c( L"menus/icon_folder.png", core_list_mode_e_static ), true, true );
		}
		else
		{
			return engine.get_resource_manager()->load_texture( string16_c( L"menus/icon_file.png", core_list_mode_e_static ), true, true );
		}
		return nullptr;
	}

	boolean_c menu_control_window_file_picker_c::item_c::get_value( string8_c const & key, string16_c & display_value, sint64_c & absolute_value ) const
	{
		if ( key == "name" )
		{
			display_value = ops::path_get_file_or_folder_name( _path );
			absolute_value = 0;
			return true;
		}
		display_value = string16_c();
		absolute_value = 0;
		return false;
	}

	void_c menu_control_window_file_picker_c::_refresh_okay_button_state()
	{
		if ( _mode == mode_e_load )
		{
			if ( _file_name_text->get_plain_text_value().is_set() )
			{
				_okay_button->set_is_enabled( ops::file_system_does_file_exist( get_file_path_absolute() ) );
			}
			else
			{
				_okay_button->set_is_enabled( false );
			}
		}
		else if ( _mode == mode_e_save )
		{
			_okay_button->set_is_enabled( _file_name_text->get_plain_text_value().is_set() );
		}
	}

	void_c menu_control_window_file_picker_c::_try_to_okay()
	{
		assert( _mother_user_interface );
		string16_c file_path_absolute = get_file_path_absolute();
		if ( _mode == mode_e_load )
		{
			if ( ops::file_system_does_file_exist( file_path_absolute ) )
			{
				if ( can_load.get_subscriber_count() == 0 || can_load.invoke_with_return_value( this ) )
				{
					_dialog_result = menu_dialog_result_e_okay;
					on_dialog_result.invoke( this );
				}
				else
				{
					_message_dialog->set_dialog_mode( menu_dialog_mode_e_okay );
					_message_dialog->set_title_plain_text_value( string8_c( "unrecognized file type.", core_list_mode_e_static ) );
					_message_dialog->set_message_plain_text_value( string8_c( "this program does not recognize that type of file.", core_list_mode_e_static ) );
					_message_dialog->show_dialog( _mother_user_interface->open_modal_screen() );
					_message_dialog->center();
				}
			}
		}
		else if ( _mode == mode_e_save )
		{
			if ( ops::file_system_does_file_exist( file_path_absolute ) )
			{
				_message_dialog->set_dialog_mode( menu_dialog_mode_e_no_yes );
				_message_dialog->set_title_plain_text_value( string8_c( "that file already exists.", core_list_mode_e_static ) );
				_message_dialog->set_message_plain_text_value( string8_c( "do you want to over write the existing file?", core_list_mode_e_static ) );
				_message_dialog->show_dialog( _mother_user_interface->open_modal_screen() );
				_message_dialog->center();
				_message_dialog_is_asking_for_over_write = true;
			}
			else
			{
				_dialog_result = menu_dialog_result_e_okay;
				on_dialog_result.invoke( this );
			}
		}
	}

	void_c menu_control_window_file_picker_c::_short_cut_list_remove_all()
	{
		_short_cut_list->remove_all_items();
		_short_cut_path_list.remove_all();
	}

	void_c menu_control_window_file_picker_c::_handle_short_cut_on_clicked( menu_event_information_c event_information )
	{
		go_to_folder_path( _short_cut_path_list[ event_information.control->get_index() ] );
	}

	void_c menu_control_window_file_picker_c::_handle_on_dialog_result( menu_control_window_c * window_dialog )
	{
		_message_dialog->hide_dialog();
		if ( _message_dialog_is_asking_for_over_write )
		{
			_message_dialog_is_asking_for_over_write = false;
			if ( _message_dialog->get_dialog_result() == menu_dialog_result_e_yes )
			{
				_dialog_result = menu_dialog_result_e_okay;
				on_dialog_result.invoke( this );
			}
		}
	}

	void_c menu_control_window_file_picker_c::_handle_on_clicked( menu_event_information_c event_information )
	{
		if ( _is_muted )
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
			_try_to_okay();
		}
		else if ( event_information.control == _cancel_button )
		{
			_dialog_result = menu_dialog_result_e_cancel;
			on_dialog_result.invoke( this );
		}
	}

	void_c menu_control_window_file_picker_c::_handle_on_plain_text_value_changed( menu_control_text_c * control )
	{
		if ( _is_muted )
		{
			return;
		}

		if ( control == _folder_path_text )
		{
			go_to_folder_path( _folder_path_text->get_plain_text_value() );
		}
		else if ( control == _file_name_text )
		{
			_refresh_okay_button_state();
		}
	}

	void_c menu_control_window_file_picker_c::_handle_file_collection_on_selected_items_changed( menu_control_collection_c * collection )
	{
		if ( _is_muted )
		{
			return;
		}

		assert( collection == _files_collection );

		if ( _file_name_text->get_is_enabled() )
		{
			_file_name_text->clear_text_value();
		}
		if ( _files_collection->get_selected_items().get_length() == 1 )
		{
			item_c * item = static_cast< item_c * >( _files_collection->get_selected_items()[ 0 ] );
			if ( _mode == mode_e_load )
			{
				if ( item->_is_folder == false )
				{
					if ( _file_name_text->get_is_enabled() )
					{
						_file_name_text->set_plain_text_value( ops::path_get_file_or_folder_name( item->_path ) );
					}
				}
				if ( can_load.get_subscriber_count() > 0 )
				{
					_okay_button->set_is_enabled( can_load.invoke_with_return_value( this ) );
				}
				else
				{
					_okay_button->set_is_enabled( ops::file_system_does_file_exist( get_file_path_absolute() ) );
				}
			}
			else if ( _mode == mode_e_save )
			{
				_okay_button->set_is_enabled( true );
			}
		}
	}

	void_c menu_control_window_file_picker_c::_handle_file_collection_on_selected_items_invoked( menu_control_collection_c * collection )
	{
		if ( _is_muted )
		{
			return;
		}

		if ( collection == _files_collection )
		{
			if ( _files_collection->get_selected_items().get_length() == 1 )
			{
				string16_c display_value;
				sint64_c absolute_value;
				item_c * item = static_cast< item_c * >( _files_collection->get_selected_items()[ 0 ] );
				if ( item->get_value( string8_c( "name", core_list_mode_e_static ), display_value, absolute_value ) )
				{
					if ( item->_is_folder == true )
					{
						if ( ops::file_system_does_folder_exist( item->_path ) )
						{
							go_to_folder_path( item->_path );
						}
					}
					else
					{
						_try_to_okay();
					}
				}
			}
		}
	}

	void_c menu_control_window_file_picker_c::_on_input( input_event_c * input_event )
	{
		menu_control_window_c::_on_input( input_event );
		if ( !input_event->get_was_handled() )
		{
			if ( input_event->get_type() == input_event_c::type_e_keyboard_key_pressed )
			{
				if ( input_event->get_keyboard_key() == input_keyboard_key_e_escape )
				{
					_dialog_result = menu_dialog_result_e_cancel;
					on_dialog_result.invoke( this );
				}
			}
		}
	}

	menu_control_window_file_picker_c::menu_control_window_file_picker_c()
		: menu_control_window_c()
		, _is_muted( false )
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
		, _mode( mode_e_load )
		, _message_dialog_is_asking_for_over_write( false )
		, _message_dialog( nullptr )
	{
		set_user_can_resize( true );
		set_size( vector32x2_c( default_size.a, default_size.b ) );
		set_title_plain_text_value( string8_c( "file picker", core_list_mode_e_static ) );

		_folder_path_text = new menu_control_text_c();
		_folder_path_text->set_name( string8_c( "folder_path_text", core_list_mode_e_static ) );
		_folder_path_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_file_picker_c::_handle_on_plain_text_value_changed );
		_folder_path_text->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right, box32x2_c( 152, 8, 8, 30 ) );
		add_daughter_control_to_client( _folder_path_text );

		_file_name_text = new menu_control_text_c();
		_file_name_text->set_name( string8_c( "file_name_text", core_list_mode_e_static ) );
		_file_name_text->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 152, 30, 8, 46 ) );
		_file_name_text->on_plain_text_value_changed.subscribe( this, &menu_control_window_file_picker_c::_handle_on_plain_text_value_changed );
		add_daughter_control_to_client( _file_name_text );

		_back_button = new menu_control_button_c();
		_back_button->set_name( string8_c( "back_button", core_list_mode_e_static ) );
		_back_button->set_plain_text_value( string16_c( L"\uE3F6", core_list_mode_e_static ) ); // the code point references a custom (not standard) glyph in the engine's built-in font.
		_back_button->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( 8, 8, 40, 30 ) );
		_back_button->on_clicked.subscribe( this, &menu_control_window_file_picker_c::_handle_on_clicked );
		add_daughter_control_to_client( _back_button );

		_forward_button = new menu_control_button_c();
		_forward_button->set_name( string8_c( "forward_button", core_list_mode_e_static ) );
		_forward_button->set_plain_text_value( string16_c( L"\uE3F7", core_list_mode_e_static ) ); // the code point references a custom (not standard) glyph in the engine's built-in font.
		_forward_button->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( 56, 8, 40, 30 ) );
		_forward_button->on_clicked.subscribe( this, &menu_control_window_file_picker_c::_handle_on_clicked );
		add_daughter_control_to_client( _forward_button );

		_up_button = new menu_control_button_c();
		_up_button->set_name( string8_c( "up_button", core_list_mode_e_static ) );
		_up_button->set_plain_text_value( string16_c( L"\uE3F9", core_list_mode_e_static ) ); // the code point references a custom (not standard) glyph in the engine's built-in font.
		_up_button->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top, box32x2_c( 104, 8, 40, 30 ) );
		_up_button->on_clicked.subscribe( this, &menu_control_window_file_picker_c::_handle_on_clicked );
		add_daughter_control_to_client( _up_button );

		_files_collection = new menu_control_collection_c();
		_files_collection->set_name( string8_c( "files_collection", core_list_mode_e_static ) );
		_files_collection->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 152, 46, 8, 84 ) );
		_files_collection->add_column( string8_c( "name", core_list_mode_e_static ), string16_c( L"name", core_list_mode_e_static ), 500, menu_control_collection_c::sort_by_e_display_value, false );
		_files_collection->set_sort( string8_c( "name", core_list_mode_e_static ), menu_control_collection_c::sort_order_e_ascending );
		_files_collection->on_selected_items_changed.subscribe( this, &menu_control_window_file_picker_c::_handle_file_collection_on_selected_items_changed );
		_files_collection->on_selected_items_invoked.subscribe( this, &menu_control_window_file_picker_c::_handle_file_collection_on_selected_items_invoked );
		add_daughter_control_to_client( _files_collection );

		_short_cut_list = new menu_control_list_c();
		_short_cut_list->set_name( string8_c( "short_cut_list", core_list_mode_e_static ) );
		_short_cut_list->set_selected_item_limit( 1 );
		_short_cut_list->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_bottom, box32x2_c( 8, 46, 136, 46 ) );
		add_daughter_control_to_client( _short_cut_list );

		_cancel_button = new menu_control_button_c();
		_cancel_button->set_name( string8_c( "cancel_button", core_list_mode_e_static ) );
		_cancel_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 116, 8 ) );
		_cancel_button->set_plain_text_value( string16_c( L"cancel", core_list_mode_e_static ) );
		_cancel_button->on_clicked.subscribe( this, &menu_control_window_file_picker_c::_handle_on_clicked );
		add_daughter_control_to_client( _cancel_button );

		_okay_button = new menu_control_button_c();
		_okay_button->set_name( string8_c( "okay_button", core_list_mode_e_static ) );
		_okay_button->set_layout_box_anchor( menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 100, 30, 8, 8 ) );
		_okay_button->set_plain_text_value( string16_c( L"okay", core_list_mode_e_static ) );
		_okay_button->on_clicked.subscribe( this, &menu_control_window_file_picker_c::_handle_on_clicked );
		add_daughter_control_to_client( _okay_button );

		_message_dialog = new menu_control_window_message_c();
		_message_dialog->set_name( string8_c( "message_dialog", core_list_mode_e_static ) );
		_message_dialog->add_reference();
		_message_dialog->set_is_showed_immediately( false );
		_message_dialog->on_dialog_result.subscribe( this, &menu_control_window_file_picker_c::_handle_on_dialog_result );
		add_supplemental_daughter_control( _message_dialog );
	}

	menu_control_window_file_picker_c::~menu_control_window_file_picker_c()
	{
		assert( _mother_user_interface == nullptr );

		_message_dialog->remove_reference();
		_message_dialog = nullptr;
	}

	menu_control_window_file_picker_c::mode_e menu_control_window_file_picker_c::get_mode() const
	{
		return _mode;
	}

	void_c menu_control_window_file_picker_c::set_mode( mode_e value )
	{
		assert( _is_muted == false );
		_mode = value;
		_is_muted = true;
		_file_name_text->clear_text_value();
		_okay_button->set_is_enabled( false );
		_is_muted = false;
	}

	void_c menu_control_window_file_picker_c::clear_history()
	{
		_history_stack.remove_all();
		_history_index = 0;
		_back_button->set_is_enabled( false );
		_forward_button->set_is_enabled( false );
	}

	boolean_c menu_control_window_file_picker_c::go_to_folder_path( string16_c const & folder_path )
	{
		assert( _is_muted == false );
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
		if ( ops::file_system_is_path_formatted_for_windows( folder_path_2, ops::file_system_path_type_e_folder ) )
		{
#endif
			if ( ( _history_stack.get_length() == 0 || _folder_path != folder_path_2 ) && ops::file_system_does_folder_exist( folder_path_2 ) )
			{
				sint32_c how_many_to_remove = _history_stack.get_length() - ( _history_index + 1 );
				if ( how_many_to_remove > 0 )
				{
					_history_stack.remove( -1, how_many_to_remove );
				}
				_history_stack.insert( -1, folder_path_2 );
				_history_index = _history_stack.get_length() - 1;
				_is_muted = true;
				_back_button->set_is_enabled( _history_index > 0 );
				_forward_button->set_is_enabled( false );
				_folder_path = folder_path_2;
				_folder_path_text->set_plain_text_value( folder_path_2 );
				_up_button->set_is_enabled( ops::path_get_folder_path( _folder_path ).get_length() > 0 );
				_is_muted = false;
				refresh_files_collection();
			}
			_refresh_okay_button_state();
			return true;
#if defined( cheonsa_platform_windows )
		}
#endif
		return false;
	}

	boolean_c menu_control_window_file_picker_c::go_back()
	{
		assert( _is_muted == false );
		if ( _history_index > 0 )
		{
			_history_index--;
			_is_muted = true;
			_back_button->set_is_enabled( _history_index > 0 );
			_forward_button->set_is_enabled( true );
			_folder_path = _history_stack[ _history_index ];
			_folder_path_text->set_plain_text_value( _folder_path );
			_up_button->set_is_enabled( ops::path_get_folder_path( _folder_path ).get_length() > 0 );
			_is_muted = false;
			refresh_files_collection();
			_refresh_okay_button_state();
			return true;
		}
		return false;
	}

	boolean_c menu_control_window_file_picker_c::go_forward()
	{
		assert( _is_muted == false );
		if ( _history_index + 1 < _history_stack.get_length() )
		{
			_history_index++;
			_is_muted = true;
			_back_button->set_is_enabled( true );
			_forward_button->set_is_enabled( _history_index < _history_stack.get_length() - 1 );
			_folder_path = _history_stack[ _history_index ];
			_folder_path_text->set_plain_text_value( _folder_path );
			_up_button->set_is_enabled( ops::path_get_folder_path( _folder_path ).get_length() > 0 );
			_is_muted = false;
			refresh_files_collection();
			_refresh_okay_button_state();
			return true;
		}
		return false;
	}

	boolean_c menu_control_window_file_picker_c::go_up()
	{
		string16_c path = ops::path_get_folder_path( _history_stack[ _history_index ] );
		if ( path.get_length() > 0 )
		{
			return go_to_folder_path( path );
		}
		return false;
	}

	string16_c menu_control_window_file_picker_c::get_folder_path_absolute() const
	{
		return _folder_path;
	}

	string16_c menu_control_window_file_picker_c::get_file_name() const
	{
		return _file_name_text->get_plain_text_value();
	}

	void_c menu_control_window_file_picker_c::set_file_name( string16_c const & value )
	{
		_file_name_text->set_plain_text_value( value );
	}

	void_c menu_control_window_file_picker_c::set_user_can_pick_file_name( boolean_c value )
	{
		_file_name_text->set_is_enabled( value );
	}

	string16_c menu_control_window_file_picker_c::menu_control_window_file_picker_c::get_file_path_absolute() const
	{
		string16_c result;
		result = _folder_path;
		result += _file_name_text->get_plain_text_value();
		return result;
	}

	void_c menu_control_window_file_picker_c::refresh_short_cut_list()
	{
		_short_cut_list->remove_all_items();
		_short_cut_path_list.remove_all();

		string16_c folder_path;
		if ( ops::file_system_get_quick_access_folder_path( ops::file_system_quick_access_folder_e_desktop, folder_path ) )
		{
			_short_cut_path_list.insert( -1, folder_path );
			menu_control_list_item_text_c * item = new menu_control_list_item_text_c();
			item->set_name( string8_c( "list_item", core_list_mode_e_static ) );
			item->set_plain_text_value( string16_c( L"Desktop", core_list_mode_e_static ) );
			item->on_clicked.subscribe( this, &menu_control_window_file_picker_c::_handle_short_cut_on_clicked );
			_short_cut_list->add_item( item );
		}

		if ( ops::file_system_get_quick_access_folder_path( ops::file_system_quick_access_folder_e_documents, folder_path ) )
		{
			_short_cut_path_list.insert( -1, folder_path );
			menu_control_list_item_text_c * item = new menu_control_list_item_text_c();
			item->set_name( string8_c( "list_item", core_list_mode_e_static ) );
			item->set_plain_text_value( string16_c( L"Documents", core_list_mode_e_static ) );
			item->on_clicked.subscribe( this, &menu_control_window_file_picker_c::_handle_short_cut_on_clicked );
			_short_cut_list->add_item( item );
		}

		core_list_c< ops::file_system_file_information_c > drive_path_list;
		ops::file_system_get_drive_path_list( drive_path_list );
		for ( sint32_c i = 0; i < drive_path_list.get_length(); i++ )
		{
			_short_cut_path_list.insert( -1, drive_path_list[ i ].path );
			menu_control_list_item_text_c * item = new menu_control_list_item_text_c();
			item->set_name( string8_c( "list_item", core_list_mode_e_static ) );
			string16_c label = drive_path_list[ i ].path;
			if ( drive_path_list[ i ].label.get_length() > 0 )
			{
				label += " (";
				label += drive_path_list[ i ].label;
				label += ")";
			}
			item->set_plain_text_value( label );
			item->on_clicked.subscribe( this, &menu_control_window_file_picker_c::_handle_short_cut_on_clicked );
			_short_cut_list->add_item( item );
		}
	}

	void_c menu_control_window_file_picker_c::refresh_files_collection()
	{
		_files_collection->remove_and_delete_all_items();
		string16_c folder_path_absolute = get_folder_path_absolute();
		core_list_c< ops::file_system_file_information_c > file_information_list;
		ops::file_system_get_file_information_list( file_information_list, folder_path_absolute, true, true, nullptr );
		for ( sint32_c i = 0; i < file_information_list.get_length(); i++ )
		{
			ops::file_system_file_information_c & file_information = file_information_list[ i ];
			item_c * item = new item_c( file_information );
			_files_collection->give_item( item );
		}
		_files_collection->refresh();
	}

}
