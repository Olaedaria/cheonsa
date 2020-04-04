#include "cheonsa_scene_component_light.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	void_c scene_component_light_c::_handle_after_added_to_scene()
	{
		scene_component_c::_handle_after_added_to_scene();
		_scene_object->get_scene()->add_light( &_scene_light ); // a little loopy.
	}

	void_c scene_component_light_c::_handle_before_removed_from_scene()
	{
		scene_component_c::_handle_before_removed_from_scene();
		_scene_object->get_scene()->remove_light( &_scene_light ); // a little loopy.
	}

	void_c scene_component_light_c::_handle_before_property_modified( scene_object_property_e property )
	{
		_scene_light.set_world_space_transform( _scene_object->get_world_space_transform() );
	}

	void_c scene_component_light_c::_handle_after_property_modified( scene_object_property_e property )
	{
		_scene_light.set_world_space_transform( _scene_object->get_world_space_transform() );
	}

	scene_component_light_c::scene_component_light_c()
		: scene_component_c()
		, _scene_light()
	{
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
