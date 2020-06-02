#pragma once

#include "cheonsa_scene_component.h"

namespace cheonsa
{

	// shines a light into the scene.
	// +y axis is the light's forward axis for spot and directional light types.
	class scene_component_light_c
		: public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_light; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	private:
		scene_light_c _scene_light;

		virtual void_c _handle_before_removed_from_scene() override;
		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator ) override;

	public:
		scene_component_light_c();
		virtual ~scene_component_light_c() override;

		scene_light_c const & get_scene_light() const;
		scene_light_c & get_scene_light();

	};

}
