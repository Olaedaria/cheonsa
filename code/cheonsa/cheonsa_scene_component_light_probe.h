#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_video_interface.h"

namespace cheonsa
{

	class scene_component_light_probe_c
		: public scene_component_c
	{
		friend class scene_c;
		friend class video_renderer_interface_c;

	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_light_probe; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	private:
		core_linked_list_c< scene_component_light_probe_c * >::node_c _scene_light_probe_list_node;

		video_texture_c * _cube_color; // texture array of 6 elements (interpret as cube map), with a render target view for each slice, and with a texture cube shader resource view. captures incoming light energy around the probe.
		video_depth_stencil_c * _cube_depth_stencil; // texture array of 6 elements (interpret as cube map).
		boolean_c _is_up_to_date; // will be set to false whenever the light probe is invalidated, which happens if the light probe moves or any other renderable objects in its range move. is set to true after the renderer gets a chance to render its cube map.
		scene_component_model_c * _local_model; // if set, then this light probe is intended to capture ambient light for this model specifically. this model will not be rendered into the cube map (to avoid self-occlusion).

		virtual void_c _handle_before_removed_from_scene() override;
		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator ) override;

		scene_component_light_probe_c();

	public:
		~scene_component_light_probe_c() override;

		static scene_component_light_probe_c * make_new_instance();

		scene_component_model_c * get_local_model() const;
		void_c set_local_model( scene_component_model_c * value );

	};

}
