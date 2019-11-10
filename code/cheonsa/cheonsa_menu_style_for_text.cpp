#include "cheonsa_menu_style_for_text.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_style_for_text_c::reference_c::reference_c()
		: _key( mode_e_static, "" )
		, _value( nullptr )
	{
	}

	void_c menu_style_for_text_c::reference_c::refresh()
	{
		assert( engine_c::get_instance()->get_menu_style_manager() != nullptr );
		if ( _key == "[none]" )
		{
			_value = nullptr;
		}
		else if ( _key == "[default]" )
		{
			_value = engine_c::get_instance()->get_menu_style_manager()->get_default_style_for_text();
		}
		else
		{
			_value = engine_c::get_instance()->get_menu_style_manager()->find_style_for_text( _key );
		}
		on_refreshed.invoke( this );
	}

	string8_c const & menu_style_for_text_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_style_for_text_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		refresh();
	}

	menu_style_for_text_c const * menu_style_for_text_c::reference_c::get_value() const
	{
		return _value;
	}

	void_c menu_style_for_text_c::reference_c::set_value( menu_style_for_text_c const * value )
	{
		_key = value ? value->key : string8_c();
		_value = value;
		on_refreshed.invoke( this );
	}

	menu_style_for_text_c::state_c::state_c()
	{
		reset();
	}

	void_c menu_style_for_text_c::state_c::reset()
	{
		shared_color.set_key( string8_c() );
		color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		saturation = 1.0f;
		apparent_scale = 1.0f;
	}

	vector32x4_c menu_style_for_text_c::state_c::get_expressed_color() const
	{
		return shared_color.get_value() ? shared_color.get_value()->value * color : color;
	}

	menu_style_for_text_c::menu_style_for_text_c()
	{
		reset();
	}

	void_c menu_style_for_text_c::reset()
	{
		key = string8_c();
		font_is_defined = false;
		font = nullptr;
		size_is_defined = false;
		size = 0;
		shared_color_is_defined = false;
		shared_color.set_key( string8_c() );
		color_is_defined = false;
		color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		skew_is_defined = false;
		skew = 0;
		weight_is_defined = false;
		weight = 0;
		softness_is_defined = false;
		softness = 0;
		underline_is_defined = false;
		underline = 0;
		paragraph_spacing_is_defined = false;
		paragraph_spacing = 0.0f;
		line_spacing_is_defined = false;
		line_spacing = 0.0f;
		glyph_spacing_is_defined = false;
		glyph_spacing = 0.0f;
		text_align_horizontal_is_defined = false;
		text_align_horizontal = menu_text_align_horizontal_e_left;
		text_align_vertical_is_defined = false;
		text_align_vertical = menu_text_align_vertical_e_center;
		for ( sint32_c i = 0; i < menu_state_e_count_; i++ )
		{
			state_list[ i ].reset();
		}
	}

	void_c menu_style_for_text_c::load( data_scribe_markup_c::node_c const * node )
	{
		reset();

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "key" );
		if ( attribute )
		{
			key = attribute->get_value();
		}

		attribute = node->find_attribute( "font" );
		if ( attribute )
		{
			font = engine_c::get_instance()->get_resource_manager()->load_font( string16_c( attribute->get_value() ) );
			font_is_defined = font.is_reference_set_and_loaded();
		}

		attribute = node->find_attribute( "size" );
		if ( attribute )
		{
			size_is_defined = ops::convert_string8_to_float32( attribute->get_value(), size );
		}

		attribute = node->find_attribute( "shared_color_key" );
		if ( attribute )
		{
			shared_color_is_defined = true;
			shared_color.set_key( attribute->get_value() );
		}

		attribute = node->find_attribute( "color" );
		if ( attribute )
		{
			color_is_defined = ops::convert_string8_to_rgba( attribute->get_value(), color );
		}

		attribute = node->find_attribute( "skew" );
		if ( attribute )
		{
			skew_is_defined = ops::convert_string8_to_float32( attribute->get_value(), skew );
		}

		attribute = node->find_attribute( "weight" );
		if ( attribute )
		{
			weight_is_defined = ops::convert_string8_to_float32( attribute->get_value(), weight );
		}

		attribute = node->find_attribute( "softness" );
		if ( attribute )
		{
			softness_is_defined = ops::convert_string8_to_float32( attribute->get_value(), softness );
		}

		attribute = node->find_attribute( "underline" );
		if ( attribute )
		{
			underline_is_defined = ops::convert_string8_to_float32( attribute->get_value(), underline );
		}

		attribute = node->find_attribute( "paragraph_spacing" );
		if ( attribute )
		{
			paragraph_spacing_is_defined = ops::convert_string8_to_float32( attribute->get_value(), paragraph_spacing );
		}

		attribute = node->find_attribute( "line_spacing" );
		if ( attribute )
		{
			line_spacing_is_defined = true;
			ops::convert_string8_to_float32( attribute->get_value(), line_spacing );
		}

		attribute = node->find_attribute( "glyph_spacing" );
		if ( attribute )
		{
			glyph_spacing_is_defined = ops::convert_string8_to_float32( attribute->get_value(), glyph_spacing );
		}

		attribute = node->find_attribute( "align_horizontal" );
		if ( attribute )
		{
			text_align_horizontal_is_defined = true;
			if ( attribute->get_value() == "left" )
			{
				text_align_horizontal = menu_text_align_horizontal_e_left;
			}
			else if ( attribute->get_value() == "center" )
			{
				text_align_horizontal = menu_text_align_horizontal_e_center;
			}
			else if ( attribute->get_value() == "right" )
			{
				text_align_horizontal = menu_text_align_horizontal_e_right;
			}
			else if ( attribute->get_value() == "justify" )
			{
				text_align_horizontal = menu_text_align_horizontal_e_justify;
			}
		}

		attribute = node->find_attribute( "align_vertical" );
		if ( attribute )
		{
			text_align_vertical_is_defined = true;
			if ( attribute->get_value() == "top" )
			{
				text_align_vertical = menu_text_align_vertical_e_top;
			}
			else if ( attribute->get_value() == "center" )
			{
				text_align_vertical = menu_text_align_vertical_e_center;
			}
			else if ( attribute->get_value() == "bottom" )
			{
				text_align_vertical = menu_text_align_vertical_e_bottom;
			}
		}

		sint32_c state_index = 0;
		data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
		while ( sub_node )
		{
			if ( sub_node->get_value() == "state" )
			{
				attribute = sub_node->find_attribute( "key" );
				if ( attribute )
				{
					if ( attribute->get_value() == "normal" )
					{
						state_index = 0;
					}
					else if ( attribute->get_value() == "selected" )
					{
						state_index = 1;
					}
					else if ( attribute->get_value() == "pressed" )
					{
						state_index = 2;
					}
					else if ( attribute->get_value() == "disabled" )
					{
						state_index = 3;
					}
				}
				
				if ( state_index < menu_state_e_count_ )
				{
					state_c & state = state_list[ state_index ];

					attribute = sub_node->find_attribute( "color" );
					if ( attribute )
					{
						ops::convert_string8_to_rgba( attribute->get_value(), state.color );
					}

					attribute = sub_node->find_attribute( "shared_color_key" );
					if ( attribute )
					{
						state.shared_color.set_key( attribute->get_value() );
					}

					attribute = sub_node->find_attribute( "saturation" );
					if ( attribute )
					{
						ops::convert_string8_to_float32( attribute->get_value(), state.saturation );
					}

					attribute = sub_node->find_attribute( "apparent_scale" );
					if ( attribute )
					{
						ops::convert_string8_to_float32( attribute->get_value(), state.apparent_scale );
					}

					state_index++;
				}
			}
			sub_node = sub_node->get_next_sister();
		}
	}

	//vector32x4_c menu_style_for_text_c::get_color() const
	//{
	//	return shared_color_is_defined && shared_color.get_value() ? shared_color.get_value()->value : color;
	//}

}
