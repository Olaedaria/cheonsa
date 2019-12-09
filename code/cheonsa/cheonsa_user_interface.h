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

	// the user interfaces is the highest level interface between the user and the game.
	//
	// it manages a canvas (collection of texture render targets) which is associated with the client window.
	//
	// it contains at most one 3d scene.
	// it may contain any number of 3d objects and 3d menu controls (2d menu controls positioned in 3d space).
	//
	// it contains any number of root level 2d menu controls.
	// these 2d menu controls may in turn contain other 2d menu controls and 3d scenes.
	//
	// it user interface delegates:
	//		routing user input events to 2d and 3d menu controls and to the game.
	//		updating animations of 2d menu controls.
	//		rendering the 3d scene.
	//		rendering the 2d menus.
	//	
	// the user's 2d screen space inputs are routed through each 2d/3d system seamlessly.
	// input focus is managed between all of these things.
	//
	// your game will probably handle user input by plugging in event handlers on the root level menu_control_scene_c instance.
	// your game may also plug in event handlers on sub level menu_control_scene_c instances.
	//
	// sorting convention for controls is that controls that come first are layered on top of controls that come later.
	// but remember that these flat lists contain a mix of 2d and 3d controls, and the 2d ones may be assigned to their own layer irrespective of their position in the list.
	// so when picking controls with the mouse, the list is walked from front to back once for all layers.
	// and when rendering controls, the list is walked from back to front several times, once for each layer.
	class user_interface_c
	{
	protected:
		friend class video_renderer_interface_c;

		video_renderer_canvas_c * _canvas_and_output; // canvas and output associated with operating system window that this user interface will render to.

		//menu_render_procedure_c _menu_render_procedure; // used to compile draw lists for 2d menus and to render 2d menus.

		scene_c * _scene; // this scene needs to be set by the game. it will be the world that the user experiences.

		core_list_c< menu_control_c * > _control_list; // list of root level daughter menu controls. this contains 2d and 3d controls. for 2d controls, controls that appear near the end of the list appear layered in front of controls that come before. for 3d controls, order is irrelevant.

		menu_control_c * _mouse_overed; // this is the control that the mouse is over, regardless of if any control has trapped mouse input focus or not.
		menu_control_c * _mouse_focused; // this is the control that has mouse input focus and which will receieve mouse events. controls will trap mouse input focus temporarily if the user initiates a click on them and then drags the mouse, so that the control will continue to receive mouse input events even if the mouse is dragged off of it.
		menu_control_c * _text_focused; // this is the control that was last clicked on and it is the control will exclusively receieve all keyboard and character input events.

		boolean_c _is_mouse_inside; // tracks if the mouse pointer intersects with any of the controls in this menu context. this lets other parts of the program to determine if the mouse ray32_c intersected with the the user interface before it tries to determine if the mouse ray32_c intersects with the scene.
		boolean_c _is_set_text_focus; // tracks if set_text_focus() is currently in the process of being processed, so that we can assert if a sequence of events nests and cascaeds where it wants to set text focus recursively and we aren't designd to deal with that.

		box32x2_c _local_box; // used to lay out root controls that use anchor layout, because they need a rectangle to anchor to. minimum will always be (0, 0). maximum will be (width, height). this is the "window" area of the user interface.

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

	public:
		user_interface_c();
		~user_interface_c();

		boolean_c start( void_c * window_handle );

		void_c update_canvas();
		void_c update( float32_c time_step ); // time_step here is the time since last update, updates animations, deletes controls that want to be deleted.
		void_c render_and_present( float32_c time_step ); // time_step here is the time since last render, which is used to calculate per-second statistics. also resolves the 3d pixel perfect pick query.

		//sint32_c get_width() const; // gets the width of the canvas associated with the client window.
		//sint32_c get_height() const; // gets the height of the canvas associated with the client window.
		box32x2_c const & get_local_box() const; // gets a rectangle that can be used to lay out 2d menu controls in the client window.
		core_event_c< void_c, user_interface_c * > on_local_box_changed; // is invoked when local box changes, and before 2d menu controls are laid out again.

		scene_c * get_scene() const; // gets the scene that is associated with this user interface.
		void_c set_scene( scene_c * scene ); // sets the scene to associate with this user interface. if the current scene has any 3d menu controls, then they will be removed from this user interface. if the new scene has any 3d menu controls, then they will be added to this user interface. you can set it to nullptr to disassociate.

		void_c add_control( menu_control_c * control, sint32_c index = -1 );
		void_c remove_control( menu_control_c * control );
		void_c bring_control_to_front( menu_control_c * control );
		core_list_c< menu_control_c * > const & get_control_list() const;

		//boolean_c is_mouse_inside(); // returns true if the mouse intersects with any normal or modal control in this context, false if otherwise.
		boolean_c has_text_focus(); // returns true if this menu context has text input focus.

		menu_control_c * get_mouse_overed() const;
		menu_control_c * get_mouse_focused() const;
		menu_control_c * get_text_focused() const;
		void_c set_text_focused( menu_control_c * menu_control );

		void_c _suspend_control( menu_control_c * control ); // called whenever an element is removed, disabled, or hidden so that the affected controls states can be returned to a default state.

		menu_non_client_type_e perform_non_client_hit_test( vector32x2_c const & point ); // the engine can use this to query if there are any controls that are serving a non-client purpose, which in turn is used by the operating system to do things like resize or move the window, or detect clicks on minimize, maximize, and close buttons. it's kinda a close relationship we have with the operating system with this function.

		void_c let_go_of_mouse_overed(); // the engine can call this when the mouse enters a non-client window area to tell the user interface to let go of _mouse_overed, so that it doesn't get stuck in a highlighted state.

		//void_c find_drop_down_rectangle( vector32x2_c const & open_at_global_origin, matrix32x2x2_c const & open_at_global_basis, box32x2_c const & open_at_local_box, vector32x2_c & local_drop_down_origin, box32x2_c & local_drop_down_rectangle );

		// popup_type determines which directions the algorithm will prefer to try to open towards.
		// global_origin and global_basis define an origin space to try to open the popup in.
		// local_around defines a rectangle (relative to global_origin and global_basis) that the popup result will try to open around.
		// local_size defines the size of the popup to open (relative to global_origin and global_basis).
		//box32x2_c find_popup_box( menu_popup_type_e popup_type, vector32x2_c const & global_origin, matrix32x2x2_c const & global_basis, box32x2_c const & local_around, vector32x2_c const & local_size );

	};

}
