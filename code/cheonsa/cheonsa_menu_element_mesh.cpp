#include "cheonsa_menu_element_mesh.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_element_mesh_c::_build_draw_list()
	{
		assert( _mother_control );

		_draw_list.reset();

		if ( _is_showed_from_style == false || _is_showed == false || _local_color.d <= 0.0f )
		{
			return;
		}
		
		video_pixel_shader_c * pixel_shader = engine.get_video_renderer_shader_manager()->get_menu_ps_solid_color();
		_draw_list.append_triangle_list( vertex_list, index_list, pixel_shader, nullptr );
	}

	menu_element_mesh_c::menu_element_mesh_c()
		: menu_element_c()
		, vertex_list()
		, index_list()
	{
	}

	void_c menu_element_mesh_c::update_animations( float32_c time_step )
	{
	}

	void_c menu_element_mesh_c::set_style_key( string8_c const & value )
	{
	}

}
