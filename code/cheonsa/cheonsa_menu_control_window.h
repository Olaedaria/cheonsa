#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"
#include "cheonsa_menu_control_panel_i.h"
#include "cheonsa_menu_control_button.h"

using namespace cheonsa;

namespace cheonsa
{

	// a window is a control that can have daughter client controls added to it.
	// it has a title bar along the top edge.
	// it has options to be movable and resizable by the user.
	// unlike other controls, its local origin is located as its top left corner.
	class menu_control_window_c
		: public menu_control_panel_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	protected:
		menu_element_frame_c _title_bar_frame_element; // name is "title_bar".
		menu_element_text_c _title_bar_text_element; // name is "text".

		menu_control_button_c * _close_button; // optional close button that may be displayed in title bar.

		float32_c _edge_size; // thickness of each grab-able edge, used for hit testing, user can grab this to resize the window.
		float32_c _title_bar_size; // extra thickness of top edge, for title bar, for hit testing, user can grab this to move the window.

		boolean_c _user_can_move; // if true then the user can move and resize the window. the window must be a root level control.
		boolean_c _user_can_resize; // if true then the user can move and resize the window. the window must be a root level control.

		vector32x2_c _minimum_size; // minimum size that user can size this control to, <= (0, 0) then a default will be used.
		vector32x2_c _maximum_size; // maximum size that the user can size this control to, if <= (0, 0) then a default maximum will be used, which will be quite generous and might as well be unlimited.

		enum grabbed_element_e
		{
			grabbed_element_e_none = 0x00,
			grabbed_element_e_title_bar = 0x01,
			grabbed_element_e_edge_left = 0x10,
			grabbed_element_e_edge_top = 0x20,
			grabbed_element_e_edge_right = 0x40,
			grabbed_element_e_edge_bottom = 0x80,
		};

		grabbed_element_e _grabbed_element; // which element was grabbed, determines if the user is resizing or moving the panel.
		vector32x2_c _grabbed_point_local; // local point relative to origin of where the panel was grabbed by the user, for resizing or moving.

		menu_control_c * _modal_screen; // if set, then when this window is responsible for hiding this modal screen when this window is dismissed.

		void_c const * _dialog_operation; // similar to user pointer, but intended to be used by the game to associate this window with some kind of command or operation, so that it can respond accordingly in its on_dialog_result handler.

		menu_dialog_result_e _dialog_result; // optional, used by dialog windows to hold result of dialog interaction.

		void_c _apply_client_margins();

		void_c _handle_button_on_clicked( menu_event_information_c event_information ); // responds to close button.

		void_c _handle_modal_screen_on_input( menu_event_information_c event_information );

		virtual void_c _on_is_deep_text_focused_changed( menu_control_c * next_control ) override;
		virtual void_c _on_is_mouse_focused_changed() override;
		virtual void_c _on_input( input_event_c * input_event ) override;

		virtual void_c _update_daughter_element_animations( float32_c time_step ) override;

		virtual void_c _update_transform_and_layout() override;

	public:
		menu_control_window_c();
		virtual ~menu_control_window_c() override;

		void_c constrain_transform(); // snaps window to fit in bounds of user interface, so that the user doesn't lose it.

		boolean_c get_close_button_is_enabled() const;
		void_c set_close_button_is_enabled( boolean_c value );

		string16_c get_title_plain_text_value() const;
		void_c set_title_plain_text_value( string8_c const & plain_text );
		void_c set_title_plain_text_value( string16_c const & plain_text );
		void_c clear_title_text_value();

		boolean_c get_user_can_move() const;
		void_c set_user_can_move( boolean_c value );

		boolean_c get_user_can_resize() const;
		void_c set_user_can_resize( boolean_c value );

		float32_c get_title_bar_size() const;
		void_c set_title_bar_size( float32_c value );

		float32_c get_edge_size() const;
		void_c set_edge_size( float32_c value );

		vector32x2_c const & get_minimum_size() const;
		void_c set_minimum_size( vector32x2_c const & value );

		vector32x2_c const & get_maximum_size() const;
		void_c set_maximum_size( vector32x2_c const & value );

		vector32x2_c const & get_position() const; // gets the position of the top left corner of the window.
		void_c set_position( vector32x2_c const & value ); // sets the position of the top left corner of the window.

		vector32x2_c get_size() const; // gets the width and height of the window.
		void_c set_size( vector32x2_c const & value ); // sets the width and height of the window.

		menu_visibility_mode_e get_scroll_bar_x_visibility_mode() const;
		void_c set_scroll_bar_x_visibility_mode( menu_visibility_mode_e value );

		menu_visibility_mode_e get_scroll_bar_y_visibility_mode() const;
		void_c set_scroll_bar_y_visibility_mode( menu_visibility_mode_e value );

		sint32_c get_daughter_controls_in_client_count() const;
		menu_control_c * get_daughter_control_in_client_at_index( sint32_c control_index ) const;

		void_c add_daughter_control_to_client( menu_control_c * control, sint32_c index = -1 ); // adds a control to the client area of the window.
		void_c remove_daughter_control_from_client( menu_control_c * control ); // removes a control from the client area of the window.
		void_c remove_all_daughter_controls_from_client(); // removes all controls from the client area of the window.

		// centers this window in the user interface, preserves current size.
		void_c center();

		// shows this window like a dialog.
		// modal_screen is optional, but if it's proivided then this window needs to be responsible for hiding and deleting it with set_is_showed( false, true ).
		// this modal screen is a root level control, created by the user interface system, provided for us to remember.
		virtual void_c show_dialog( menu_control_c * modal_screen );

		// hides this window and also hides and deletes modal screen if one is associated.
		virtual void_c hide_dialog( boolean_c and_delete = false );

		void_c const * get_dialog_operation() const;
		void_c set_dialog_operation( void_c const * value );

		menu_dialog_result_e get_dialog_result() const; // gets the current or last result.
		core_event_c< void_c, menu_control_window_c * > on_dialog_result; // dialog windows are not responsible for calling hide_dialog() on their own.

	};

}
