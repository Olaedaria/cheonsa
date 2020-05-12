#include "cheonsa_scene_component_light.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	void_c scene_component_light_c::_handle_before_removed_from_scene()
	{
		scene_component_c::_handle_before_removed_from_scene();
		_scene_object->get_scene()->remove_light( &_scene_light ); // a little loopy.
	}

	void_c scene_component_light_c::_handle_after_added_to_scene()
	{
		scene_component_c::_handle_after_added_to_scene();
		_scene_object->get_scene()->add_light( &_scene_light ); // a little loopy.
	}

	void_c scene_component_light_c::_handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator )
	{
		scene_component_c::_handle_on_world_space_transform_modified( old_world_space_transform, initiator );
		if ( _scene_object->get_scene() )
		{
			_scene_object->get_scene()->invalidate_light_probes_with_light( &_scene_light, &old_world_space_transform );
		}
	}

	scene_component_light_c::scene_component_light_c()
		: scene_component_c()
		, _scene_light()
	{
	}

	scene_component_light_c::~scene_component_light_c()
	{
	}

	scene_component_light_c * scene_component_light_c::make_new_instance()
	{
		return new scene_component_light_c();
	}

	scene_light_c const & scene_component_light_c::get_data() const
	{
		return _scene_light;
	}

	scene_light_c & scene_component_light_c::get_data()
	{
		return _scene_light;
	}

}
