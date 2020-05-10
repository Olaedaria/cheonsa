#include "cheonsa_sprite.h"
#include <cassert>

namespace cheonsa
{

	void_c sprite_c::_handle_sprite_sheet_resource_on_loaded( resource_file_c * resource )
	{
		assert( _sprite_sheet_resource.get_is_value_set_and_loaded() );
		_animation = _sprite_sheet_resource->find_animation( _animation_name );
		if ( _animation )
		{
			if ( _frame_index > _animation->get_frame_list().get_length() )
			{
				_frame_index = 0;
				_frame_time = 0.0f;
			}
		}
	}

	void_c sprite_c::_handle_sprite_sheet_resource_on_unloaded( resource_file_c * resource )
	{
		_animation = nullptr;
	}

	sprite_c::sprite_c()
		: _sprite_sheet_resource()
		, _animation_name()
		, _animation( nullptr )
		, _frame_index( 0 )
		, _frame_time( 0.0f )
	{
	}

	void_c sprite_c::update( float32_c time_step )
	{
		if ( _animation && _animation->get_frame_list().get_length() > 0 )
		{
			if ( _frame_index >= _animation->get_frame_list().get_length() )
			{
				_frame_index = 0;
			}

			_frame_time += time_step;
			while ( _frame_time >= _animation->get_frame_list()[ _frame_index ].get_duration() )
			{
				resource_file_sprites_c::frame_c const & frame = _animation->get_frame_list()[ _frame_index ];
				if ( frame.get_duration() == 0.0f ) // this frame lasts forever.
				{
					return;
				}
				_frame_time -= frame.get_duration();
				_frame_index++;
				if ( _frame_index >= _animation->get_frame_list().get_length() )
				{
					_frame_index = 0;
				}
			}
		}
		else
		{
			_frame_index = 0;
			_frame_time = 0.0f;
		}
	}

	resource_file_sprites_c * sprite_c::get_sprite_sheet_resource() const
	{
		return _sprite_sheet_resource;
	}

	void_c sprite_c::set_sprite_sheet_resource( resource_file_sprites_c * value )
	{
		if ( _sprite_sheet_resource )
		{
			if ( _sprite_sheet_resource->get_is_loaded() )
			{
				_handle_sprite_sheet_resource_on_unloaded( _sprite_sheet_resource );
			}
			_sprite_sheet_resource->on_loaded.unsubscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_loaded );
			_sprite_sheet_resource->on_unloaded.unsubscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_unloaded );
		}

		_sprite_sheet_resource = value;

		if ( _sprite_sheet_resource )
		{
			_sprite_sheet_resource->on_loaded.subscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_loaded );
			_sprite_sheet_resource->on_unloaded.subscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_unloaded );
			if ( _sprite_sheet_resource->get_is_loaded() )
			{
				_handle_sprite_sheet_resource_on_loaded( _sprite_sheet_resource );
			}
		}
	}

	void_c sprite_c::set_animation( string8_c const & name )
	{
		_animation_name = name;
		_animation = nullptr;
		if ( _sprite_sheet_resource.get_is_value_set_and_loaded() )
		{
			_animation = _sprite_sheet_resource->find_animation( _animation_name );
		}
		_frame_index = 0;
		_frame_time = 0.0f;
	}

	resource_file_sprites_c::animation_c const * sprite_c::get_animation() const
	{
		return _animation;
	}

	resource_file_sprites_c::frame_c const * sprite_c::get_frame() const
	{
		if ( _animation )
		{
			return &_animation->get_frame_list()[ _frame_index ];
		}
		return nullptr;
	}

}
