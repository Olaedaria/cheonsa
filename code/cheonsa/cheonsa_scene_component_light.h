#pragma once

#include "cheonsa_scene_component.h"

namespace cheonsa
{

	// -z is light's forward.
	// same as blender.
	class scene_component_light_c : public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_light; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	public:
		scene_light_c _scene_light;

		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_before_removed_from_scene() override;

		virtual void_c _handle_before_property_modified( scene_object_property_e property ) override;
		virtual void_c _handle_after_property_modified( scene_object_property_e property ) override;

	public:
		scene_component_light_c(); // constructor is protected to prevent instantiation on the stack. use make_new_instance() to make a new instance on the heap. then use delete as normal to delete it.

		scene_light_c const & get_data() const;
		scene_light_c & get_data();

	};

}
