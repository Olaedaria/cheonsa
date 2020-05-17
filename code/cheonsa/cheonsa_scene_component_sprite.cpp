#include "cheonsa_scene_component_sprite.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"

namespace cheonsa
{

	void_c scene_component_sprite_c::_handle_after_added_to_scene()
	{
		//_scene_object->_scene->_sprite_list.insert_at_end( this );
	}

	void_c scene_component_sprite_c::_handle_before_removed_from_scene()
	{
		//_scene_object->_scene->_sprite_list.remove( this );
	}

	void_c scene_component_sprite_c::_update_local_space_obb()
	{
		_local_space_obb.minimum.a = -_size;
		_local_space_obb.minimum.b = -_size;
		_local_space_obb.minimum.c = -_size;
		_local_space_obb.maximum.a = _size;
		_local_space_obb.maximum.b = _size;
		_local_space_obb.maximum.c = _size;
	}

	scene_component_sprite_c::scene_component_sprite_c()
		: scene_component_c()
		, _sprite()
		, _material()
		, _opacity( 1.0f )
		, _render_enable( true )
		, _sprite_type( scene_sprite_type_e_fixed )
		, _size( 0.5f )
		, _last_render_frame( 0 )
		, _renderer_vertex_start( 0 )
	{
	}

	scene_component_sprite_c::~scene_component_sprite_c()
	{
	}

	scene_component_sprite_c * scene_component_sprite_c::make_new_instance()
	{
		return new scene_component_sprite_c();
	}

	sprite_c & scene_component_sprite_c::get_sprite()
	{
		return _sprite;
	}

	scene_sprite_type_e scene_component_sprite_c::get_sprite_type() const
	{
		return _sprite_type;
	}

	void_c scene_component_sprite_c::set_sprite_type( scene_sprite_type_e value )
	{
		_sprite_type = value;
	}

	float32_c scene_component_sprite_c::get_size() const
	{
		return _size;
	}

	void_c scene_component_sprite_c::set_size( float32_c value )
	{
		if ( _size != value )
		{
			_size = value;
			_update_local_space_obb();
			_update_world_space_abb_from_local_space_obb();
			_insert_or_update_component_tree();
		}
	}

}
