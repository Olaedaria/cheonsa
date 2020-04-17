#pragma once

#include "cheonsa_menu_element.h"
#include "cheonsa_resource_file_texture.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_video_renderer_shader_manager.h"

namespace cheonsa
{

	// just a textured quad.
	// but with lots of styling options.
	class menu_element_frame_c
		: public menu_element_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "element_frame"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	private:
		menu_frame_style_c::reference_c _style_reference;
		menu_frame_style_c * _override_style; // if set, then this style will be used instead of the one defined by _style_reference. this lets the program define a style, rather than the data.
		resource_file_texture_c::reference_c _override_texture; // if set, then this texture will be used instead of the one defined by _override_style or _style_reference. this lets the program define a texture, rather than the data.

		virtual void_c _build_draw_list() override;

	public:
		menu_element_frame_c( string8_c const & name );
		~menu_element_frame_c();

		virtual void_c update_animations( float32_c time_step ) override;

		virtual void_c set_style_key( string8_c const & value ) override;

		menu_frame_style_c::reference_c const & get_style_reference() const;
		menu_frame_style_c::reference_c & get_style_reference();

		menu_frame_style_c * get_override_style() const;
		void_c set_override_style( menu_frame_style_c * value );

		resource_file_texture_c * get_override_texture() const;
		void_c set_override_texture( resource_file_texture_c * value );

	};

}
