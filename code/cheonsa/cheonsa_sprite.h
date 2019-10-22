#pragma once

#include "cheonsa_resource_object_sprite_set.h"

namespace cheonsa
{

	// sprite instance.
	// manages play back state of a sprite resource.
	class sprite_c
	{
	private:
		resource_object_sprite_set_c::reference_c _sprite_set_resource;
		string8_c _sprite_name; // the name of the sprite within the sprite set to play.
		resource_object_sprite_set_c::sprite_c const * _sprite; // pointer to sprite within _sprite_resource with name of _sprite_name.
		sint32_c _current_frame; // index of the current frame.
		float32_c _current_frame_time; // how long the current frame was displayed.

		void_c _handle_sprite_set_resource_on_load( resource_object_c * resource );
		void_c _handle_sprite_set_resource_on_unload( resource_object_c * resource );

	public:
		sprite_c();

		void_c update( float32_c time_step );

		resource_object_sprite_set_c * get_sprite_set_resource() const;
		void_c set_sprite_set_resource( resource_object_sprite_set_c * value );

		string8_c const & get_sprite_name() const;
		void_c set_sprite_name( string8_c const & value );

		resource_object_sprite_set_c::sprite_c const * get_sprite() const;
		resource_object_sprite_set_c::frame_c const * get_frame() const;

		boolean_c get_is_ready() const; // reutrns true if sprite set resource is loaded and sprite is set and sprite texture is loaded.

	};

}
