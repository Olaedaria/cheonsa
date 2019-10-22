#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_button.h"
#include "cheonsa_menu_control_text.h"
#include "cheonsa_menu_control_list.h"
#include "cheonsa_menu_control_collection.h"
#include "cheonsa_menu_element_frame.h"

namespace cheonsa
{

	class menu_control_file_picker_c : public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "file_picker"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		enum mode_e
		{
			mode_e_load, // select a file or folder to load.
			mode_e_save, // select a file or folder to save.
		};

		typedef boolean_c( *can_load_call_back_f )( string16_c const & folder_or_file_path );

	private:
		menu_element_frame_c _element_frame; // name is "frame", makes the background of this control.

		menu_control_button_c * _control_back_button; // name is "back_button", back button at top, left of address bar.
		menu_control_button_c * _control_forward_button; // name is "forward_button", forward button at top, left of address bar.
		menu_control_button_c * _control_up_button; // name is "up_button", up button at top, left of address bar.
		menu_control_text_c * _control_folder_path_text; // name is "folder_path_text", address bar at top.
		menu_control_list_c * _control_shortcut_list; // name is "shortcut_list", shortcuts at left side, for current user's desktop, documents, and connected disk drives. uses short and friendly names. each item in this list corresponds to an item in _shortcut_path_list.
		menu_control_collection_c * _control_files_collection; // name is "files_collection", main files view at middle.
		menu_control_text_c * _control_file_name_text; // name is "file_name_text", file name at bottom.
		menu_control_button_c * _control_okay_button; // name is "okay_button", save or open button at bottom, right of file name.
		menu_control_button_c * _control_cancel_button; // name is "cancel_button", cancel button at bottom, right of file name.

		core_list_c< string16_c > _shortcut_path_list; // list of absolute file paths that are associated with the list items in _control_shortcut_list.
		void_c _remove_all_shortcut_paths();
		void_c _add_shortcut_path( string16_c const & absolute_path, string16_c const & friendly_name );

		mode_e _mode; // if the file picker is being used to load a file or save a file.
		boolean_c _warn_on_over_write; // if true, then if the user tries to save over an existing file, then a dialog window will pop up asking the user if they are sure about it.

		can_load_call_back_f _can_load_call_back; // if this call back is set, then this file dialog will call this function each time a file is selected in order to determine if that file can be chosen.

	public:
		menu_control_file_picker_c();

		mode_e get_mode() const;
		void_c set_mode( mode_e value );

		boolean_c get_warn_on_over_write() const;
		void_c set_warn_on_over_write( boolean_c value );

		void_c set_can_load_call_back( can_load_call_back_f can_load_call_back ); // when this is set and used with mode_e_load, then the provided function will be called whenever a folder or file is selected by the user. the return value of this function will determine the enabled state of the open button. this function can either just look at the file extension, or it can scan the contents of the file to determine if it is a valid choice.

		string16_c get_folder_path() const; // gets the folder path in the address bar.
		void_c set_folder_path( string16_c const & value ); // sets the folder path in the address bar.

		string16_c get_file_name() const; // gets the name of the selected file (or folder) within the currently set folder path.
		void_c set_file_name( string16_c const & value ); // sets the name of the selected file (or folder) within the currently set folder path.

		void_c refresh_shortcut_list(); // refreshes shortcut list, may need to be called when drives are added or removed.

		void_c refresh_file_collection(); // refreshes file collection view.

	};

}
