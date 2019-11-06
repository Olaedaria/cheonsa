#include "cheonsa_menu_style_for_frame.h"
#include "cheonsa_engine.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	menu_style_for_frame_c::menu_style_for_frame_c()
		: key()
		, texture( nullptr )
		, texture_map_mode( texture_map_mode_e_stretch )
		, texture_map_fill_middle( false )
	{
	}

	void_c menu_style_for_frame_c::reset()
	{
		key = string8_c();
		texture = nullptr;
		texture_map_mode = texture_map_mode_e_stretch;
		texture_map_fill_middle = false;
		for ( sint32_c i = 0; i < menu_state_e_count_; i++ )
		{
			state_list[ i ].reset();
		}
	}

	void_c menu_style_for_frame_c::load( data_scribe_markup_c::node_c const * node )
	{
		reset();

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "key" );
		if ( attribute )
		{
			key = attribute->get_value();
		}

		attribute = node->find_attribute( "texture" );
		if ( attribute )
		{
			texture = global_engine_instance.interfaces.resource_manager->load_texture( string16_c( attribute->get_value() ) );
		}

		boolean_c texture_map_size_is_defined = false;
		sint16_c texture_map_size[ 2 ] = {};
		attribute = node->find_attribute( "texture_map_size" );
		if ( attribute )
		{
			texture_map_size_is_defined = ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( mode_e_static, texture_map_size, 2 ) );
		}

		boolean_c texture_map_edges_is_defined = false;
		sint16_c texture_map_edges[ 4 ] = {};
		attribute = node->find_attribute( "texture_map_edges" );
		if ( attribute )
		{
			texture_map_edges_is_defined = ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( mode_e_static, texture_map_edges, 4 ) );
		}

		boolean_c texture_map_origin_is_defined = false;
		sint16_c texture_map_origin[ 2 ] = {};
		attribute = node->find_attribute( "texture_map_origin" );
		if ( attribute )
		{
			texture_map_origin_is_defined = ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( mode_e_static, texture_map_origin, 2 ) );
		}

		attribute = node->find_attribute( "texture_map_mode" );
		if ( attribute )
		{
			if ( attribute->get_value() == "stretch" )
			{
				texture_map_mode = texture_map_mode_e_stretch;
			}
			else if ( attribute->get_value() == "scale_to_fit" )
			{
				texture_map_mode = texture_map_mode_e_scale_to_fit;
			}
			else if ( attribute->get_value() == "scale_to_fill" )
			{
				texture_map_mode = texture_map_mode_e_scale_to_fill;
			}
			else if ( attribute->get_value() == "nine_slice_stretch" )
			{
				texture_map_mode = texture_map_mode_e_nine_slice_stretch;
			}
			else if ( attribute->get_value() == "nine_slice_tile" )
			{
				texture_map_mode = texture_map_mode_e_nine_slice_tile;
			}
		}

		attribute = node->find_attribute( "texture_map_fill_middle" );
		if ( attribute )
		{
			ops::convert_string8_to_boolean( attribute->get_value(), texture_map_fill_middle );
		}

		attribute = node->find_attribute( "pixel_shader" );
		if ( attribute )
		{
			pixel_shader_reference = global_engine_instance.interfaces.video_renderer_shader_manager->load_pixel_shader( string16_c( attribute->get_value() ) );
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

					attribute = sub_node->find_attribute( "shared_color_key" );
					if ( attribute )
					{
						state.shared_color.set_key( attribute->get_value() );
					}

					attribute = sub_node->find_attribute( "color" );
					if ( attribute )
					{
						ops::convert_string8_to_rgba( attribute->get_value(), state.color );
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

					attribute = sub_node->find_attribute( "texture_map" );
					if ( attribute )
					{
						ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( mode_e_static, state.texture_map, 4 ) );
					}

					attribute = sub_node->find_attribute( "texture_map_edges" );
					if ( attribute )
					{
						ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( mode_e_static, state.texture_map_edges, 4 ) );
					}

					attribute = sub_node->find_attribute( "texture_map_origin" );
					if ( texture_map_size_is_defined && texture_map_edges_is_defined )
					{
						if ( attribute )
						{
							sint16_c state_texture_map_origin[ 2 ] = {};
							if ( ops::convert_string8_to_sint16xn( attribute->get_value(), core_list_c< sint16_c >( mode_e_static, state_texture_map_origin, 2 ) ) )
							{
								texture_map_origin[ 0 ] = state_texture_map_origin[ 0 ];
								texture_map_origin[ 1 ] = state_texture_map_origin[ 1 ];
							}
						}
						state.texture_map[ 0 ] = texture_map_origin[ 0 ];
						state.texture_map[ 1 ] = texture_map_origin[ 1 ];
						state.texture_map[ 2 ] = texture_map_origin[ 0 ] + texture_map_size[ 0 ];
						state.texture_map[ 3 ] = texture_map_origin[ 1 ] + texture_map_size[ 1 ];
						state.texture_map_edges[ 0 ] = texture_map_edges[ 0 ];
						state.texture_map_edges[ 1 ] = texture_map_edges[ 1 ];
						state.texture_map_edges[ 2 ] = texture_map_edges[ 2 ];
						state.texture_map_edges[ 3 ] = texture_map_edges[ 3 ];
					}

					state_index++;
				}
			}
			sub_node = sub_node->get_next_sister();
		}
	}

	menu_style_for_frame_c::state_c::state_c()
		: shared_color()
		, color( 1.0f, 1.0f, 1.0f, 1.0f )
		, saturation( 1.0f )
		, apparent_scale( 1.0f )
		, texture_map()
		, texture_map_edges()
	{
	}

	void_c menu_style_for_frame_c::state_c::reset()
	{
		shared_color.set_key( string8_c() );
		color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		saturation = 1.0f;
		apparent_scale = 1.0f;
		texture_map[ 0 ] = 0;
		texture_map[ 1 ] = 0;
		texture_map[ 2 ] = 0;
		texture_map[ 3 ] = 0;
		texture_map_edges[ 0 ] = 0;
		texture_map_edges[ 1 ] = 0;
		texture_map_edges[ 2 ] = 0;
		texture_map_edges[ 3 ] = 0;
	}

	vector32x4_c menu_style_for_frame_c::state_c::get_expressed_color() const
	{
		return shared_color.get_value() ? shared_color.get_value()->value * color : color;
	}

	menu_style_for_frame_c::reference_c::reference_c()
		: _key( mode_e_static, "" )
		, _value( nullptr )
	{
	}

	void_c menu_style_for_frame_c::reference_c::refresh()
	{
		assert( global_engine_instance.interfaces.menu_style_manager != nullptr );
		if ( _key == "[none]" )
		{
			_value = nullptr;
		}
		else if ( _key == "[default]" )
		{
			_value = &global_engine_instance.interfaces.menu_style_manager->default_frame_style;
		}
		else
		{
			_value = global_engine_instance.interfaces.menu_style_manager->find_style_for_frame( _key );
		}
		on_refreshed.invoke( this );
	}

	void_c menu_style_for_frame_c::reference_c::set_value( menu_style_for_frame_c const * value )
	{
		_key = value ? value->key : string8_c();
		_value = value;
		on_refreshed.invoke( this );
	}

	string8_c const & menu_style_for_frame_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_style_for_frame_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		refresh();
	}

	menu_style_for_frame_c const * menu_style_for_frame_c::reference_c::get_value() const
	{
		return _value;
	}

}
