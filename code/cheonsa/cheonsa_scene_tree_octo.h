#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_core_linked_list.h"
#include "cheonsa_scene_types.h"

namespace cheonsa
{

	class scene_component_c;

	// used to hold: scene_component_model_c, scene_component_sprite_c, scene_component_light_c, scene_component_sound_c, scene_component_menu_control_c.
	// type_c needs to have the following members that are accessable to scene_tree_octo< type_c >:
	//     scene_tree_octo< type_c >::node_c * _scene_tree_leaf_node;
	//     core_linked_list_c< type_c >::node_c _scene_tree_list_node;
	//     box32x3_c const & get_local_space_obb() const;
	//     box64x3_c const & get_world_space_aabb() const;
	//     transform3d_c const & get_world_space_transform() const;
	//     
	// sparse octo tree implementation, with loose nodes (because objects are index via their axis aligned bounding box volume rather than their origin points).
	// the tree is used to store components (rather than objects), because each component on a given object may have a different sized bounding box.
	// the tree is world space axis aligned.
	// values are inserted based on their world space axis aligned _world_bounding_box.
	// values are inserted into only one node at a time within the tree.
	// values are inserted into the depth that can most optimally contain the value's bounding box, which is determined by taking the bounding box's longest dimension and finding the deepest node that is just large enough to hold it.
	// values with _scene_bounds.maximum.a < _scene_bounds.minimum.a are considered to be infinite or non-local, they will be placed in the _root_node.
	// values with _scene_bounds that lie outside of the _root_node's bounds will be placed in the _root_node.
	// values that default to be inserted in the root node won't benefit from the spatial lookup optimization that the octo tree is supposed to provide.
	// the largest dimension that a node is allowed to contain is equal to that node's _half_extent * 2.0.
	// the tree has loose nodes at a factor of 2.0, which means that each node has an effective bounding box that is 2x larger than _half_extent * 2.0, and this means that node bounds overlap each other.
	// the drawback to having loose nodes is that there is overlap, so intersection tests against node bounds will pass more often than if the nodes were sized usually.
	template< typename type_c >
	class scene_tree_octo_c
	{
	public:
		// an octant leaf node.
		// octant child index:  0 1 2 3 4 5 6 7
		// x:                   - + - + - + - +
		// y:                   - - + + - - + +
		// z:                   - - - - + + + +
		class node_c
		{
		private:
			friend class scene_tree_octo_c< type_c >;
			scene_tree_octo_c< type_c > * _tree; // the octo tree that this node is contained in.
			sint8_c _octant; // this nodes relation to its mother.
			node_c * _mother_node; // * to this node's mother node if any.
			node_c * _daughter_nodes[ 8 ]; // pointers to this node's daughter nodes if any.
			sint8_c _depth; // this node's depth from the root.
			vector64x3_c _center; // this node's center point in absolute scene space.
			float64_c _half_extent; // this node's base extent around the _center point, which does not include the looseness factor of 2. the actual extent and largest dimension of an object that the node can contain is _half_extent * 2. the actual dimension of the node is _half_extent * 2 * 2 because of the looseness factor of 2.
			core_linked_list_c< type_c > _item_list; // all of the items in this node.

			node_c()
				: _tree( nullptr )
				, _octant( -1 )
				, _mother_node( nullptr )
				, _daughter_nodes()
				, _depth( 0 )
				, _center( 0.0, 0.0, 0.0 )
				, _half_extent( 0.0 )
				, _item_list()
			{
				for ( sint32_c i = 0; i < 8; i++ )
				{
					_daughter_nodes[ i ] = nullptr;
				}
			}

			boolean_c _fully_contains( box64x3_c const & bounds )
			{
				float64_c full_extent = _half_extent * 2.0 * 2.0;
				return ( bounds.minimum.a >= _center.a - full_extent && bounds.maximum.a <= _center.a + full_extent ) && ( bounds.minimum.b >= _center.b - full_extent && bounds.maximum.b <= _center.b + full_extent ) && ( bounds.maximum.c >= _center.c - full_extent && bounds.maximum.c <= _center.c + full_extent );
			}

