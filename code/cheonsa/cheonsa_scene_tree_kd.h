#pragma once

#include "cheonsa_math_types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_linked_list.h"

namespace cheonsa
{

	class scene_component_c;

	// what does "kd" even mean?
	// whatever, it's a type of binary tree, uses separating planes to recursively partition space in half.
	// used to hold light probes to optimize nearest neighbor search, versus a brute force search.
	class scene_tree_kd_c
	{
	public:
		class node_c
		{
		public:
			scene_tree_kd_c * _tree;
			node_c * _mother;
			node_c * _positive_daughter; // sub-node with a value that is on the positive side of the _separating_plane.
			node_c * _negative_daughter; // sub-node with a value that is on the negative side of the _separating_plane.
			core_list_c< scene_component_c * > * _positive_value_list; // values that lie above _separating_plane.
			core_list_c< scene_component_c * > * _negative_value_list; // values that lie below _separating_plane.
			sint32_c _separating_axis; // 0 is x, 1 is y, 2 is z.
			float64_c _separating_distance; // distance from origin along axis defined by _separating_axis.
			//plane64_c _separating_plane; // separating plane is placed in the middle of this node's value's position and the mother node's value's position, and is oriented with the positive side facing this node's value's position. except for the root node, who's separating plane is always normal( 0, 0, 1 ) distance( 0 ).

			void_c _build( core_list_c< scene_component_c * > const & value_list, sint32_c depth, sint32_c depth_limit );

		public:
			node_c();
			node_c & operator = ( node_c const & other );

		};

		core_list_c< core_list_c< scene_component_c * > > _list_heap; // these lists are reused by leaf nodes each time the tree is rebuilt. to avoid potentially frequent copying and resizing lists.
		core_list_c< node_c > _node_heap; // these nodes are reused each time the tree is rebuilt. to avoid potentially frequent copying and resizing.
		node_c _root_node;

	public:
		scene_tree_kd_c();

		void_c reset();
		void_c build( core_list_c< scene_component_c * > const & value_list );

		scene_component_c * find_nearest_value( vector64x3_c const & position );

	};

}
