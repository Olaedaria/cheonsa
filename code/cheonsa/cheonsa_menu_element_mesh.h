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
		menu_element_mesh_c();

		virtual void_c update_animations( float32_c time_step ) override;

		// mesh element styles don't exist at this time, so this function does not do anything, but it needs to be implemented anyway since it's a pure virtual method in the base class.
		virtual void_c set_style( string8_c const & key ) override;

	};

}
