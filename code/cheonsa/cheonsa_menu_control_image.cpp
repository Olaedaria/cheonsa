#include "cheonsa_menu_control_image.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	menu_control_image_c::menu_control_image_c()
		: menu_control_c()
		, _element_frame()
		, _element_frame_style()
	{
		_element_frame.set_name( string8_c( mode_e_static, "frame" ) );
		_element_frame.set_layout_box_anchor( menu_anchor_e_left | menu_anchor_e_top | menu_anchor_e_right | menu_anchor_e_bottom, box32x2_c( 0.0f, 0.0f, 0.0f, 0.0f ) );
		_element_frame.set_override_style( &_element_frame_style );
		_element_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_scale_to_fit;
		_add_element( &_element_frame );
	}

	menu_control_image_c::mode_e menu_control_image_c::get_mode() const
	{
		if ( _element_frame_style.texture_map_mode == menu_frame_style_c::texture_map_mode_e_stretch )
		{
			return mode_e_stretch;
		}
		else if ( _element_frame_style.texture_map_mode == menu_frame_style_c::texture_map_mode_e_scale_to_fit )
		{
			return mode_e_scale_to_fit;
		}
		else if ( _element_frame_style.texture_map_mode == menu_frame_style_c::texture_map_mode_e_scale_to_fill )
		{
			return mode_e_scale_to_fill;
		}
		assert( false );
		return mode_e_stretch;
	}

	void_c menu_control_image_c::set_mode( mode_e mode )
	{
		if ( mode == mode_e_stretch )
		{
			_element_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_stretch;
		}
		else if ( mode == mode_e_scale_to_fit )
		{
			_element_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_scale_to_fit;
		}
		else if ( mode == mode_e_scale_to_fill )
		{
			_element_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_scale_to_fill;
		}
	}

	void_c menu_control_image_c::set_image( resource_file_texture_c * value )
	{
		_element_frame_style.texture = value;
	}

	void_c menu_control_image_c::load_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_properties( node );

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "mode" );
		if ( attribute )
		{
			if ( attribute->get_value() == "stretch" )
			{
				set_mode( mode_e_stretch );
			}
			else if ( attribute->get_value() == "scale_to_fit" )
			{
				set_mode( mode_e_scale_to_fit );
			}
			else if ( attribute->get_value() == "scale_to_fill" )
			{
				set_mode( mode_e_scale_to_fill );
			}
		}

		attribute = node->find_attribute( "texture_path" );
		if ( attribute )
		{
			_element_frame_style.texture = engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( attribute->get_value() ) );
		}
	}

}
