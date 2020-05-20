#pragma once

#include "cheonsa_scene_types.h"
#include "cheonsa_scene_tree_octo.h"
#include "cheonsa_core_linked_list.h"

namespace cheonsa
{

	class user_interface_c;

	// do not create scene component instances on the stack.
	// always use new to create scene component instances on the heap.
	// once a scene component is added to a scene object, that scene object owns it and will try to delete it when it is destructed.
	class scene_component_c
	{
		friend class scene_tree_octo_c< scene_component_c * >;
		friend class scene_object_c;

	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_none; }
		virtual inline uint8_c get_type_code() const { return get_type_code_static(); }

	protected:
		scene_object_c * _scene_object;

		core_linked_list_c< scene_component_c * >::node_c _scene_component_list_node;

		scene_tree_octo_c< scene_component_c * >::node_c * _scene_tree_leaf_node; // the leaf node that this component is stored in.
		core_linked_list_c< scene_component_c * >::node_c _scene_tree_list_node; // the list node of the list in the leaf node that the component is stored in.

		box32x3_c _local_space_obb; // local space oriented bounding box, oriented by _scene_object->_world_space_transform.
		box64x3_c _world_space_aabb; // world space axis aligned bounding box. axis aligned bounding box works better with the octal tree.
		void_c _reset_local_space_obb(); // resets _local_space_obb to a default state.
		void_c _update_world_space_abb_from_local_space_obb();

		sint32_c _reference_count;

		void_c _insert_or_update_component_tree(); // inserts or updates this component in the spatial partition of the scene.
		void_c _remove_from_component_tree(); // removes this component from the spatial partiation of the scene.

		virtual void_c _handle_after_added_to_user_interface();
		virtual void_c _handle_before_removed_from_user_interface();
		
		virtual void_c _handle_after_added_to_scene(); // is called when the scene object is removed from a scene. the scene may or may not already be associated with the engine's user interface at this point.
		virtual void_c _handle_before_removed_from_scene(); // is called when the scene object is added to a scene. the scene may or may not already be associated with the engine's user interface at this point.

		// this gets invoked by the scene object whenever the world space transform is modified.
		// the new world space transform is the scene object's current world space transform.
		// old_world_space_transform is the scene object's previous world space transform.
		// initiator may optionally be set. if it is set, it is the scene component that initiated the change. this lets the initator choose not to respond to the modification if it knows that it was the one that initiated it.
		virtual void_c _handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator );

		scene_component_c();

	public:
		virtual ~scene_component_c();

		sint32_c add_reference();
		sint32_c remove_reference();

		scene_object_c * get_scene_object() const; // gets the scene object that this component belongs to.

		box32x3_c const & get_local_space_obb() const;
		box64x3_c const & get_world_space_aabb() const;
		transform3d_c const & get_world_space_transform() const;

	};

}
