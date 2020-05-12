#pragma once

#include "cheonsa___build_configuration.h"
#include "cheonsa_types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_event.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"
#include "cheonsa_string.h"

namespace cheonsa
{

	enum input_mouse_mode_e : uint8_c
	{
		input_mouse_mode_e_absolute, // mouse * is visible and it's position is controlled by the user.
		input_mouse_mode_e_relative // mouse * is invisible and it's position is centered, but position_delta and PositionDeltaSmooth values are still updated.
	};

	enum input_modifier_flag_e : uint8_c
	{
		input_modifier_flag_e_none = 0x00,
		input_modifier_flag_e_shift = 0x01,
		input_modifier_flag_e_ctrl = 0x02,
		input_modifier_flag_e_alt = 0x04
	};

	enum input_mouse_key_e : uint8_c
	{
		input_mouse_key_e_none,

		input_mouse_key_e_left,
		input_mouse_key_e_middle,
		input_mouse_key_e_right,
		input_mouse_key_e_extra1, // i believe this is usually mapped as the browser back button.
		input_mouse_key_e_extra2, // i believe this is usually mapped as the browser forward button.

		input_mouse_key_e_count_
	};

	enum input_keyboard_key_e : uint8_c
	{
		input_keyboard_key_e_none,

		input_keyboard_key_e_escape,
		input_keyboard_key_e_f1,
		input_keyboard_key_e_f2,
		input_keyboard_key_e_f3,
		input_keyboard_key_e_f4,
		input_keyboard_key_e_f5,
		input_keyboard_key_e_f6,
		input_keyboard_key_e_f7,
		input_keyboard_key_e_f8,
		input_keyboard_key_e_f9,
		input_keyboard_key_e_f10,
		input_keyboard_key_e_f11,
		input_keyboard_key_e_f12,

		input_keyboard_key_e_grave,
		input_keyboard_key_e_1,
		input_keyboard_key_e_2,
		input_keyboard_key_e_3,
		input_keyboard_key_e_4,
		input_keyboard_key_e_5,
		input_keyboard_key_e_6,
		input_keyboard_key_e_7,
		input_keyboard_key_e_8,
		input_keyboard_key_e_9,
		input_keyboard_key_e_0,
		input_keyboard_key_e_minus,
		input_keyboard_key_e_equals,
		input_keyboard_key_e_back_space,

		input_keyboard_key_e_tab,
		input_keyboard_key_e_q,
		input_keyboard_key_e_w,
		input_keyboard_key_e_e,
		input_keyboard_key_e_r,
		input_keyboard_key_e_t,
		input_keyboard_key_e_y,
		input_keyboard_key_e_u,
		input_keyboard_key_e_i,
		input_keyboard_key_e_o,
		input_keyboard_key_e_p,
		input_keyboard_key_e_l_bracket,
		input_keyboard_key_e_r_bracket,
		input_keyboard_key_e_back_slash,

		input_keyboard_key_e_caps_lock,
		input_keyboard_key_e_a,
		input_keyboard_key_e_s,
		input_keyboard_key_e_d,
		input_keyboard_key_e_f,
		input_keyboard_key_e_g,
		input_keyboard_key_e_h,
		input_keyboard_key_e_j,
		input_keyboard_key_e_k,
		input_keyboard_key_e_l,
		input_keyboard_key_e_semi_colon,
		input_keyboard_key_e_apostrophe,
		input_keyboard_key_e_enter,

		input_keyboard_key_e_l_shift,
		input_keyboard_key_e_z,
		input_keyboard_key_e_x,
		input_keyboard_key_e_c,
		input_keyboard_key_e_v,
		input_keyboard_key_e_b,
		input_keyboard_key_e_n,
		input_keyboard_key_e_m,
		input_keyboard_key_e_comma,
		input_keyboard_key_e_period,
		input_keyboard_key_e_slash,
		input_keyboard_key_e_r_shift,

		input_keyboard_key_e_l_ctrl,
		input_keyboard_key_e_l_alt,
		input_keyboard_key_e_l_win,
		input_keyboard_key_e_space,
		input_keyboard_key_e_menu,
		input_keyboard_key_e_r_alt,
		input_keyboard_key_e_r_win,
		input_keyboard_key_e_r_ctrl,

		input_keyboard_key_e_print_screen,
		input_keyboard_key_e_scroll_lock,
		input_keyboard_key_e_pause,

		input_keyboard_key_e_insert,
		input_keyboard_key_e_delete,
		input_keyboard_key_e_home,
		input_keyboard_key_e_end,
		input_keyboard_key_e_page_up,
		input_keyboard_key_e_page_down,

		input_keyboard_key_e_up,
		input_keyboard_key_e_left,
		input_keyboard_key_e_down,
		input_keyboard_key_e_right,

