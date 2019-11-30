#include "cheonsa_menu_control_file_picker.h"

#if defined( cheonsa_platform_windows )
#include <Shlobj.h> // needed for SHGetKnownFolderPath, used to build shortcut list to common locations on windows.
#endif

namespace cheonsa
{

	void_c menu_control_file_picker_c::_remove_all_shortcut_paths()
	{
		_control_shortcut_list->remove_and_delete_all_items();
		_shortcut_path_list.remove_all();
	}

	void_c menu_control_file_picker_c::_add_shortcut_path( string16_c const & absolute_path, string16_c const & friendly_name )
	{
		menu_control_list_item_c * list_item = new menu_control_list_item_c();
		list_item->set_plain_text_value( friendly_name );
		_control_shortcut_list->add_item( list_item );

		_shortcut_path_list.insert_at_end( absolute_path );
	}

	menu_control_file_picker_c::menu_control_file_picker_c()
		: menu_control_c()
		, _element_frame()
		, _control_back_button( nullptr )
		, _control_forward_button( nullptr )
		, _control_up_button( nullptr )
		, _control_folder_path_text( nullptr )
		, _control_shortcut_list( nullptr )
		, _control_files_collection( nullptr )
		, _control_file_name_text( nullptr )
		, _control_okay_button( nullptr )
		, _control_cancel_button( nullptr )
		, _shortcut_path_list()
		, _mode( mode_e_load )
		, _warn_on_over_write( true )
		, _can_load_call_back( nullptr )
	{
		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_style_key( string8_c( mode_e_static, "e_field_frame" ) );
		_add_element( &_element_frame );

		_control_back_button = new menu_control_button_c();
		_control_back_button->set_name( string8_c( mode_e_static, "back_button" ) );
		_add_control( _control_back_button );

		_control_forward_button = new menu_control_button_c();
		_control_forward_button->set_name( string8_c( mode_e_static, "forward_button" ) );
		_add_control( _control_forward_button );

		_control_up_button = new menu_control_button_c();
		_control_up_button->set_name( string8_c( mode_e_static, "up_button" ) );
		_add_control( _control_up_button );

		_control_folder_path_text = new menu_control_text_c();
		_control_folder_path_text->set_name( string8_c( mode_e_static, "folder_path_text" ) );
		_add_control( _control_folder_path_text );

		_control_shortcut_list = new menu_control_list_c();
		_control_shortcut_list->set_name( string8_c( mode_e_static, "shortcut_list" ) );
		_add_control( _control_shortcut_list );

		_control_files_collection = new menu_control_collection_c();
		_control_files_collection->set_name( string8_c( mode_e_static, "files_collection" ) );
		_control_files_collection->set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 10.0f, 0.0f, 10.0f ) );
		_add_control( _control_files_collection );

		_control_file_name_text = new menu_control_text_c();
		_control_file_name_text->set_name( string8_c( mode_e_static, "file_name_text" ) );
		_add_control( _control_file_name_text );

		_control_okay_button = new menu_control_button_c();
		_control_okay_button->set_name( string8_c( mode_e_static, "okay_button" ) );
		_add_control( _control_okay_button );

		_control_cancel_button = new menu_control_button_c();
		_control_cancel_button->set_name( string8_c( mode_e_static, "cancel_button" ) );
		_add_control( _control_cancel_button );
	}

	menu_control_file_picker_c::mode_e menu_control_file_picker_c::get_mode() const
	{
		return _mode;
	}

	void_c menu_control_file_picker_c::set_mode( mode_e value )
	{
		_mode = value;
	}

	boolean_c menu_control_file_picker_c::get_warn_on_over_write() const
	{
		return _warn_on_over_write;
	}

	void_c menu_control_file_picker_c::set_warn_on_over_write( boolean_c value )
	{
		_warn_on_over_write = value;
	}

	void_c menu_control_file_picker_c::set_can_load_call_back( can_load_call_back_f can_load_call_back )
	{
		_can_load_call_back = can_load_call_back;
	}

	string16_c menu_control_file_picker_c::get_folder_path() const
	{
		return _control_folder_path_text->get_plain_text_value();
	}

	void_c menu_control_file_picker_c::set_folder_path( string16_c const & value )
	{
		_control_folder_path_text->set_plain_text_value( value );
		_control_file_name_text->clear_text_value();
		refresh_file_collection();
	}

	string16_c menu_control_file_picker_c::get_file_name() const
	{
		return _control_file_name_text->get_plain_text_value();
	}

	void_c menu_control_file_picker_c::set_file_name( string16_c const & value )
	{
		_control_file_name_text->set_plain_text_value( value );
	}

	void_c menu_control_file_picker_c::refresh_shortcut_list()
	{
		_control_shortcut_list->remove_and_delete_all_items();
		_shortcut_path_list.remove_all();

#if defined( cheonsa_platform_windows )
		PWSTR ppszPath = NULL;

		// scan for desktop folder.
		if ( S_OK == SHGetKnownFolderPath( FOLDERID_Desktop, 0, NULL, &ppszPath ) )
		{
			_shortcut_path_list.insert_at_end( string16_c( mode_e_static_volatile, ppszPath ) );
			menu_control_list_item_c * list_item = new menu_control_list_item_c();
			list_item->set_plain_text_value( string16_c( mode_e_static_volatile, L"desktop" ) );
			_control_shortcut_list->add_item( list_item );
			CoTaskMemFree( ppszPath );
		}

		// scan for documents folder.
		if ( S_OK == SHGetKnownFolderPath( FOLDERID_Documents, 0, NULL, &ppszPath ) )
		{
			_shortcut_path_list.insert_at_end( string16_c( mode_e_static_volatile, ppszPath ) );
			CoTaskMemFree( ppszPath );
		}

		// scan all 26 letters, kinda bleh that we have to do it one by one instead of just asking the OS which ones are connected but eh.
		DWORD logical_drives = GetLogicalDrives();
		for ( sint32_c i = 0; i < 26; i++ )
		{
			uint32_c bit = static_cast< uint32_c >( 0x0001 ) << i;
			if ( logical_drives & bit )
			{
				string16_c drive_path;
				drive_path += static_cast< char16_c >( 'a' + i );
				drive_path += ":\\";
				UINT drive_type = GetDriveTypeW( drive_path.character_list.get_internal_array() );
				if ( drive_type >= 2 )
				{
					WCHAR drive_label[ MAX_PATH + 1 ];
					if ( GetVolumeInformationW( drive_path.character_list.get_internal_array(), drive_label, MAX_PATH + 1, NULL, 0, 0, NULL, 0 ) )
					{
						drive_path += " - ";
						drive_path += drive_label;
						_shortcut_path_list.insert_at_end( drive_path );
					}
				}
			}
		}
#endif
	}

	void_c menu_control_file_picker_c::refresh_file_collection()
	{
	}

}
