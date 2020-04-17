#pragma once

#include "cheonsa_menu_element.h"

namespace cheonsa
{

	class menu_element_mesh_c
		: public menu_element_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "element_mesh"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		core_list_c< video_renderer_vertex_menu_c > vertex_list;
		core_list_c< uint16_c > index_list;

	protected:
		virtual void_c _build_draw_list() override;

	public:
		menu_element_mesh_c( string8_c const & name );

		virtual void_c update_animations( float32_c time_step ) override;

		virtual void_c set_style_key( string8_c const & value ) override;

	};

}
