#include "cheonsa_sprite.h"
#include <cassert>

namespace cheonsa
{

	void_c sprite_c::_handle_sprite_sheet_resource_on_loaded( resource_file_c * resource )
	{
		assert( _sprites_resource.get_is_value_set_and_loaded() );
		_sprite = _sprites_resource->find_sprite( _sprite_name );
		if ( _sprite )
		{
			if ( _frame_index > _sprite->get_frame_list().get_length() )
			{
				_frame_index = 0;
				_frame_time = 0.0f;
			}
		}
	}

	void_c sprite_c::_handle_sprite_sheet_resource_on_unloaded( resource_file_c * resource )
	{
		_sprite = nullptr;
	}

	sprite_c::sprite_c()
		: _sprites_resource()
		, _sprite_name()
		, _sprite( nullptr )
		, _frame_index( 0 )
		, _frame_time( 0.0f )
	{
	}

	void_c sprite_c::update_animations( float32_c time_step )
	{
		if ( _sprite && _sprite->get_frame_list().get_length() > 0 )
		{
			if ( _frame_index >= _sprite->get_frame_list().get_length() )
			{
				_frame_index = 0;
			}

			_frame_time += time_step;
			while ( _frame_time >= _sprite->get_frame_list()[ _frame_index ].get_duration() )
			{
				resource_file_sprites_c::frame_c const & frame = _sprite->get_frame_list()[ _frame_index ];
				if ( frame.get_duration() == 0.0f ) // this frame lasts forever.
				{
					return;
				}
				_frame_time -= frame.get_duration();
				_frame_index++;
				if ( _frame_index >= _sprite->get_frame_list().get_length() )
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

	resource_file_sprites_c * sprite_c::get_sprites_resource() const
	{
		return _sprites_resource;
	}

	void_c sprite_c::set_sprites_resource( resource_file_sprites_c * value )
	{
		if ( _sprites_resource != value )
		{
			if ( _sprites_resource )
			{
				_sprite = nullptr;
				_sprites_resource->on_loaded.unsubscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_loaded );
				_sprites_resource->on_unloaded.unsubscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_unloaded );
			}

			_sprites_resource = value;

			if ( _sprites_resource )
			{
				_sprites_resource->on_loaded.subscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_loaded );
				_sprites_resource->on_unloaded.subscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_unloaded );
				if ( _sprites_resource->get_is_loaded() )
				{
					_handle_sprite_sheet_resource_on_loaded( _sprites_resource );
				}
			}
		}
	}

	void_c sprite_c::set_sprite_with_name( string8_c const & name )
	{
		_sprite_name = name;
		_sprite = nullptr;
		if ( _sprites_resource.get_is_value_set_and_loaded() )
		{
			_sprite = _sprites_resource->find_sprite( _sprite_name );
		}
		_frame_index = 0;
		_frame_time = 0.0f;
	}

	resource_file_sprites_c::sprite_c const * sprite_c::get_sprite() const
	{
		return _sprite;
	}

	resource_file_sprites_c::frame_c const * sprite_c::get_frame() const
	{
		if ( _sprite )
		{
			return &_sprite->get_frame_list()[ _frame_index ];
		}
		return nullptr;
	}

	boolean_c sprite_c::set_sprites_resource_and_sprite_with_name( resource_file_sprites_c * sprites_resource, string8_c const & sprite_name )
	{
		if ( _sprites_resource != sprites_resource )
		{
			if ( _sprites_resource )
			{
				_sprite = nullptr;
				_sprites_resource->on_loaded.unsubscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_loaded );
				_sprites_resource->on_unloaded.unsubscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_unloaded );
			}

			_sprites_resource = sprites_resource;

			if ( _sprites_resource )
			{
				_sprites_resource->on_loaded.subscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_loaded );
				_sprites_resource->on_unloaded.subscribe( this, &sprite_c::_handle_sprite_sheet_resource_on_unloaded );
			}
		}

		_sprite_name = sprite_name;

		if ( _sprites_resource.get_is_value_set_and_loaded() )
		{
			_handle_sprite_sheet_resource_on_loaded( _sprites_resource );
			return _sprite != nullptr;
		}

		return false;
	}

}
