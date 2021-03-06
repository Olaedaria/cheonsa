#pragma once

#include "cheonsa__types.h"
#include "cheonsa_menu_types.h"
#include "cheonsa_core_linked_list.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_scribe_structure.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_input_manager.h"
#include "cheonsa_resource_file_menu_layout.h"

namespace cheonsa
{

	class scene_component_menu_control_c;

	// menu control base class.
	//
	// a control is a rectangular thing made up of one or more rectangular elements that the user can interact with in a certain way.
	// a control can be added to as a daughter to the engine's user interface (a root level control), or it can be added as a daughter to another control (a non-root level control).
	// this class can be inherited from to create everything from simple buttons to complex color pickers and property inspectors.
	// cheonsa menus are 2d by nature, but can be placed in 3d space when _scene_component is linked (but it will still be a flat plane in 3d space).
	//
	// control lifetimes are managed by reference counting with add_reference() and remove_reference().
	// controls initially have a reference count of 0 after they are constructed.
	// if you create a control instance on the stack, call add_reference() on it right after so that it has a "fake" user so that its reference count never drops to zero and so that it never tries to delete itself.
	// adding a control as a daughter to a mother (user interface or control) increases its reference count by one.
	// removing a control from its mother reduces its reference count by one.
	// when a control's reference count drops to 0 it will delete itself.
	//
	// controls have many properties, which can be classified into categories as a way to make it easier to think about and understand.
	//   static data properties:
	//     these are things like non-user-editable text values displayed by the control.
	//     these are things like layout, transform, and style map assignment values.
	//     these values may be managed by the program, but the menu layout xml file may define these values.
	//   dynamic data and functional properties:
	//     these are things like user-editable text and number values displayed by the control.
	//     these are things like events that the program hooks up to.
	//     these may also be hidden values like the page range and scroll position of a scroll bar, which is closely integrated with the layout logic system.
	//     these values are always managed by the program, the menu layout xml file can not define these values.
	class menu_control_c
	{
		friend class user_interface_c;
		friend class menu_style_manager_c; // to call _global_resolve_style_maps.
		friend class video_renderer_interface_c;
		friend class menu_control_list_i;
		friend class menu_control_panel_i;

	public:
		static inline char8_c const * get_type_name_static() { return "control"; }
		virtual inline char8_c const * get_type_name() const { return get_type_name_static(); }

	protected:
		// the global list is used to hold a pointer to every control that is instantiated.
		// each control adds itself to this list when it is constructed.
		// each control removes itself from this list when it is destructed.
		// this lets us reload style files during run time and force controls to reacquire their style map references.
		// this lets us reload layout files during run time and force controls to reload their properties and layouts.
		static core_linked_list_c< menu_control_c * > _global_list;
		core_linked_list_c< menu_control_c * >::node_c _global_list_node;

		string8_c _name; // used to identify this control in the hierarchy, it should be unique within the context of its immediate superior (context or control), but it doesn't need to be globally unique.

		user_interface_c * _mother_user_interface; // the mother user interface, only set on the root control.

		menu_control_c * _mother_control; // the mother control
		core_linked_list_c< menu_control_c * > _daughter_control_list; // daughter controls.
		core_linked_list_c< menu_control_c * >::node_c _daughter_control_list_node;

		core_list_c< menu_element_c * > _daughter_element_list; // private daughter elements are added by this control's private implementation.
		void_c _add_daughter_element( menu_element_c * element );
		void_c _remove_daughter_element( menu_element_c * element );
		void_c _find_daughter_elements_with_name( string8_c const & name, core_list_c< menu_element_c * > & result ); // searches private daughter elements for all elements that match the given name.

		menu_control_c * _supplemental_mother_control; // if this control is a supplemental daughter control, then _supplemental_mother_control is the supplemental mother control. the _supplemental_mother_control owns the life of this control.
		core_list_c< menu_control_c * > _supplemental_daughter_control_list; // these are controls that will be added to or removed from the root level of the user interface when this control is added to or removed from the user interface.

