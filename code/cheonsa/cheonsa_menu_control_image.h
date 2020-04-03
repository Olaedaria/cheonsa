#pragma once

#include "cheonsa_menu_control.h"
#include "cheonsa_menu_element_frame.h"

namespace cheonsa
{

	// uses a frame element to draw a texture, but rather than relying on the style of the frame element to define how the texture is mapped, you can define it programatically.
	// styles are meant to be shared with all instances.
	// images are usually going to be unique to each instance.
	// so defining the image at the control level instead of the style level is more convenient.
	// because we can define the image programatically/dynamically at run time instead of with the static styles.xml data file.
	class menu_control_image_c
		: public menu_control_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "image"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		enum mode_e
		{
			mode_e_stretch,
			mode_e_scale_to_fit,
			mode_e_scale_to_fill,
		};

	private:
		menu_element_frame_c _element_frame; // name is "frame".
		menu_frame_style_c _element_frame_style;

	public:
		menu_control_image_c();

		mode_e get_mode() const;
		void_c set_mode( mode_e mode );

		void_c set_image( resource_file_texture_c * value );

	public:
		virtual void_c load_static_data_properties( data_scribe_markup_c::node_c const * node ) override;

	};

}
