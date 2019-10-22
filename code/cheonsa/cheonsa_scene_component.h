#pragma once

#include "cheonsa_scene_types.h"
#include "cheonsa_scene_tree_octo.h"
#include "cheonsa_core_linked_list.h"

namespace cheonsa
{

	// do not create scene component instances on the stack.
	// always use new to create scene component instances on the heap.
	// once a scene component is added to a scene object, that scene object owns it and will try to delete it when it is destructed.
	class scene_component_c
	{
	public:
		static scene_component_c * make_new_instance( uint8_c type_code );

	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_none; }
		virtual inline uint8_c get_type_code() const { return get_type_code_static(); }

	protected:
		friend class scene_tree_octo_c< scene_component_c * >;
		friend class scene_object_c;

		scene_object_c * _scene_object;

		core_linked_list_c< scene_component_c * >::node_c _component_list_node;
		scene_tree_octo_c< scene_component_c * >::node_c * _scene_tree_leaf_node; // the leaf node that this component is stored in.
		core_linked_list_c< scene_component_c * >::node_c _scene_tree_list_node; // the list node of the list in the leaf node that the component is stored in.

		box32x3_c _local_space_obb; // local space oriented bounding box, oriented by _scene_object->_world_space_transform.
		box64x3_c _world_space_aabb; // world space axis aligned bounding box. axis aligned bounding box works better with the octal tree.
		void_c _reset_local_space_obb(); // resets _local_space_obb to a default state.
		void_c _update_world_space_abb_from_local_space_obb();

		virtual void_c _handle_after_added_to_scene(); // is called when the component is added to the scene, which can occur when the component is added to a scene object that is already in the scene or when the scene object that the component belongs to is added to the scene.
		virtual void_c _handle_before_removed_from_scene(); // is called when the component is removed from the scene, which can occur when the component is removed from a scene object that is already in the scene or when the scene object that the component belongs to is removed from the scene.

		virtual void_c _handle_before_property_modified( scene_object_property_e property ); // is called by when one or more spatial properties on _scene_object is about to change.
		virtual void_c _handle_after_property_modified( scene_object_property_e property ); // is called by when one or more spatial properties on _scene_object was just changed.

		void_c _insert_or_update_component_tree(); // inserts or updates this component in the spatial partition of the scene.
		void_c _remove_from_component_tree(); // removes this component from the spatial partiation of the scene.

	public:
		// you should always allocate scene components on the heap.
		// you can allocate them on the stack, but then you have to remove them from scene objects before they go out of scope.
		scene_component_c();
		virtual ~scene_component_c();

		scene_object_c * get_scene_object() const; // gets the scene object that this component is attached to.

		box32x3_c const & get_local_space_obb() const;
		box64x3_c const & get_world_space_aabb() const;
		space_transform_c const & get_world_space_transform() const;

	};

}
