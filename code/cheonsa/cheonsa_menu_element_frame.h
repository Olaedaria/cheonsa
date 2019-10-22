#pragma once

#include "cheonsa_menu_element.h"
#include "cheonsa_menu_style_for_frame.h"
#include "cheonsa_resource_object_texture.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_video_renderer_shader_manager.h"

namespace cheonsa
{

	class menu_element_frame_c
		: public menu_element_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "element_frame"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	private:
		friend class menu_render_procedure_c;

		menu_style_for_frame_c::reference_c _style_reference;
		menu_style_for_frame_c * _override_style; // if set, then this style will be used instead of the one defined by the _style_reference.

		virtual void_c _build_draw_list() override;

	public:
		menu_element_frame_c();
		~menu_element_frame_c();

		// updates visual state animations.
		virtual void_c update_animations( float32_c time_step ) override;

		virtual void_c set_style_key( string8_c const & value ) override;

		menu_style_for_frame_c::reference_c const & get_style_reference() const;
		menu_style_for_frame_c::reference_c & get_style_reference();

		//virtual string8_c const & get_style_key() const override;
		//virtual void_c set_style_key( string8_c const & value ) override;
		//void_c set_style( menu_style_for_frame_c const * style ); // situational, cheaper than setting the key and doing a lookup.

		menu_style_for_frame_c * get_override_style() const;
		void_c set_override_style( menu_style_for_frame_c * value );

	public:
		// only loads layout and style key.
		virtual void_c load_properties( data_scribe_markup_c::node_c const * node ) override;

	};

}
