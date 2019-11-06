#pragma once

#include "cheonsa__types.h"
#include "cheonsa_menu_types.h"
#include "cheonsa_video_renderer_shader_manager.h"
#include "cheonsa_data_scribe_structure.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_string16.h"

namespace cheonsa
{

	// an element is a rectangular thing that is managed by a control.
	// elements make up the visual representation of the control.
	// they are divorced from layout logic (layout of the element is handled by the control that owns the element).
	// they are supposed to be divorced from user input and response logic as well, but the text element is able to handle user input on its own, so the control that owns it is responsible for channeling input events into the text element as needed.
	// for example, for a scroll bar control, the scroll bar is the control, and it contains two sub elements: bacground rail frame and foreground grip frame. the scroll bar control logic performs mouse hit detection against the elements and it lays them out appropriately.
	// for example, for a button control, the button is the control, and it contains two sub elements: background frame and foreground text.
	class menu_element_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "element"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

		static menu_element_c * make_new_instance( string8_c const & type );

	protected:
		friend class menu_control_c;
		friend class video_renderer_interface_c;

		menu_control_c * _mother_control; // the control that owns and manages this element.

		string8_c _name; // name to identify this element by, unique within the control that this element belongs to, can be used by the control to find the element and map styles to it.

		vector32x4_c _local_color; // color tint of this element. opacity is inherited from control, this is a limitation to keep the render procedure design simple.

		menu_anchor_e _local_box_anchor;
		box32x2_c _local_box_anchor_measures;
		box32x2_c _local_box;

		boolean_c _is_showing_from_style; // style map assignment may set this to false to hide this element. the element will still be laid out, but it just won't be rendered.

		boolean_c _is_showing; // if _control->_is_showing is false then it overrides this value.
		//float32_c _is_showing_weight; // _control->_is_showing is false, then the renderer takes _control->_global_is_showing_weight * _is_showing_weight to use as the element's actual opacity.

		boolean_c _is_enabled; // if _control->_is_enabled is false then it overrides this value.
		//float32_c _is_enabled_weight; // for now this does not animate, it will only flip between 0 and 1 based on the state of _control->_is_enabled && _is_enabled.

		boolean_c _is_selected;
		//float32_c _is_selected_weight;

		boolean_c _is_pressed;
		//float32_c _is_pressed_weight;

		menu_draw_list_c _draw_list; // used to build render procedure for this element.
		boolean_c _draw_list_is_dirty; // if true then _draw_unit needs to be rebuilt.
		virtual void_c _build_draw_list() = 0;

		virtual void_c _on_local_box_modified(); // override in sub-classes to be notified about local_box value changing and enable response.

	public:
		menu_element_c();

		virtual void_c update_animations( float32_c time_step );

		virtual void_c set_style_key( string8_c const & value ) = 0;

		void_c set_layout_simple( box32x2_c const & local_box ); // sets layout mode to simple and sets metrics.
		void_c set_layout_box_anchor( menu_anchor_e local_anchor, box32x2_c anchor_measures ); // sets layout mode to anchor and sets metrices.
		void_c update_layout( box32x2_c const & mother_local_box ); // if anchor layout is enabled then this recalculates _local_box.

		box32x2_c const & get_local_box() const;

		menu_control_c * get_mother_control() const;
		//void_c set_mother_control( menu_control_c * value );

		string8_c const & get_name() const;
		void_c set_name( string8_c const & value );

		vector32x4_c const & get_local_color() const;
		void_c set_local_color( vector32x4_c const & value );

		boolean_c get_is_showing() const;
		void_c set_is_showing( boolean_c value );

		//float32_c get_is_showing_weight() const;
		//void_c set_is_showing_weight( float32_c value );

		boolean_c get_is_enabled() const;
		void_c set_is_enabled( boolean_c value );

		//float32_c get_is_enabled_weight() const;
		//void_c set_is_enabled_weight( float32_c value );

		boolean_c get_is_selected() const;
		void_c set_is_selected( boolean_c value );

		//float32_c get_is_selected_weight() const;
		//void_c set_is_selected_weight( float32_c value );

		boolean_c get_is_pressed() const;
		void_c set_is_pressed( boolean_c value );

		//float32_c get_is_pressed_weight() const;
		//void_c set_is_pressed_weight( float32_c value );

		menu_draw_list_c & get_draw_list() const;

		menu_state_e get_state() const; // evaluates the boolean states of this element to arrive at an enumerated menu state.

	public:
		//virtual void_c save_element_properties( data_scribe_structure_c * structure );
		//virtual void_c load_element_properties( data_scribe_structure_c * structure );

		virtual void_c load_properties( data_scribe_markup_c::node_c const * node );

	};

}
