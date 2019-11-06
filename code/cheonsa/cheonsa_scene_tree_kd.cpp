#include "cheonsa_scene_tree_kd.h"
#include "cheonsa_scene_component.h"
#include "cheonsa_scene_object.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	scene_tree_kd_c::node_c::node_c()
		: _tree( nullptr )
		, _mother( nullptr )
		, _positive_daughter( nullptr )
		, _negative_daughter( nullptr )
		, _positive_value_list( nullptr )
		, _negative_value_list( nullptr )
		, _separating_axis( 0 )
		, _separating_distance( 0.0 )
	{
	}

	scene_tree_kd_c::node_c & scene_tree_kd_c::node_c::operator = ( node_c const & other )
	{
		_tree = other._tree;
		_mother = other._mother;
		_positive_daughter = other._positive_daughter;
		_negative_daughter = other._negative_daughter;
		_positive_value_list = other._positive_value_list;
		_negative_value_list = other._negative_value_list;
		_separating_axis = other._separating_axis;
		_separating_distance = other._separating_distance;
		return *this;
	}

	void_c scene_tree_kd_c::node_c::_build( core_list_c< scene_component_c * > const & value_list, sint32_c depth, sint32_c depth_limit )
	{
		_positive_daughter = nullptr;
		_negative_daughter = nullptr;
		_positive_value_list = nullptr;
		_negative_value_list = nullptr;

		sint32_c separating_axis = depth % 3;
		float64_c median = 0.0;
		for ( sint32_c i = 0; i < value_list.get_length(); i++ )
		{
			median += value_list[ i ]->get_scene_object()->get_world_space_transform().position.get_element_at_index( _separating_axis );
		}
		_separating_distance = median / static_cast< float64_c >( value_list.get_length() );
		_separating_axis = separating_axis;

		core_list_c< scene_component_c * > positive_value_list;
		core_list_c< scene_component_c * > negative_value_list;

		if ( depth < depth_limit )
		{
			_positive_value_list = &positive_value_list;
			positive_value_list.construct_mode_dynamic( 0, value_list.get_internal_array_length_allocated() / 2 );
			_negative_value_list = &negative_value_list;
			negative_value_list.construct_mode_dynamic( 0, value_list.get_internal_array_length_allocated() / 2 );
		}
		else
		{
			_positive_value_list = _tree->_list_heap.emplace_at_end();
			_positive_value_list->remove_all();
			_negative_value_list = _tree->_list_heap.emplace_at_end();
			_negative_value_list->remove_all();
		}

		for ( sint32_c i = 0; i < value_list.get_length(); i++ )
		{
			scene_component_c * value = value_list[ i ];
			if ( value->get_scene_object()->get_world_space_transform().position.get_element_at_index( separating_axis ) >= _separating_distance )
			{
				_positive_value_list->insert_at_end( value );
			}
			else
			{
				_negative_value_list->insert_at_end( value );
			}
		}

		if ( depth < depth_limit )
		{
			_positive_daughter = _tree->_node_heap.emplace_at_end();
			_positive_daughter->_tree = _tree;
			_positive_daughter->_build( positive_value_list, depth + 1, depth_limit );
			_negative_daughter = _tree->_node_heap.emplace_at_end();
			_negative_daughter->_tree = _tree;
			_negative_daughter->_build( negative_value_list, depth + 1, depth_limit );
		}
		else
		{
			_positive_daughter = nullptr;
			_negative_daughter = nullptr;
		}
	}

	scene_tree_kd_c::scene_tree_kd_c()
		: _node_heap()
		, _list_heap()
		, _root_node()
	{
		_list_heap.construct_mode_dynamic( 0, 64 );
		_root_node._tree = this;
	}

	void_c scene_tree_kd_c::reset()
	{
		_list_heap.remove_all();
		_node_heap.remove_all();
		_root_node._positive_daughter = nullptr;
		_root_node._negative_daughter = nullptr;
		_root_node._positive_value_list = nullptr;
		_root_node._negative_value_list = nullptr;
	}

	void_c scene_tree_kd_c::build( core_list_c< scene_component_c * > const & value_list )
	{
		reset();

		sint32_c values_at_max_depth = 16;
		sint32_c values = value_list.get_length();
		sint32_c depth_limit = 1;
		while ( values > values_at_max_depth && depth_limit < 6 )
		{
			values /= 2;
			depth_limit++;
		}

		_root_node._build( value_list, 0, depth_limit );
	}

	scene_component_c * scene_tree_kd_c::find_nearest_value( vector64x3_c const & position )
	{
		if ( _root_node._positive_daughter == nullptr && _root_node._positive_value_list == nullptr )
		{
			return nullptr; // collection is empty.
		}

		core_list_c< scene_component_c * > * value_list = nullptr;
		node_c * node = &_root_node;
		while ( true )
		{
			if ( position.get_element_at_index( node->_separating_axis ) >= node->_separating_distance )
			{
				if ( node->_positive_daughter )
				{
					node = node->_positive_daughter;
				}
				else
				{
					value_list = node->_positive_value_list;
					break;
				}
			}
			else
			{
				if ( node->_negative_daughter )
				{
					node = node->_negative_daughter;
				}
				else
				{
					value_list = node->_negative_value_list;
					break;
				}
			}
		}

		if ( value_list->get_length() == 0 )
		{
			return nullptr;
		}

		scene_component_c * closest_value = (*value_list)[ 0 ];
		float64_c closest_value_distance = ops::make_float64_length( (*value_list)[ 0 ]->get_scene_object()->get_world_space_transform().position - position );
		for ( sint32_c i = 1; i < value_list->get_length(); i++ )
		{
			scene_component_c * value = (*value_list)[ i ];
			float64_c value_distance = ops::make_float64_length( value->get_scene_object()->get_world_space_transform().position - position );
			if ( value_distance < closest_value_distance )
			{
				closest_value = value;
				closest_value_distance = value_distance;
			}
		}

		return closest_value;
	}

}