			void_c _insert_or_update_item( type_c value )
			{
				box64x3_c const & value_box = value->get_world_space_aabb();
				vector64x3_c const & value_position = value->get_world_space_transform().position;
				if ( value_box.maximum.a >= value_box.minimum.a && value_box.get_longest_dimension() < _half_extent * 2.0 && _fully_contains( value_box ) )
				{
					// if value_box.maximum.a < value_box.minimum.a, it indicates that the box is infinite size, and so it will be inserted into the _root_node.
					// we can still sub divide and the value can fit in a sub division.
					uint8_c daughter_node_octant = 0;
					vector64x3_c daughter_node_center;

					if ( value_position.a <= _center.a )
					{
						daughter_node_center.a = _center.a + _half_extent * 0.5;
					}
					else
					{
						daughter_node_octant |= 1;
						daughter_node_center.a = _center.a - _half_extent * 0.5;
					}

					if ( value_position.b <= _center.b )
					{
						daughter_node_center.b = _center.b - _half_extent * 0.5;
					}
					else
					{
						daughter_node_octant |= 2;
						daughter_node_center.b = _center.b + _half_extent * 0.5;
					}

					if ( value_position.c <= _center.c )
					{
						daughter_node_center.c = _center.c - _half_extent * 0.5;
					}
					else
					{
						daughter_node_octant |= 4;
						daughter_node_center.c = _center.c + _half_extent * 0.5;
					}

					if ( _daughter_nodes[ daughter_node_octant ] == nullptr )
					{
						node_c * daughter_node = new node_c();
						daughter_node->_tree = _tree;
						daughter_node->_octant = daughter_node_octant;
						daughter_node->_mother_node = this;
						daughter_node->_depth = _depth + 1;
						daughter_node->_center = daughter_node_center;
						daughter_node->_half_extent = _half_extent * 0.5;
						_daughter_nodes[ daughter_node_octant ] = daughter_node;
					}

					_daughter_nodes[ daughter_node_octant ]->_insert_or_update_item( value );
				}
				else
				{
					if ( value->_scene_tree_leaf_node == this )
					{
						// if the value is already inserted in this node then we don't need to do anything.
						return;
					}
					else if ( value->_scene_tree_leaf_node )
					{
						// if the value is already inserted in another node then we have to remove it from that node.
						assert( value->_scene_tree_leaf_node->_tree == _tree );
						_tree->remove_item( value );
					}

					// insert the value in this node.
					_item_list.insert_at_end( &value->_scene_tree_list_node );
				}
			}

			// gets the bounds of this node, including the loose margin.
			box64x3_c _get_bounds() const
			{
				return box64x3_c( vector64x3_c( _center.a - _half_extent * 2.0, _center.b - _half_extent * 2.0, _center.c - _half_extent * 2.0 ), vector64x3_c( _center.a + _half_extent * 2.0, _center.b + _half_extent * 2.0, _center.c + _half_extent * 2.0 ) );
			}

			void_c _gather_items_in_segment( core_list_c< type_c > & result, segment64_c const & segment ) const
			{
				if ( ops::intersect_segment_vs_box( segment, _get_bounds() ) )
				{
					core_linked_list_c< type_c >::node_c const * item_list_node = _item_list.get_first();
					while ( item_list_node )
					{
						if ( ops::intersect_segment_vs_box( segment, item_list_node->get_value()->get_world_space_aabb() ) )
						{
							result.insert( -1, item_list_node->get_value() );
						}
						item_list_node = item_list_node->get_next();
					}
					for ( sint32_c i = 0; i < 8; i++ )
					{
						if ( _daughter_nodes[ i ] )
						{
							_daughter_nodes[ i ]->_gather_items_in_segment( result, segment );
						}
					}
				}
			}

