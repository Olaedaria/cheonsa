#pragma once

#include "cheonsa_resource_file_sprites.h"

namespace cheonsa
{

	// sprite instance.
	// manages animation play back state of a sprite sheet resource.
	// used by menu sprite controls and scene component sprites.
	class sprite_c
	{
	private:
		resource_file_sprites_c::reference_c _sprites_resource; // the sprite sheet resource that holds all of the frame lists to make available to this sprite instance.
		string8_c _sprite_name; // the name of the frame list to play back.
		resource_file_sprites_c::sprite_c const * _sprite; // pointer to the current frame list within _sprite_sheet_resource.
		sint32_c _frame_index; // the index of the current frame.
		float32_c _frame_time; // keeps track of how long the current frame has been displayed for.

		void_c _handle_sprite_sheet_resource_on_loaded( resource_file_c * resource );
		void_c _handle_sprite_sheet_resource_on_unloaded( resource_file_c * resource );

	public:
		sprite_c();

		void_c update_animations( float32_c time_step );

		resource_file_sprites_c * get_sprites_resource() const;
		void_c set_sprites_resource( resource_file_sprites_c * value );

		void_c set_sprite_with_name( string8_c const & name ); // sets the animation to play on this sprite instance. this state is applied even if no sprite sheet resource is currently bound (set and loaded).
		resource_file_sprites_c::sprite_c const * get_sprite() const; // may return nullptr.
		resource_file_sprites_c::frame_c const * get_frame() const; // may return nullptr.

		// returns true if sprites resource is loaded and contains a sprite with the given name.
		boolean_c set_sprites_resource_and_sprite_with_name( resource_file_sprites_c * sprites_resource, string8_c const & sprite_name );

	};

}
