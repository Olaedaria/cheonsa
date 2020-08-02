#include "cheonsa_menu_control_grid.h"
#include "cheonsa_menu_element.h"

namespace cheonsa
{

	void_c menu_control_grid_c::_update_transform_and_layout()
	{
		_update_transform_and_layout_base();

		// layout daughter elements.
		for ( sint32_c i = 0; i < _daughter_element_list.get_length(); i++ )
		{
			menu_element_c * daughter_element = _daughter_element_list[ i ];
			daughter_element->update_layout( _local_box );
		}

		// layout daughter controls.
		box32x2_c grid_box;
		grid_box.minimum.a = _local_box.minimum.a - ( _grid_cell_spacing_x * 0.5f );
		grid_box.maximum.a = _local_box.maximum.a + ( _grid_cell_spacing_x * 0.5f );
		grid_box.minimum.b = _local_box.minimum.b - ( _grid_cell_spacing_y * 0.5f );
		grid_box.maximum.b = _local_box.maximum.b + ( _grid_cell_spacing_y * 0.5f );
		vector32x2_c grid_cell_size;
		grid_cell_size.a = grid_box.get_width() / _grid_cell_count_x;
		grid_cell_size.b = grid_box.get_height() / _grid_cell_count_y;
		core_linked_list_c< menu_control_c * >::node_c const * daughter_control_list_node = _daughter_control_list.get_first();
		while ( daughter_control_list_node )
		{
			menu_control_c * daughter_control = daughter_control_list_node->get_value();
			box32x2_c grid_cell_box;
			grid_cell_box.minimum.a = grid_box.minimum.a + ( grid_cell_size.a * daughter_control->get_grid_cell_index_x() ) + ( _grid_cell_spacing_x * 0.5f );
			grid_cell_box.maximum.a = grid_cell_box.minimum.a + grid_cell_size.a - ( _grid_cell_spacing_x * 0.5f );
			grid_cell_box.minimum.b = grid_box.minimum.b + ( grid_cell_size.b * daughter_control->get_grid_cell_index_y() ) + ( _grid_cell_spacing_y * 0.5f );
			grid_cell_box.maximum.b = grid_cell_box.minimum.b + grid_cell_size.b - ( _grid_cell_spacing_y * 0.5f );
			daughter_control->set_layout_simple( grid_cell_box );
			daughter_control_list_node = daughter_control_list_node->get_next();
		}
	}

	menu_control_grid_c::menu_control_grid_c()
		: menu_control_c()
		, _grid_cell_count_x( 1 )
		, _grid_cell_count_y( 1 )
		, _grid_cell_spacing_x( 0.0f )
		, _grid_cell_spacing_y( 0.0f )
	{
	}

	sint32_c menu_control_grid_c::get_grid_cell_count_x() const
	{
		return _grid_cell_count_x;
	}

	void_c menu_control_grid_c::set_grid_cell_count_x( sint32_c value )
	{
		_grid_cell_count_x = value >= 1 ? value : 1;
	}

	sint32_c menu_control_grid_c::get_grid_cell_count_y() const
	{
		return _grid_cell_count_y;
	}

	void_c menu_control_grid_c::set_grid_cell_count_y( sint32_c value )
	{
		_grid_cell_count_y = value >= 1 ? value : 1;
	}

	float32_c menu_control_grid_c::get_grid_cell_spacing_x() const
	{
		return _grid_cell_spacing_x;
	}

	void_c menu_control_grid_c::set_grid_cell_spacing_x( float32_c value )
	{
		_grid_cell_spacing_x = value >= 0.0f ? value : 0.0f;
	}

	float32_c menu_control_grid_c::get_grid_cell_spacing_y() const
	{
		return _grid_cell_spacing_y;
	}

	void_c menu_control_grid_c::set_grid_cell_spacing_y( float32_c value )
	{
		_grid_cell_spacing_y = value >= 0.0f ? value : 0.0f;
	}

}