		input_keyboard_key_e_num_lock,
		input_keyboard_key_e_num_pad_0,
		input_keyboard_key_e_num_pad_1,
		input_keyboard_key_e_num_pad_2,
		input_keyboard_key_e_num_pad_3,
		input_keyboard_key_e_num_pad_4,
		input_keyboard_key_e_num_pad_5,
		input_keyboard_key_e_num_pad_6,
		input_keyboard_key_e_num_pad_7,
		input_keyboard_key_e_num_pad_8,
		input_keyboard_key_e_num_pad_9,
		input_keyboard_key_e_num_pad_multiply,
		input_keyboard_key_e_num_pad_subtract,
		input_keyboard_key_e_num_pad_add,
		input_keyboard_key_e_num_pad_dot,
		input_keyboard_key_e_num_pad_divide,
		input_keyboard_key_e_num_pad_enter,

		input_keyboard_key_e_volume_mute,
		input_keyboard_key_e_volume_up,
		input_keyboard_key_e_volume_down,

		input_keyboard_key_e_media_play_pause,
		input_keyboard_key_e_media_stop,
		input_keyboard_key_e_media_previous,
		input_keyboard_key_e_media_next,

		input_keyboard_key_e_computer_power,
		input_keyboard_key_e_computer_sleep,
		input_keyboard_key_e_computer_wake,

		input_keyboard_key_e_browser_back,
		input_keyboard_key_e_browser_forward,
		input_keyboard_key_e_browser_refresh,
		input_keyboard_key_e_browser_stop,
		input_keyboard_key_e_browser_search,
		input_keyboard_key_e_browser_favorites,
		input_keyboard_key_e_browser_home,

		input_keyboard_key_e_count_
	};

	// maps to virtual keys in xinput input events.
	// these are virtual because some of these include interpreting analog inputs as binary inputs.
	// a little redundant with input_gamepad_button_e.
	// these are used with input events.
	enum input_gamepad_key_e : uint8_c
	{
		input_gamepad_key_e_none = 0x00,

		input_gamepad_key_e_dpad_left, // directional pad left.
		input_gamepad_key_e_dpad_right, // directional pad right.
		input_gamepad_key_e_dpad_up, // directional pad up.
		input_gamepad_key_e_dpad_down, // directional pad down.
		input_gamepad_key_e_menu,	// start/menu/options button.
		input_gamepad_key_e_view,	// select/back/view/share button.
		input_gamepad_key_e_left_stick, // left stick depress.
		input_gamepad_key_e_right_stick, // right stick depress.
		input_gamepad_key_e_left_bumper, // left bumper.
		input_gamepad_key_e_right_bumper, // right bumper.
		input_gamepad_key_e_left_trigger, // simulated button, when left analog trigger is pressed.
		input_gamepad_key_e_right_trigger, // simulated button, when right analog trigger is pressed.
		input_gamepad_key_e_a,	// a/cross button.
		input_gamepad_key_e_b,	// b/circle button.
		input_gamepad_key_e_x,	// x/square button.
		input_gamepad_key_e_y,	// y/triangle button.
		input_gamepad_key_e_left_stick_up,	// simulated button, when left analog stick is pressed up.
		input_gamepad_key_e_left_stick_down,	// simulated button, when left analog stick is pressed down.
		input_gamepad_key_e_left_stick_right,	// simulated button, when left analog stick is pressed right.
		input_gamepad_key_e_left_stick_left,	// simulated button, when left analog stick is pressed left.
		input_gamepad_key_e_left_stick_up_left,	// simulated button, when left analog stick is pressed up-left.
		input_gamepad_key_e_left_stick_up_right,	// simulated button, when left analog stick is pressed up-right.
		input_gamepad_key_e_left_stick_down_right,	// simulated button, when left analog stick is pressed down-right.
		input_gamepad_key_e_left_stick_down_left,	// simulated button, when left analog stick is pressed down-left.
		input_gamepad_key_e_right_stick_up,	// simulated button, when right analog stick is pressed up.
		input_gamepad_key_e_right_stick_down,	// simulated button, when right analog stick is pressed down.
		input_gamepad_key_e_right_stick_right,	// simulated button, when right analog stick is pressed right.
		input_gamepad_key_e_right_stick_left,	// simulated button, when right analog stick is pressed left.
		input_gamepad_key_e_right_stick_up_left,	// simulated button, when right analog stick is pressed up-left.
		input_gamepad_key_e_right_stick_up_right,	// simulated button, when right analog stick is pressed up-right.
		input_gamepad_key_e_right_stick_down_right,	// simulated button, when right analog stick is pressed down-right.
		input_gamepad_key_e_right_stick_down_left,	// simulated button, when right analog stick is pressed down-left.

		input_gamepad_key_e_count_
	};

