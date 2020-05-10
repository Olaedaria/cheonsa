#pragma once

#include "cheonsa__types.h"
#include "cheonsa_scene_types.h"
#include "cheonsa_core_linked_list.h"
#include "cheonsa_data_scribe_binary_property.h"

namespace cheonsa
{

	// lacks an update function because most of the time the majority of objects in the scene are static, and so to walk over them during an update frame is a waste of time.
	// if your game has a dynamic set of scene objects then it should manage its own list and walk over them and update them itself.
	// lacks typical "scene graph" hierarchy structure, it's a limitation, but not really since i don't need it.
	class scene_object_c
	{
		friend class scene_c;
		friend class scene_component_c;
		friend class video_renderer_interface_c;

	private:
		core_linked_list_c< scene_object_c * >::node_c _scene_object_list_node;

		scene_c * _scene; // the scene that this scene object is in.

		transform3d_c _world_space_transform; // scene space basis with scale and position.

		core_linked_list_c< scene_component_c * > _scene_component_list; // all of the components in the object in a doubly linked list.

		uint8_c _outline_color_index; // 0 means no outline, any other value causes this object's opaque geometry components (models and sprites) to be rendered to the outline buffer, and this value acts as an index to a color in the canvas's outline color buffer, which is used by the post process pixel shader to draw outlines of a given color around objects.

		void_c * _user_pointer; // game can use this to associate its stuff with this scene object.

		void_c _handle_after_added_to_user_interface();
		void_c _handle_before_removed_from_user_interface();

		void_c _handle_after_added_to_scene(); // for example, lets the object add its relevant components to lists in the scene.
		void_c _handle_before_removed_from_scene(); // for example, lets the object remove its relevant components from lists in the scene.

		void_c _handle_world_space_transform_modified( transform3d_c const & old_world_space_transform );

		sint32_c _reference_count;

		scene_object_c();

	public:
		~scene_object_c(); // deletes all of the components in this scene object.

		static scene_object_c * make_new_instance();

		void_c add_reference();
		void_c remove_reference();

		scene_c * get_scene() const;

		transform3d_c const & get_world_space_transform() const;
		void_c set_world_space_transform( transform3d_c const & value );

		vector64x3_c const & get_world_space_position() const;
		void_c set_world_space_position( vector64x3_c const & value );

		quaternion32_c const & get_world_space_rotation() const;
		void_c set_world_space_rotation( quaternion32_c const & value );

		vector32x3_c const & get_world_space_scale() const;
		void_c set_world_space_scale( vector32x3_c const & value );

		void_c add_component( scene_component_c * component ); // adds a component to the end of the component list. the scene object then takes ownership of the component, it will delete the component in its destructor.
		void_c remove_component( scene_component_c * component ); // removes a component and takes ownership of it, someone will need to delete it if it isn't going to be reused.
		scene_component_c * find_component( uint8_c type_code ); // finds the first component of the given type.

		core_linked_list_c< scene_component_c * > const & get_scene_component_list() const;

		uint8_c get_outline_color_index() const;
		void_c set_outline_color_index( uint8_c value );

		void_c * get_user_pointer() const;
		void_c set_user_pointer( void_c * value );

	};

}
