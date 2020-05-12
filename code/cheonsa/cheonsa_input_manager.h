#pragma once

#include "cheonsa_types.h"
#include "cheonsa_input_types.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	// we use a hybrid approach of both input events and state snap shots.
	// we record and process all input events received from the OS between frames, and we translate those to the state snapshot.
	// during the frame, the game has access to both the sequence of input events that happened between the last frame and the current frame, and the state snapshot that is up to date for the current frame.
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
		friend class window_manager_members_c;

	private:
		core_list_c< input_event_c > * _events_current; // input events for the current frame. points to _events_ping or _events_pong.
		core_list_c< input_event_c > * _events_next; // input events for the next frame. points to the other list that _events_current does not point to.
		core_list_c< input_event_c > _events_ping;
		core_list_c< input_event_c > _events_pong;

		input_modifier_flag_e _event_modifier_flags; // snapshot of modifier key state, updated and saved with each input event.
		vector32x2_c _event_mouse_position; // snapshot of mouse position state, updated and saved with each input event.
		boolean_c _event_mouse_keys_state[ input_mouse_key_e_count_ ]; // snapshot of mouse keys state, updated and saved with each input event.
		boolean_c _event_keyboard_keys_state[ input_keyboard_key_e_count_ ]; // snapshot of keyboard keys state, updated and saved with each input event.
		boolean_c _event_gamepad_keys_state[ input_gamepad_key_e_count_ ]; // snapshot of gamepad keys state, updated and saved with each input event.

		vector32x2_c _frame_mouse_position_last; // position of mouse at start of last frame.
		vector32x2_c _frame_mouse_position; // position of mouse at start of current frame.
		vector32x2_c _frame_mouse_position_delta; // change in position between start of last frame and start of current frame.
		boolean_c _frame_mouse_keys_state[ input_mouse_key_e_count_ ]; // snapshot of mouse keys state at start of current frame.
		boolean_c _frame_keyboard_keys_state[ input_keyboard_key_e_count_ ]; // snapshot of keyboard keys state at start of current frame.
		input_gamepad_state_c _frame_gamepad_state; // snapshot of gamepad keys state at start of current frame.

		boolean_c _mouse_pointer_visibility;

		boolean_c _drag_drop_is_in_progress;
		input_drag_drop_payload_c _drag_drop_payload;

		input_event_c * _emplace_input_event();
		void_c _finalize_input_event( input_event_c * input_event );

		void_c _push_keyboard_key_pressed( input_keyboard_key_e keyboard_key );
		void_c _push_keyboard_key_released( input_keyboard_key_e keyboard_key );
		void_c _push_character( char16_c character, uint8_c character_repeat_count );
		void_c _push_mouse_move( sint32_c mouse_position_x, sint32_c mouse_position_y );
		void_c _push_mouse_wheel( float32_c mouse_wheel_delta );
		void_c _push_mouse_key_pressed( input_mouse_key_e mouse_key );
		void_c _push_mouse_key_released( input_mouse_key_e mouse_key );
		void_c _push_gamepad_key_pressed( input_gamepad_key_e gamepad_key );
		void_c _push_gamepad_key_released( input_gamepad_key_e gamepad_key );

		void_c _release_all_keys(); // is called when the engine's window loses focus. prevents keys from getting stuck in a pressed state if the user alt+tabs or if another program violates us for attention.

	public:
		input_manager_c();
		~input_manager_c();

		boolean_c start(); // doesn't do anything right now.

		void_c update(); // swaps input buffers.

		boolean_c get_mouse_pointer_visibility();
		void_c set_mouse_pointer_visibility( boolean_c value );

		//vector32x2_c get_mouse_pointer_position(); // gets the last known position of the mouse cursor, relative to the client window's client area.
		void_c set_mouse_pointer_position( vector32x2_c value ); // sets the position of the mouse cursor, relative to the client window's client area.

		core_list_c< input_event_c > & get_events() const; // gets the list of input events to process for the current frame.

		//boolean_c get_mouse_key_is_pressed( input_mouse_key_e key ) const; // gets state of a key in the frame's input state snapshot.
		//boolean_c get_keyboard_key_is_pressed( input_keyboard_key_e key ) const; // gets state of a key in the frame's input state snapshot.

		vector32x2_c get_frame_snapshot_mouse_position() const; // gets the position of the mouse pointer at the start of the current frame, relative to the client window's client area.
		vector32x2_c get_frame_snapshot_mouse_position_delta() const; // gets the delta position of the mouse pointer between the start of the last frame and the start of the current frame.
		boolean_c get_frame_snapshot_mouse_key_state( input_mouse_key_e mouse_key ) const;
		boolean_c get_frame_snapshot_keyboard_key_state( input_keyboard_key_e keyboard_key ) const;
		input_gamepad_state_c const & get_frame_snapshot_gamepad_state() const;

		// the left motor is a heavier weight, higher amplitude, lower frequency.
		// the right motor is a lighter weight, lower amplitude, higher frequency.
		void_c set_gamepad_vibration( float32_c left_motor_speed, float32_c right_motor_speed ) const;

	public:
		// returns true if drag drop operation is in progress, false if otherwise.
		boolean_c drag_drop_is_in_progress();
		// initiates a drag drop operation with the given payload.
		// returns true if initiation was successful, false if otherwise.
		// this will replace any existing drag drop operation.
		boolean_c drag_drop_start( string8_c const & payload_type, core_list_c< uint8_c > const & payload_data );
		// ends or cancels the current drag drop operation.
		void_c drag_drop_end();
		// gets the current drag drop operation payload.
		// this allows potential recipients to inspect the contents to determine if they can or can't process it, and as feed back to the user can change their visual state or the mouse cursor to indicate validity as a drop target.
		input_drag_drop_payload_c * const drag_drop_get_payload() const;

	private:
		string8_c _action_scope; // the current scope, which determines which actions are invoked by input events.
		core_list_c< input_action_c * > _action_list; // all of the actions that can be performed in the game.

	public:
		// adds an action to the end of the action list.
		// the whole list should be initialized by the game at game initialization.
		void_c add_action( input_action_c * action );

		// looks up an action that was previously added.
		input_action_c * find_action( string8_c const & key ) const;

		// gets the current action context.
		string8_c const & get_action_scope() const;
		// sets the action context.
		// only actions with contexts that match the current context can be activated by their shortcut.
		// the action list is iterated through from bottom to top.
		// a string_starts_with( context, action.context ) is done for each action to determine if it can be invoked by its shortcut.
		void_c set_action_scope( string8_c const & value );

		void_c _update_action_snapshot_states();

		// called by the user interface manager when processing each input event.
		// this gives input actions the opportunity to respond first, then (regardless of if an action is invoked or not) the input event is routed normally through the rest of the menu system and then the game.
		void_c _invoke_action_with_input_event( input_event_c * input_event );

		// if an action exists that is associated with the given shortcut, and if it's context matches the current context, then it tries to invoke that action.
		// if an action is invoked then it returns the action that was invoked, otherwise it returns nullptr.
		//input_action_c * _invoke_action_with_shortcut( input_shortcut_c const & shortcut );

		// compares the given shortcut to the current input state.
		//boolean_c is_shortcut_pressed( input_shortcut_c const & shortcut ) const;

	public:
		// gets the operating system's clip board value if it is a plain text value.
		static boolean_c get_plain_text_clip_board_value( string16_c & value );
		// sets the operating system's clip board value to a plain text value.
		static boolean_c set_plain_text_clip_board_value( string16_c const & value );

	};

}
