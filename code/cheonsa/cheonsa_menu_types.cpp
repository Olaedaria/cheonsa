#include "cheonsa_menu_types.h" 
#include "cheonsa_menu_control.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	//
	//
	// menu_color_style_c
	//
	//

	string8_c const & menu_color_style_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_color_style_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		if ( _key.get_length() > 0 )
		{
			assert( engine_c::get_instance()->get_menu_style_manager() != nullptr );
			_value = engine_c::get_instance()->get_menu_style_manager()->find_color_style( _key );
		}
		else
		{
			_value = nullptr;
		}
	}

	menu_color_style_c const * menu_color_style_c::reference_c::get_value() const
	{
		return _value;
	}

	menu_color_style_c::menu_color_style_c()
		: key()
		, index( -1 )
		, value( 1.0f, 1.0f, 1.0f, 1.0f )
	{
	}

	void_c menu_color_style_c::reset()
	{
		key = string8_c();
		index = -1;
		value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	void_c menu_color_style_c::load( data_scribe_markup_c::node_c const * node )
	{
		reset();

		data_scribe_markup_c::attribute_c const * attribute = nullptr;

		attribute = node->find_attribute( "key" );
		if ( attribute )
		{
			key = attribute->get_value();
		}

		attribute = node->find_attribute( "value" );
		if ( attribute )
		{
			ops::convert_string8_to_rgba( attribute->get_value(), value );
		}
	}




	//
	//
	// menu_frame_style_c
	//
	//

	menu_frame_style_c::menu_frame_style_c()
		: key()
		, texture( nullptr )
		, texture_map_mode( texture_map_mode_e_stretch )
		, texture_map_fill_middle( false )
	{
	}

	void_c menu_frame_style_c::reset()
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

	void_c menu_frame_style_c::load( data_scribe_markup_c::node_c const * node )
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
			texture = engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( attribute->get_value() ) );
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
			pixel_shader_reference = engine_c::get_instance()->get_video_renderer_shader_manager()->load_pixel_shader( string16_c( attribute->get_value() ) );
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

					attribute = sub_node->find_attribute( "color_style" );
					if ( attribute )
					{
						state.color_style.set_key( attribute->get_value() );
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

	menu_frame_style_c::state_c::state_c()
		: color_style()
		, color( 1.0f, 1.0f, 1.0f, 1.0f )
		, saturation( 1.0f )
		, apparent_scale( 1.0f )
		, texture_map()
		, texture_map_edges()
	{
	}

	void_c menu_frame_style_c::state_c::reset()
	{
		color_style.set_key( string8_c() );
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

	vector32x4_c menu_frame_style_c::state_c::get_expressed_color() const
	{
		return color_style.get_value() ? color_style.get_value()->value * color : color;
	}

	menu_frame_style_c::reference_c::reference_c()
		: _key( mode_e_static, "" )
		, _value( nullptr )
	{
	}

	void_c menu_frame_style_c::reference_c::refresh()
	{
		assert( engine_c::get_instance()->get_menu_style_manager() != nullptr );
		if ( _key == "[none]" )
		{
			_value = nullptr;
		}
		else if ( _key == "[default]" )
		{
			_value = engine_c::get_instance()->get_menu_style_manager()->get_default_frame_style();
		}
		else
		{
			_value = engine_c::get_instance()->get_menu_style_manager()->find_frame_style( _key );
		}
		on_refreshed.invoke( this );
	}

	void_c menu_frame_style_c::reference_c::set_value( menu_frame_style_c const * value )
	{
		_key = value ? value->key : string8_c();
		_value = value;
		on_refreshed.invoke( this );
	}

	string8_c const & menu_frame_style_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_frame_style_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		refresh();
	}

	menu_frame_style_c const * menu_frame_style_c::reference_c::get_value() const
	{
		return _value;
	}



	//
	//
	// menu_text_style_c
	//
	//

	menu_text_style_c::reference_c::reference_c()
		: _key( mode_e_static, "" )
		, _value( nullptr )
	{
	}

	void_c menu_text_style_c::reference_c::refresh()
	{
		assert( engine_c::get_instance()->get_menu_style_manager() != nullptr );
		if ( _key == "[none]" )
		{
			_value = nullptr;
		}
		else if ( _key == "[default]" )
		{
			_value = engine_c::get_instance()->get_menu_style_manager()->get_default_text_style();
		}
		else
		{
			_value = engine_c::get_instance()->get_menu_style_manager()->find_text_style( _key );
		}
		on_refreshed.invoke( this );
	}

	string8_c const & menu_text_style_c::reference_c::get_key() const
	{
		return _key;
	}

	void_c menu_text_style_c::reference_c::set_key( string8_c const & value )
	{
		_key = value;
		refresh();
	}

	menu_text_style_c const * menu_text_style_c::reference_c::get_value() const
	{
		return _value;
	}

	void_c menu_text_style_c::reference_c::set_value( menu_text_style_c const * value )
	{
		_key = value ? value->key : string8_c();
		_value = value;
		on_refreshed.invoke( this );
	}

	menu_text_style_c::state_c::state_c()
	{
		reset();
	}

	void_c menu_text_style_c::state_c::reset()
	{
		color_style.set_key( string8_c() );
		color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		saturation = 1.0f;
		apparent_scale = 1.0f;
	}

	vector32x4_c menu_text_style_c::state_c::get_expressed_color() const
	{
		return color_style.get_value() ? color_style.get_value()->value * color : color;
	}

	menu_text_style_c::menu_text_style_c()
	{
		reset();
	}

	void_c menu_text_style_c::reset()
	{
		key = string8_c();
		font_is_defined = false;
		font = nullptr;
		size_is_defined = false;
		size = 0;
		color_style_is_defined = false;
		color_style.set_key( string8_c() );
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

	void_c menu_text_style_c::load( data_scribe_markup_c::node_c const * node )
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

		attribute = node->find_attribute( "color_style" );
		if ( attribute )
		{
			color_style_is_defined = true;
			color_style.set_key( attribute->get_value() );
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

					attribute = sub_node->find_attribute( "color_style" );
					if ( attribute )
					{
						state.color_style.set_key( attribute->get_value() );
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




	//
	//
	// menu_text_glyph_style_c
	//
	//

	menu_text_glyph_style_c::menu_text_glyph_style_c()
		: font( nullptr )
		, color( 1.0f, 1.0f, 1.0f, 1.0f )
		, size( 1.0f )
		, weight( 0.0f )
		, skew( 0.0f )
		, softness( 0.0f )
	{
	}

	menu_text_glyph_style_c::menu_text_glyph_style_c( menu_text_glyph_style_c const & other )
	{
		*this = other;
	}

	menu_text_glyph_style_c & menu_text_glyph_style_c::operator = ( menu_text_glyph_style_c const & other )
	{
		font = other.font;
		color = other.color;
		size = other.size;
		weight = other.weight;
		skew = other.skew;
		softness = other.softness;
		return *this;
	}

	boolean_c menu_text_glyph_style_c::operator == ( menu_text_glyph_style_c const & other ) const
	{
		return ( font == other.font ) && ( size == other.size ) && ( color == other.color ) && ( skew == other.skew ) && ( weight == other.weight ) && ( softness == other.softness );
	}




	//
	//
	// menu_text_entity_c
	//
	//

	menu_text_entity_c::menu_text_entity_c( sint32_c type_code )
		: type_code( type_code )
	{
	}

	menu_text_entity_sprite_c::menu_text_entity_sprite_c()
		: menu_text_entity_c( type_code_static() )
		, value()
	{
	}




	//
	//
	// menu_event_info_c
	//
	//

	menu_event_info_c::menu_event_info_c()
		: control( nullptr )
		, event( nullptr )
	{
	}

	menu_event_info_c::menu_event_info_c( menu_control_c * control, input_event_c * event )
		: control( control )
		, event( event )
	{
	}




	//
	//
	// menu_draw_list_c
	//
	//

	menu_draw_list_c::draw_c::draw_c()
		: pixel_shader( nullptr )
		, texture( nullptr )
		, vertex_start( 0 )
		, vertex_count( 0 )
		, index_start( 0 )
		, index_count( 0 )
	{
	}

	menu_draw_list_c::menu_draw_list_c()
		: vertex_list()
		, index_list()
		, draw_list()
		, vertex_base( 0 )
		, index_base( 0 )
	{
	}

	void_c menu_draw_list_c::reset()
	{
		vertex_list.remove_all();
		index_list.remove_all();
		draw_list.remove_all();
		vertex_base = 0;
		index_base = 0;
	}

	void_c menu_draw_list_c::append_rectangle_list( core_list_c< video_renderer_vertex_menu_c > const & input_vertex_list, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture )
	{
		// add draw.
		draw_c * draw = draw_list.emplace_at_end();
		draw->pixel_shader = pixel_shader;
		draw->texture = texture;
		draw->vertex_start = vertex_list.get_length();
		draw->vertex_count = input_vertex_list.get_length();
		draw->index_start = index_list.get_length();
		draw->index_count = input_vertex_list.get_length() / 4 * 6;

		// add indices.
		assert( vertex_list.get_length() + 4 < 0xFFFF );
		sint16_c base_vertex = static_cast< sint16_c >( vertex_list.get_length() );
		sint32_c rectangle_count = input_vertex_list.get_length() / 4;
		for ( sint32_c i = 0; i < rectangle_count; i++ )
		{
			index_list.insert_at_end( base_vertex ); // top left.
			index_list.insert_at_end( base_vertex + 1 ); // bottom left.
			index_list.insert_at_end( base_vertex + 2 ); // top right.
			index_list.insert_at_end( base_vertex + 2 ); // top right.
			index_list.insert_at_end( base_vertex + 1 ); // bottom left.
			index_list.insert_at_end( base_vertex + 3 ); // bottom right.
			base_vertex += 4;
		}

		// add vertices.
		vertex_list.insert_at_end( input_vertex_list.get_internal_array(), input_vertex_list.get_length() );
	}

	void_c menu_draw_list_c::append_rectangle( box32x2_c const & box, box32x2_c const & map, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture, vector32x4_c const & color )
	{
		// add draw.
		draw_c * draw = draw_list.emplace_at_end();
		draw->pixel_shader = pixel_shader;
		draw->texture = texture;
		draw->vertex_start = vertex_list.get_length();
		draw->vertex_count = 4; // 4 vertices which make up 1 quad.
		draw->index_start = index_list.get_length();
		draw->index_count = 6; // 6 indices which make up 2 triangles which make up 1 quad.

		// add indices.
		assert( index_list.get_length() < 0xFFFF - 6 ); // if this trips then we should try using 32-bit indices.
		uint16_c base_vertex = static_cast< uint16_c >( vertex_list.get_length() );
		index_list.insert_at_end( base_vertex ); // top left
		index_list.insert_at_end( base_vertex + 1 ); // bottom left
		index_list.insert_at_end( base_vertex + 2 ); // top right
		index_list.insert_at_end( base_vertex + 2 ); // top right
		index_list.insert_at_end( base_vertex + 1 ); // bottom left
		index_list.insert_at_end( base_vertex + 3 ); // bottom right

		// add vertices.
		video_renderer_vertex_menu_c * vertex = vertex_list.emplace_at_end(); // top left
		vertex->position = vector32x3_c( box.minimum.a, box.minimum.b, 0.0f );
		vertex->texture = vector32x3_c( map.minimum.a, map.minimum.b, 0.0f );
		vertex->color = color;
		vertex = vertex_list.emplace_at_end(); // bottom left
		vertex->position = vector32x3_c( box.minimum.a, box.maximum.b, 0.0f );
		vertex->texture = vector32x3_c( map.minimum.a, map.maximum.b, 0.0f );
		vertex->color = color;
		vertex = vertex_list.emplace_at_end(); // top right
		vertex->position = vector32x3_c( box.maximum.a, box.minimum.b, 0.0f );
		vertex->texture = vector32x3_c( map.maximum.a, map.minimum.b, 0.0f );
		vertex->color = color;
		vertex = vertex_list.emplace_at_end(); // bottom right
		vertex->position = vector32x3_c( box.maximum.a, box.maximum.b, 0.0f );
		vertex->texture = vector32x3_c( map.maximum.a, map.maximum.b, 0.0f );
		vertex->color = color;
	}

}
