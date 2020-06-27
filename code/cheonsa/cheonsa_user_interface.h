#pragma once

#include "cheonsa_menu_types.h"
#include "cheonsa_platform_pointer.h"
#include "cheonsa_input_manager.h"
#include "cheonsa_video_renderer_canvas.h"
#include "cheonsa_menu_control_scene.h"
#include "cheonsa_scene_types.h"
#include "cheonsa_scene_object.h"

namespace cheonsa
{

	class scene_c;
	class scene_object_c;

	// the user interface is the highest level interface between the human user or player and the game.
	//
	// it manages a canvas (collection of texture render targets) which is associated with the client window.
	//
	// it can associate with one 3d scene at a time.
	// the 3d scene may contain any number of 3d objects and 3d menu controls (2d menu controls positioned in 3d space).
	//
	// it can contain any number of root level 2d menu controls.
	// these 2d menu controls may in turn contain other 2d menu controls.
	//
	// it delegates:
	//     routing user input events to 2d and 3d menu controls and to the game.
	//     updating animations of 2d menu controls.
	//     rendering the 3d scene.
	//     rendering the 2d menus.
	//
	// the user's 2d screen space inputs are routed through each 2d/3d system seamlessly.
	// input focus is managed between all of these things.
	class user_interface_c
	{
		friend class engine_c;
		friend class video_renderer_interface_c;

	private:
		box32x2_c _local_box; // used to lay out root controls that use anchor layout, because they need a rectangle to anchor to. minimum will always be (0, 0). maximum will be (width, height). this is the "window" area of the user interface.
		//box32x2_c _effective_local_box; // the client window is borderless, but it has "imaginary" edges and title bar, so this rectangle is a little smaller. this area is guranteed to be clickable by the user. this rectangle is used when finding pop up rectangles.

		video_renderer_canvas_c * _canvas_and_output; // canvas and output associated with operating system window that this user interface will render to.

		scene_c * _scene; // this scene needs to be set by the game. it will be the world that the user experiences. this scene is rendered behind all of the 2d menus.

		core_linked_list_c< menu_control_c * > _daughter_control_list; // list of root level menu controls. this contains 2d and 3d controls. for 2d controls, controls that are placed towards the end of the list appear layered in front of controls that come before. for 3d controls, they may appear anywhere in the list and order is irrelevant.

		core_list_c< menu_control_c * > _text_focused_stack; // when a modal screen is opened, the last control to have text focus is pushed onto this stack. when the modal screen closes, the control at the top of this stack is popped and given text focus again.

		menu_control_c * _mouse_overed; // this is the control that the mouse is over, regardless of if any control has trapped mouse input focus or not.
		menu_control_c * _mouse_focused; // this is the control that has mouse input focus and which will receieve mouse events. controls will trap mouse input focus temporarily if the user initiates a click on them and then drags the mouse, so that the control will continue to receive mouse input events even if the mouse is dragged off of it.
		menu_control_c * _text_focused; // this is the control that was last clicked on and it is the control that will receieve all keyboard and character input events first. these events will bubble up the control hierarchy until they are handled.

		boolean_c _is_mouse_overed; // tracks if the mouse pointer intersects with any of the controls in this menu context. this lets other parts of the program to determine if the mouse ray32_c intersected with the the user interface before it tries to determine if the mouse ray32_c intersects with the scene.
		boolean_c _is_set_text_focused_changing; // tracks if set_text_focus() is currently in the process of being processed, so that we can assert if a sequence of events nests and cascades where it wants to set text focus recursively and we aren't designd to deal with that (at this time it's easier for me to detect such a condition than to design the system to work through it).

		uint8_c _is_pressed; // tracks if mouse is pressed on nothing (the scene basically).

		menu_control_c * _multi_click_control; // the control that left mouse key down occurred, so that multi-clicks can be detected or reset.
		sint64_c _multi_click_time; // time that last left mouse key down occurred, so that multi-clicks can be detected or reset.
		vector32x2_c _multi_click_position; // position of mouse cursor when left mouse key down occurred, so that multi-clicks can be detected or reset.
		sint32_c _multi_click_count;
		float32_c _get_multi_click_time() const;
		float32_c _get_multi_click_space() const;

		// uses the cpu to pick the control under the mouse pointer.
		boolean_c _pick_control( input_event_c * input_event, menu_control_c * & result );
		// uses the cpu to pick the control under the mouse pointer.
		boolean_c _pick_control_level_2( input_event_c * input_event, menu_control_c * window, menu_layer_e minimum_layer, menu_control_c * & result, float64_c & result_distance );

		// bubbles input event to control and up the hierarchy until someone processes it.
		void_c _bubble_input_event( menu_control_c * control, input_event_c * input_event );

		// routes an input event through the menu system.
		// all raw input events are bubbled.
		// input events that are a result of interpreting inputs (clicks, multi-clicks) are not bubbled.
		void_c _process_input_event( input_event_c * input_event ); // processes a single input event.