		menu_control_c * _temporary_supplemental_mother_control; // this is intended to be used for right click context menus, which are open for a short time. we want to associate with the control that they were right clicked on so that is_ascendant_of() can determine that text focused pop up menu is related to the control that was right clicked on. this also makes it possible for the program to create a single context menu instance (for example "select all, cut, copy, paste") and use it with all text box control instances. this is a one-way link (the temporary mother doesn't hold any reference to the temporary daughter), but the temporary daughter will hold a reference count on the temporary mother (may not be needed, but doing it just to be safer, and it's not the only way to be safe, idk).

		sint32_c _index; // this control's index within its mother's _control_list or _private_control_list.
		sint32_c _grid_cell_index_x; // defines which grid cell this control occupies, when this control is added as a daughter to a grid control.
		sint32_c _grid_cell_index_y; // defines which grid cell this control occupies, when this control is added as a daughter to a grid control.

		vector32x2_c _content_offset; // scroll bars can plug in to this to scroll the daughter controls of this control.
		box32x2_c _content_bounds; // minimum axis aligned bounding box that contains all of the daughter controls and elements.
		boolean_c _content_bounds_is_dirty; // if true then _content_bounds is out of date and needs to be recalculated.

		menu_style_map_c::reference_c _style_map_reference; // the style map that is currently applied to this control.
		void_c _handle_style_map_reference_on_resolved( menu_style_map_c::reference_c const * style_map ); // responds to when _style_map_resource is loaded.
		void_c _handle_style_map_reference_on_unresolved( menu_style_map_c::reference_c const * style_map ); // responds to when _style_map_resource is unloaded.
		//void_c _handle_style_map_reference_on_refreshed( menu_style_map_c::reference_c const * value ); // resolves style map key to a style map reference, and applies the style map to daughter elements and controls.

		menu_layer_e _layer; // determines priority for hit detection and render ordering. lets control heirarchies mix layers, for example so that a pop up layer control can be a daughter to a base layer control.

		menu_select_mode_e _select_mode; // determines if and how this control responds to mouse input, if it ignores it, or blocks it and consumes it, and if this control can be selected via direcitonal input.

		boolean_c _wants_to_be_deleted; // if true, then this control is in the process of hiding (fading out), and it wants its mother to delete it once its opacity reaches 0.
		
		boolean_c _is_showed; // controls visual state. if true then this control can receive user input. if false then this control is not considered for user input.
		float32_c _is_showed_weight; // 0 is fully transparent, 1 is fully opaque.

		boolean_c _is_enabled; // controls visual state.

		sint32_c _is_deep_text_focused; // is incremented when this or any of this control's daughters gains text input focus. is decremented when this or any of this control's daughters loses text input focus. it's valid values are 0 and 1, if it settles at a value besides 0 or 1 then there's a bug with state transition management. we use a counter so that we can call _on_is_deep_text_focused only once per transition between 0 and 1, and because it may count higher than 1 if it is transitioning.
		uint8_c _is_text_focused; // controls visual state effects. is true if this control has trapped text input (will receive text events), is false if not.
		uint8_c _is_mouse_overed; // controls visual state effects for buttons. is true if this control intersects with the mouse pointer, false if not. or this may be used by the context if directional input is used to select controls rather than the mouse.
		uint8_c _is_mouse_focused; // controls visual state effects. is true if this control has trapped mouse input (will receive mouse input events even if the mouse is not over this control), is false if not.
		uint8_c _is_pressed; // controls visual state effects. will have different bits set based on which inputs are pressing this control. 0x01 for left mouse key, 0x02 for right mouse key, 0x04 for middle mouse key, and 0x08 for keyboard enter key.