			void_c _gather_items_in_box( core_list_c< type_c > & result, box64x3_c const & box ) const
			{
				if ( ops::intersect_box_vs_box( box, _get_bounds() ) )
				{
					core_linked_list_c< type_c >::node_c const * item_list_node = _item_list.get_first();
					while ( item_list_node )
					{
						if ( ops::intersect_box_vs_box( box, item_list_node->get_value()->get_world_space_aabb() ) )
						{
							result.insert( -1, item_list_node->get_value() );
						}
						item_list_node = item_list_node->get_next();
					}
					for ( sint32_c i = 0; i < 8; i++ )
					{
						if ( _daughter_nodes[ i ] )
						{
							_daughter_nodes[ i ]->_gather_items_in_box( result, box );
						}
					}
				}
			}

			void_c _gather_items_in_sphere( core_list_c< type_c > & result, sphere64_c const & sphere ) const
			{
				if ( ops::intersect_box_vs_sphere( _get_bounds(), sphere ) )
				{
					core_linked_list_c< type_c >::node_c const * item_list_node = _item_list.get_first();
					while ( item_list_node )
					{
						if ( ops::intersect_box_vs_sphere( item_list_node->get_value()->get_world_space_aabb(), sphere ) )
						{
							result.insert( -1, item_list_node->get_value() );
						}
						item_list_node = item_list_node->get_next();
					}
					for ( sint32_c i = 0; i < 8; i++ )
					{
						if ( _daughter_nodes[ i ] )
						{
							_daughter_nodes[ i ]->_gather_items_in_sphere( result, sphere );
						}
					}
				}
			}

			void_c _gather_items_in_frustum( core_list_c< type_c > & result, frustum64_c const & frustum ) const
			{
				if ( ops::intersect_frustum_vs_box( frustum, _get_bounds() ) )
				{
					core_linked_list_c< type_c >::node_c const * item_list_node = _item_list.get_first();
					while ( item_list_node )
					{
						if ( ops::intersect_frustum_vs_box( frustum, box64x3_c( item_list_node->get_value()->get_local_space_obb() ), item_list_node->get_value()->get_world_space_transform() ) )
						{
							result.insert( -1, item_list_node->get_value() );
						}
						item_list_node = item_list_node->get_next();
					}
					for ( sint32_c i = 0; i < 8; i++ )
					{
						if ( _daughter_nodes[ i ] )
						{
							_daughter_nodes[ i ]->_gather_items_in_frustum( result, frustum );
						}
					}
				}
			}

		};

	private:
		sint8_c _maximum_depth; // maximum number of sub division levels.
		node_c * _root_node;

	public:
		scene_tree_octo_c()
			: _maximum_depth( 0 )
			, _root_node( nullptr )
		{
		}

		void_c initialize( float64_c half_extent, sint8_c maximum_depth )
		{
			_maximum_depth = maximum_depth;
			_root_node = new node_c();
			_root_node->_tree = this;
			_root_node->_center = vector64x3_c( 0.0f, 0.0f, 0.0f );
			_root_node->_half_extent = half_extent;
		}

		// inserts an item into the tree, or updates that item's place within the tree if it is already in the tree.
		void_c insert_or_update_item( type_c value )
		{
			_root_node->_insert_or_update_item( value );
		}

		// removes an item from the tree.
		void_c remove_item( type_c value )
		{
			assert( value );
			value->_scene_tree_leaf_node->_item_list.remove( &value->_scene_tree_list_node );
			value->_scene_tree_leaf_node = nullptr;
		}

		void_c gather_items_in_segment( core_list_c< type_c> & result, segment64_c const & segment ) const
		{
			assert( _root_node );
			_root_node->_gather_items_in_segment( result, segment );
		}

		void_c gather_items_in_box( core_list_c< type_c > & result, box64x3_c const & box ) const
		{
			assert( _root_node );
			_root_node->_gather_items_in_box( result, box );
		}

		void_c gather_items_in_sphere( core_list_c< type_c > & result, sphere64_c const & sphere ) const
		{
			assert( _root_node );
			_root_node->_gather_items_in_sphere( result, sphere );
		}

		void_c gather_items_in_frustum( core_list_c< type_c > & result, frustum64_c const & frustum ) const
		{
			assert( _root_node );
			_root_node->_gather_items_in_frustum( result, frustum );
		}

	};

}
