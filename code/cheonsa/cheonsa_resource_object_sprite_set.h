#pragma once

#include "cheonsa_resource_object.h"
#include "cheonsa_resource_object_texture.h"
#include "cheonsa_math_types.h"

namespace cheonsa
{

	// a sprite set defines sprite animations, which reference a texture, and defines a list of frames, each with a duration and a rectangle.
	class resource_object_sprite_set_c : public resource_object_c
	{
	public:
		typedef resource_object_reference_c< resource_object_sprite_set_c > reference_c;

	public:
		static char8_c const * get_type_static() { return "sprite"; }
		virtual char8_c const * get_type() const override { return get_type_static(); }

	public:
		// a frame within a sprite animation.
		struct frame_c
		{
			box32x2_c box; // xy coordinates of textured quad in pixels, relative origin sprite instance.
			box32x2_c map; // uv coordinates of texture map in pixels|texels.
			float32_c duration; // duration of this frame in seconds. if it is equal to 0 then it means hold on the frame forever.
		};

		// a sprite references a texture and defines a set of frames that make up an animation.
		struct sprite_c
		{
			string8_c name; // name to reference this animation by.
			resource_object_texture_c::reference_c texture_resource;
			float32_c font_size; // for sprites that are inlined in text, this defines the native font size of this sprite. then the sprite will be scaled appropriately depending on the text size that is being rendered. if left unset, will be set to 1 for safety (since it is used in a division to how to scale at a given font size).
			float32_c font_horizontal_advance; // for sprites that are inlined in text, this defines how far to advance the cursor after printing the sprite.
			core_list_c< frame_c > frame_list;
		};

	private:
		core_list_c< sprite_c > _sprite_list;

		virtual boolean_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_object_sprite_set_c();
		resource_object_sprite_set_c( resource_object_sprite_set_c const & ) = delete;
		virtual ~resource_object_sprite_set_c() override;
		resource_object_sprite_set_c & operator = ( resource_object_sprite_set_c const & ) = delete;

		sprite_c const * find_sprite( string8_c const & name ) const;

	};

}