		menu_layout_mode_e _layout_mode; // how to calculate _local_box.
		vector32x2_c _local_origin; // origin point defines the point that the control will rotate and scale around. it is relative to the parent's local basis.
		menu_anchor_e _local_anchor; // which sides to anchor to parent control.
		box32x2_c _local_anchor_measures; // local rectangle anchor insets, which tells the layout logic how far to place this control's _local_box edges away from the mother control's _local_box edges, and also for anchors that are to just one horizontal or vertical edge, then the inset for the opposite edge tells the layout logic the fixed width or height of the _local_box.
		box32x2_c _local_box; // rectangle extents around this control's origin. if _local_box_anchor is set then _local_box is calculated from _local_box_anchor_offsets. otherwise, it can be set directly by the program and is used as-is.
		box32x2_c _old_local_box; // used to detect changes to _local_box so that daughters can update their layout.
		float32_c _local_angle; // local angle around this control's origin.
		float32_c _local_scale; // local scale around this control's origin.
		matrix32x2x2_c _local_basis; // calculated from _local_angle and _local_scale.
		vector32x4_c _local_color; // color and opacity tint of this control, which all daughters also inherit from.

		vector32x2_c _global_origin; // final inherited origin, which is _local_origin transformed by the inherited global origin.
		matrix32x2x2_c _global_basis; // final inherited 2D basis, which is _local_basis transformed by the inherited global basis.
		matrix32x2x2_c _global_basis_inverted; // inverse of _global_basis so that global coordinates can be easily transformed to local coordinates for easy hit detection.
		vector32x4_c _global_color; // inherited color and opacity of this control.
		void_c _update_global_space_properties(); // updates global space properties of this control based on local properties and mother. these properties need to be up to date in order for mouse picking to work correctly.

		// off screen rendering system.
		// controls are batched together for rendering into what are called control (render) groups.
		// each control in the user interface that is at root level or not full opacity defines the root control of a control group.
		// each control group has an off screen render target texture in which to render all of the controls that belong to that control group.
		// for each control group, for each control (recursive), for each element, that element is rendered at (full opacity (and for the root only: no scale and no angle)) to the control group's off-screen texture.
		// control group textures are then eventually composited|rendered (with their specified opacity, scale, and angle) into other control group textures or the canvas's target texture.
		// this solves the problem that arises with the naive approach of rendering two overlapping elements at 50% opacity which results in 75% actual opacity when 50% is what is desired.
		// the _control_group_texture is only created and used for the root control of each control group. otherwise it is deleted.
		boolean_c _control_group_is_root; // will be true if this control is the root control of a control group. which is true when the control is a root control in the user interface or is not full opacity.
		menu_control_c * _control_group_mother; // if this control is the root control of a control group, then this points to the control that is the root control of the control group that is the mother of this control group.
		core_list_c< menu_control_c * > _control_group_daughter_list; // points to daughter controls that are root controls of the control groups that are daughter to this control group.
		video_texture_c * _control_group_texture; // managed by renderer, is initialized to be large enough to draw this control to (without scale or angle).
		resource_file_texture_c _control_group_texture_wrapper;
		vector32x2_c _control_group_origin; // is zero for root, is inherited for daughters.
		matrix32x2x2_c _control_group_basis; // is identity for root control of control group, is mother control's _control_group_basis times _local_basis if otherwise.
		vector32x4_c _control_group_color; // is _local_color for root, is inherited for daughters.
		menu_draw_list_c _control_group_draw_list; // used on root control of control groups. this contains the draw lists to copy the _control_group_texture to another control group, or to the canvas output.
		void_c _compile_control_groups_and_draw_lists( core_list_c< menu_control_c * > & control_group_list, core_list_c< menu_draw_list_c * > & draw_list_list ); // used by the menu renderer to recursively compile the control group state for this control and all daughter controls, in preparation to render the menus for a given frame.

		scene_component_menu_control_c * _scene_component; // if set then this menu control is in a 3d scene.

		menu_non_client_type_e _non_client_type; // lets the user interface system define areas within the client are of the engine's client window that we want the operating system to treat like non-client areas, these are things like window title bars and window edge resize handles.

		resource_file_menu_layout_c::reference_c _menu_layout_resource;
		void_c _handle_menu_layout_resource_on_loaded( resource_file_c * resource_file );
		void_c _handle_menu_layout_resource_on_unloaded( resource_file_c * resource_file );

		sint32_c _reference_count;

