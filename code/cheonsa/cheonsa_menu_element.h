#pragma once

#include "cheonsa__types.h"
#include "cheonsa_menu_types.h"
#include "cheonsa_video_renderer_shader_manager.h"
#include "cheonsa_data_scribe_structure.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// menu element base class.
	// an element is a rectangular thing that is managed by a control.
	// every displayable thing that the user sees in the menu system is made out of elements.
	// there are only two types of elements: frames (menu_element_frame_c) and texts (menu_element_text_c).
	// each menu control will contain one or more elements.
	// elements make up the visual representation of the control.
	class menu_element_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "element"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

	protected:
		friend class menu_control_c;
		friend class video_renderer_interface_c;

		menu_control_c * _mother_control; // the control that owns and manages this element.

		string8_c _name; // name to identify this element by, unique within the control that this element belongs to, can be used by the control to find the element and map styles to it.

		menu_shared_color_class_e _shared_color_class; // defines which set of shared colors are used to draw this control's elements.

		vector32x4_c _local_color; // color tint of this element to upload to menu_colors[ 3 ]. opacity defined here is ignored. actual opacity is always inherited from control, this is a limitation to keep the render procedure (layering, compositing) design simple.

		menu_anchor_e _local_box_anchor;
		box32x2_c _local_box_anchor_measures;
		box32x2_c _local_box;

		boolean_c _is_showed_from_style; // style map assignment may set this to false to hide this element. the element will still be laid out, but it just won't be rendered.
		boolean_c _is_showed; // if _control->_is_showing is false then it overrides this value.
		boolean_c _is_enabled; // if _control->_is_enabled is false then it overrides this value.
		boolean_c _is_selected;
		boolean_c _is_pressed;

		menu_draw_list_c _draw_list; // used to build render procedure for this element.
		boolean_c _draw_list_is_dirty; // if true then _draw_unit needs to be rebuilt.
		virtual void_c _build_draw_list() = 0;

		virtual void_c _on_local_box_modified(); // override in sub-classes to be notified about local_box value changing and enable response.

		virtual void_c _load_properties( data_scribe_markup_c::node_c const * node );

	public:
		menu_element_c( string8_c const & name );

		virtual void_c update_animations( float32_c time_step ) = 0;

		virtual void_c set_style_key( string8_c const & value ) = 0;

		void_c set_layout_simple( box32x2_c const & local_box ); // sets layout mode to simple and sets metrics.
		void_c set_layout_box_anchor( menu_anchor_e local_anchor, box32x2_c anchor_measures ); // sets layout mode to anchor and sets metrices.
		void_c update_layout( box32x2_c const & mother_local_box ); // if anchor layout is enabled then this recalculates _local_box.

		box32x2_c const & get_local_box() const;

		menu_control_c * get_mother_control() const;
		void_c set_mother_control( menu_control_c * value ); // the need for this is very specific, generally you don't need to use this directly.

		string8_c const & get_name() const;
		//void_c set_name( string8_c const & value );

		vector32x4_c const & get_local_color() const;
		void_c set_local_color( vector32x4_c const & value );

		menu_shared_color_class_e get_shared_color_class() const;
		void_c set_shared_color_class( menu_shared_color_class_e value );

		boolean_c get_is_showed() const;
		void_c set_is_showed( boolean_c value );

		boolean_c get_is_enabled() const;
		void_c set_is_enabled( boolean_c value );

		boolean_c get_is_selected() const;
		void_c set_is_selected( boolean_c value );

		boolean_c get_is_pressed() const;
		void_c set_is_pressed( boolean_c value );

		menu_draw_list_c & get_draw_list() const;

		menu_state_e get_state() const; // evaluates the boolean states of this element to arrive at an enumerated menu state.

	};

}
