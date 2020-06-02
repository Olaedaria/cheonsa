#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_sprite.h"
#include "cheonsa_resource_file_sprites.h"
#include "cheonsa_video_interface.h"

namespace cheonsa
{

	// a single sprite, which is a textured quad, which is made up of four vertices and two triangles.
	class scene_component_sprite_c
		: public scene_component_c
	{
		friend class video_renderer_interface_c;

	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_sprite; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	private:
		sprite_c _sprite; // the sprite instance that drives the animation and state.

		scene_material_c _material;

		float32_c _opacity;
		boolean_c _render_enable;
		scene_sprite_type_e _sprite_type;
		float32_c _size; // width and height of sprite in meters.
		uint32_c _last_render_frame; // used by the renderer to track the last frame number that this sprite had its vertices generated, so that it does not have its vertices generated more than once per render frame.
		sint32_c _renderer_vertex_start; // used by the renderer to keep track of where the 4 vertices that make up this sprite are located at within the renderer's sprite vertex buffer.

		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_before_removed_from_scene() override;

		void_c _update_local_space_obb();

	public:
		scene_component_sprite_c();
		virtual ~scene_component_sprite_c() override;

		sprite_c & get_sprite();

		scene_sprite_type_e get_sprite_type() const;
		void_c set_sprite_type( scene_sprite_type_e value );

		float32_c get_size() const;
		void_c set_size( float32_c value );

	};

}