		void_c * _user_pointer; // can be set by the user to associate some other kind of data with this control.

		virtual void_c _handle_added_to_user_interface( user_interface_c * user_interface ); // sets _mother_user_interface to user_interface, and recurses.
		virtual void_c _handle_removed_from_user_interface(); // sets _mother_user_interface to nullptr and recurses.
		virtual void_c _handle_user_interface_local_box_changed(); // is called on root level controls when user interface local box changes, so that these controls are given an opportunity to update their layout.
		virtual void_c _on_is_showed_changed(); // is called right after _is_showed state is changed. after performing control specific implementation, it should in turn invoke the is_showed_changed event.
		virtual void_c _on_is_enabled_changed(); // is called right after _is_enabled state is changed. after performing control specific implementation, it should in turn invoke the is_enabled_changed event.
		virtual void_c _on_is_deep_text_focused_changed( menu_control_c * next_control ); // is called right after _is_deep_text_focused state is changed. after performing control specific implementation, it should in turn invoke the is_deep_text_focused_changed event.
		virtual void_c _on_is_text_focused_changed( menu_control_c * next_control ); // is called right after _is_text_focused state is changed. after performing control specific implementation, it should in turn invoke the on_is_text_focused_changed event.
		virtual void_c _on_is_mouse_overed_changed(); // is called right after _is_mouse_overed state is changed. after performing control specific implementation, it should in turn invoke the on_is_mouse_overed_changed event.
		virtual void_c _on_is_mouse_focused_changed(); // is called right after _is_mouse_focused state is changed. after performing control specific implementation, it should in turn invoke the on_is_mouse_focused_changed event.
		virtual void_c _on_is_pressed_changed(); // is called right after _is_pressed state is changed. after performing control specific implementation, it should in turn invoke the on_is_pressed_changed event.
		virtual void_c _on_clicked( input_event_c * input_event ); // is called when the user interface determines that this control has been clicked on. this can be triggered with the left mouse key, right mouse key, middle mouse key, or enter keyboard key.
		virtual void_c _on_multi_clicked( input_event_c * input_event ); // is called when the user interface determines that this control has been multi-(double, triple)-clicked on. multi-clicks are only registered with the left mouse key.
		virtual void_c _on_input( input_event_c * input_event ); // is called for any other user input events.

		void_c _update_daughter_control_animations_and_prune( float32_c time_step ); // to enable reuse of the code that removes controls that want to be removed.

		virtual void_c _update_daughter_element_animations( float32_c time_step );

		void_c _update_transform_and_layout_base(); // updates transform and layout for this control only.
		virtual void_c _update_transform_and_layout(); // calls _update_transform_and_layout_base(), then also updates layout of daughter elements, then recurses to daughter controls.

	public:
		menu_control_c();
		virtual ~menu_control_c();

		void_c add_reference();
		void_c remove_reference();

		// gets called once per frame.
		// should only update animations.
		// should not handle or respond to user input or update functional states.
		// all user input should be handled through the input event handlers.
		virtual void_c update_animations( float32_c time_step );

		// loads static data property values from the given markup node, recursive.
		// static data properties are things like non-user-editable text values, layout, transform, and style map assignments.
		// node is optional, if node is nullptr then the hierarchy state will be reset to its initial default.
		// the node should look like:
		// <control type="[control type name]" name="[name]" [additional attributes] [/]>
		void_c load_static_data_properties_recursive( data_scribe_markup_c::node_c const * node );

		// resets static data property values to a default but operable state.
		virtual void_c reset_static_data_properties();
		// loads static data property values from the given node.
		// only extracts data from the given node and applies it to this control, so it is not recursive.
		virtual void_c load_static_data_properties( data_scribe_markup_c::node_c const * node );

		// gets the menu layout of this control system.
		// this should only be set on the root control of a system (for example, the window control of a file picker dialog).
		resource_file_menu_layout_c * get_menu_layout_resource() const;
		// sets the menu layout of this control system.
		// this should only be set on the root control of a control system.
		// this menu layout resource defines layout of controls in hierarchy.
		void_c set_menu_layout_resource( resource_file_menu_layout_c * value );