	// binary inputs only.
	// a little redundant with input_gamepad_key_e.
	// these are used with frame snapshots of input state.
	enum input_gamepad_button_e : uint16_c
	{
		input_gamepad_button_e_dpad_up = 0x0001,
		input_gamepad_button_e_dpad_down = 0x0002,
		input_gamepad_button_e_dpad_left = 0x0004,
		input_gamepad_button_e_dpad_right = 0x0008,
		input_gamepad_button_e_menu = 0x0010,
		input_gamepad_button_e_view = 0x0020,
		input_gamepad_button_e_left_stick = 0x0040,
		input_gamepad_button_e_right_stick = 0x0080,
		input_gamepad_button_e_left_bumper = 0x0100,
		input_gamepad_button_e_right_bumper = 0x0200,
		input_gamepad_button_e_a = 0x0400,
		input_gamepad_button_e_b = 0x0800,
		input_gamepad_button_e_x = 0x1000,
		input_gamepad_button_e_y = 0x2000,
	};

	// mirror of XINPUT_GAMEPAD, because other APIs are ugly and i won't interface with them directly in public.
	class input_gamepad_state_c
	{
		friend class input_manager_c;

	private:
		uint16_c _index;
		boolean_c _is_connected;
		uint16_c _button_states;
		float32_c _left_trigger_state;
		float32_c _right_trigger_state;
		vector32x2_c _left_stick_state;
		vector32x2_c _right_stick_state;
		vector32x2_c _left_stick_state_with_dead_zone;
		vector32x2_c _right_stick_state_with_dead_zone;

		void_c _cap_length_and_make_dead_zone();

	public:
		input_gamepad_state_c();

		uint16_c get_index() const;

		boolean_c get_is_connected() const;

		boolean_c get_button_state( input_gamepad_button_e button ) const;

		float32_c get_left_trigger_state() const;
		float32_c get_right_trigger_state() const;
		vector32x2_c get_left_stick_state( boolean_c with_dead_zone ) const;
		vector32x2_c get_right_stick_state( boolean_c with_dead_zone ) const;

	};

	class input_event_c;

	// can be bound to an action in order to invoke that action when the shortcut is triggered.
	// input actions can be given shortcuts so that they can be invoked at the user's convenience of a key stroke.
	class input_shortcut_c
	{
		friend class input_manager_c;

	private:
		input_modifier_flag_e _mouse_modifier_flags;
		input_mouse_key_e _mouse_key;

		input_modifier_flag_e _keyboard_modifier_flags;
		input_keyboard_key_e _keyboard_key;

		input_gamepad_key_e _gamepad_key;

	public:
		input_shortcut_c();

		void_c reset(); // resets this shortcut to nothing.

		input_modifier_flag_e get_mouse_modifier_flags() const;
		void_c set_mouse_modifier_flags( input_modifier_flag_e value );

		input_mouse_key_e get_mouse_key() const;
		void_c set_mouse_key( input_mouse_key_e value );

		input_modifier_flag_e get_keyboard_modifier_flags() const;
		void_c set_keyboard_modifier_flags( input_modifier_flag_e value );

		input_keyboard_key_e get_keyboard_key() const;
		void_c set_keyboard_key( input_keyboard_key_e value );

		input_gamepad_key_e get_gamepad_key() const;
		void_c set_gamepad_key( input_gamepad_key_e value );

		boolean_c is_pressed( input_event_c const * input_event ) const; // returns true if the given input event would trigger this shortcut.

		boolean_c operator == ( input_shortcut_c const & other ) const;

	};

	// games can use this to map controls to actions.
	// contexts define when the action can be triggered by the shortcut.
	// this makes it possible for different actions to use the same shortcut and only one of them to be invoked depending on the context.
	// when the engine's user interface manager processes input events, it 
	class input_action_c
	{
		friend class input_manager_c;

	private:
		string8_c _key; // game defined key to identify this input map by.

		string8_c _scope; // which context this action can be invoked in. if blank then this action is global scope. if set then this action is specific to a certain scope.

		input_shortcut_c _shortcut; // the shortcut that will invoke this action.

		string16_c _name;

		string16_c _description;

		boolean_c _snapshot_state;

	public:
		input_action_c();

		string8_c const & get_key() const;
		void_c set_key( string8_c const & key );

		string8_c const & get_scope() const;
		void_c set_scope( string8_c const & value );

		input_shortcut_c const & get_shortcut() const;
		input_shortcut_c & get_shortcut(); // use this to modify the shortcut.

		string16_c const & get_name() const;
		void_c set_name( string16_c const & value );

		string16_c const & get_description() const;
		void_c set_description( string16_c const & value );

		boolean_c get_snapshot_state(); // if any of the inputs are pressed at any time during a given frame, then the action's state is set to true, otherwise it is set to false.

