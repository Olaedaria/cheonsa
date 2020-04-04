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
	class menu_window_c
		: public menu_control_panel_i
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		//menu_element_frame_c _element_frame; // name is "frame".
		menu_element_text_c _element_text; // name is "text"
		//menu_control_button_c * _close_button;
		//menu_control_panel_c * _client_panel; // name is "client_panel"

	protected:
		float32_c _edge_size; // thickness of each grab-able edge, used for hit testing, user can grab this to resize the window.
		float32_c _top_bar_size; // extra thickness of top edge, for title bar, for hit testing, user can grab this to move the window.
		float32_c _bottom_bar_size; // extra thickness of bottom edge, for additional dialog buttons.

		boolean_c _user_can_resize; // if true then the user can resize the panel like it's a window. in order for this to work, this panel must be a root level control in the mother menu context.
		boolean_c _user_can_move; // if true then the user can move the panel around like it's a window. in order fot this to work, this panel must be a root level control in the mother menu context.

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

		void_c _apply_client_margins();

		virtual void_c _on_is_deep_text_focused_changed() override;
		virtual void_c _on_is_mouse_focused_changed() override;
		virtual void_c _on_input( input_event_c * input_event ) override;

	public:
		menu_window_c();

		virtual void_c update_animations( float32_c time_step ) override;
		virtual void_c update_transform_and_layout() override;
		void_c constrain_transform(); // snaps window to fit in bounds of user interface, so that the user doesn't lose it.

		string16_c get_plain_text_value() const;
		void_c set_plain_text_value( string8_c const & plain_text );
		void_c set_plain_text_value( string16_c const & plain_text );
		void_c clear_text_value();

		boolean_c get_user_can_move() const;
		void_c set_user_can_move( boolean_c value );

		boolean_c get_user_can_resize() const;
		void_c set_user_can_resize( boolean_c value );

		float32_c get_top_bar_size() const;
		void_c set_top_bar_size( float32_c value );

		float32_c get_bottom_bar_size() const;
		void_c set_bottom_bar_size( float32_c value );

		float32_c get_edge_size() const;
		void_c set_edge_size( float32_c value );

		vector32x2_c const & get_minimum_size() const;
		void_c set_minimum_size( vector32x2_c const & value );

		vector32x2_c const & get_maximum_size() const;
		void_c set_maximum_size( vector32x2_c const & value );

		vector32x2_c const & get_position() const; // position of top left corner of window.
		void_c set_position( vector32x2_c const & value );

		vector32x2_c get_size() const;
		void_c set_size( vector32x2_c const & value );

		menu_visibility_mode_e get_horizontal_scroll_bar_visibility_mode() const;
		void_c set_horizontal_scroll_bar_visibility_mode( menu_visibility_mode_e value );

		menu_visibility_mode_e get_vertical_scroll_bar_visibility_mode() const;
		void_c set_vertical_scroll_bar_visibility_mode( menu_visibility_mode_e value );

		sint32_c get_controls_in_client_count() const;
		menu_control_c const * get_control_in_client( sint32_c control_index ) const;
		menu_control_c * get_control_in_client( sint32_c control_index );

		sint32_c give_control_to_client( menu_control_c * control, sint32_c index = -1 );
		menu_control_c * take_control_from_client( sint32_c control_index );
		void_c remove_and_delete_all_controls_from_client();

	};

}