		void_c _handle_modal_screen_on_is_showed_changed( menu_event_information_c event_information );

	private:
		user_interface_c();
		~user_interface_c();

		boolean_c start( void_c * window_handle );

		void_c update_canvas();
		void_c update( float32_c time_step ); // time_step here is the time since last update, updates animations, deletes controls that want to be deleted.
		void_c render_and_present( float32_c time_step ); // time_step here is the time since last render, which is used to calculate per-second statistics. also resolves the 3d pixel perfect pick query.

	public:
		box32x2_c const & get_local_box() const; // gets a rectangle that can be used to lay out 2d menu controls in the client window.
		core_event_c< void_c, user_interface_c * > on_local_box_changed; // is invoked when local box changes, and before 2d menu controls are laid out again.

		scene_c * get_scene() const; // gets the scene that is associated with this user interface.
		void_c set_scene( scene_c * scene ); // sets the scene to associate with this user interface. if the current scene has any 3d menu controls, then they will be removed from this user interface. if the new scene has any 3d menu controls, then they will be added to this user interface. you can set it to nullptr to disassociate.

		void_c add_daughter_control( menu_control_c * control, sint32_c index = -1 ); // adds a control to this user interface, also transfers ownership of a menu control to this user interface (which means that you aren't responsible for deleting it).
		void_c remove_daughter_control( menu_control_c * control ); // removes a control from this user interface, also takes ownership of a control away from this user interface (which means that you are now responsible for deleting it.
		void_c bring_daughter_control_to_front( menu_control_c * control ); // moves the given control (which is already in the user interface) to front (layered above all other controls in the user interface).

		// controls that do not respond to multi-clicks should call this in their _on_clicked() method.
		// controls that do respond to multi-clicks should call this in their _on_multi_clicked() method when the multi-click count is equal to the maximum multi-click count that the control is programmed to respond to. this allows multi-click response behavior to wrap around cycle around indefinitely rather than count up indefinitely.
		void_c reset_multi_click_detection();

		menu_control_c * get_mouse_overed() const;
		menu_control_c * get_mouse_focused() const;
		menu_control_c * get_text_focused() const;
		void_c set_text_focused( menu_control_c * menu_control );

		void_c _suspend_control( menu_control_c * control ); // called whenever an element is removed, disabled, or hidden so that the affected controls states can be returned to a default state.

		menu_non_client_type_e perform_non_client_hit_test( vector32x2_c const & point ); // the engine can use this to query if there are any controls that are serving a non-client purpose, which in turn is used by the operating system to do things like resize or move the window, or detect clicks on minimize, maximize, and close buttons. it's kinda a close relationship we have with the operating system with this function.

		void_c let_go_of_mouse_overed(); // the engine can call this when the mouse enters a non-client window area to tell the user interface to let go of _mouse_overed, so that it doesn't get stuck in a highlighted state.

		box32x2_c _make_pop_up_box_for_iteration( menu_pop_up_type_e type, sint32_c iteration, box32x2_c const & around, vector32x2_c const & pop_up_size );
		// local space of pop up is assumed to be default, so around_global_basis and around_global_origin define the global space of the mother control that the pop up is popping up around.
		// the result will be box in the local space defined by around_global_basis and around_global_origin, which can plug directly in to set_layout_simple() of the pop up control.
		box32x2_c _find_pop_up_box( box32x2_c const & around_box, matrix32x2x2_c const & around_global_basis, vector32x2_c const & around_global_origin, menu_pop_up_type_e pop_up_type, vector32x2_c const & pop_up_size, boolean_c give_result_in_global_space );

		box32x2_c find_context_menu_pop_up_box( vector32x2_c screen_space_point_to_spawn_pop_up_around, vector32x2_c const & pop_up_size ); // prefers to open down and to the right of the given point.
		box32x2_c find_sub_menu_pop_up_box( menu_control_c * menu_item_to_spawn_pop_up_around, vector32x2_c const & pop_up_size, boolean_c give_result_in_global_space ); // prefers to open down and to the right of the given box defined by the given control.
		box32x2_c find_combo_list_pop_up_box( menu_control_c * combo_button_to_spawn_pop_up_around, vector32x2_c const & pop_up_size, boolean_c give_result_in_global_space ); // prefers to open down of the given box defined by the given control.

		// opens a new modal screen to block the user's ability to interact with anything behind it.
		// after opening a modal screen, the next controls that are added or brought to front will appear over the modal screen.
		// it forces the user to interact only with the controls that are over the modal screen.
		// when the modal screen is ready to be dismissed, call set_is_showed( false, true ) on it to make it fade out and delete.
		menu_control_c * open_modal_screen();

		// is invoked when a mouse key is pressed and released on no control.
		core_event_c< void_c, input_event_c * > on_clicked;

		// input events that bubble to the top without being handled invoke this event.
		// this lets the game do something with them if it wants to.
		core_event_c< void_c, input_event_c * > on_input;

	};

}