		virtual menu_control_c * pick_control_with_global_point( vector32x2_c const & global_point, menu_layer_e & layer );

		vector32x2_c transform_global_point_to_local_point( vector32x2_c const & global_point ) const;
		vector32x2_c transform_local_point_to_global_point( vector32x2_c const & local_point ) const;

		boolean_c contains_global_point( vector32x2_c const & global_point ) const;
		virtual boolean_c contains_local_point( vector32x2_c const & local_point ) const; // can override if you want to implement non-rectangular shape.

		string8_c const & get_style_map_key() const;
		void_c set_style_map_key( string8_c const & value );

		string8_c const & get_name() const;
		void_c set_name( string8_c const & value );

		sint32_c get_index() const; // gets the index of this control within it's mother's daughter list.

		sint32_c get_grid_cell_index_x() const;
		void_c set_grid_cell_index_x( sint32_c value );

		sint32_c get_grid_cell_index_y() const;
		void_c set_grid_cell_index_y( sint32_c value );

		user_interface_c * get_mother_user_interface() const;

		menu_control_c * get_root_mother_control(); // returns the root mother control of this control, also called a window (regardless of if it acts like one or not), which is the control that is directly added to the menu context.

		menu_control_c * get_mother_control(); // returns the control that is the immediate mother of this control.
		menu_control_c const * get_mother_control() const; // returns the control that is the immediate mother of this control.

		core_linked_list_c< menu_control_c * > const & get_daughter_control_list() const;
		core_linked_list_c< menu_control_c * > & _get_daughter_control_list(); // occasionally non-const access is needed, for example to sort the items.
		core_linked_list_c< menu_control_c * >::node_c & _get_daughter_control_list_node(); // occasionally non-const access is needed, for example to sort the items.

		void_c add_daughter_control( menu_control_c * control, sint32_c index = -1 ); // index of -1 means insert at end. this control will hold a one-way reference count on the daughter control.
		void_c remove_daughter_control( menu_control_c * control );
		void_c remove_all_daughter_controls();
		menu_control_c * find_daughter_control( string8_c const & name, string8_c const & type );

		void_c add_supplemental_daughter_control( menu_control_c * control );
		void_c remove_supplemental_daughter_control( menu_control_c * control );

		void_c set_temporary_supplemental_mother_control( menu_control_c * control );

		boolean_c is_ascendant_of( menu_control_c * control ); // returns true if this control is equal to the given control or if the given control is a descendant of this control.

		menu_layer_e get_expressed_layer(); // returns the value of the first control up the heirarchy to have _layer not set to menu_layer_e_undefined, otherwise returns menu_layer_e_base.

		menu_select_mode_e get_select_mode() const;
		void_c set_select_mode( menu_select_mode_e value );

		boolean_c get_wants_to_be_deleted() const;

		boolean_c get_is_showed() const;
		float32_c get_is_showed_weight() const;
		virtual void_c set_is_showed( boolean_c value, boolean_c and_delete = false ); // and_delete is only relevant if value is false.
		virtual void_c set_is_showed_immediately( boolean_c value );

		boolean_c get_is_actually_enabled() const; // returns true if this control and all controls up the hierarchy are enabled, false if not.
		boolean_c get_is_enabled() const;
		void_c set_is_enabled( boolean_c value );

		sint32_c get_is_deep_text_focused() const;
		boolean_c get_is_mouse_overed() const;
		boolean_c get_is_mouse_focused() const;
		boolean_c get_is_text_focused() const;
		boolean_c get_is_pressed() const;
		menu_state_e get_state() const;

