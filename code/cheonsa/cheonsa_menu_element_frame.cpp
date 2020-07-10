#include "cheonsa_menu_element_frame.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_element_frame_c::_build_draw_list()
	{
		assert( _mother_control );

		_draw_list.reset();

		if ( _is_showed_from_style == false || _is_showed == false || _local_color.d <= 0.0f )
		{
			return;
		}

		menu_frame_style_c const * frame_style = _override_style ? _override_style : _style_reference.get_value();
		if ( frame_style == nullptr )
		{
			frame_style = engine.get_menu_style_manager()->get_default_frame_style();
		}

		resource_file_texture_c * texture = frame_style->texture;
		if ( _override_texture.get_is_value_set_and_loaded() )
		{
			texture = _override_texture;
		}
		if ( texture == nullptr || texture->get_video_texture() == nullptr )
		{
			return;
		}

		// state color and saturation.
		menu_state_e state = get_state();
		menu_frame_style_c::state_c const & frame_style_state = frame_style->state_list[ state ];
		if ( frame_style_state.show == false )
		{
			return;
		}

		vector32x4_c draw_color = _local_color * _mother_control->get_local_color();
		vector32x4_c draw_shared_colors[ 3 ]; // these will be uploaded to "menu_colors" in the shaders.
		menu_color_style_c * shared_color = nullptr;
		shared_color = engine.get_menu_style_manager()->get_shared_color_style( _shared_color_class, state, menu_shared_color_slot_e_primary );
		assert( shared_color );
		draw_shared_colors[ 0 ] = shared_color->value;
		shared_color = engine.get_menu_style_manager()->get_shared_color_style( _shared_color_class, state, menu_shared_color_slot_e_secondary );
		assert( shared_color );
		draw_shared_colors[ 1 ] = shared_color->value;
		shared_color = engine.get_menu_style_manager()->get_shared_color_style( _shared_color_class, state, menu_shared_color_slot_e_accent );
		assert( shared_color );
		draw_shared_colors[ 2 ] = shared_color->value;
		if ( frame_style_state.swap_shared_colors != _invert_shared_colors )
		{
			vector32x4_c t = draw_shared_colors[ 0 ];
			draw_shared_colors[ 0 ] = draw_shared_colors[ 1 ];
			draw_shared_colors[ 1 ] = t;
		}

		video_pixel_shader_c * pixel_shader = frame_style->pixel_shader_reference ? frame_style->pixel_shader_reference->get_ps() : engine.get_video_renderer_shader_manager()->get_menu_ps_frame();
		assert( pixel_shader );

		// to hold calculated measurements of what to draw.
		float32_c texture_width = static_cast< float32_c >( texture->get_video_texture()->get_width() );
		float32_c texture_width_inverse = 1.0f / texture_width;
		float32_c texture_height = static_cast< float32_c >( texture->get_video_texture()->get_height() );
		float32_c texture_height_inverse = 1.0f / texture_height;
		float32_c x[ 4 ]; // always from left to right.
		float32_c y[ 4 ]; // always from top to bottom.
		float32_c u[ 4 ]; // can be from left to right or from right to left (depending on the orientation of the texture_map_diagonal).
		float32_c v[ 4 ]; // can be from top to bottom or from bottom to top (depending on orientation of the texture_map_diagonal).
		float32_c tile_size[ 2 ];
		float32_c overlap;

		if ( frame_style->texture_map_mode != menu_texture_map_mode_e_center )
		{
			// calculate some measurements that are usable in most texture map modes.
			x[ 0 ] = _local_box.minimum.a; // left edge.
			x[ 3 ] = _local_box.maximum.a; // right edge.
			y[ 0 ] = _local_box.minimum.b; // top edge.
			y[ 3 ] = _local_box.maximum.b; // bottom edge.
			u[ 0 ] = static_cast< float32_c >( frame_style_state.texture_map[ 0 ] );
			u[ 3 ] = static_cast< float32_c >( frame_style_state.texture_map[ 2 ] );
			v[ 0 ] = static_cast< float32_c >( frame_style_state.texture_map[ 1 ] );
			v[ 3 ] = static_cast< float32_c >( frame_style_state.texture_map[ 3 ] );
			if ( frame_style_state.texture_map[ 0 ] <= frame_style_state.texture_map[ 2 ] )
			{
				// normal u.
				u[ 1 ] = u[ 0 ] + static_cast< float32_c >( frame_style_state.texture_map_edges[ 0 ] );
				x[ 1 ] = x[ 0 ] + static_cast< float32_c >( frame_style_state.texture_map_edges[ 0 ] );
				u[ 2 ] = u[ 3 ] - static_cast< float32_c >( frame_style_state.texture_map_edges[ 2 ] );
				x[ 2 ] = x[ 3 ] - static_cast< float32_c >( frame_style_state.texture_map_edges[ 2 ] );
				tile_size[ 0 ] = u[ 2 ] - u[ 1 ];
			}
			else
			{
				// mirror u.
				u[ 1 ] = u[ 0 ] + static_cast< float32_c >( frame_style_state.texture_map_edges[ 2 ] );
				x[ 1 ] = x[ 0 ] + static_cast< float32_c >( frame_style_state.texture_map_edges[ 2 ] );
				u[ 2 ] = u[ 3 ] - static_cast< float32_c >( frame_style_state.texture_map_edges[ 0 ] );
				x[ 2 ] = x[ 3 ] - static_cast< float32_c >( frame_style_state.texture_map_edges[ 0 ] );
				tile_size[ 0 ] = u[ 1 ] - u[ 2 ];
			}
			if ( frame_style_state.texture_map[ 1 ] <= frame_style_state.texture_map[ 3 ] )
			{
				// normal y.
				v[ 1 ] = v[ 0 ] + static_cast< float32_c >( frame_style_state.texture_map_edges[ 0 ] );
				y[ 1 ] = y[ 0 ] + static_cast< float32_c >( frame_style_state.texture_map_edges[ 0 ] );
				v[ 2 ] = v[ 3 ] - static_cast< float32_c >( frame_style_state.texture_map_edges[ 2 ] );
				y[ 2 ] = y[ 3 ] - static_cast< float32_c >( frame_style_state.texture_map_edges[ 2 ] );
				tile_size[ 1 ] = v[ 2 ] - v[ 1 ];
			}
			else
			{
				// mirror y.
				v[ 1 ] = v[ 0 ] + static_cast< float32_c >( frame_style_state.texture_map_edges[ 2 ] );
				y[ 1 ] = y[ 0 ] + static_cast< float32_c >( frame_style_state.texture_map_edges[ 2 ] );
				v[ 2 ] = v[ 3 ] - static_cast< float32_c >( frame_style_state.texture_map_edges[ 0 ] );
				y[ 2 ] = y[ 3 ] - static_cast< float32_c >( frame_style_state.texture_map_edges[ 0 ] );
				tile_size[ 1 ] = v[ 1 ] - v[ 2 ];
			}
			// transform from pixel (texel) coordinate space to uv coordinate space.
			u[ 0 ] *= texture_width_inverse;
			u[ 1 ] *= texture_width_inverse;
			u[ 2 ] *= texture_width_inverse;
			u[ 3 ] *= texture_width_inverse;
			v[ 0 ] *= texture_height_inverse;
			v[ 1 ] *= texture_height_inverse;
			v[ 2 ] *= texture_height_inverse;
			v[ 3 ] *= texture_height_inverse;

			// branch based on texture map mode.
			if ( frame_style->texture_map_mode == menu_texture_map_mode_e_stretch )
			{
				box32x2_c box( x[ 0 ], y[ 0 ], x[ 3 ], y[ 3 ] );
				box32x2_c map( u[ 0 ], v[ 0 ], u[ 3 ], v[ 3 ] );
				_draw_list.append_rectangle( box, map, pixel_shader, texture, draw_color, draw_shared_colors );
			}
			else if ( frame_style->texture_map_mode == menu_texture_map_mode_e_scale_to_fit )
			{
				float32_c texture_aspect_ratio = texture_width / texture_height;
				float32_c frame_width = _local_box.get_width();
				float32_c frame_height = _local_box.get_height();
				float32_c frame_aspect_ratio = frame_width / frame_height;

				float32_c frame_width_scale = 0.5f;
				float32_c frame_height_scale = 0.5f;
				if ( texture_aspect_ratio > frame_aspect_ratio )
				{
					frame_height_scale = frame_aspect_ratio / texture_aspect_ratio * 0.5f;
				}
				else
				{
					frame_width_scale = texture_aspect_ratio / frame_aspect_ratio * 0.5f;
				}

				vector32x2_c frame_center = _local_box.get_center();

				box32x2_c box( frame_center.a - frame_width_scale * frame_width, frame_center.b - frame_height_scale * frame_height, frame_center.a + frame_width_scale * frame_width, frame_center.b + frame_height_scale * frame_height );
				box32x2_c map( 0.0f, 0.0f, 1.0f, 1.0f );
				_draw_list.append_rectangle( box, map, pixel_shader, texture, draw_color, draw_shared_colors );
			}
			else if ( frame_style->texture_map_mode == menu_texture_map_mode_e_scale_to_fill )
			{
				float32_c texture_aspect_ratio = texture_width / texture_height;
				float32_c frame_width = _local_box.get_width();
				float32_c frame_height = _local_box.get_height();
				float32_c frame_aspect_ratio = frame_width / frame_height;

				float32_c texture_width_scale = 1.0f;
				float32_c texture_height_scale = 1.0f;
				if ( texture_aspect_ratio > frame_aspect_ratio )
				{
					// texture is wider than frame, so horizontal texture coordinates need to be clipped.
					texture_width_scale = 1.0f / ( ( texture_width / texture_height ) / ( frame_width / frame_height ) );
				}
				else if ( texture_aspect_ratio < frame_aspect_ratio )
				{
					// texture is taller than frame, so vertical texture coordinates need to be clipped.
					texture_height_scale = 1.0f / ( ( texture_height / texture_width ) / ( frame_height / frame_width ) );
				}

				box32x2_c map( -texture_width_scale * 0.5f + 0.5f, -texture_height_scale * 0.5f + 0.5f, texture_width_scale * 0.5f + 0.5f, texture_height_scale * 0.5f + 0.5f );
				_draw_list.append_rectangle( _local_box, map, pixel_shader, texture, draw_color, draw_shared_colors );
			}
			else if ( frame_style->texture_map_mode == menu_texture_map_mode_e_nine_slice_stretch )
			{
				for ( sint32_c row = 0; row < 3; row++ )
				{
					if ( y[ row + 1 ] > y[ row ] )
					{
						for ( sint32_c column = 0; column < 3; column++ )
						{
							if ( frame_style->texture_map_fill_middle == false && row == 1 && column == 1 )
							{
								continue;
							}
							if ( x[ column + 1 ] > x[ column ] )
							{
								box32x2_c box( x[ column ], y[ row ], x[ column + 1 ], y[ row + 1 ] );
								box32x2_c map( u[ column ], v[ row ], u[ column + 1 ], v[ row + 1 ] );
								_draw_list.append_rectangle( box, map, pixel_shader, texture, draw_color, draw_shared_colors );
							}
						}
					}
				}
			}
			else if ( frame_style->texture_map_mode == menu_texture_map_mode_e_nine_slice_tile )
			{
				float32_c fill_size[ 2 ];
				fill_size[ 0 ] = x[ 2 ] - x[ 1 ];
				fill_size[ 1 ] = y[ 2 ] - y[ 1 ];
			
				sint32_c whole_tile_count[ 2 ];
				whole_tile_count[ 0 ] = static_cast< sint32_c >( fill_size[ 0 ] / tile_size[ 0 ] );
				whole_tile_count[ 1 ] = static_cast< sint32_c >( fill_size[ 1 ] / tile_size[ 1 ] );

				float32_c partial_tile_size[ 2 ];
				partial_tile_size[ 0 ] = ( fill_size[ 0 ] - ( whole_tile_count[ 0 ] * tile_size[ 0 ] ) ) * 0.5f; // width of non-whole tiles needed to fill each end of row of whole tiles.
				partial_tile_size[ 1 ] = ( fill_size[ 1 ] - ( whole_tile_count[ 1 ] * tile_size[ 1 ] ) ) * 0.5f; // height of non-whole tiles needed to fill each end of column of whole tiles.

				// represents a tile in a row or column.
				struct interval_c
				{
					float32_c minimum_pos;
					float32_c maximum_pos;
					float32_c minimum_tex;
					float32_c maximum_tex;
				};

				// a string of these represents tiles in a row or column.
				core_list_c< interval_c > interval_list_x;
				core_list_c< interval_c > interval_list_y;

				float32_c xc = x[ 0 ];
				if ( x[ 1 ] > x[ 0 ] ) // left edge column.
				{
					interval_c * interval = interval_list_x.emplace( -1, 1 );
					interval->minimum_pos = xc;
					interval->minimum_tex = u[ 0 ];
					xc += x[ 1 ] - x[ 0 ];
					interval->maximum_pos = xc;
					interval->maximum_tex = u[ 1 ];
				}
				if ( partial_tile_size[ 0 ] > 0.0f ) // left edge partial column.
				{
					interval_c * interval = interval_list_x.emplace( -1, 1 );
					interval->minimum_pos = xc;
					interval->minimum_tex = u[ 2 ] - ( partial_tile_size[ 0 ] * texture_width_inverse );
					xc += partial_tile_size[ 0 ];
					interval->maximum_pos = xc;
					interval->maximum_tex = u[ 2 ];
				}
				if ( tile_size[ 0 ] > 0.0f ) // middle whole columns.
				{
					for ( sint32_c i = 0; i < whole_tile_count[ 0 ]; i++ )
					{
						interval_c * interval = interval_list_x.emplace( -1, 1 );
						interval->minimum_pos = xc;
						interval->minimum_tex = u[ 1 ];
						xc += tile_size[ 0 ];
						interval->maximum_pos = xc;
						interval->maximum_tex = u[ 2 ];
					}
				}
				if ( partial_tile_size[ 0 ] > 0.0f ) // right edge partial column.
				{
					interval_c * interval = interval_list_x.emplace( -1, 1 );
					interval->minimum_pos = xc;
					interval->minimum_tex = u[ 1 ];
					xc += partial_tile_size[ 0 ];
					interval->maximum_pos = xc;
					interval->maximum_tex = u[ 1 ] + ( partial_tile_size[ 0 ] * texture_width_inverse );
				}
				if ( x[ 3 ] > x[ 2 ] ) // right edge column.
				{
					interval_c * interval = interval_list_x.emplace( -1, 1 );
					interval->minimum_pos = xc;
					interval->minimum_tex = u[ 2 ];
					xc += x[ 3 ] - x[ 2 ];
					interval->maximum_pos = xc;
					interval->maximum_tex = u[ 3 ];
				}

				float32_c yc = y[ 0 ];
				if ( y[ 1 ] > y[ 0 ] ) // top edge row.
				{
					interval_c * interval = interval_list_y.emplace( -1, 1 );
					interval->minimum_pos = yc;
					interval->minimum_tex = v[ 0 ];
					yc += y[ 1 ] - y[ 0 ];
					interval->maximum_pos = yc;
					interval->maximum_tex = v[ 1 ];
				}
				if ( partial_tile_size[ 1 ] > 0.0f ) // top edge partial row.
				{
					interval_c * interval = interval_list_y.emplace( -1, 1 );
					interval->minimum_pos = yc;
					interval->minimum_tex = v[ 2 ] - ( partial_tile_size[ 1 ] * texture_height_inverse );
					yc += partial_tile_size[ 1 ];
					interval->maximum_pos = yc;
					interval->maximum_tex = v[ 2 ];
				}
				if ( tile_size[ 1 ] > 0.0f ) // middle whole rows.
				{
					for ( sint32_c i = 0; i < whole_tile_count[ 1 ]; i++ )
					{
						interval_c * interval = interval_list_y.emplace( -1, 1 );
						interval->minimum_pos = yc;
						interval->minimum_tex = v[ 1 ];
						yc += tile_size[ 1 ];
						interval->maximum_pos = yc;
						interval->maximum_tex = v[ 2 ];
					}
				}
				if ( partial_tile_size[ 1 ] > 0.0f ) // bottom edge partial row.
				{
					interval_c * interval = interval_list_y.emplace( -1, 1 );
					interval->minimum_pos = yc;
					interval->minimum_tex = v[ 1 ];
					yc += partial_tile_size[ 1 ];
					interval->maximum_pos = yc;
					interval->maximum_tex = v[ 1 ] + ( partial_tile_size[ 0 ] * texture_width_inverse );
				}
				if ( y[ 3 ] > y[ 2 ] ) // bottom edge row.
				{
					interval_c * interval = interval_list_y.emplace( -1, 1 );
					interval->minimum_pos = yc;
					interval->minimum_tex = v[ 2 ];
					yc += y[ 3 ] - y[ 2 ];
					interval->maximum_pos = yc;
					interval->maximum_tex = v[ 3 ];
				}

				for ( sint32_c yi = 0; yi < interval_list_y.get_length(); yi++ )
				{
					interval_c interval_y = interval_list_y[ yi ];
					for ( sint32_c xi = 0; xi < interval_list_x.get_length(); xi++ )
					{
						interval_c interval_x = interval_list_x[ xi ];
						box32x2_c box( interval_x.minimum_pos, interval_y.minimum_pos, interval_x.maximum_pos, interval_y.maximum_pos );
						box32x2_c map( interval_x.minimum_tex, interval_y.minimum_tex, interval_x.maximum_tex, interval_y.maximum_tex );
						_draw_list.append_rectangle( box, map, pixel_shader, texture, draw_color, draw_shared_colors );
					}
				}
			}
		}
		else //if ( frame_style->texture_map_mode == menu_frame_style_c::texture_map_mode_e_center )
		{
			// default to perfect 1:1 match between element dimensions and texture map dimensions.
			x[ 0 ] = _local_box.minimum.a; // left edge.
			x[ 3 ] = _local_box.maximum.a; // right edge.
			y[ 0 ] = _local_box.minimum.b; // top edge.
			y[ 3 ] = _local_box.maximum.b; // bottom edge.
			u[ 0 ] = u[ 1 ] = static_cast< float32_c >( frame_style_state.texture_map[ 0 ] );
			u[ 3 ] = u[ 2 ] = static_cast< float32_c >( frame_style_state.texture_map[ 2 ] );
			v[ 0 ] = v[ 1 ] = static_cast< float32_c >( frame_style_state.texture_map[ 1 ] );
			v[ 3 ] = v[ 2 ] = static_cast< float32_c >( frame_style_state.texture_map[ 3 ] );
			// calculate horizontal layout.
			overlap = ( u[ 3 ] - u[ 0 ] ) - ( x[ 3 ] - x[ 0 ] );
			if ( overlap > 0.0f )
			{
				// texture is wider than element, shrink texture coordinates.
				u[ 1 ] = u[ 0 ] + ( overlap * 0.5f );
				u[ 2 ] = u[ 3 ] - ( overlap * 0.5f );
			}
			else if ( overlap < 0.0f )
			{
				// element is wider than texture, clamp element coordinates.
				x[ 1 ] = x[ 2 ] = ( ( x[ 0 ] + x[ 3 ] ) * 0.5f );
				x[ 1 ] -= ( overlap * 0.5f );
				x[ 2 ] += ( overlap * 0.5f );
				u[ 1 ] = u[ 0 ];
				u[ 2 ] = u[ 3 ];
			}
			// calculate vertical layout.
			overlap = ( v[ 3 ] - v[ 0 ] ) - ( y[ 3 ] - y[ 0 ] );
			if ( overlap > 0.0f )
			{
				// texture is higher than element, shrink texture coordinates.
				v[ 1 ] = v[ 0 ] + ( overlap * 0.5f );
				v[ 2 ] = v[ 3 ] - ( overlap * 0.5f );
			}
			else if ( overlap < 0.0f )
			{
				// element is higher than texture, clamp element coordinates.
				y[ 1 ] = y[ 2 ] = ( ( y[ 0 ] + y [ 3 ] ) * 0.5f );
				y[ 1 ] -= ( overlap * 0.5f );
				y[ 2 ] += ( overlap * 0.5f );
				v[ 1 ] = v[ 0 ];
				v[ 2 ] = v[ 3 ];
			}
			// transform from pixel (texel) coordinate space to uv coordinate space.
			u[ 1 ] *= texture_width_inverse;
			u[ 2 ] *= texture_width_inverse;
			v[ 1 ] *= texture_height_inverse;
			v[ 2 ] *= texture_height_inverse;
			// draw the patch.
			_draw_list.append_rectangle( box32x2_c( x[ 1 ], y[ 1 ], x[ 2 ], y[ 2 ] ), box32x2_c( u[ 1 ], v[ 1 ], u[ 2 ], v[ 2 ] ), pixel_shader, texture, draw_color, draw_shared_colors );
		}

		_draw_list_is_dirty = false;
	}

	menu_element_frame_c::menu_element_frame_c( string8_c const & name )
		: menu_element_c( name )
		, _style_reference( this )
		, _override_style( nullptr )
	{
	}

	menu_element_frame_c::~menu_element_frame_c()
	{
	}

	void_c menu_element_frame_c::update_animations( float32_c time_step )
	{
	}

	void_c menu_element_frame_c::set_style_key( string8_c const & value )
	{
		_style_reference.set_key( value );
	}

	menu_frame_style_c::reference_c const & menu_element_frame_c::get_style_reference() const
	{
		return _style_reference;
	}

	menu_frame_style_c::reference_c & menu_element_frame_c::get_style_reference()
	{
		return _style_reference;
	}

	menu_frame_style_c * menu_element_frame_c::get_override_style() const
	{
		return _override_style;
	}

	void_c menu_element_frame_c::set_override_style( menu_frame_style_c * value )
	{
		_override_style = value;
	}

	resource_file_texture_c * menu_element_frame_c::get_override_texture() const
	{
		return _override_texture;
	}

	void_c menu_element_frame_c::set_override_texture( resource_file_texture_c * value )
	{
		_override_texture = value;
	}

}
