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
		_local_space_obb.minimum.a = -_radius;
		_local_space_obb.minimum.b = -_radius;
		_local_space_obb.minimum.c = -_radius;
		_local_space_obb.maximum.a = _radius;
		_local_space_obb.maximum.b = _radius;
		_local_space_obb.maximum.c = _radius;
	}

	scene_component_sprite_c::scene_component_sprite_c()
		: scene_component_c()
		, _material()
		, _opacity( 1.0f )
		, _render_enable( true )
		, _sprite_type( scene_sprite_type_e_real )
		, _radius( 0.5f )
		, _last_render_frame( 0 )
		, _renderer_vertex_start( 0 )
	{
	}

	scene_sprite_type_e scene_component_sprite_c::get_sprite_type() const
	{
		return _sprite_type;
	}

	void_c scene_component_sprite_c::set_sprite_type( scene_sprite_type_e value )
	{
		_sprite_type = value;
	}

	float32_c scene_component_sprite_c::get_radius() const
	{
		return _radius;
	}

	void_c scene_component_sprite_c::set_radius( float32_c value )
	{
		if ( _radius != value )
		{
			_radius = value;
			_update_local_space_obb();
			_update_world_space_abb_from_local_space_obb();
			_insert_or_update_component_tree();
		}
	}

}
