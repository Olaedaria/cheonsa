#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_video_interface.h"

namespace cheonsa
{

	class scene_component_light_probe_c : public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_light_probe; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	protected:
		friend class scene_c;
		friend class scene_component_model_c;
		friend class video_renderer_interface_c;

		core_linked_list_c< scene_component_light_probe_c * >::node_c _scene_light_probe_list_node;

		video_texture_c * _cube_color; // texture array of 6 elements (interpret as cube map), with a render target view for each slice, and with a texture cube shader resource view. captures incoming light energy around the probe.
		video_depth_stencil_c * _cube_depth_stencil;
		boolean_c _is_up_to_date; // will be set to false whenever the light probe is invalidated, which happens if the light probe moves or any other renderable objects in its range move.
		scene_component_model_c * _local_model; // used directly by the renderer and the model component, this model will not be baked into this light probe, but this light probe will capture lighting around the model and then be used to light that model.

		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_before_removed_from_scene() override;

		virtual void_c _handle_after_property_modified( scene_object_property_e property );

	public:
		scene_component_light_probe_c(); // constructor is protected to prevent instantiation on the stack. use make_new_instance() to make a new instance on the heap. then use delete as normal to delete it.

	};

}
