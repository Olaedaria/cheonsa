#pragma once

#include "cheonsa__types.h"
#include "cheonsa_resource_file.h"
#include "cheonsa_resource_file_texture.h"

namespace cheonsa
{

	// a sprite sheet defines a list of animations, each of which references a texture and defines a list of frames, each of which defines a duration and a rectangle sub-area of the texture.
	class resource_file_sprites_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_sprites_c > reference_c;

	public:
		static char8_c const * get_resource_file_type_static() { return "sprites"; }
		virtual char8_c const * get_resource_file_type() const override { return get_resource_file_type_static(); }

	public:
		// a sprite frame defines a single frame in a sprite animation.
		class frame_c
		{
			friend class resource_file_sprites_c;

		private:
			box32x2_c _box; // xy coordinates of textured quad in pixels, relative to the sprite instance origin.
			box32x2_c _map; // uv coordinates of texture map in pixels|texels.
			float32_c _duration; // duration of this frame in seconds. if it is equal to 0 then it means hold on the frame forever.

		public:
			frame_c();

			box32x2_c const & get_box() const;
			box32x2_c const & get_map() const;
			float32_c get_duration() const;

		};

		// a sprite animation references a texture and defines a set of frames that make up an animation.
		class animation_c
		{
			friend class resource_file_sprites_c;

		private:
			string8_c _name; // name to reference this animation by.
			resource_file_texture_c::reference_c _texture_resource;
			float32_c _font_size; // this should be set to the height of the the sprite in pixels (each frame may have a different height, so this doesn't have to be exact).
			float32_c _font_horizontal_advance; // for sprites that are inlined in text, this defines how far to advance the cursor in pixels.
			core_list_c< frame_c > _frame_list;

		public:
			animation_c();

			string8_c const & get_name() const;

			resource_file_texture_c * get_texture_resource() const;

			float32_c get_font_size() const;

			float32_c get_font_horizontal_advance() const;

			core_list_c< frame_c > const & get_frame_list() const;

		};

	private:
		core_list_c< animation_c > _animation_list;

		virtual void_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_sprites_c( string16_c const & file_path );

		animation_c const * find_animation( string8_c const & name ); // looks up an animation with its name. may return nullptr.

	};

}
