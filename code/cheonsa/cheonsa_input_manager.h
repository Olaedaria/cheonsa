#pragma once

#include "cheonsa__types.h"
#include "cheonsa_input_types.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	// we use a hybrid approach of both input events and state snap shots.
	// we record and process all input events received from the OS between frames, and we translate those to the state snap shot.
	// during the frame, the game has access to both the sequence of input events that happened between the last frame and the current frame, and the state snap shot that is up to date for the current frame.
	// for systems need to handle user input in a deterministic way, input events should be used.
	// for systems that are bound to frame rate, snap shots are often easier and better, however not as reliable if frame rate is slow (fast down/up pressed/released inputs that last less than a frame will be invisible).
	//
	// information about multi clicks (double clicks and triple clicks):
	// this input manager supports detection of multi clicks, up to 100 because why not even though it's impractical.
	// multi click detection is only performed for the 3 primary mouse buttons: left, right, and middle.
	// a multi click is triggered when a mouse button is pressed and it is within a small window of time and space from the last time it was pressed.
	// super important: multi click events are triggered upon mouse key press, not release.
	// 
	// information about single clicks:
	// single click detection is not performed here, it is performed in the menu context, but here is some information about them regardless.
	// a single click is triggered when a mouse button is pressed and released on the same control, regardless of distance moved and time between events.
	// super important: single click events are triggered upon mouse key release.
	//
	// information about mouse coordinates and screen coordinates:
	// cheonsa's screen space coordinate space convention is that the origin is in the top left, and x coordinate ascends towards the right and y coordinate ascends towards the down.
	//
	// information about drag drop operations:
	// drag drop operations are managed by the input manager, which is a high level and global system.
	// this means that potentially, data can be dragged and dropped between different menu contexts and scene contexts.
	class input_manager_c
	{
	private:
		core_list_c< input_event_c > * _events; // input events that game systems need to process for the current frame.
		core_list_c< input_event_c > * _next_events; // input events that occur during the current frame and are held here to be processed in the next frame.

		vector32x2_c _mouse_position;
		input_key_state_bit_e _modifier_keys_state[ input_modifier_key_e_count_ ];
		input_key_state_bit_e _mouse_keys_state[ input_mouse_key_e_count_ ]; // this is up to date at the start of the update frame.
		input_key_state_bit_e _keyboard_keys_state[ input_keyboard_key_e_count_ ]; // this is up to date at the start of the update frame.
		input_key_state_bit_e _gamepad_keys_state[ input_gamepad_key_e_count_ ];
		static void_c _update_keys_states( input_key_state_bit_e * states, sint32_c count );

		core_list_c< input_event_c > _events_a;
		core_list_c< input_event_c > _events_b;
		boolean_c _mouse_pointer_visibility;

		vector32x2_c _mouse_position_when_key_pressed; // tracks the position that the last mouse key was pressed so that double/triple clicks can be determined. if this position changes more than a certain amount between subsequent clicks then double/triple clicks won't be registered for those keys.
		sint64_c _mouse_key_multi_click_times[ input_mouse_key_e_count_ ]; // tracks the time that each mouse key was last pressed so that double/triple clicks can be determined. if the time between subsequent clicks is greater than a certain amount then double/triple clicks won't be registered for those keys.
		sint8_c _mouse_key_multi_click_counts[ input_mouse_key_e_count_ ]; // tracks the number of valid subsequent mouse key downs that contribute to double/triple clicks. is reset to 1 for each key down that does not meet requirements. is incremented to a maximum of 3 for each key down that does. wraps around back to 1 if subsequent fast clicks are made.
		boolean_c _is_any_mouse_key_on();

		input_event_c * _emplace_input_event();
		void_c _finalize_input_event( input_event_c * input_event );

		boolean_c _drag_drop_is_in_progress;
		input_drag_drop_payload_c _drag_drop_payload;

		string8_c _action_context; // the current action context, which determines which actions can be invoked by shortcuts. if it's blank then shortcuts will be disabled.
		core_list_c< input_action_c * > _action_list; // all of the actions that can be performed in the game.

	public:
		void_c _release_all_keys(); // called by the engine when the engine's window loses focus. prevents keys from getting stuck in a pressed state if the user alt+tabs or if another program violates us for attention.

	public:
		input_manager_c();
		~input_manager_c();

		boolean_c start();

		void_c update(); // swaps input buffers.

		float32_c get_double_click_time(); // gets time in seconds that subsequent clicks of the same button need to be within to be considered as double click.
		float32_c get_double_click_space(); // gets distance in pixels that subsequent clicks of the same button need to be within to be considered as double clicks.

		boolean_c get_mouse_pointer_visibility();
		void_c set_mouse_pointer_visibility( boolean_c value );

		vector32x2_c get_mouse_pointer_position(); // gets the last known position of the mouse cursor, relative to the client window's client area.
		void_c set_mouse_pointer_position( vector32x2_c value ); // sets the position of the mouse cursor, relative to the client window's client area.

		void_c push_keyboard_key_pressed( input_keyboard_key_e keyboard_key );
		void_c push_keyboard_key_released( input_keyboard_key_e keyboard_key );
		void_c push_character( char16_c character, uint8_c character_repeat_count );
		void_c push_mouse_move( sint32_c mouse_position_x, sint32_c mouse_position_y );
		void_c push_mouse_wheel( float32_c mouse_wheel_delta );
		void_c push_mouse_key_pressed( input_mouse_key_e mouse_key );
		void_c push_mouse_key_released( input_mouse_key_e mouse_key );

		core_list_c< input_event_c > & get_events() const; // gets the list of input events to process for the current frame.

		input_key_state_bit_e get_keyboard_key_state( input_keyboard_key_e key ) const;
		boolean_c get_keyboard_key_is_pressed( input_keyboard_key_e key ) const;

	public:
		// returns true if drag drop operation is in progress, false if otherwise.
		boolean_c drag_drop_is_in_progress();
		// initiates a drag drop operation with the given payload.
		// returns true if initiation was successful, false if otherwise.
		// this will replace any existing drag drop operation.
		boolean_c drag_drop_start( string8_c payload_type, core_list_c< uint8_c > const & payload_data );
		// ends or cancels the current drag drop operation.
		void_c drag_drop_end();
		// gets the current drag drop operation payload.
		// this allows potential recipients to inspect the contents to determine if they can or can't process it, and as feed back to the user can change their visual state or the mouse cursor to indicate validity as a drop target.
		input_drag_drop_payload_c * const drag_drop_get_payload() const;

	public:
		// creates a new action and returns it.
		// the game should add all of the actions to the input manager at start up and before it creates its menus.
		// actions should be added from top to bottom, which means that if contexts are nested:
		//     "level0.level1.level2"
		// then add all actions at "" level first, then at "level0" next, then at "level0.level1" next, and so on.
		// if context is "level0.level1", and an action's context is "level0", then that action can still be activated by its shortcut if no actions at "level0.level1" also have that shortcut.
		input_action_c * create_action( string8_c const & key, string8_c const & context );
		// looks up an action that was previously added.
		input_action_c * find_action( string8_c const & key ) const;

		// gets the current action context.
		string8_c const & get_action_context() const;
		// sets the action context.
		// only actions with contexts that match the current context can be activated by their shortcut.
		// the action list is iterated through from bottom to top.
		// a string_starts_with( context, action.context ) is done for each action to determine if it can be invoked by its shortcut.
		void_c set_action_context( string8_c const & value );

		// tries to invoke an action with the given shortcut and the currently set action context.
		// if an action is invoked then it returns the action that was invoked, otherwise it returns nullptr.
		input_action_c * invoke_action_with_shortcut( input_shortcut_c const & shortcut );

		// compares the given shortcut to the current input state.
		boolean_c is_shortcut_pressed( input_shortcut_c const & shortcut ) const;

	public:
		// gets the operating system's clip board value if it is a plain text value.
		static boolean_c clip_board_get_plain_text( string16_c & value );
		// sets the operating system's clip board value to a plain text value.
		static boolean_c clip_board_set_plain_text( string16_c const & value );

	};

}
