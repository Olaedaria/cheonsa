#include "cheonsa_sprite.h"
#include "cheonsa_debug.h"

namespace cheonsa
{

	void_c sprite_c::_handle_sprite_set_resource_on_load( resource_object_c * resource )
	{
		_sprite = _sprite_set_resource->find_sprite( _sprite_name );
		if ( _sprite )
		{
			if ( _current_frame > _sprite->frame_list.get_length() )
			{
				_current_frame = 0;
				_current_frame_time = 0.0f;
			}
		}
	}

	void_c sprite_c::_handle_sprite_set_resource_on_unload( resource_object_c * resource )
	{
		_sprite = nullptr;
	}

	sprite_c::sprite_c()
		: _sprite_set_resource()
		, _sprite_name()
		, _sprite( nullptr )
		, _current_frame( 0 )
		, _current_frame_time( 0.0f )
	{
	}

	void_c sprite_c::update( float32_c time_step )
	{
		if ( _sprite )
		{
			_current_frame_time += time_step;
			if ( _current_frame >= _sprite->frame_list.get_length() )
			{
				_current_frame = 0;
			}

			while ( _current_frame_time >= _sprite->frame_list[ _current_frame ].duration )
			{
				if ( _sprite->frame_list[ _current_frame ].duration <= 0.001f )
				{
					return;
				}
				_current_frame_time -= _sprite->frame_list[ _current_frame ].duration;
				_current_frame++;
				if ( _current_frame >= _sprite->frame_list.get_length() )
				{
					_current_frame = 0;
				}
			}
		}
		else
		{
			_current_frame = 0;
			_current_frame_time = 0.0f;
		}
	}

	resource_object_sprite_set_c * sprite_c::get_sprite_set_resource() const
	{
		return _sprite_set_resource;
	}

	void_c sprite_c::set_sprite_set_resource( resource_object_sprite_set_c * value )
	{
		if ( _sprite_set_resource )
		{
			_sprite_set_resource->on_load.unsubscribe( this, &sprite_c::_handle_sprite_set_resource_on_load );
			_sprite_set_resource->on_unload.unsubscribe( this, &sprite_c::_handle_sprite_set_resource_on_unload );
			if ( _sprite_set_resource->get_is_loaded() )
			{
				_handle_sprite_set_resource_on_unload( nullptr );
			}
		}

		_sprite_set_resource = value;

		if ( _sprite_set_resource )
		{
			_sprite_set_resource->on_load.subscribe( this, &sprite_c::_handle_sprite_set_resource_on_load );
			_sprite_set_resource->on_unload.subscribe( this, &sprite_c::_handle_sprite_set_resource_on_unload );
			if ( _sprite_set_resource->get_is_loaded() )
			{
				_handle_sprite_set_resource_on_load( nullptr );
			}
		}
	}

	string8_c const & sprite_c::get_sprite_name() const
	{
		return _sprite_name;
	}

	void_c sprite_c::set_sprite_name( string8_c const & value )
	{
		_sprite_name = value;
		_current_frame = 0;
		_current_frame_time = 0.0f;
		if ( _sprite_set_resource.is_reference_set_and_loaded() )
		{
			_sprite = _sprite_set_resource->find_sprite( _sprite_name );
		}
	}

	resource_object_sprite_set_c::sprite_c const * sprite_c::get_sprite() const
	{
		cheonsa_assert( _sprite );
		return _sprite;
	}

	resource_object_sprite_set_c::frame_c const * sprite_c::get_frame() const
	{
		cheonsa_assert( _sprite );
		return &_sprite->frame_list[ _current_frame ];
	}

	boolean_c sprite_c::get_is_ready() const
	{
		return _sprite != nullptr && _sprite->texture_resource.is_reference_set_and_loaded();
	}

}