		void_c set_layout_simple( vector32x2_c const & local_origin, vector32x2_c const & local_box_size_around_origin, float32_c local_angle = 0.0f, float32_c local_scale = 1.0f ); // spatial properties are in mother's coordinate space.
		void_c set_layout_simple( vector32x2_c const & local_origin, box32x2_c const & local_box_around_origin, float32_c local_angle = 0.0f, float32_c local_scale = 1.0f ); // spatial properties are in mother's coordinate space.
		void_c set_layout_simple( box32x2_c const & local_box, float32_c local_angle = 0.0f, float32_c local_scale = 1.0f ); // spatial properties are in mother's coordinate space. origin will be placed at center of local_box.
		void_c set_layout_box_anchor( menu_anchor_e local_anchor, box32x2_c const & local_anchor_measures, float32_c local_angle = 0.0f, float32_c local_scale = 1.0f ); // enables anchor layout. measurements are in mother control's coordinate space.
		void_c set_layout_point_anchor( menu_anchor_e local_anchor, box32x2_c const & local_box, float32_c local_angle = 0.0f, float32_c local_scale = 1.0f );

		vector32x2_c const & get_local_origin() const;
		void_c set_local_origin( vector32x2_c const & value );

		box32x2_c const & get_local_box() const;

		box32x2_c const & get_local_anchor_measures() const;
		void_c set_local_anchor_measures( box32x2_c const & value );

		float32_c get_local_angle() const;
		void_c set_local_angle( float32_c value );

		float32_c get_local_scale() const;
		void_c set_local_scale( float32_c value );

		vector32x4_c const & get_local_color() const;
		void_c set_local_color( vector32x4_c const & value );

		void_c set_color_theme( string8_c const & value ); // sets the color theme of all elements using a color theme key which is used to look up a color theme instance, so that they might express a different set of colors than default. resolves (looks up) the color theme from the menu style manager by its key.
		void_c set_color_theme( menu_color_theme_c const * value ); // sets the color theme of all elements using a color theme instance, so that they might express a different set of colors than default.

		vector32x2_c const & get_global_origin() const;
		matrix32x2x2_c const & get_global_basis() const;
		matrix32x2x2_c const & get_global_basis_inverted() const;
		vector32x4_c const & get_global_color() const;

		scene_component_menu_control_c * get_scene_component() const;
		void_c set_scene_component( scene_component_menu_control_c * value );

		menu_non_client_type_e get_non_client_type() const;
		void_c set_non_client_type( menu_non_client_type_e value );

		void_c * get_user_pointer() const;
		void_c set_user_pointer( void_c * value );

		void_c bring_to_front(); // moves this control to the front of the list.

		void_c give_text_focus(); // gives this control text focus, which lets it receive text input events.

		box32x2_c const & get_content_bounds();
		vector32x2_c const & get_content_offset() const;
		void_c set_content_offset( vector32x2_c const & value );
		void_c set_content_offset_horizontal( float32_c value );
		void_c set_content_offset_vertical( float32_c value );

		// used by menu renderer to compile clip planes for this control, in preparation to render this control to the control group render target texture.
		// clip planes are not generated when rendering control group root controls since they will be rendering directly to the control group render target without angle or scale, so the rasterizer's clip rectangle serves the same purpose.
		// clip planes are generated when rendering non control group root controls, since they may be angled and scaled. the planes are placed to be relative to the control group root.
		void_c get_control_group_clip_planes( core_list_c< vector32x4_c > & result );
		matrix32x2x2_c get_control_group_basis() const;
		vector32x2_c get_control_group_origin() const;

	public:
		core_event_c< void_c, user_interface_c * > on_user_interface_association_changed;
		core_event_c< void_c, menu_event_information_c > on_is_showed_changed;
		core_event_c< void_c, menu_event_information_c > on_is_enabled_changed;
		core_event_c< void_c, menu_event_information_c > on_is_deep_text_focused_changed;
		core_event_c< void_c, menu_event_information_c > on_is_text_focused_changed;
		core_event_c< void_c, menu_event_information_c > on_is_mouse_overed_changed;
		core_event_c< void_c, menu_event_information_c > on_is_mouse_focused_changed;
		core_event_c< void_c, menu_event_information_c > on_is_pressed_changed;
		core_event_c< void_c, menu_event_information_c > on_clicked;
		core_event_c< void_c, menu_event_information_c > on_multi_clicked;
		core_event_c< void_c, menu_event_information_c > on_input;

	};

}
