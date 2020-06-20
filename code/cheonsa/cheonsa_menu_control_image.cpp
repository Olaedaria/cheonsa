#include "cheonsa_menu_control_image.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	menu_control_image_c::menu_control_image_c()
		: menu_control_c()
		, _frame_element( string8_c( "frame", core_list_mode_e_static ) )
		, _frame_style()
	{
		_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_scale_to_fit;

		_frame_element.set_override_style( &_frame_style );
		_add_daughter_element( &_frame_element );
	}

	menu_control_image_c::~menu_control_image_c()
	{
	}

	void_c menu_control_image_c::load_static_data_properties( data_scribe_markup_c::node_c const * node )
	{
		menu_control_c::load_static_data_properties( node );

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
			_frame_style.texture = engine.get_resource_manager()->load_texture( string16_c( attribute->get_value() ), true, true );
		}
	}

	menu_control_image_c::mode_e menu_control_image_c::get_mode() const
	{
		if ( _frame_style.texture_map_mode == menu_frame_style_c::texture_map_mode_e_stretch )
		{
			return mode_e_stretch;
		}
		else if ( _frame_style.texture_map_mode == menu_frame_style_c::texture_map_mode_e_scale_to_fit )
		{
			return mode_e_scale_to_fit;
		}
		else if ( _frame_style.texture_map_mode == menu_frame_style_c::texture_map_mode_e_scale_to_fill )
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
			_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_stretch;
		}
		else if ( mode == mode_e_scale_to_fit )
		{
			_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_scale_to_fit;
		}
		else if ( mode == mode_e_scale_to_fill )
		{
			_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_scale_to_fill;
		}
	}

	void_c menu_control_image_c::set_image( resource_file_texture_c * value )
	{
		_frame_style.texture = value;
	}

	void_c menu_control_image_c::set_pixel_shader( video_renderer_pixel_shader_c * pixel_shader )
	{
		_frame_style.pixel_shader_reference = pixel_shader;
	}

}
