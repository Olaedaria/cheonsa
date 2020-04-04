#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_menu_control.h"

namespace cheonsa
{

	// enables a 2d menu control to operate in a 3d scene.
	// menu units are in pixels, whereas scene units are in meters.
	// the size of the menu control that is associated with this component determines the size and bounds of this component.
	// 3d renderer is hard coded with a corrective scale of 0.001, so that pixels map to millimeters.
	class scene_component_menu_control_c
		: public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_menu; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	public:
		boolean_c _control_is_ours; // if true then we are responsible for deleting _control in our destructor.
		menu_control_c * _control;
		uint32_c _last_render_frame;
		void_c _give_control_to_user_interface();
		void_c _take_control_from_user_interface();
		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_before_removed_from_scene() override;

	public:
		scene_component_menu_control_c(); // constructor is protected to prevent instantiation on the stack. use make_new_instance() to make a new instance on the heap. then use delete as normal to delete it.
		virtual ~scene_component_menu_control_c() override;

		void_c set_control( menu_control_c * control, boolean_c and_transfer_ownership ); // sets the control of this menu control component.

	};

}
