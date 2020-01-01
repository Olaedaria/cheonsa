#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_frame.h"
#include "cheonsa_menu_element_text.h"
#include "cheonsa_menu_control_panel.h"
#include "cheonsa_menu_control_button.h"

using namespace cheonsa;

namespace cheonsa
{

	// a window is a control that can have daughter client controls added to it.
	// it has a title bar along the top edge.
	// it has options to be movable and resizable by the user.
	// unlike other controls, its local origin is located as its top left corner.
	class menu_window_c : public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "window"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		menu_element_frame_c _element_frame;
		menu_element_text_c _element_text;
		//menu_control_button_c * _close_button;
		menu_control_panel_c * _client_panel;

	protected:
		boolean_c _user_can_resize; // if true then the user can resize the panel like it's a window. in order for this to work, this panel must be a root level control in the mother menu context.
		float32_c _local_edge_size; // width or height of each edge, used for hit testing, user can use this to resize the window.
		vector32x2_c _local_size_minimum; // minimum size that user can size this control to, <= (0, 0) then a default will be used.
		vector32x2_c _local_size_maximum; // maximum size that the user can size this control to, if <= (0, 0) then a default maximum will be used, which will be quite generous and might as well be unlimited.

		boolean_c _user_can_move; // if true then the user can move the panel around like it's a window. in order fot this to work, this panel must be a root level control in the mother menu context.
		float32_c _local_title_bar_size; // height of title bar along top edge of window, used for hit testing, user can use this to move the window.

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

		virtual void_c _on_deep_text_focus_gained() override;
		virtual void_c _on_mouse_focus_lost() override;
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

		float32_c get_local_title_bar_size() const;
		void_c set_local_title_bar_size( float32_c value );

		float32_c get_local_edge_size() const;
		void_c set_local_edge_size( float32_c value );

		vector32x2_c const & get_local_size_minimum() const;
		void_c set_local_size_minimum( vector32x2_c const & value );

		vector32x2_c const & get_local_size_maximum() const;
		void_c set_local_size_maximum( vector32x2_c const & value );

		vector32x2_c const & get_position() const; // position of top left corner of window.
		void_c set_position( vector32x2_c const & value );

		vector32x2_c get_size() const;
		void_c set_size( vector32x2_c const & value );

		void_c give_control( menu_control_c * control );
		menu_control_c * take_control( menu_control_c * control );

	};

}
