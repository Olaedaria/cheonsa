#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_frame.h"

namespace cheonsa
{

	// base interface and private implementation for scroll_bar_x, scroll_bar_y, scrub_bar_x, and scrub_bar_y.
	// preferred thickness property is provided, but is not really respected unless you implement code to re-set the box layout of the scroll bar in response to it changing, so a on_preferred_thickness_changed event is provided.
	class menu_control_scroll_bar_i
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "scroll_bar_i"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		enum mode_e
		{
			mode_e_scroll_bar, // scroll bar, default mode, if _fixed_grip_length > 0 then grip is of fixed length, otherwise grip length is calculated dynamically based on the ratio of ( _value_maximum - _value_minimum ) to _page_size.
			mode_e_scrub_bar, // scrub bar, grip is of a fixed length defined by _fixed_grip_length, and as long as the grip is dragged off of the center position then it will continue to move the value in that direction over time, and the grip will center itself when the user lets go of it.
		};

		enum orientation_e
		{
			orientation_e_x, // horizontal.
			orientation_e_y, // vertical.
		};

	protected:
		menu_element_frame_c _rail_element; // name is "rail_frame".
		menu_element_frame_c _grip_element; // name is "grip_frame".

		mode_e _mode; // if mode_e_slider or mode_e_scrubber, then the _slider_grip_length will be static. if mode_e_scroller, then the grip will be sized dynamically.
		orientation_e _orientation; // determines how this scroll bar will lay itself out in its parent.

		float64_c _page_size; // how far to go for page_up and page_down, which can be invoked by clicking in the rail element, or by pressing page up or page down on the keyboard.
		float64_c _line_size; // how far to scroll when up or down arrow is pressed or when the scroll wheel is scrolled.

		float64_c _value_minimum; // minimum value allowed.
		float64_c _value_maximum; // maximum value allowed.
		float64_c _value_increment; // if greater than 0, then value can only be set to increments of this value, relative to _value_minimum.
		float64_c _value; // true value. also doubles as the target for _value_smoothed. for scroll bar mode, this won't be more than _value_maximum - _page_size.
		float64_c _value_smoothed; // smoothed value, with animates quickly towards _value over time.
		float64_c _value_original; // original value when mouse drag was initiated.

		float64_c _scrub_speed; // rate of value change in units per second when the scrubber is moved to its farthest extent.
		float64_c _scrub_speed_ease_in; // exponential ramp, such that scrubbing near the dead-zone incurs slow change, and scrubbing farther from the dead-zone incurs exponentially faster change. 0.5 for inverse square, 1 for linear (no exponential curve), 2 for square, 3 for cube, etc. generally shouldn't be set to less than 0.5 i suppose.
		float64_c _scrub_input; // will be in range -1 to 1, which is the current input state of the scrubber as found in _on_input, so that it can be responded to in update().
		float64_c _scrub_value_accumulated; // accumulates changes in value over time from the scrubber, which is needed in the case that _value_increment is > 0, so that value changes are only applied when the accumulated value crosses the threshold.

		//float32_c _scroll_page_range; // total number of values visible on a page.
		//float32_c _scroll_increment; // how many pixels to scroll per mouse wheel notch.
		//boolean_c _smooth_scroll_enable; // if true, then any changes to the value will be animated smoothly, and on_scrolled may be triggered for any number of subsequent frames after a value change event. if false, then on_scrolled is only triggered once per value change event.
		//float64_c _smooth_scroll_value; // the smooth scroll position within the range 0 to _value_maximum - _range_page. it animates towards _value over time.
		//float32_c _smooth_scroll_position_target; // the smooth scroll target position within the range 0 to _total_capacity - _range_page.

		float32_c _preferred_thickness; // how thick in pixels that scroll bar wants to be. i don't have a good system for handling scroll bar layout. it is the responsibility of the mother control to respect this value when it lays out the scroll bar.
		float32_c _fixed_grip_length; // used when _mode is mode_e_scrubber.
		float32_c _dynamic_grip_length; // if _fixed_grip_length < 1 && _mode == mode_e_scroller, then this value is calculated dynamically based on the ratio of _page_size / ( _value_maximum - _value_minimum ). otherwise, it is set to the value in _fixed_grip_length.
		float32_c _dynamic_grip_position; // the position offset of the grip from the top edge of the scroll bar in pixels.

		boolean_c _mouse_is_grabbed; // will be set to true if the mouse was pressed down on the grip.
		float32_c _mouse_is_grabbed_offset; // if the mouse was pressed down on the grip, then this is the offset in pixels from the left edge of the slider to the mouse *.

		virtual void_c _on_is_mouse_focused_changed() override;
		virtual void_c _on_input( input_event_c * input_event ) override;

		virtual void_c _update_transform_and_layout() override; // updates layout of elements based on current mode and value.

		menu_control_scroll_bar_i( mode_e mode, orientation_e orientation );

	public:
		virtual void_c update_animations( float32_c time_step ) override; // updates scrubber, updates smooth scroll, which may invoke a layout of daughter elements and controls.
		virtual void_c load_static_data_properties( data_scribe_markup_c::node_c const * node ) override;

		orientation_e get_orientation() const;

		mode_e get_mode() const;

		boolean_c update_visibility( menu_visibility_mode_e value ); // returns true if _is_showed state changed.

		void_c set_value_range_and_page_size( float64_c value_minimum, float64_c value_maximum, float64_c page_size ); // lets you set three different properties at once then calls update_transform_and_layout(), because to set the properties individually would call update_transform_and_layout() each time.

		float64_c get_page_size() const;
		void_c set_page_size( float64_c value );

		float64_c get_line_size() const;
		void_c set_line_size( float64_c value );

		float64_c get_value_minimum() const;
		void_c set_value_minimum( float64_c value );

		float64_c get_value_maximum() const;
		void_c set_value_maximum( float64_c value );

		float64_c get_value_increment() const;
		void_c set_value_increment( float64_c value );

		float64_c get_value() const;
		boolean_c set_value( float64_c value );

		float64_c get_value_smoothed() const;

		float64_c get_scrub_speed_ease_in() const;
		void_c set_scrub_speed_ease_in( float64_c value );

		float64_c get_scrub_speed() const;
		void_c set_scrub_speed( float64_c value );

		float32_c get_preferred_thickness() const;
		void_c set_preferred_thickness( float32_c value );

		float32_c get_fixed_grip_length() const;
		void_c set_fixed_grip_length( float32_c value );

		// scroll bars actually don't respond directly to mouse wheel events.
		// it's the responsibility of the mother control (which has a larger surface area) to route those mouse wheel events to the scroll bar.
		// returns true if scroll value changed, false if not. this lets the event bubble up to potentially another scroll bar if the scroll bar is already at its limit.
		boolean_c inject_mouse_wheel_input( float32_c value );

	public:
		core_event_c< void_c, menu_control_scroll_bar_i * > on_value_changed_preview; // occurs as the user drags the mouse.
		core_event_c< void_c, menu_control_scroll_bar_i * > on_value_changed; // occurs if the value is changed using the arrow keys or scroll wheel, or when the user lets go of the mouse drag.
		core_event_c< void_c, menu_control_scroll_bar_i * > on_smooth_value_changed; // occurs at most once per frame when the smooth value changes.
		core_event_c< void_c, menu_control_scroll_bar_i * > on_preferred_thickness_changed; // lets the mother control set the layout of the scroll bar to respect the preferred thickness.

	};

}
