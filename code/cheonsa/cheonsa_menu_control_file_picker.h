#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_control_button.h"
#include "cheonsa_menu_control_text.h"
#include "cheonsa_menu_control_list.h"
#include "cheonsa_menu_control_collection.h"
#include "cheonsa_menu_element_frame.h"

namespace cheonsa
{

	class menu_control_file_picker_item_c
		: public menu_control_collection_item_i
	{
		friend class menu_control_file_picker_c;

	private:
		string16_c _path; // absolute path of file or folder.
		uint64_c _creation_time;
		uint64_c _last_write_time;
		boolean_c _is_folder;

	public:
		menu_control_file_picker_item_c( ops::file_system_file_information_c const & file_information );

		virtual resource_file_texture_c * get_icon_texture() const override;
		virtual boolean_c get_value( string8_c const & key, string16_c & display_value, sint64_c & absolute_value ) const override;

	};

	class menu_control_file_picker_c
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "file_picker"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		enum result_e
		{
			result_e_cancel,
			result_e_okay,
		};

		enum mode_e
		{
			mode_e_load, // select a file to load.
			mode_e_save, // select a file to save.
		};

		typedef boolean_c( *can_load_call_back_f )( string16_c const & folder_or_file_path );

	private:
		menu_element_frame_c _frame; // name is "frame", makes the background of this control.

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
		void_c _short_cut_list_add( string16_c const & folder_path, string16_c const & friendly_name ); // folder path is absolute path of folder.

		menu_control_button_c * _okay_button; // name is "okay_button", save or open button at bottom, right of file name.
		menu_control_button_c * _cancel_button; // name is "cancel_button", cancel button at bottom, right of file name.
		result_e _result; // holds okay or cancel result.

		mode_e _mode; // if the file picker is being used to load a file or save a file.
		void_c _try_to_invoke_file_for_load(); // tries to pick the currently selected file, which means that if mode is load then it checks if the file can be opened, if mode is save then it warns on overwrite.

		boolean_c _is_asking_for_overwrite;

		boolean_c _muted; // when true, the _handle_* functions will return immediately. the file picker is in the middle of changing values of controls, so it wants to temporarily ignore any events that are created by those controls.

		void_c _handle_button_on_clicked( menu_event_information_c event_information );
		void_c _handle_text_on_value_changed( menu_control_text_c * control );
		void_c _handle_file_collection_on_selected_items_changed( menu_control_collection_c * collection );
		void_c _handle_file_collection_on_selected_items_invoked( menu_control_collection_c * collection );

	public:
		menu_control_file_picker_c();

		mode_e get_mode() const;
		void_c set_mode( mode_e value );

		void_c clear_history(); // clears the history, but keeps the current folder path text.
		boolean_c go_to_folder_path( string16_c const & folder_path ); // tries to go to the given folder path and adds the new folder to the history.
		boolean_c go_back(); // tries to go back in the history.
		boolean_c go_forward(); // tries to go forward in the history.
		boolean_c go_up(); // tries to go up and adds the new folder to the history.

		string16_c get_folder_path() const; // gets the folder path of the folder that is currently displaying in the file collection.

		string16_c get_file_name() const; // gets the current text value of the file name text control.
		void_c set_file_name( string16_c const & value ); // sets the value of the file name text control.

		string16_c get_file_path() const;

		void_c refresh_short_cut_list(); // refreshes shortcut list, may need to be called when drives are added or removed.

		void_c refresh_files_collection(); // scans file system state and refreshes file collection view.

		result_e get_result() const;

		core_event_c< boolean_c, menu_control_file_picker_c * > can_load; // is invoked each time a file is selected if mode is load.

		core_event_c< void_c, menu_control_file_picker_c * > on_submitted; // is called when dialog is canceled or okayed.

	};

}
