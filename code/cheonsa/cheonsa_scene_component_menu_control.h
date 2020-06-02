#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_menu_control.h"

namespace cheonsa
{

	// enables a 2d menu control to operate in a 3d scene.
	// the controls are projected onto the xy axes of thd 3d object.
	// menu units are in pixels, whereas scene units are in meters.
	// the size of the menu control that is associated with this component determines the size and bounds of this component.
	// 3d renderer is hard coded with a corrective scale of 0.001, so that pixels map to millimeters.
	class scene_component_menu_control_c
		: public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_menu_control; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	private:
		menu_control_c * _control;

		virtual void_c _handle_before_removed_from_user_interface() override;
		virtual void_c _handle_after_added_to_user_interface() override;

		virtual void_c _handle_before_removed_from_scene() override;
		virtual void_c _handle_after_added_to_scene() override;

	public:
		scene_component_menu_control_c();
		virtual ~scene_component_menu_control_c() override;

		menu_control_c * get_control() const;
		void_c set_control( menu_control_c * control );

	};

}
