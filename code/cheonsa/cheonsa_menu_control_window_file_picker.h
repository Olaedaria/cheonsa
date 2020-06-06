#pragma once

#include "cheonsa_menu_control_window.h"
#include "cheonsa_menu_control_button.h"
#include "cheonsa_menu_control_text.h"
#include "cheonsa_menu_control_list.h"
#include "cheonsa_menu_control_collection.h"
#include "cheonsa_menu_control_window_message.h"
#include "cheonsa_menu_element_frame.h"

namespace cheonsa
{

	// a dialog window with a file picker.
	class menu_control_window_file_picker_c
		: public menu_control_window_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window_file_picker"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

		static vector32x2_c default_size; // new file picker windows will be set to this size by default.

	public:
		enum mode_e
		{
			mode_e_load, // select a file to load.
			mode_e_save, // select a file to save.
		};

		class item_c
			: public menu_control_collection_c::item_i
		{
			friend class menu_control_window_file_picker_c;

		private:
			string16_c _path; // absolute path of file or folder.
			uint64_c _creation_time;
			uint64_c _last_write_time;
			boolean_c _is_folder;

		public:
			item_c( ops::file_system_file_information_c const & file_information );

			virtual resource_file_texture_c * get_icon_texture() const override;
			virtual boolean_c get_value( string8_c const & key, string16_c & display_value, sint64_c & absolute_value ) const override;

		};

	protected:
		boolean_c _is_muted; // when true, the _handle_* functions will return immediately. the file picker is in the middle of changing values of controls, so it wants to temporarily ignore any events that are created by those controls.

		menu_control_text_c * _folder_path_text; // name is "folder_path_text", address bar at top. this path will be formatted with the operating system's file system path format.
		menu_control_text_c * _file_name_text; // name is "file_name_text", file name at bottom.
		menu_control_button_c * _back_button; // name is "back_button", back button at top, left of address bar.
		menu_control_button_c * _forward_button; // name is "forward_button", forward button at top, left of address bar.
		menu_control_button_c * _up_button; // name is "up_button", up button at top, left of address bar.
		menu_control_collection_c * _files_collection; // name is "files_collection", main files view at middle.
		core_list_c< string16_c > _history_stack; // each time a new folder is navigated to via (going up, going down, entering folder path in folder text box), that folder path gets added to the history stack. these paths must be absolute. these paths must be formatted in the operating system's path format.
		sint32_c _history_index; // current index in the history stack.
		string16_c _folder_path; // absolute folder path of folder that is currently being displayed in the file collection, formatted with the operating system's file system path format. this is here because we can't rely on the value of _folder_path_text to always be synced, or the value of _history_stack to always be synced.

		menu_control_list_c * _short_cut_list; // name is "shortcut_list", shortcuts at left side, for current user's desktop, documents, and connected disk drives. uses short and friendly names. each item in this list corresponds to an item in _shortcut_path_list.
		core_list_c< string16_c > _short_cut_path_list; // list of absolute file paths that are associated with the list items in _control_shortcut_list.
		void_c _short_cut_list_remove_all(); // clears short cut list.
		void_c _handle_short_cut_on_clicked( menu_event_information_c event_information );

		menu_control_button_c * _cancel_button; // name is "cancel_button", cancel button at bottom, right of file name.
		menu_control_button_c * _okay_button; // name is "okay_button", save or open button at bottom, right of file name.

		mode_e _mode; // if the file picker is being used to load a file or save a file.
		void_c _try_to_okay(); // tries to pick the currently selected file, which means that if mode is load then it checks if the file can be opened, if mode is save then it warns on overwrite.

		boolean_c _message_dialog_is_asking_for_over_write;
		menu_control_window_message_c * _message_dialog; // used to ask the user if they want to overwrite an existing file or not.

		void_c _handle_on_dialog_result( menu_control_window_c * window_dialog );
		void_c _handle_button_on_clicked( menu_event_information_c event_information );
		void_c _handle_text_on_value_changed( menu_control_text_c * control );
		void_c _handle_file_collection_on_selected_items_changed( menu_control_collection_c * collection );
		void_c _handle_file_collection_on_selected_items_invoked( menu_control_collection_c * collection );

		virtual void_c _on_input( input_event_c * input_event ) override;

	public:
		menu_control_window_file_picker_c( string8_c const & name );
		virtual ~menu_control_window_file_picker_c() override;

		mode_e get_mode() const;
		void_c set_mode( mode_e value );

		void_c clear_history(); // clears the history, but keeps the current folder path text.
		boolean_c go_to_folder_path( string16_c const & folder_path ); // tries to go to the given folder path and adds the new folder to the history.
		boolean_c go_back(); // tries to go back in the history.
		boolean_c go_forward(); // tries to go forward in the history.
		boolean_c go_up(); // tries to go up and adds the new folder to the history.

		string16_c get_folder_path_absolute() const; // gets the folder path of the folder that is currently displaying in the file collection.

		string16_c get_file_name() const; // gets the current text value of the file name text control.
		void_c set_file_name( string16_c const & value ); // sets the value of the file name text control.

		void_c set_user_can_pick_file_name( boolean_c value ); // if true then the user can edit the file name, otherwise the user can't (it should be set to a fixed value by the program).

		string16_c get_file_path_absolute() const;

		void_c refresh_short_cut_list(); // refreshes shortcut list, may need to be called when drives are added or removed.

		void_c refresh_files_collection(); // scans file system state and refreshes file collection view.

		core_event_c< boolean_c, menu_control_window_file_picker_c * > can_load; // is invoked each time a file is selected if mode is load, and when okay is pressed. the invoked function or method should check the file at get_file_path() and return true if it can load it, or false if not. this will dynamically control the is enabled state of the okay button.

	};

}