		core_event_c< void_c, input_action_c const * > on_invoked; // occurs whenever this action is invoked from any source. the subscriber to this event should actually perform the action.

	};

	// represents an input event produced by the user.
	class input_event_c
	{
		friend class input_manager_c;
		friend class user_interface_c;

	public:
		enum type_e
		{
			type_e_none,
			type_e_mouse_key_pressed,
			type_e_mouse_key_released,
			type_e_mouse_move,
			type_e_mouse_wheel,
			type_e_keyboard_key_pressed,
			type_e_keyboard_key_released,
			type_e_character,
			type_e_gamepad_key_pressed,
			type_e_gamepad_key_released,
		};

	private:
		type_e _type; // what kind of event occurred, and which data in this structure is relevant to it.

		sint64_c _time; // when this event was recorded (don't expect it to be perfect, but it should be very close to when it actually happened), via call to ops::time_get_high_resolution_timer_count(). used for multi-click detection.

		input_mouse_key_e _mouse_key; // the mouse key which's state was changed. only used when event_type is input_event_type_e_mouse_key_pressed or input_event_type_e_mouse_key_released.
		vector32x2_c _mouse_position; // snapshot of mouse position at the time that this input event was recorded.
		vector32x2_c _mouse_position_delta; // delta position of mouse relative to last mouse_move input event. only used when event_type is input_event_type_e_mouse_move.
		float32_c _mouse_wheel_delta; // delta position of mouse wheel relative to last mouse_wheel input event. only used when event_type is input_event_type_e_mouse_wheel.

		input_keyboard_key_e _keyboard_key; // only used when event_type is input_event_type_e_mouse_key_pressed or input_event_type_e_mouse_key_released.

		char16_c _character; // the character that is being inputted. only used when event_type is input_event_type_e_character.
		uint8_c _character_repeat_count; // the number of times that the character repeats itself, if the engine is running at a smooth frame rate though then it's highly unlikely that this value will ever be greater than 1, but it is here in case it is needed. only used when event_type is input_event_type_e_character.

		input_gamepad_key_e _gamepad_key;

		input_modifier_flag_e _modifier_flags; // snapshot of modifier keys state at the time that this input event was recorded.
		boolean_c _mouse_keys_state[ input_mouse_key_e_count_ ]; // snapshot of mouse keys state at the time that this input event was recorded.
		boolean_c _keyboard_keys_state[ input_keyboard_key_e_count_ ]; // snapshot of keyboard keys state at the time that this input event was recorded.
		boolean_c _gamepad_keys_state[ input_gamepad_key_e_count_ ]; // snapshot of gamepad keys state at the time that this input event was recorded.

		sint32_c _menu_multi_click_count; // this is managed by the user interface system.
		vector32x2_c _menu_mouse_position; // this is managed by the user interface system. 2d mouse position in menu space. for 2d user interfaces this is basically screen space. for 3d user interfaces this the result of a 3d ray projected through the 3d camera onto the 3d plane of the root 3d menu control.

		boolean_c _was_handled; // is initially set to false, until some object uses or consumes the event. used with menu context event bubbling, so that an event will continue to bubble up the control heirarchy until it is processed.

	public:
		input_event_c();

		void_c reset();

		type_e get_type() const;

		sint64_c get_time() const;

		input_mouse_key_e get_mouse_key() const;
		vector32x2_c const & get_mouse_position() const;
		vector32x2_c const & get_mouse_position_delta() const;
		float32_c get_mouse_wheel_delta() const;

		input_keyboard_key_e get_keyboard_key() const;

		char16_c get_character() const;
		uint8_c get_character_repeat_count() const;

		input_gamepad_key_e get_gamepad_key() const;

		input_modifier_flag_e get_modifier_flags() const;
		boolean_c get_mouse_key_state( input_mouse_key_e key );
		boolean_c get_keyboard_key_state( input_keyboard_key_e key );
		boolean_c get_gamepad_key_state( input_gamepad_key_e key );

		vector32x2_c const & get_menu_mouse_position() const;
		sint32_c get_menu_multi_click_count() const;

		boolean_c get_was_handled() const;
		void_c set_was_handled( boolean_c value );

	};

	// not fully implemented yet.
	// but this will allow data to be dragged and dropped between menu controls.
	class input_drag_drop_payload_c
	{
	public:
		string8_c type; // hint to the program as to what kind of data is in the payload.
		core_list_c< uint8_c > data; // any kind of data. could be a string, could be a pointer, etc.

	public:
		input_drag_drop_payload_c();

	};

	input_keyboard_key_e input_convert_windows_virtual_key_code_to_cheonsa_keyboard_key_code( sint_native_c windows_virtual_key_code );
	input_gamepad_key_e input_convert_xinput_virtual_key_code_to_cheonsa_gamepad_key_code( sint16_c xinput_virtual_key_code );

}
