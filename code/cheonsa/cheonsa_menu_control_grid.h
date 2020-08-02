#pragma once

#include "cheonsa_menu_control.h"

namespace cheonsa
{

	// used to lay out daughter controls in a regular grid.
	// uses the grid_index_x and grid_index_y properties on the daughter control to determine which row column cell to lay it out in.
	class menu_control_grid_c
		: public menu_control_c
	{
	private:
		sint32_c _grid_cell_count_x; // number of columns.
		sint32_c _grid_cell_count_y; // number of rows.
		float32_c _grid_cell_spacing_x; // horizontal spacing between contents of neighboring cells in pixels.
		float32_c _grid_cell_spacing_y; // vertical spacing between contents of neighboring cells in pixels.

		virtual void_c _update_transform_and_layout() override;

	public:
		menu_control_grid_c();

		sint32_c get_grid_cell_count_x() const; // row count.
		void_c set_grid_cell_count_x( sint32_c value ); // row count.

		sint32_c get_grid_cell_count_y() const; // column count.
		void_c set_grid_cell_count_y( sint32_c value ); // column count.

		float32_c get_grid_cell_spacing_x() const;
		void_c set_grid_cell_spacing_x( float32_c value );

		float32_c get_grid_cell_spacing_y() const;
		void_c set_grid_cell_spacing_y( float32_c value );

		
	};

}
