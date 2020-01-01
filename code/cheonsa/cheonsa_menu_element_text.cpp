#include "cheonsa_menu_element_text.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	void_c menu_element_text_c::_build_draw_list()
	{
		assert( _mother_control );

		_draw_list.reset();

		if ( !_is_showing_from_style || !_is_showing || _local_color.d <= 0.0f )
		{
			return;
		}

		// get text style, the text element must have one in order to be rendered.
		menu_text_style_c const * text_style = _text_style_reference.get_value();
		if ( !text_style )
		{
			text_style = engine_c::get_instance()->get_menu_style_manager()->get_default_text_style();
		}

		// last moment text reflow.
		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		// state color and saturation.
		menu_state_e state = get_state();
		menu_text_style_c::state_c const & text_style_state = text_style->state_list[ state ];
		//vector32x4_c element_color = text_style->state_list[ state ].get_expressed_color() * _local_color;
		//float32_c element_saturation = text_style->state_list[ state ].saturation;

		vector32x4_c element_color = text_style_state.get_expressed_color() * _local_color;
		vector32x4_c element_shared_colors[ 3 ]; // these will be uploaded to "menu_colors" in the shaders.
		menu_color_style_c * shared_color = nullptr;
		shared_color = engine_c::get_instance()->get_menu_style_manager()->find_shared_color_style( _shared_color_class, state, menu_shared_color_slot_e_primary );
		assert( shared_color );
		element_shared_colors[ 0 ] = shared_color->value;
		shared_color = engine_c::get_instance()->get_menu_style_manager()->find_shared_color_style( _shared_color_class, state, menu_shared_color_slot_e_secondary );
		assert( shared_color );
		element_shared_colors[ 1 ] = shared_color->value;
		shared_color = engine_c::get_instance()->get_menu_style_manager()->find_shared_color_style( _shared_color_class, state, menu_shared_color_slot_e_accent );
		assert( shared_color );
		element_shared_colors[ 2 ] = shared_color->value;

		// these lists will be used to simultaniously build geometry for different layers.
		// at the end, they will be appened to the draw batch so that the layers are drawn in the correct order.
		// static, so not thread safe.
		static core_list_c< video_renderer_vertex_menu_c > vertex_list_for_selection;
		static core_list_c< video_renderer_vertex_menu_c > vertex_list_for_glyphs;
		static core_list_c< video_renderer_vertex_menu_c > vertex_list_for_cursor;

		struct laid_out_sprite_c
		{
			resource_file_texture_c * texture_resource;
			vector32x4_c color;
			box32x2_c box;
			box32x2_c map;
		};

		core_list_c< laid_out_sprite_c > sprite_list;

		sint32_c selected_character_start;
		sint32_c selected_character_count;
		get_selected_text_range( selected_character_start, selected_character_count );
		sint32_c selected_character_end = selected_character_start + selected_character_count;

		box32x2_c margin = _get_style_margin();

		float32_c vertically_algined_y = margin.minimum.b;
		menu_text_align_vertical_e text_align_vertical = _get_style_text_align_vertical();
		if ( text_align_vertical == menu_text_align_vertical_e_center )
		{
			float32_c effective_height = _local_box.get_height() - margin.minimum.b - margin.maximum.b;
			if ( effective_height < 0.0f )
			{
				effective_height = 0.0f;
			}
			vertically_algined_y = ( effective_height * 0.5f ) - ( get_content_height() * 0.5f );
		}
		else if ( text_align_vertical == menu_text_align_vertical_e_bottom )
		{
			vertically_algined_y = _local_box.get_height() - margin.maximum.b - get_content_height();
		}

		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			menu_element_text_c::text_paragraph_c const * paragraph = _paragraph_list[ i ];

			float32_c paragraph_left = _local_box.minimum.a + margin.minimum.a + _content_offset.a;
			float32_c paragraph_right = paragraph_left + _local_box.get_width() - ( margin.maximum.a * 2 );
			float32_c paragraph_top = _local_box.minimum.b + margin.minimum.b + _content_offset.b + paragraph->_top + vertically_algined_y;
			float32_c paragraph_bottom = paragraph_top + paragraph->_content_height;

			// render this paragraph if it is visible.
			if ( paragraph_bottom >= _local_box.minimum.b && paragraph_top <= _local_box.maximum.b )
			{
				for ( sint32_c j = 0; j < paragraph->_line_list.get_length(); j++ )
				{
					text_line_c const * line = &paragraph->_line_list[ j ];

					float32_c line_left = paragraph_left + line->_left; // baseline offset from left.
					float32_c line_top = paragraph_top + line->_top;
					float32_c line_top_baseline = line_top + line->_ascender; // baseline offset from top.
					float32_c line_bottom = line_top_baseline - line->_descender;

					// render this line if it is visible.
					if ( line_bottom >= _local_box.minimum.b && line_top <= _local_box.maximum.b )
					{
						// draw selection for current line.
						if ( ( _text_interact_mode == menu_text_interact_mode_e_static_selectable || _text_interact_mode == menu_text_interact_mode_e_editable ) && selected_character_start != selected_character_end && line->_character_start < selected_character_end && selected_character_start < line->_character_end )
						{
							menu_element_text_c::text_glyph_c const * glyph_left = &paragraph->_glyph_list[ ops::math_maximum( selected_character_start, line->_character_start ) - paragraph->_character_start ];
							float32_c selection_left = line_left + glyph_left->_left;
							menu_element_text_c::text_glyph_c const * glyph_right = &paragraph->_glyph_list[ ops::math_minimum( selected_character_end, line->_character_end ) - 1 - paragraph->_character_start ];
							float32_c selection_right = line_left + glyph_right->_left + glyph_right->_horizontal_advance;
							float32_c selection_top = line_top;
							float32_c selection_bottom = line_bottom;

							menu_color_style_c const * selection_color_style = engine_c::get_instance()->get_menu_style_manager()->find_shared_color_style( _shared_color_class, get_state(), menu_shared_color_slot_e_accent );
							assert( selection_color_style );
							vector32x4_c selection_color = selection_color_style->value;
							selection_color.d *= 0.25f;

							video_renderer_vertex_menu_c * vertex = vertex_list_for_selection.emplace_at_end();
							vertex->position.a = ( selection_left );
							vertex->position.b = ( selection_top );
							vertex->color = selection_color;

							vertex = vertex_list_for_selection.emplace_at_end();
							vertex->position.a = ( selection_left );
							vertex->position.b = ( selection_bottom );
							vertex->color = selection_color;

							vertex = vertex_list_for_selection.emplace_at_end();
							vertex->position.a = ( selection_right );
							vertex->position.b = ( selection_top );
							vertex->color = selection_color;

							vertex = vertex_list_for_selection.emplace_at_end();
							vertex->position.a = ( selection_right );
							vertex->position.b = ( selection_bottom );
							vertex->color = selection_color;
						}

						sint32_c glyph_start = line->_character_start - paragraph->_character_start;
						sint32_c glyph_end = line->_character_end - paragraph->_character_start;
						for ( sint32_c k = glyph_start; k < glyph_end; k++ )
						{
							char16_c character = _plain_text.character_list[ paragraph->_character_start + k ];
							menu_element_text_c::text_glyph_c const * laid_out_glyph = &paragraph->_glyph_list[ k ];
							assert( laid_out_glyph->_style_index != -1 );
							menu_text_glyph_style_c const * text_glyph_style = _text_glyph_style_cache[ laid_out_glyph->_style_index ];
							vector32x4_c laid_out_glyph_color = text_glyph_style->color;// * element_color;
							if ( laid_out_glyph_color.d != 0.0f )
							{
								if ( ( character & 0xF000 ) != 0xF000 )
								{
									if ( laid_out_glyph->_glyph )
									{
										assert( laid_out_glyph->_glyph->key.code_point == character );

										// this glyph is displayable.
										float32_c glyph_left = line_left + laid_out_glyph->_left + laid_out_glyph->_box.minimum.a;
										float32_c glyph_top = line_top_baseline + laid_out_glyph->_box.minimum.b;
										float32_c glyph_right = line_left + laid_out_glyph->_left + laid_out_glyph->_box.maximum.a;
										float32_c glyph_bottom = line_top_baseline + laid_out_glyph->_box.maximum.b;
										float32_c glyph_map_texture_slice_index = static_cast< float32_c >( laid_out_glyph->_glyph->atlas_index );

										if ( glyph_right >= _local_box.minimum.a && glyph_left <= _local_box.maximum.a && glyph_bottom >= _local_box.minimum.b && glyph_top <= _local_box.maximum.b )
										{
											float32_c skew = text_glyph_style->skew;
											float32_c softness_bias = 0.5f / text_glyph_style->size; // softness bias, so that edges are base-line level of soft (anti-aliased) by default.
											vector32x4_c parameters = vector32x4_c( text_glyph_style->skew, text_glyph_style->weight, text_glyph_style->softness + softness_bias, static_cast< float32_c >( k + paragraph->_character_start ) );

											video_renderer_vertex_menu_c * vertex = vertex_list_for_glyphs.emplace_at_end(); // top left.
											vertex->position.a = ( glyph_left - ( laid_out_glyph->_box.minimum.b * skew ) ); // rounding coordinates isn't neccessary but it keeps all text looking consistently sharp even when positioned at sub-pixel coordinates.
											vertex->position.b = ( glyph_top );
											vertex->position.c = 0.0f;
											vertex->texture.a = laid_out_glyph->_glyph->map.minimum.a;
											vertex->texture.b = laid_out_glyph->_glyph->map.minimum.b;
											vertex->texture.c = glyph_map_texture_slice_index;
											vertex->color = laid_out_glyph_color;
											vertex->parameters = parameters;

											vertex = vertex_list_for_glyphs.emplace_at_end(); // bottom left.
											vertex->position.a = ( glyph_left + ( laid_out_glyph->_box.maximum.b * skew ) );
											vertex->position.b = ( glyph_bottom );
											vertex->position.c = 0.0f;
											vertex->texture.a = laid_out_glyph->_glyph->map.minimum.a;
											vertex->texture.b = laid_out_glyph->_glyph->map.maximum.b;
											vertex->texture.c = glyph_map_texture_slice_index;
											vertex->color = laid_out_glyph_color;
											vertex->parameters = parameters;

											vertex = vertex_list_for_glyphs.emplace_at_end(); // top right.
											vertex->position.a = ( glyph_right - ( laid_out_glyph->_box.minimum.b * skew ) );
											vertex->position.b = ( glyph_top );
											vertex->position.c = 0.0f;
											vertex->texture.a = laid_out_glyph->_glyph->map.maximum.a;
											vertex->texture.b = laid_out_glyph->_glyph->map.minimum.b;
											vertex->texture.c = glyph_map_texture_slice_index;
											vertex->color = laid_out_glyph_color;
											vertex->parameters = parameters;

											vertex = vertex_list_for_glyphs.emplace_at_end(); // bottom right.
											vertex->position.a = ( glyph_right + ( laid_out_glyph->_box.maximum.b * skew ) );
											vertex->position.b = ( glyph_bottom );
											vertex->position.c = 0.0f;
											vertex->texture.a = laid_out_glyph->_glyph->map.maximum.a;
											vertex->texture.b = laid_out_glyph->_glyph->map.maximum.b;
											vertex->texture.c = glyph_map_texture_slice_index;
											vertex->color = laid_out_glyph_color;
											vertex->parameters = parameters;
										}
									}
								}
								else
								{
									// this is an entity.
									menu_text_entity_c * entity = paragraph->_entity_list[ character & 0x0FFF ];
									if ( entity->type_code == menu_text_entity_sprite_c::type_code_static() )
									{
										// this is a sprite entity.
										menu_text_entity_sprite_c * entity_sprite = static_cast< menu_text_entity_sprite_c * >( entity );
										if ( entity_sprite->value.get_is_ready() )
										{
											resource_file_sprite_set_c::sprite_c const * current_sprite = entity_sprite->value.get_sprite();
											resource_file_sprite_set_c::frame_c const * current_frame = entity_sprite->value.get_frame();

											float32_c scale_to_size = text_glyph_style->size / entity_sprite->value.get_sprite()->font_size;

											float32_c origin_a = line_left + laid_out_glyph->_left;
											float32_c origin_b = line_top_baseline;

											laid_out_sprite_c * laid_out_sprite = sprite_list.emplace_at_end();
											laid_out_sprite->texture_resource = entity_sprite->value.get_sprite()->texture_resource;
											laid_out_sprite->color = laid_out_glyph_color;
											laid_out_sprite->box.minimum.a = origin_a + ( current_frame->box.minimum.a * scale_to_size );
											laid_out_sprite->box.minimum.b = origin_b + ( current_frame->box.minimum.b * scale_to_size );
											laid_out_sprite->box.maximum.a = origin_a + ( current_frame->box.maximum.a * scale_to_size );
											laid_out_sprite->box.maximum.b = origin_b + ( current_frame->box.maximum.b * scale_to_size );
											laid_out_sprite->map = current_frame->map;
										}
									}
								}
							}
						}
					}
					else if ( line_top > _local_box.maximum.b )
					{
						goto stitch;
					}
				}
			}
			else if ( paragraph_top > _local_box.maximum.b )
			{
				goto stitch;
			}
		}

		// render the cursor.
		// do this out-of-line with glyph rendering, because in the case that the text element has no text then we still want to be able to render the cursor.
		if ( _is_text_focused && _text_interact_mode != menu_text_interact_mode_e_static )
		{
			glyph_information_c cursor_glyph_information = _get_glyph_information( _cursor_index );
			text_paragraph_c * cursor_paragraph = _paragraph_list[ cursor_glyph_information.paragraph_index ];
			text_span_c * cursor_span = cursor_glyph_information.paragraph_span_index >= 0 ? cursor_paragraph->_span_list[ cursor_glyph_information.paragraph_span_index ] : nullptr;
			text_line_c * cursor_line = &cursor_paragraph->_line_list[ cursor_glyph_information.paragraph_line_index - cursor_paragraph->_line_index_base ];

			float32_c paragraph_left = _local_box.minimum.a + margin.minimum.a + _content_offset.a;
			float32_c paragraph_top = _local_box.minimum.b + margin.minimum.b + _content_offset.b + cursor_paragraph->_top + vertically_algined_y;

			boolean_c do_cursor = false;
			float32_c cursor_left;

			menu_color_style_c const * cursor_color_style =  engine_c::get_instance()->get_menu_style_manager()->find_shared_color_style( _shared_color_class, get_state(), menu_shared_color_slot_e_accent );
			assert( cursor_color_style );
			vector32x4_c cursor_color = cursor_color_style->value;
			if ( !_cursor_is_on_previous_line )
			{
				assert( _cursor_index >= cursor_line->_character_start && _cursor_index <= cursor_line->_character_end );
				menu_element_text_c::text_glyph_c const * laid_out_glyph = &cursor_paragraph->_glyph_list[ _cursor_index - cursor_paragraph->_character_start ];
				cursor_left = paragraph_left + cursor_line->_left + laid_out_glyph->_left;
				do_cursor = true;
			}
			else
			{
				assert( _cursor_index == cursor_line->_character_start );
				assert( cursor_glyph_information.paragraph_line_index > 0 );
				menu_element_text_c::text_glyph_c const * laid_out_glyph = &cursor_paragraph->_glyph_list[ _cursor_index - cursor_paragraph->_character_start - 1 ];
				cursor_line = &cursor_paragraph->_line_list[ cursor_glyph_information.paragraph_line_index - 1 ]; // get previous line.
				cursor_left = paragraph_left + cursor_line->_left + laid_out_glyph->_left + laid_out_glyph->_horizontal_advance;
				do_cursor = true;
			}
			if ( do_cursor )
			{
				float32_c line_top = paragraph_top + cursor_line->_top;
				float32_c line_top_baseline = line_top + cursor_line->_ascender; // baseline offset from top.
				float32_c line_bottom = line_top_baseline - cursor_line->_descender;

				float32_c cursor_width = 2.0f;
				cursor_left -= cursor_width * 0.5f;
				float32_c cursor_right = cursor_left + cursor_width;
				float32_c cursor_top = line_top;
				float32_c cursor_bottom = line_bottom;
				cursor_color.d = ops::math_saturate( ops::math_sine( _cursor_time * 2.0f ) * 0.5f + 0.5f );

				video_renderer_vertex_menu_c * vertex = vertex_list_for_cursor.emplace_at_end();
				vertex->position.a = cursor_left;
				vertex->position.b = cursor_top;
				vertex->color = cursor_color;

				vertex = vertex_list_for_cursor.emplace_at_end();
				vertex->position.a = cursor_left;
				vertex->position.b = cursor_bottom;
				vertex->color = cursor_color;

				vertex = vertex_list_for_cursor.emplace_at_end();
				vertex->position.a = cursor_right;
				vertex->position.b = cursor_top;
				vertex->color = cursor_color;

				vertex = vertex_list_for_cursor.emplace_at_end();
				vertex->position.a = cursor_right;
				vertex->position.b = cursor_bottom;
				vertex->color = cursor_color;
			}
		}

	stitch:
		// stitch the various draw lists together.
		if ( vertex_list_for_selection.get_length() > 0 )
		{
			_draw_list.append_rectangle_list( vertex_list_for_selection, engine_c::get_instance()->get_video_renderer_shader_manager()->get_menu_ps_solid_color(), nullptr, element_color, element_shared_colors );
		}
		if ( vertex_list_for_glyphs.get_length() > 0 )
		{
			_draw_list.append_rectangle_list( vertex_list_for_glyphs, engine_c::get_instance()->get_video_renderer_shader_manager()->get_menu_ps_text(), nullptr, element_color, element_shared_colors );
		}
		if ( vertex_list_for_cursor.get_length() > 0 )
		{
			_draw_list.append_rectangle_list( vertex_list_for_cursor, engine_c::get_instance()->get_video_renderer_shader_manager()->get_menu_ps_solid_color(), nullptr, element_color, element_shared_colors );
		}
		vertex_list_for_selection.remove_all();
		vertex_list_for_glyphs.remove_all();
		vertex_list_for_cursor.remove_all();
		_draw_list_is_dirty = false;
	}

	core_linked_list_c< menu_element_text_c * > menu_element_text_c::_global_list;

	void_c menu_element_text_c::invalidate_glyph_layout_of_all_instances()
	{
		core_linked_list_c< menu_element_text_c * >::node_c const * menu_element_text_list_node = _global_list.get_first();
		while ( menu_element_text_list_node )
		{
			menu_element_text_list_node->get_value()->_invalidate_glyph_layout_in_all_paragraphs();
			menu_element_text_list_node = menu_element_text_list_node->get_next();
		}
	}

	void_c menu_element_text_c::invalidate_glyph_layout_of_all_instances_with_font( resource_file_font_c * font )
	{
		core_linked_list_c< menu_element_text_c * >::node_c const * menu_element_text_list_node = _global_list.get_first();
		while ( menu_element_text_list_node )
		{
			menu_element_text_list_node->get_value()->_invalidate_glyph_layout_in_paragraphs_with_font( font );
			menu_element_text_list_node = menu_element_text_list_node->get_next();
		}
	}

	menu_element_text_c::text_line_c::text_line_c()
		: _character_start( 0 )
		, _character_end( 0 )
		, _width( 0.0f )
		, _left( 0.0f )
		, _top( 0.0f )
		, _ascender( 0.0f )
		, _descender( 0.0f )
	{
	}

	menu_element_text_c::text_line_c & menu_element_text_c::text_line_c::operator = ( text_line_c const & other )
	{
		_character_start = other._character_start;
		_character_end = other._character_end;
		_width = other._width;
		_top = other._top;
		_ascender = other._ascender;
		_descender = other._descender;
		return *this;
	}

	void_c menu_element_text_c::text_line_c::_update_horizontal_layout( menu_text_align_horizontal_e text_align_horizontal, float32_c element_width )
	{
		if ( text_align_horizontal == menu_text_align_horizontal_e_left )
		{
			_left = 0.0f;
		}
		else if ( text_align_horizontal == menu_text_align_horizontal_e_center )
		{
			_left = ( element_width * 0.5f ) - ( _width * 0.5f );
		}
		else if ( text_align_horizontal == menu_text_align_horizontal_e_right )
		{
			_left = element_width - _width;
		}
		else
		{
			assert( false );
		}
	}

	menu_element_text_c::text_glyph_c::text_glyph_c()
		: _glyph( nullptr )
		, _style_index( -1 )
		, _uri_index( -1 )
		, _box()
		, _left( 0.0f )
		, _ascender( 0.0f )
		, _descender( 0.0f )
		, _horizontal_advance( 0.0f )
	{
	}

	menu_element_text_c::text_glyph_c & menu_element_text_c::text_glyph_c::operator = ( text_glyph_c const & other )
	{
		_glyph = other._glyph;
		_style_index = other._style_index;
		_uri_index = other._uri_index;
		_box = other._box;
		_left = other._left;
		_ascender = other._ascender;
		_descender = other._descender;
		_horizontal_advance = other._horizontal_advance;
		return *this;
	}




	//
	//
	// menu_element_text_c::text_span_c
	//
	//

	void_c menu_element_text_c::text_span_c::_handle_text_style_reference_on_refreshed( menu_text_style_c * text_style )
	{
		text_span_c * root_span = this;
		assert( root_span->_mother_paragraph );
		root_span->_mother_paragraph->_is_glyph_layout_dirty = true;
		root_span->_mother_paragraph->_mother_element_text->_is_glyph_layout_dirty = true;
	}

	menu_element_text_c::text_span_c::text_span_c()
		: _mother_paragraph( nullptr )
		, _character_start( -1 )
		, _character_end( -1 )
		, _text_style_reference()
	{
	}

	void_c menu_element_text_c::text_span_c::_handle_insert_character_range( sint32_c character_start, sint32_c character_count )
	{
		// process this.
		if ( character_start < _character_start )
		{
			_character_start += character_count;
		}
		else if ( character_start <= _character_end )
		{
			_character_end += character_count;
		}
	}

	void_c menu_element_text_c::_remove_spans_in_character_range( core_list_c< text_span_c * > & span_list, sint32_c character_start, sint32_c character_count )
	{
		sint32_c character_end_plus_one = character_start + character_count;
		for ( sint32_c i = 0; i < span_list.get_length(); i++ )
		{
			text_span_c * span = span_list[ i ];

			if ( character_start > span->_character_end )
			{
				// nothing to do yet.
				continue;
			}
			else if ( character_end_plus_one <= span->_character_start )
			{
				// nothing left to do.
				return;
			}

			if ( character_start <= span->_character_start && character_end_plus_one > span->_character_end )
			{
				// character range entirely contains this span.
				// delete this span.
				span_list.remove_at_index( i );
				delete span;
				i--;
				continue;
			}
			else if ( character_start > span->_character_start && character_end_plus_one - 1 < span->_character_end )
			{
				// character range is entirely contained by this span.

				// create new span to hold everything on right side of split.
				text_span_c * right_span = new text_span_c();
				right_span->_mother_paragraph = span->_mother_paragraph;
				right_span->_character_start = character_end_plus_one;
				right_span->_character_end = span->_character_end;
				right_span->_text_style_reference.set_key( span->_text_style_reference.get_key() );
				span_list.insert_at_index( i, right_span );

				// move current span to left side of split.
				span->_character_end = character_start - 1;

				return;
			}
			else if ( character_start > span->_character_start && character_end_plus_one > span->_character_end )
			{
				// character range intersects with tail end of this span.
				span->_character_end = character_start - 1;
			}
			else if ( character_start <= span->_character_start && character_end_plus_one - 1 < span->_character_end )
			{
				// character range intersects with head end of this span.
				span->_character_start = character_end_plus_one;
			}
		}
	}

	resource_file_font_c * menu_element_text_c::text_span_c::_get_style_font() const
	{
		text_span_c const * span = this;
		if ( span->_text_style_reference.get_value() )
		{
			if ( span->_text_style_reference.get_value()->font_is_defined )
			{
				assert( span->_text_style_reference.get_value()->font.is_reference_set_and_loaded() );
				return span->_text_style_reference.get_value()->font;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->_get_style_font();
	}

	vector32x4_c menu_element_text_c::text_span_c::_get_style_color() const
	{
		text_span_c const * span = this;
		if ( span->_text_style_reference.get_value() )
		{
			/*if ( span->_text_style_reference.get_value()->color_style_is_defined && span->_text_style_reference.get_value()->color_style.get_value() )
			{
				return span->_text_style_reference.get_value()->color_style.get_value()->value;
			}
			else */if ( span->_text_style_reference.get_value()->color_is_defined )
			{
				return span->_text_style_reference.get_value()->color;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->_get_style_color();
	}

	float32_c menu_element_text_c::text_span_c::_get_style_size() const
	{
		text_span_c const * span = this;
		if ( span->_text_style_reference.get_value() )
		{
			if ( span->_text_style_reference.get_value()->size_is_defined )
			{
				return span->_text_style_reference.get_value()->size;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->_get_style_size();
	}

	float32_c menu_element_text_c::text_span_c::_get_style_skew() const
	{
		text_span_c const * span = this;
		if ( span->_text_style_reference.get_value() )
		{
			if ( span->_text_style_reference.get_value()->skew_is_defined )
			{
				return span->_text_style_reference.get_value()->skew;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->_get_style_skew();
	}

	float32_c menu_element_text_c::text_span_c::_get_style_weight() const
	{
		text_span_c const * span = this;
		if ( span->_text_style_reference.get_value() )
		{
			if ( span->_text_style_reference.get_value()->weight_is_defined )
			{
				return span->_text_style_reference.get_value()->weight;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->_get_style_weight();
	}

	float32_c menu_element_text_c::text_span_c::_get_style_softness() const
	{
		text_span_c const * span = this;
		if ( span->_text_style_reference.get_value() )
		{
			if ( span->_text_style_reference.get_value()->softness_is_defined )
			{
				return span->_text_style_reference.get_value()->softness;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->_get_style_softness();
	}

	float32_c menu_element_text_c::text_span_c::_get_style_glyph_spacing() const
	{
		text_span_c const * span = this;
		if ( span->_text_style_reference.get_value() )
		{
			if ( span->_text_style_reference.get_value()->glyph_spacing_is_defined )
			{
				return span->_text_style_reference.get_value()->glyph_spacing;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->_get_style_glyph_spacing();
	}

	menu_text_glyph_style_c menu_element_text_c::text_span_c::_get_style() const
	{
		menu_text_glyph_style_c result;
		result.font = _get_style_font();
		result.color = _get_style_color();
		result.size = _get_style_size();
		result.weight = _get_style_weight();
		result.skew = _get_style_skew();
		result.softness = _get_style_softness();
		return result;
	}




	//
	//
	// menu_element_text_c::text_paragraph_c
	//
	//

	menu_element_text_c::text_span_c const * menu_element_text_c::text_paragraph_c::_find_span( sint32_c character_index ) const
	{
		for ( sint32_c i = 0; i < _span_list.get_length(); i++ )
		{
			text_span_c const * span = _span_list[ i ];
			if ( character_index >= span->_character_start && character_index <= span->_character_end )
			{
				return span;
			}
			else if ( span->_character_start > character_index )
			{
				break;
			}
		}
		return nullptr;
	}

	void_c menu_element_text_c::text_paragraph_c::_handle_text_style_reference_on_refreshed( menu_text_style_c::reference_c const * text_style_reference )
	{
		assert( text_style_reference == &_text_style_reference );
		_mother_element_text->_is_glyph_layout_dirty = true;
		_is_glyph_layout_dirty = true;
	}

	menu_element_text_c::text_paragraph_c::text_paragraph_c()
		: _mother_element_text( nullptr )
		, _character_start( -1 )
		, _character_end( -1 )
		, _text_style_reference()
		, _span_list()
		, _top( 0.0f )
		, _content_width( 0.0f )
		, _content_height( 0.0f )
		, _line_index_base( 0 )
		, _line_list()
		, _glyph_list()
		, _entity_list()
		, _is_glyph_layout_dirty( false )
	{
		_text_style_reference.on_refreshed.subscribe( this, &text_paragraph_c::_handle_text_style_reference_on_refreshed );
	}

	menu_element_text_c::text_paragraph_c::~text_paragraph_c()
	{
		_entity_list.remove_and_delete_all();
		_span_list.remove_and_delete_all();
	}

	void_c menu_element_text_c::text_paragraph_c::_do_glyph_layout()
	{
		assert( _character_end >= _character_start );
		assert( _mother_element_text->_plain_text.character_list[ _character_end ] == L'\n' );
		_content_width = 0.0f;
		_line_list.remove_all();
		_glyph_list.remove_all();

		box32x2_c margin = _mother_element_text->_get_style_margin();

		float32_c word_wrap_width = _mother_element_text->_local_box.get_width() - ( margin.minimum.a + margin.minimum.b );
		if ( word_wrap_width < 1.0f )
		{
			word_wrap_width = 1.0f;
		}
		boolean_c word_wrap_has_word = false; // will be set to true if we are parsing a string of displayable characters.
		boolean_c word_wrap_has_space = false; // will be set to true if we are parsing a string of invisible characters.
		boolean_c word_wrap_just_wrapped = true; // will be set to true if a virtual line break was just inserted to prevent a perpetual insertion of virtual line breaks for words that are wider than word_wrap_width. is initially set to true to prevent a virtual line break from being inserted at character index 0, in the case that the very first word is wider than the box.
		sint32_c current_line_start = 0; // index of character in _plain_text that is the first character of the current line.
		float32_c current_word_width = 0.0f; // current accumulated width of consecutive displayable non-line-breaking characters.
		sint32_c current_word_start = 0; // index of character in _plain_text that is the first character of the current word.
		float32_c current_space_width = 0.0f; // current accumulated width of consecutive spaces. space width does not contribute to the width of the line until it is followed up by a word.
		float32_c current_line_top = 0.0f; // current offset of top of line from top of paragraph.
		float32_c current_line_width = 0.0f; // current accumulated width of line.
		float32_c current_line_spacing = _get_style_line_spacing(); // extra space to place between lines is defined by the paragraph.
		float32_c current_laid_out_ascender; // current unquantized ascender for the current style of the current character.
		float32_c current_laid_out_descender; // current unquantized descender for the current style of the current character.
		float32_c current_laid_out_glyph_spacing; // current unquantized glyph spacing (extra horizontal advance) for the current style of the current character.
		float32_c current_scale_to_unquantize_size; // corrective scale, to scale glyph from size it was baked at (quantized) to arbitrary size that the style wants it to be rendered at (dequantized).

		text_span_c const * current_span = nullptr; // span of the current character, may be nullptr.

		menu_text_glyph_style_c current_text_glyph_style; // used to build compiled text glyph style based on element's style, paragraph's style, and any span's style (of which may be nested).
		menu_text_glyph_style_c const * current_text_glyph_style_in_cache = nullptr; // cached equivalent of current_style_for_text_glyph, 
		uint16_c current_text_glyph_style_in_cache_index = -1;
		menu_text_glyph_style_c const * last_text_glyph_style_in_cache = nullptr;

		char16_c last_code_point = 0; // previous character.
		char16_c current_code_point = 0; // current character.

		boolean_c span_changed = true;
		for ( sint32_c j = _character_start; j <= _character_end; j++ )
		{
			// exit current span if end was reached.
			last_text_glyph_style_in_cache = current_text_glyph_style_in_cache;
			if ( current_span && j > current_span->_character_end )
			{
				current_span = nullptr;
				span_changed = true;
			}

			// enter new span if one exists.
			if ( !current_span )
			{
				current_span = _find_span( j );
				if ( current_span )
				{
					span_changed = true;
				}
			}

			// handle change over of current span.
			if ( span_changed )
			{
				if ( current_span )
				{
					current_text_glyph_style = current_span->_get_style();
					current_laid_out_glyph_spacing = current_span->_get_style_glyph_spacing() * current_text_glyph_style.size;
				}
				else
				{
					current_text_glyph_style = _get_style();
					current_laid_out_glyph_spacing = _get_style_glyph_spacing() * current_text_glyph_style.size;
				}
				current_scale_to_unquantize_size = current_text_glyph_style.size / static_cast< float32_c >( glyph_manager_c::get_quantized_size( current_text_glyph_style.size ) );
				current_laid_out_ascender = current_text_glyph_style.font->get_unquantized_ascender( current_text_glyph_style.size );
				current_laid_out_descender = current_text_glyph_style.font->get_unquantized_descender( current_text_glyph_style.size );
				current_space_width = current_text_glyph_style.font->get_unquantized_horizontal_advance_for_space( current_text_glyph_style.size );
				_mother_element_text->_cache_text_glyph_style( current_text_glyph_style, current_text_glyph_style_in_cache, current_text_glyph_style_in_cache_index );
			}

			assert( current_text_glyph_style.color.d != 0.0f );

			// process the current character.
			boolean_c current_character_is_space = false;
			last_code_point = current_code_point;
			current_code_point = _mother_element_text->_plain_text.character_list[ j ];
			text_glyph_c * current_glyph = _glyph_list.emplace_at_end(); // create the laid out glyph instance that maps to the current character.
			current_glyph->_glyph = nullptr;
			current_glyph->_left = 0.0f; // will be set properly when _finalize_glyph_flow_for_line() is called.
			current_glyph->_ascender = current_laid_out_ascender;
			current_glyph->_descender = current_laid_out_descender;
			current_glyph->_style_index = current_text_glyph_style_in_cache_index;

			if ( current_code_point != L'\n' && current_code_point != L' ' && current_code_point != L'\t' && ( current_code_point & 0xF000 ) != 0xF000 )
			{
				// this character is displayable.
				glyph_c const * glyph = engine_c::get_instance()->get_glyph_manager()->load_quantized_glyph( current_text_glyph_style.font, current_text_glyph_style_in_cache->size, current_code_point );
				if ( !glyph )
				{
					// the glyph cache filled up.
					// since we tried to load a glyph and it failed, the glyph cache knows that its full, and it will request the engine to clear|reset it at the start of the next update frame.
					// a reflow will be triggered for all text elements instances in existence and we will try to do layout again.
					return;
				}

				assert( glyph->key.code_point == current_code_point || glyph->key.code_point == L'?' );

				// create laid out glyph for the current character.
				current_glyph->_glyph = glyph;
				current_glyph->_box.minimum.a = glyph->box.minimum.a * current_scale_to_unquantize_size;
				current_glyph->_box.minimum.b = glyph->box.minimum.b * current_scale_to_unquantize_size;
				current_glyph->_box.maximum.a = glyph->box.maximum.a * current_scale_to_unquantize_size;
				current_glyph->_box.maximum.b = glyph->box.maximum.b * current_scale_to_unquantize_size;
				current_glyph->_horizontal_advance = ( glyph->horizontal_advance * current_scale_to_unquantize_size ) + current_laid_out_glyph_spacing;

				// apply kerning if possible.
				if ( last_text_glyph_style_in_cache && last_text_glyph_style_in_cache->font == current_text_glyph_style_in_cache->font )
				{
					current_glyph->_horizontal_advance += current_text_glyph_style_in_cache->font->find_kern_advance( last_text_glyph_style_in_cache->size, last_code_point, current_text_glyph_style_in_cache->size, current_code_point );
				}
			}
			else if ( current_code_point == ' ' )
			{
				current_character_is_space = true;

				// create laid out glyph of width of space.
				current_glyph->_horizontal_advance = ( current_space_width * current_scale_to_unquantize_size ) + current_laid_out_glyph_spacing;
				current_glyph->_box.minimum.a = 0.0f;
				current_glyph->_box.minimum.b = current_laid_out_ascender * current_scale_to_unquantize_size;
				current_glyph->_box.maximum.a = current_glyph->_horizontal_advance;
				current_glyph->_box.maximum.b = current_laid_out_descender * current_scale_to_unquantize_size;
			}
			else if ( current_code_point == L'\t' )
			{
				current_character_is_space = true;

				// create laid out glyph of width of tab, which is dynamic.
				current_glyph->_horizontal_advance = ( current_space_width * current_scale_to_unquantize_size ) + current_laid_out_glyph_spacing;
				current_glyph->_box.minimum.a = 0.0f;
				current_glyph->_box.minimum.b = current_laid_out_ascender * current_scale_to_unquantize_size;
				current_glyph->_box.maximum.a = current_glyph->_horizontal_advance;
				current_glyph->_box.maximum.b = current_laid_out_descender * current_scale_to_unquantize_size;
			}
			else if ( current_code_point == L'\n' )
			{
				current_character_is_space = false;

				// create laid out glyph of same size as space character.
				current_glyph->_horizontal_advance = ( current_space_width * current_scale_to_unquantize_size ) + current_laid_out_glyph_spacing;
				current_glyph->_box.minimum.a = 0.0f;
				current_glyph->_box.minimum.b = current_laid_out_ascender * current_scale_to_unquantize_size;
				current_glyph->_box.maximum.a = current_glyph->_horizontal_advance;
				current_glyph->_box.maximum.b = current_laid_out_descender * current_scale_to_unquantize_size;

				_do_glyph_layout_finish_line( current_line_start, _glyph_list.get_length() - 1, current_line_width + current_word_width + current_space_width, current_line_top );
				current_line_start = j - _character_start + 1;
				assert( current_line_start >= 0 );
				current_word_start = j - _character_start + 1;
				current_line_width = 0.0f;
				current_word_width = 0.0f;
				current_space_width = 0.0f;

				word_wrap_has_word = false;
				word_wrap_has_space = false;

				continue;
			}
			//else if ( ( current_code_point & 0xF000 ) == 0xF000 )
			//{
			//	// current character is referencing an entity.
			//	current_glyph->_glyph = nullptr;
			//	current_glyph->_box.minimum.a = 0.0f; // box is not used by sprite entities.
			//	current_glyph->_box.minimum.b = current_laid_out_ascender;
			//	current_glyph->_box.maximum.a = 0.0f;
			//	current_glyph->_box.maximum.b = current_laid_out_descender;
			//	current_glyph->_left = 0.0f;
			//	current_glyph->_ascender = current_laid_out_ascender;
			//	current_glyph->_descender = current_laid_out_descender;
			//	current_glyph->_style_index = current_text_glyph_style_in_cache_index;
			//	// the entity instance exists in the paragraph's entity list from when the rich text markup was processed, retreive it.
			//	uint16_c entity_index = 0x0FFF & current_code_point;
			//	menu_text_entity_c * entity = _entity_list[ entity_index ];
			//	if ( entity->type_code == menu_text_entity_sprite_c::type_code_static() )
			//	{
			//		menu_text_entity_sprite_c * entity_sprite = static_cast< menu_text_entity_sprite_c * >( entity );
			//		if ( entity_sprite->value.get_is_ready() )
			//		{
			//			current_glyph->_horizontal_advance = entity_sprite->value.get_sprite()->font_horizontal_advance * ( current_text_glyph_style_in_cache->size / entity_sprite->value.get_sprite()->font_size );
			//		}
			//		else
			//		{
			//			current_glyph->_horizontal_advance = current_text_glyph_style_in_cache->size;
			//		}
			//	}
			//}

			// perform word wrap if needed.
			if ( _mother_element_text->_word_wrap && _mother_element_text->_multi_line )
			{
				if ( !current_character_is_space )
				{
					if ( !word_wrap_has_word )
					{
						word_wrap_has_word = true;
						current_word_start = j - _character_start;
						current_word_width = 0.0f;
					}
					current_word_width += current_glyph->_horizontal_advance;
					if ( word_wrap_has_space )
					{
						word_wrap_has_space = false;
					}
					if ( !word_wrap_just_wrapped && current_line_width + current_space_width + current_word_width - current_laid_out_glyph_spacing > word_wrap_width )
					{
						_do_glyph_layout_finish_line( current_line_start, current_word_start - 1, current_line_width, current_line_top ); // note that automatic word wrapping does not factor in the current accumulated width of white space at the end of the line, this lets the line ignore this trailing white space when text is horizontally aligned center or right.
						current_line_start = current_word_start;
						current_line_width = 0.0f;
						current_space_width = 0.0f;
						current_word_start = j - _character_start;
						word_wrap_just_wrapped = true;
					}
				}
				else
				{
					if ( word_wrap_has_word )
					{
						word_wrap_has_word = false;
						current_line_width += current_space_width + current_word_width;
						current_word_width = 0.0f;
					}
					if ( !word_wrap_has_space )
					{
						word_wrap_has_space = true;
						word_wrap_just_wrapped = false;
						current_space_width = 0.0f;
					}
					current_space_width += current_glyph->_horizontal_advance;
				}
			}
			else
			{
				current_line_width += current_glyph->_horizontal_advance;
			}
		}
		assert( _line_list.get_length() > 0 );
		_top = -1.0f; // invalidate this value, it will be recalculated via subsequent call to _update_vertical_layout_of_paragraphs_at_and_after().
		_content_height = current_line_top - current_line_spacing; // subtract extra line spacing that would have been added by previous call to _finalize_glyph_flow_for_line().
		_is_glyph_layout_dirty = false;
	}

	void_c menu_element_text_c::text_paragraph_c::_do_glyph_layout_finish_line( sint32_c character_start, sint32_c character_end, float32_c laid_out_width, float32_c & laid_out_top )
	{
		assert( character_start >= _character_start );
		assert( character_end - _character_start < _glyph_list.get_length() );

		// advance by extra line spacing if needed.
		if ( _line_list.get_length() > 0 )
		{
			laid_out_top += _get_style_line_spacing();
		}

		// create new laid out line.
		text_line_c * line = _line_list.emplace_at_end();
		line->_character_start = _character_start + character_start;
		line->_character_end = _character_start + character_end;
		line->_width = laid_out_width;
		if ( _content_width < laid_out_width )
		{
			_content_width = laid_out_width;
		}

		// take into account horizontal text alignment.
		line->_update_horizontal_layout( _get_style_text_align_horizontal(), _mother_element_text->_local_box.get_width() );

		// this is straight forward because it's relative to the paragraph's top, so we don't need to take into account vertical text alignment here, that is done in the paragraph.
		line->_top = laid_out_top;

		// find largest ascender and descender so that we can find out where to place the base line and how tall the line is.
		// and finalize the horizontal position the glyphs.
		float32_c cursor_x = 0.0f; //line->_left;
		float32_c ascender = 0.0f; // accumulates the largest positive value above the baseline.
		float32_c descender = 0.0f; // accumulates the largest negative value below the baseline.
		for ( sint32_c i = character_start; i <= character_end; i++ )
		{
			text_glyph_c * glyph = &_glyph_list[ i - _character_start ];
			if ( glyph->_ascender > ascender )
			{
				ascender = glyph->_ascender;
			}
			if ( glyph->_descender < descender )
			{
				descender = glyph->_descender;
			}
			glyph->_left += cursor_x;
			cursor_x += glyph->_horizontal_advance;
		}
		line->_ascender = ascender;
		line->_descender = descender;

		// advance to bottom of line.
		laid_out_top += ascender - descender;
	}

	sint32_c menu_element_text_c::_handle_delete_character_range( sint32_c character_start, sint32_c character_count, sint32_c & _character_start, sint32_c & _character_end )
	{
		assert( character_start >= 0 && character_count > 0 );
		assert( character_start <= _character_end );

		sint32_c result = 0;
		sint32_c character_end_plus_one = character_start + character_count;

		if ( character_start <= _character_start && character_end_plus_one > _character_end )
		{
			// delete character range contains this entire paragraph's character range.
			// tell our mother to delete this.
			return -1;
		}
		else if ( character_end_plus_one <= _character_start )
		{
			// delete character range is entirely before this paragraph's character range.
			_character_start -= character_count;
			_character_end -= character_count;
		}
		else if ( character_start <= _character_start && character_end_plus_one - 1 < _character_end )
		{
			// delete character range clips head end of this paragraph's character range.
			_character_end -= character_end_plus_one - _character_start;
			_character_start = character_start;
		}
		else if ( character_start > _character_start && character_end_plus_one > _character_end )
		{
			// delete character range clips tail end of this paragraph's character range.
			_character_end = character_start - 1;
			result = 1;
		}
		else if ( character_start >= _character_start && character_end_plus_one - 1 < _character_end )
		{
			// delete character range is entirely contained in this paragraph's character range.
			_character_end -= character_count;
		}
		else
		{
			assert( false );
		}

		return result;
	}

	sint32_c menu_element_text_c::text_paragraph_c::_handle_delete_character_range( sint32_c character_start, sint32_c character_count )
	{
		assert( _mother_element_text );
		assert( _mother_element_text->_text_interact_mode == menu_text_interact_mode_e_editable );

		sint32_c result = menu_element_text_c::_handle_delete_character_range( character_start, character_count, _character_start, _character_end );
		if ( result >= 0 )
		{
			_is_glyph_layout_dirty = true;
			for ( sint32_c i = 0; i < _span_list.get_length(); i++ )
			{
				text_span_c * span = _span_list[ i ];
				if ( character_start > span->_character_end )
				{
					// nothing to do yet.
					continue;
				}
				sint32_c sub_result = menu_element_text_c::_handle_delete_character_range( character_start, character_count, span->_character_start, span->_character_end );
				if ( sub_result == -1 )
				{
					_span_list.remove_at_index( i );
					delete span;
					i--;
				}
			}
		}
		return result;
	}

	void_c menu_element_text_c::text_paragraph_c::_handle_insert_character_range( sint32_c character_start, sint32_c character_count )
	{
		assert( character_start <= _character_end );
		assert( character_count > 0 );
		if ( character_start < _character_start )
		{
			_character_start += character_count;
			_character_end += character_count;
		}
		else if ( character_start <= _character_end )
		{
			_character_end += character_count;
		}
		for ( sint32_c i = 0; i < _span_list.get_length(); i++ )
		{
			text_span_c * span = _span_list[ i ];
			span->_handle_insert_character_range( character_start, character_count );
		}
		_is_glyph_layout_dirty = true;
	}

	menu_element_text_c::text_span_c * menu_element_text_c::text_paragraph_c::_get_span_at_character_range( sint32_c character_start, sint32_c character_count )
	{
		sint32_c character_end_plus_one = character_start + character_count;
		assert( character_start >= _character_start && character_end_plus_one - 1 <= _character_end );
		for ( sint32_c i = 0; i < _span_list.get_length(); i++ )
		{
			text_span_c * span = _span_list[ i ];
			if ( character_start > span->_character_end )
			{
				// nothing to do yet.
				continue;
			}
			else if ( character_end_plus_one <= span->_character_start )
			{
				// nothing left to do.
				break;
			}
			else if ( character_start >= span->_character_start && character_end_plus_one - 1 <= span->_character_end )
			{
				// character range is entirely within the current span.
				return span;
			}
			// character range intersects with boundaries of current span, so no result.
			return nullptr;
		}
		// no span of interest found.
		return nullptr;
	}

	menu_element_text_c::text_line_c const * menu_element_text_c::text_paragraph_c::_get_line_at_character_index( sint32_c character_index, sint32_c * result_line_index ) const
	{
		if ( result_line_index )
		{
			*result_line_index = -1;
		}
		for ( sint32_c i = 0; i < _line_list.get_length(); i++ )
		{
			text_line_c const * line = &_line_list[ i ];
			if ( character_index >= line->_character_start && character_index <= line->_character_end )
			{
				if ( result_line_index )
				{
					*result_line_index = i;
				}
				return line;
			}
		}
		assert( false );
		return nullptr;
	}

	menu_element_text_c::text_line_c * menu_element_text_c::text_paragraph_c::_get_line_at_character_index( sint32_c character_index, sint32_c * result_line_index )
	{
		if ( result_line_index )
		{
			*result_line_index = -1;
		}
		for ( sint32_c i = 0; i < _line_list.get_length(); i++ )
		{
			text_line_c * line = &_line_list[ i ];
			if ( character_index >= line->_character_start && character_index <= line->_character_end )
			{
				if ( result_line_index )
				{
					*result_line_index = i;
				}
				return line;
			}
		}
		assert( false );
		return nullptr;
	}

	menu_text_align_horizontal_e menu_element_text_c::text_paragraph_c::_get_style_text_align_horizontal() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value())
		{
			if ( _text_style_reference.get_value()->text_align_horizontal_is_defined )
			{
				return _text_style_reference.get_value()->text_align_horizontal;
			}
		}
		return _mother_element_text->_get_style_text_align_horizontal();
	}

	float32_c menu_element_text_c::text_paragraph_c::_get_style_paragraph_spacing() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->paragraph_spacing_is_defined )
			{
				return _text_style_reference.get_value()->paragraph_spacing;
			}
		}
		return _mother_element_text->_get_style_paragraph_spacing();
	}

	float32_c menu_element_text_c::text_paragraph_c::_get_style_line_spacing() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->line_spacing_is_defined )
			{
				return _text_style_reference.get_value()->line_spacing;
			}
		}
		return _mother_element_text->_get_style_line_spacing();
	}

	float32_c menu_element_text_c::text_paragraph_c::_get_style_glyph_spacing() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->glyph_spacing_is_defined )
			{
				return _text_style_reference.get_value()->glyph_spacing;
			}
		}
		return _mother_element_text->_get_style_glyph_spacing();
	}

	resource_file_font_c * menu_element_text_c::text_paragraph_c::_get_style_font() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->font_is_defined )
			{
				assert( _text_style_reference.get_value()->font.is_reference_set_and_loaded() );
				return _text_style_reference.get_value()->font;
			}
		}
		return _mother_element_text->_get_style_font();
	}

	vector32x4_c menu_element_text_c::text_paragraph_c::_get_style_color() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value() )
		{
			/*if ( _text_style_reference.get_value()->color_style_is_defined && _text_style_reference.get_value()->color_style.get_value() )
			{
				return _text_style_reference.get_value()->color_style.get_value()->value;
			}
			else */if ( _text_style_reference.get_value()->color_is_defined )
			{
				return _text_style_reference.get_value()->color;
			}
		}
		return _mother_element_text->_get_style_color();
	}

	float32_c menu_element_text_c::text_paragraph_c::_get_style_size() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->size_is_defined )
			{
				return _text_style_reference.get_value()->size;
			}
		}
		return _mother_element_text->_get_style_size();
	}

	float32_c menu_element_text_c::text_paragraph_c::_get_style_skew() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->skew_is_defined )
			{
				return _text_style_reference.get_value()->skew;
			}
		}
		return _mother_element_text->_get_style_skew();
	}

	float32_c menu_element_text_c::text_paragraph_c::_get_style_weight() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->weight_is_defined )
			{
				return _text_style_reference.get_value()->weight;
			}
		}
		return _mother_element_text->_get_style_weight();
	}

	float32_c menu_element_text_c::text_paragraph_c::_get_style_softness() const
	{
		assert( _mother_element_text );
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->softness_is_defined )
			{
				return _text_style_reference.get_value()->softness;
			}
		}
		return _mother_element_text->_get_style_softness();
	}

	menu_text_glyph_style_c menu_element_text_c::text_paragraph_c::_get_style() const
	{
		menu_text_glyph_style_c result;
		result.font = _get_style_font();
		result.color = _get_style_color();
		result.size = _get_style_size();
		result.weight = _get_style_weight();
		result.skew = _get_style_skew();
		result.softness = _get_style_softness();
		return result;
	}

	void_c menu_element_text_c::_handle_text_style_reference_on_refreshed( menu_text_style_c::reference_c const * value )
	{
		_invalidate_glyph_layout_in_all_paragraphs();
	}

	void_c menu_element_text_c::_clear_cached_data()
	{
		_text_glyph_style_cache.remove_and_delete_all();
		//_uri_cache.remove_and_delete_all();
	}

	void_c menu_element_text_c::_cache_text_glyph_style( menu_text_glyph_style_c const & value, menu_text_glyph_style_c const * & result_pointer, uint16_c & result_index )
	{
		for ( sint32_c i = 0; i < _text_glyph_style_cache.get_length(); i++ )
		{
			if ( *_text_glyph_style_cache[ i ] == value )
			{
				result_pointer = _text_glyph_style_cache[ i ];
				result_index = static_cast< uint16_c >( i );
				return;
			}
		}
		result_index = static_cast< uint16_c >( _text_glyph_style_cache.get_length() );
		result_pointer = new menu_text_glyph_style_c( value );
		_text_glyph_style_cache.insert_at_end( result_pointer );
	}

	//uint16_c menu_element_text_c::_cache_uri( string8_c const & value )
	//{
	//	for ( sint32_c i = 0; i < _uri_cache.get_length(); i++ )
	//	{
	//		if ( *_uri_cache[ i ] == value )
	//		{
	//			return static_cast< uint16_c >( i );
	//		}
	//	}
	//	_uri_cache.insert_at_end( new string8_c( value ) );
	//	return static_cast< uint16_c >( _uri_cache.get_length() - 1 );
	//}

	void_c menu_element_text_c::_invalidate_glyph_layout_in_all_paragraphs()
	{
		_is_glyph_layout_dirty = true;
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
			paragraph->_is_glyph_layout_dirty = true;
		}
	}

	void_c menu_element_text_c::_invalidate_glyph_layout_in_paragraphs_with_font( resource_file_font_c * font )
	{
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
			if ( paragraph->_get_style_font() == font )
			{
				_is_glyph_layout_dirty = true;
				paragraph->_is_glyph_layout_dirty = true;
			}
			else
			{
				for ( sint32_c span_index = 0; span_index < paragraph->_span_list.get_length(); span_index++ )
				{
					text_span_c * span = paragraph->_span_list[ span_index ];
					if ( span->_get_style_font() == font )
					{
						_is_glyph_layout_dirty = true;
						paragraph->_is_glyph_layout_dirty = true;
						break;
					}
				}
			}
		}
	}

	void_c menu_element_text_c::_do_glyph_layout()
	{
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
			if ( paragraph->_is_glyph_layout_dirty )
			{
				paragraph->_do_glyph_layout();
			}
		}
		_is_glyph_layout_dirty = false;
		_update_vertical_layout_of_all_paragraphs();
		_cursor_is_on_previous_line = false; // cancel this state, so things don't break.
		_update_cursor_sticky_x();
	}

	void_c menu_element_text_c::_update_horizontal_layout_of_all_lines()
	{
		assert( !_is_glyph_layout_dirty );
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			assert( !paragraph->_is_glyph_layout_dirty );
			for ( sint32_c j = 0; j < paragraph->_line_list.get_length(); j++ )
			{
				text_line_c * line = &paragraph->_line_list[ j ];
				line->_update_horizontal_layout( paragraph->_get_style_text_align_horizontal(), _local_box.get_width() );
			}
		}
	}

	void_c menu_element_text_c::_update_vertical_layout_of_all_paragraphs()
	{
		_content_width = 0.0f;
		_content_height = 0.0f;
		float32_c paragraph_spacing = _get_style_paragraph_spacing();
		float32_c paragraph_top = 0.0f;
		sint32_c paragraph_line_index_base = 0;
		text_paragraph_c * paragraph = nullptr;
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			paragraph = _paragraph_list[ i ];
			if ( i > 0 )
			{
				paragraph_top += paragraph_spacing;
			}
			paragraph->_line_index_base = paragraph_line_index_base;
			paragraph_line_index_base += paragraph->_line_list.get_length();
			paragraph->_top = paragraph_top;
			paragraph_top += paragraph->_content_height;
			if ( paragraph->_content_width > _content_width )
			{
				_content_width = paragraph->_content_width;
			}
		}
		if ( paragraph )
		{
			float32_c paragraph_top_offset = 0.0f;
			_content_height = paragraph->_top + paragraph->_content_height;
		}
	}

	void_c menu_element_text_c::_place_cursor_at_line_index( sint32_c line_index, boolean_c shift )
	{
		assert( !_is_glyph_layout_dirty );

		// cap line.
		text_paragraph_c * last_paragraph = _paragraph_list[ _paragraph_list.get_length() - 1 ];
		sint32_c last_line_index = last_paragraph->_line_index_base + last_paragraph->_line_list.get_length() - 1;
		if ( line_index < 0 )
		{
			line_index = 0;
		}
		else if ( line_index > last_line_index )
		{
			line_index = last_line_index;
		}

		// find target paragraph and line to move cursor to.
		text_paragraph_c * paragraph = nullptr;
		text_line_c * line = nullptr;
		boolean_c line_is_virtual = false; // virtual lines are generated by word wrapping, they don't end with a new line character and they are not the last line in a paragraph.
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			paragraph = _paragraph_list[ paragraph_index ];
			if ( line_index >= paragraph->_line_index_base && line_index < paragraph->_line_index_base + paragraph->_line_list.get_length() )
			{
				line = &paragraph->_line_list[ line_index - paragraph->_line_index_base ];
				line_is_virtual = _plain_text.character_list[ line->_character_end ] != L'\n' && line_index - paragraph->_line_index_base < paragraph->_line_list.get_length() - 1;
				break;
			}
		}

		// place cursor on target line and preserve sticky x.
		sint32_c j = line->_character_start;
		sint32_c j_end = line->_character_end;
		if ( _plain_text.character_list[ line->_character_end ] == L'\n' )
		{
			j_end--; // exclude terminating new line from scan if one is present.
		}
		for ( ; j <= j_end; j++ )
		{
			text_glyph_c * glyph = &paragraph->_glyph_list[ j - paragraph->_character_start ];
			if ( _cursor_sticky_x < line->_left + glyph->_left + ( glyph->_horizontal_advance * 0.5f ) )
			{
				break;
			}
		}
		_cursor_index = j;
		_cursor_is_on_previous_line = line_is_virtual && j > j_end;

		if ( !shift )
		{
			_text_select_anchor_index_start = _cursor_index;
			_text_select_anchor_index_end = _cursor_index;
		}
	}

	void_c menu_element_text_c::_place_cursor_at_local_point( vector32x2_c const & local_point, boolean_c shift )
	{
		glyph_information_c glyph_information = _get_glyph_information( local_point );
		text_paragraph_c const * paragraph = _paragraph_list[ glyph_information.paragraph_index ];
		text_line_c const * line = &paragraph->_line_list[ glyph_information.paragraph_line_index ];
		text_glyph_c const * glyph = &paragraph->_glyph_list[ glyph_information.character_index - paragraph->_character_start ];

		if ( _text_select_mode == menu_text_select_mode_e_character )
		{
			_cursor_index = glyph_information.cursor_index;
			_cursor_is_on_previous_line = glyph_information.cursor_is_on_previous_line;
			if ( !shift )
			{
				_text_select_anchor_index_start = _cursor_index;
				_text_select_anchor_index_end = _cursor_index;
			}
			if ( _cursor_index == line->_character_start )
			{
				_cursor_sticky_x = line->_left;
			}
			else
			{
				text_glyph_c const * glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start - 1 ];
				_cursor_sticky_x = line->_left + glyph->_left + glyph->_horizontal_advance;
			}
		}
		else if ( _text_select_mode == menu_text_select_mode_e_word )
		{
			sint32_c word_start;
			sint32_c word_end;
			sint32_c character_index = glyph_information.cursor_index;
			if ( glyph_information.cursor_is_on_previous_line || _plain_text.character_list[ character_index ] == L'\n' )
			{
				// this bias will ensure that the last word on the current line is selected, rather than the first word on the next line.
				// and also ensure that the terminating new line remains unselectable.
				assert( character_index > 0 );
				character_index--;
			}
			_get_word_at_character_index( character_index, word_start, word_end );
			if ( !shift )
			{
				_text_select_anchor_index_start = word_start;
				_text_select_anchor_index_end = word_end;
				_cursor_index = word_end;
			}
			else
			{
				if ( word_start < _text_select_anchor_index_start )
				{
					_cursor_index = word_start;
				}
				else
				{
					_cursor_index = word_end;
				}
			}
			_cursor_is_on_previous_line = false;
			_update_cursor_sticky_x();
		}
		else if ( _text_select_mode == menu_text_select_mode_e_paragraph )
		{
			text_paragraph_c * paragraph = _get_paragraph_at_character_index( glyph_information.cursor_index, nullptr );
			assert( paragraph );
			if ( !shift )
			{
				_text_select_anchor_index_start = paragraph->_character_start;
				_text_select_anchor_index_end = paragraph->_character_end;
				if ( _text_select_anchor_index_end > _plain_text.character_list.get_length() - 2 ) // don't allow selection of terminating new line character.
				{
					_text_select_anchor_index_end = _plain_text.character_list.get_length() - 2;
				}
				_cursor_index = _text_select_anchor_index_end;
			}
			else
			{
				if ( paragraph->_character_start < _text_select_anchor_index_start )
				{
					_cursor_index = paragraph->_character_start;
				}
				else
				{
					_cursor_index = paragraph->_character_end;
					if ( _cursor_index > _plain_text.character_list.get_length() - 2 ) // don't allow selection of terminating new line character.
					{
						_cursor_index = _plain_text.character_list.get_length() - 2;
					}
				}
			}
			_cursor_is_on_previous_line = false;
			_update_cursor_sticky_x();
		}
	}

	void_c menu_element_text_c::_get_word_at_character_index( sint32_c character_index, sint32_c & character_start, sint32_c & character_end )
	{
		sint8_c word_type = 0; // letter.
		assert( character_index < _plain_text.character_list.get_length() - 2 );
		//if ( character_index > 0 && character_index == _plain_text.character_list.get_length() - 2 )
		//{
		//	character_index--; // bias, so that word can still be selected even if cursor is at very end of text.
		//}
		char16_c previous_character = character_index > 0 ? _plain_text.character_list[ character_index - 1 ] : 0;
		char16_c character = _plain_text.character_list[ character_index ];
		if ( ops::char16_is_space( character ) )
		{
			if ( ops::char16_is_space( previous_character ) )
			{
				// current character is space and previous character is space.
				// select the connected block of space.
				word_type = 1; // space.
			}
			else
			{
				// current character is space and previous character is not space.
				// bias where we begin the word search so that we select the word behind the cursor.
				character_index--;
				if ( ops::char16_is_punctuation( previous_character ) )
				{
					word_type = 2; // punctuation.
				}
			}
		}
		else
		{
			if ( ops::char16_is_punctuation( character ) )
			{
				word_type = 2; // punctuation.
			}
		}

		// scan behind.
		character_start = 0;
		sint32_c start = character_index;
		while ( start > 0 )
		{
			character = _plain_text.character_list[ start ];
			if ( word_type == 0 )
			{
				if ( ops::char16_is_space( character ) || ops::char16_is_punctuation( character ) )
				{
					character_start = start + 1;
					break;
				}
			}
			else if ( word_type == 1 )
			{
				if ( !ops::char16_is_space( character ) )
				{
					character_start = start + 1;
					break;
				}
			}
			else if ( word_type == 2 )
			{
				if ( !ops::char16_is_punctuation( character ) )
				{
					character_start = start + 1;
					break;
				}
			}
			start--;
		}
		// scan ahead.
		sint32_c count = _plain_text.character_list.get_length() - 1;
		sint32_c end = character_index;
		while ( end < count )
		{
			character = _plain_text.character_list[ end ];
			if ( word_type == 0 )
			{
				if ( ops::char16_is_space( character ) || ops::char16_is_punctuation( character ) )
				{
					break;
				}
			}
			else if ( word_type == 1 )
			{
				if ( !ops::char16_is_space( character ) )
				{
					break;
				}
			}
			else if ( word_type == 2 )
			{
				if ( !ops::char16_is_punctuation( character ) )
				{
					break;
				}
			}
			end++;
		}
		character_end = end;
		assert( character_start < _plain_text.get_length() );
		assert( character_end < _plain_text.get_length() );
		return;
	}

	menu_element_text_c::text_paragraph_c const * menu_element_text_c::_get_paragraph_at_character_index( sint32_c character_index, sint32_c * result_paragraph_index ) const
	{
		if ( result_paragraph_index )
		{
			*result_paragraph_index = -1;
		}
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
			if ( character_index >= paragraph->_character_start && character_index <= paragraph->_character_end )
			{
				if ( result_paragraph_index )
				{
					*result_paragraph_index = paragraph_index;
				}
				return paragraph;
			}
		}
		assert( false );
		return nullptr;
	}

	menu_element_text_c::text_paragraph_c * menu_element_text_c::_get_paragraph_at_character_index( sint32_c character_index, sint32_c * result_paragraph_index )
	{
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
			if ( character_index >= paragraph->_character_start && character_index <= paragraph->_character_end )
			{
				if ( result_paragraph_index )
				{
					*result_paragraph_index = paragraph_index;
				}
				return paragraph;
			}
		}
		assert( false );
		return nullptr;
	}

	menu_element_text_c::text_glyph_c * menu_element_text_c::_get_text_glyph_at_character_index( sint32_c character_index )
	{
		assert( character_index >= 0 && character_index < _plain_text.get_length() );
		text_paragraph_c * text_paragraph = _get_paragraph_at_character_index( character_index, nullptr );
		assert( text_paragraph );
		return &text_paragraph->_glyph_list[ character_index - text_paragraph->_character_start ];
	}

	menu_element_text_c::text_glyph_c const * menu_element_text_c::_get_text_glyph_at_character_index( sint32_c character_index ) const
	{
		assert( character_index >= 0 && character_index < _plain_text.get_length() );
		text_paragraph_c const * text_paragraph = _get_paragraph_at_character_index( character_index, nullptr );
		assert( text_paragraph );
		return &text_paragraph->_glyph_list[ character_index - text_paragraph->_character_start ];
	}

	void_c menu_element_text_c::_update_cursor_sticky_x()
	{
		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		text_paragraph_c * paragraph = _get_paragraph_at_character_index( _cursor_index, nullptr );
		sint32_c line_index = -1;
		paragraph->_get_line_at_character_index( _cursor_index, &line_index );
		if ( _cursor_is_on_previous_line )
		{
			assert( line_index > 0 );
			line_index--;
		}
		text_line_c * line = &paragraph->_line_list[ line_index ];
		if ( _cursor_index == line->_character_start )
		{
			_cursor_sticky_x = line->_left;
		}
		else
		{
			text_glyph_c const * glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start - 1 ];
			_cursor_sticky_x = line->_left + glyph->_left + glyph->_horizontal_advance;
		}
	}

	void_c menu_element_text_c::_input_plain_text( string16_c const & value )
	{
		// filter input if needed.
		string16_c filtered_value; // same as value but with all the illegal stuff removed.
		if ( _text_filter_mode == menu_text_filter_mode_e_number_integer || _text_filter_mode == menu_text_filter_mode_e_number_real )
		{
			assert( _text_format_mode == menu_text_format_mode_e_plain );
			assert( !_multi_line );

			boolean_c flip_negative_state = false;
			boolean_c already_has_decimal_point = false;
			if ( _text_filter_mode == menu_text_filter_mode_e_number_real )
			{
				already_has_decimal_point = _plain_text.character_list.find_index_of( '.' ) >= 0;
			}
			for ( sint32_c i = 0; i < value.get_length(); i++ )
			{
				char16_c c = value.character_list[ i ];
				if ( c == L'\n' ) // maybe text being pasted contains a new line, just break out so we ignore whatever comes after it.
				{
					break;
				}
				if ( _text_filter_mode == menu_text_filter_mode_e_number_integer )
				{
					assert( _text_value_length_limit > 1 );
					if ( value.get_length() + filtered_value.get_length() >= _text_value_length_limit - 1 ) // minus 1 to exclude optional leading - sign.
					{
						break;
					}
					if ( c == L'-' )
					{
						flip_negative_state = !flip_negative_state;
					}
					else if ( c < L'0' || c > L'9' )
					{
						continue;
					}
				}
				else if ( _text_filter_mode == menu_text_filter_mode_e_number_real )
				{
					assert( _text_value_length_limit > 1 );
					if ( value.get_length() + filtered_value.get_length() >= _text_value_length_limit - 1 ) // minus 1 to exclude optional leading - sign.
					{
						break;
					}
					if ( c == L'.' )
					{
						if ( already_has_decimal_point )
						{
							continue;
						}
						already_has_decimal_point = true;
					}
					else if ( c == L'-' )
					{
						flip_negative_state = !flip_negative_state;
					}
					else if ( c < L'0' || c > L'9' )
					{
						continue;
					}
				}
				else if ( _text_filter_mode == menu_text_filter_mode_e_string )
				{
					if ( _text_filter_string.character_list.find_index_of( c ) < 0 )
					{
						continue;
					}
				}
				filtered_value += c;
			}

			// insert or remove minus sign at start of string.
			if ( flip_negative_state )
			{
				if ( _plain_text.character_list[ 0 ] == '-' )
				{
					_delete_text( 0, 1 );
					_cursor_index--;
				}
				else
				{
					_insert_plain_text( L"-", 1, 0 );
				}
			}

			// insert filtered values.
			_insert_plain_text( filtered_value.character_list.get_internal_array(), filtered_value.get_length(), -1 );
		}
		else
		{
			// we still may need to filter values.
			for ( sint32_c i = 0; i < value.get_length(); i++ )
			{
				char16_c c = value.character_list[ i ];

				if ( _text_filter_mode == menu_text_filter_mode_e_string && _text_filter_string.character_list.find_index_of( c ) < 0 )
				{
					continue;
				}

				// may need to reinterpret current character.
				if ( c == L'\n' )
				{
					if ( !_multi_line )
					{
						break;
					}
					if ( _text_format_mode == menu_text_format_mode_e_rich )
					{
						// close current paragraph, insert new paragraph.
						// we can only get here if the user is pasting text from the clip board which contains a new line character.
						// other wise the input_return() code path is invoked when the user presses enter|return on the key board.
						_insert_new_paragraph();
						continue;
					}
				}

				// insert character as is.
				_insert_plain_text( &c, 1, -1 );
			}
		}

		_text_select_anchor_index_start = _cursor_index;
		_text_select_anchor_index_end = _cursor_index;

		_is_text_value_modified = true;
		on_text_value_changed_preview.invoke( this );		
	}

	void_c menu_element_text_c::_insert_plain_text( char16_c const * values, sint32_c values_count, sint32_c at_character_index )
	{
		assert( values && values_count > 0 );
		for ( sint32_c i = 0; i < values_count; i++ )
		{
			assert( values[ i ] != L'\0' );
		}
		if ( _text_value_length_limit >= 0 && ( _plain_text.character_list.get_length() - 2 ) + values_count > _text_value_length_limit )
		{
			values_count = _text_value_length_limit - ( _plain_text.character_list.get_length() - 2 );
			if ( values_count == 0 )
			{
				return;
			}
		}
		if ( at_character_index < 0 )
		{
			// insert index is negative, so insert at current cursor index.
			at_character_index = _cursor_index;
			_cursor_index += values_count;
			_text_select_anchor_index_start = _cursor_index;
			_text_select_anchor_index_end = _cursor_index;
		}
		else if ( at_character_index <= _cursor_index )
		{
			// insert index is at or before current cursor index, so advance cursor index.
			_cursor_index += values_count;
		}
		_plain_text.character_list.insert_at_index( at_character_index, values, values_count );
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			if ( at_character_index <= paragraph->_character_end )
			{
				paragraph->_handle_insert_character_range( at_character_index, values_count );
			}
		}

		_is_text_value_modified = true;
		_is_glyph_layout_dirty = true;
	}

	void_c menu_element_text_c::_delete_text( sint32_c character_start, sint32_c character_count )
	{
		assert( _text_interact_mode == menu_text_interact_mode_e_editable );
		assert( character_start >= 0 && character_count > 0 && character_start + character_count < _plain_text.character_list.get_length() );

		// remove characters from plain text.
		_plain_text.character_list.remove_at_index( character_start, character_count );

		// update paragraphs and spans to deal with the removed character range.
		sint32_c character_end_plus_one = character_start + character_count;
		sint32_c paragraph_to_merge_index = -1;
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			if ( character_start > paragraph->_character_end )
			{
				// nothing to do yet.
				continue;
			} 
			sint32_c sub_result = paragraph->_handle_delete_character_range( character_start, character_count );
			if ( sub_result == -1 )
			{
				_paragraph_list.remove_at_index( i );
				delete paragraph;
				i--;
				continue;
			}
			else if ( sub_result == 1 )
			{
				paragraph_to_merge_index = i;
			}
		}

		// merge paragraphs if one of them was clipped on the tail end.
		if ( paragraph_to_merge_index >= 0 )
		{
			assert( paragraph_to_merge_index + 1 < _paragraph_list.get_length() );
			text_paragraph_c * paragraph_to_merge = _paragraph_list[ paragraph_to_merge_index ];
			text_paragraph_c * paragraph_to_merge_next = _paragraph_list[ paragraph_to_merge_index + 1 ];

			// do the merge, transfer spans.
			paragraph_to_merge->_is_glyph_layout_dirty = true;
			paragraph_to_merge->_character_end = paragraph_to_merge_next->_character_end;
			if ( paragraph_to_merge_next->_span_list.get_length() > 0 )
			{
				// we also may need to merge the last span, if it is touching the first span of the next paragraph and is the same style as it.
				sint32_c span_to_merge_index = paragraph_to_merge->_span_list.get_length() - 1;
				for ( sint32_c i = 0; i < paragraph_to_merge_next->_span_list.get_length(); i++ )
				{
					text_span_c * span = paragraph_to_merge_next->_span_list[ i ];
					span->_mother_paragraph = paragraph_to_merge;
					paragraph_to_merge->_span_list.insert_at_end( span );
				}
				paragraph_to_merge_next->_span_list.remove_all();
				if ( span_to_merge_index >= 0 && span_to_merge_index + 1 < paragraph_to_merge->_span_list.get_length() )
				{
					text_span_c * span_to_merge = paragraph_to_merge->_span_list[ span_to_merge_index ];
					text_span_c * span_to_merge_next = paragraph_to_merge->_span_list[ span_to_merge_index + 1 ];
					if ( span_to_merge->_character_end + 1 == span_to_merge_next->_character_start && span_to_merge->_text_style_reference.get_key() == span_to_merge_next->_text_style_reference.get_key() )
					{
						span_to_merge->_character_end = span_to_merge_next->_character_end;
						paragraph_to_merge->_span_list.remove_at_index( span_to_merge_index + 1 );
						delete span_to_merge_next;
					}
				}
			}
			_paragraph_list.remove_at_index( paragraph_to_merge_index + 1 );
			delete paragraph_to_merge_next;
		}

		_is_text_value_modified = true;
		_is_glyph_layout_dirty = true;
	}

	void_c menu_element_text_c::_delete_selected_text()
	{
		sint32_c selected_character_start;
		sint32_c selected_character_count;
		if ( get_selected_text_range( selected_character_start, selected_character_count ) )
		{
			_delete_text( selected_character_start, selected_character_count );
			_cursor_index = selected_character_start;
			_text_select_anchor_index_start = selected_character_start;
			_text_select_anchor_index_end = selected_character_start;
		}
	}

	menu_element_text_c::glyph_information_c menu_element_text_c::_get_glyph_information( sint32_c character_index )
	{
		assert( _text_format_mode == menu_text_format_mode_e_plain );
		assert( character_index >= 0 && character_index < _plain_text.get_length() );

		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		glyph_information_c result;
		result.character_index = character_index;

		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			if ( character_index >= paragraph->_character_start && character_index <= paragraph->_character_end )
			{
				result.paragraph_index = i;
				for ( sint32_c j = 0; j < paragraph->_span_list.get_length(); j++ )
				{
					text_span_c * span = paragraph->_span_list[ j ];
					if ( character_index >= span->_character_start && character_index <= span->_character_end )
					{
						result.paragraph_span_index = j;
						break;
					}
				}
				assert( paragraph->_line_list.get_length() > 0 );
				for ( sint32_c j = 0; j < paragraph->_line_list.get_length(); j++ )
				{
					text_line_c * line = &paragraph->_line_list[ j ];
					if ( character_index >= line->_character_start && character_index <= line->_character_end )
					{
						result.paragraph_line_index = paragraph->_line_index_base + j;
						break;
					}
				}
				assert( result.paragraph_line_index != -1 );
				return result;
			}
		}
		assert( result.paragraph_index != -1 );
		return result;
	}

	menu_element_text_c::glyph_information_c menu_element_text_c::_get_glyph_information( vector32x2_c const & local_point )
	{
		glyph_information_c result;

		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		box32x2_c margin = _get_style_margin();

		float32_c vertically_algined_y = margin.minimum.b;
		menu_text_align_vertical_e text_align_vertical = _get_style_text_align_vertical();
		if ( text_align_vertical == menu_text_align_vertical_e_center )
		{
			float32_c effective_height = _local_box.get_height() - margin.minimum.b - margin.maximum.b;
			if ( effective_height < 0.0f )
			{
				effective_height = 0.0f;
			}
			vertically_algined_y = ( effective_height * 0.5f ) - ( get_content_height() * 0.5f );
		}
		else if ( text_align_vertical == menu_text_align_vertical_e_bottom )
		{
			vertically_algined_y = _local_box.get_height() - margin.maximum.b - get_content_height();
		}

		boolean_c picked = false;
		
		float32_c paragraph_left;
		//float32_c paragraph_right;
		float32_c paragraph_top;
		//float32_c paragraph_top_virtual;
		float32_c paragraph_bottom;

		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];

			paragraph_left = _local_box.minimum.a + margin.minimum.a + _content_offset.a;
			//paragraph_right = paragraph_left + _local_box.get_width() - ( _margin * 2 );
			paragraph_top = _local_box.minimum.b + margin.minimum.b + _content_offset.b + paragraph->_top + vertically_algined_y;
			//paragraph_top_virtual = i == 0 ? paragraph_top : paragraph_bottom; // virtual top of all paragraphs after the frist, so that if the pick point falls within the padding between paragraphs then we can still pick characters.
			paragraph_bottom = paragraph_top + paragraph->_content_height;

			vector32x2_c local_point_copy = local_point;

			// if point lies above first paragraph, then clamp point to top of first paragraph.
			if ( i == 0 && local_point_copy.b < paragraph_top )
			{
				local_point_copy.b = paragraph_top;
			}
			// if point lies below last paragraph, then clamp point to bottom of last paragraph.
			else if ( i == _paragraph_list.get_length() - 1 && local_point_copy.b > paragraph_bottom )
			{
				local_point_copy.b = paragraph_bottom;
			}

			if ( local_point_copy.b >= paragraph_top && local_point_copy.b <= paragraph_bottom )
			{
				result.paragraph_index = i;
				for ( sint32_c j = 0; j < paragraph->_line_list.get_length(); j++ )
				{
					text_line_c const * line = &paragraph->_line_list[ j ];

					float32_c line_left = paragraph_left + line->_left; // baseline offset from left.
					float32_c line_top = paragraph_top + line->_top;
					float32_c line_bottom = paragraph_top + line->_top + line->_ascender - line->_descender;

					// if point lies above first line, then clamp point to top of first line.
					if ( j == 0 && local_point_copy.b < line_top )
					{
						local_point_copy.b = line_top;
					}
					// if point lies below last line, then clamp point to bottom of last line.
					else if ( j == paragraph->_line_list.get_length() - 1 && local_point_copy.b > line_bottom )
					{
						local_point_copy.b = line_bottom;
					}

					if ( local_point_copy.b >= line_top && local_point_copy.b <= line_bottom )
					{
						// point lies vertically in this line.

						result.paragraph_line_index = j;

						float32_c line_width_with_trailing_white_space = paragraph->_glyph_list[ line->_character_end - paragraph->_character_start ]._left; // because line->_width does not include traling white space.
						if ( local_point_copy.a < line_left )
						{
							// point lies to the left of formatted line.
							result.character_index = line->_character_start;
							result.cursor_index = result.character_index;
							result.cursor_is_on_previous_line = false;
							result.cursor_x = line_left;
						}
						else if ( local_point_copy.a > line_left + line_width_with_trailing_white_space )
						{
							// point lies to the right of formatted line.
							result.character_index = line->_character_end;
							result.cursor_index = result.character_index + 1;
							// special case, don't allow the cursor to be placed after the last terminating new line character.
							if ( result.cursor_index > 0 && _plain_text.character_list[ result.cursor_index - 1 ] == L'\n' )
							{
								result.cursor_index--;
								result.character_index = result.cursor_index;
							}
							else
							{
								result.cursor_is_on_previous_line = true;
							}
							text_glyph_c * glyph = &paragraph->_glyph_list[ result.character_index - paragraph->_character_start ];
							result.cursor_x = line_left + glyph->_left + glyph->_horizontal_advance;
						}
						else
						{
							// point lies somewhere within formatted line.
							// figure out character index.
							text_glyph_c * glyph = nullptr;
							result.character_index = line->_character_start;
							for ( ; result.character_index <= line->_character_end; result.character_index++ )
							{
								glyph = &paragraph->_glyph_list[ result.character_index - paragraph->_character_start ];
								if ( local_point_copy.a < line_left + glyph->_left + glyph->_horizontal_advance )
								{
									break;
								}
							}
							// figure out cursor index.
							// this will either be character index or character index plus one.
							result.cursor_index = result.character_index;
							assert( result.cursor_index < _plain_text.get_length() );
							assert( glyph );
							if ( local_point.a >= line_left + glyph->_left + ( glyph->_horizontal_advance * 0.5f ) )
							{
								result.cursor_index++;
							}
							// special case, don't allow the cursor to be placed after any terminating new line character.
							if ( result.cursor_index > 0 && _plain_text.character_list[ result.cursor_index - 1 ] == L'\n' )
							{
								result.cursor_index--;
								result.character_index = result.cursor_index;
							}
							// determine if cursor is placed at end of virtual line.
							else if ( j < paragraph->_line_list.get_length() - 1 && result.character_index == line->_character_end && _plain_text.character_list[ result.character_index ] != L'\n' )
							{
								result.cursor_is_on_previous_line = true;
							}
							glyph = &paragraph->_glyph_list[ result.cursor_index - paragraph->_character_start ];
							result.cursor_x = line_left + glyph->_left;
						}
						return result;
					}
				}
			}
		}
		return result;
	}

	void_c menu_element_text_c::_get_thing_at_character_range( sint32_c character_start, sint32_c character_count, text_paragraph_c * & result_paragraph, text_span_c * & result_span )
	{
		result_paragraph = nullptr;
		result_span = nullptr;
		sint32_c character_end_plus_one = character_start + character_count;
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			if ( character_start > paragraph->_character_end )
			{
				// nothing to do yet.
				continue;
			}
			else if ( character_end_plus_one <= paragraph->_character_start )
			{
				// nothing left to do.
				break;
			}
			else if ( character_start >= paragraph->_character_start && character_end_plus_one - 1 <= paragraph->_character_end )
			{
				// try to find a span of interest.
				result_span = paragraph->_get_span_at_character_range( character_start, character_count );
				if ( result_span )
				{
					return;
				}
				// no span of interest was found.
				result_paragraph = paragraph;
				return;
			}
		}
	}

	void_c menu_element_text_c::_remove_spans_in_character_range( sint32_c character_start, sint32_c character_count )
	{
		assert( character_start >= 0 && character_start + character_count < _plain_text.get_length() );
		sint32_c character_end_plus_one = character_start + character_count;
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			if ( character_start > paragraph->_character_end )
			{
				// nothing to do yet.
				continue;
			}
			else if ( character_end_plus_one <= paragraph->_character_start )
			{
				// nothing left to do.
				break;
			}
			_remove_spans_in_character_range( paragraph->_span_list, character_start, character_count );
			paragraph->_is_glyph_layout_dirty = true;
		}
		_is_glyph_layout_dirty = true;
	}

	void_c menu_element_text_c::_create_spans_in_character_range( sint32_c character_start, sint32_c character_count, string8_c const & text_style_key )
	{
		assert( character_start >= 0 && character_start + character_count < _plain_text.get_length() );
		sint32_c character_end_plus_one = character_start + character_count;
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			if ( character_start > paragraph->_character_end )
			{
				// nothing to do yet.
				continue;
			}
			else if ( character_end_plus_one <= paragraph->_character_start )
			{
				// nothing left to do.
				break;
			}
			
			sint32_c capped_character_start = ops::math_maximum( paragraph->_character_start, character_start );
			sint32_c capped_character_end = ops::math_minimum( paragraph->_character_end - 1, character_end_plus_one - 1 ); // minus one from paragraph end to exclude terminating new line of paragraph, because spans aren't allowed to wrap around it.
			
			// figure out which index to insert the new span to.
			sint32_c new_span_insertion_index = 0;
			if ( paragraph->_span_list.get_length() > 0 && character_start > paragraph->_span_list[ paragraph->_span_list.get_length() - 1 ]->_character_end )
			{
				// insert after last span.
				new_span_insertion_index = paragraph->_span_list.get_length() - 1;
			}
			else
			{
				// insert somewhere before last span, maybe in the middle.
				for ( sint32_c i = 0; i < paragraph->_span_list.get_length(); i++ )
				{
					text_span_c * span = paragraph->_span_list[ i ];
					// ensure that the given character range does not intersect with any existing spans.
					// they should have been cleaned out already via _remove_spans_in_character_range.
					assert( !( capped_character_end >= span->_character_start && character_start <= span->_character_end ) );
					if ( span->_character_start > capped_character_end )
					{
						new_span_insertion_index = i;
						break;
					}
				}
			}

			// create and insert the new span.
			text_span_c * new_span = new text_span_c();
			new_span->_mother_paragraph = paragraph;
			new_span->_character_start = capped_character_start;
			new_span->_character_end = capped_character_end;
			new_span->_text_style_reference.set_key( text_style_key );
			paragraph->_span_list.insert_at_index( new_span_insertion_index, new_span );
			paragraph->_is_glyph_layout_dirty = true;
		}
		_is_glyph_layout_dirty = true;
	}

	void_c menu_element_text_c::_insert_new_paragraph()
	{
		assert( _text_interact_mode == menu_text_interact_mode_e_editable );
		assert( _text_format_mode == menu_text_format_mode_e_rich ); // only rich text is allowed to insert new paragraphs.
		assert( _cursor_index >= 0 && _cursor_index <= _plain_text.character_list.get_length() - 2 );
		sint32_c i = 0;
		for ( ; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			if ( _cursor_index >= paragraph->_character_start && _cursor_index <= paragraph->_character_end )
			{
				_plain_text.character_list.insert_at_index( _cursor_index, L'\n' );
				paragraph->_character_end = _cursor_index;
				paragraph->_is_glyph_layout_dirty = true;
				text_paragraph_c * new_paragraph = new text_paragraph_c();
				new_paragraph->_mother_element_text = this;
				new_paragraph->_character_start = _cursor_index + 1;
				new_paragraph->_character_end = paragraph->_character_end + 1;
				new_paragraph->_is_glyph_layout_dirty = true;
				_paragraph_list.insert_at_index( i + 1, new_paragraph );
				_cursor_index++;
				i += 2;
				break;
			}
		}
		for ( ; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			paragraph->_character_start++;
			paragraph->_character_end++;
		}
		_is_glyph_layout_dirty = true;
	}

	void_c menu_element_text_c::_clear_text_value()
	{
		if ( _plain_text == "\n" )
		{
			// we should be clear already.
			return;
		}

		// clear and set plain text.
		_clear_cached_data();
		_paragraph_list.remove_and_delete_all();
		_plain_text = "\n";
		_cursor_index = 0;
		_text_select_anchor_index_start = 0;
		_text_select_anchor_index_end = 0;

		// create and add paragraph to wrap plain text.
		text_paragraph_c * paragraph = new text_paragraph_c();
		paragraph->_mother_element_text = this;
		paragraph->_character_start = 0;
		paragraph->_character_end = 0;
		paragraph->_is_glyph_layout_dirty = true;
		_paragraph_list.insert_at_end( paragraph );
		_is_glyph_layout_dirty = true;
	}

	void_c menu_element_text_c::_set_plain_text_value( string16_c const & value )
	{
		_clear_text_value();
		_input_plain_text( value );
	}

	boolean_c menu_element_text_c::_set_rich_text_value( string8_c const & value )
	{
		_clear_text_value();

		// try to parse the markup.
		data_scribe_markup_c markup( '[' );
		if ( !markup.parse( value ) )
		{
			return false;
		}
			
		// process markup.
		data_scribe_markup_c::node_c const * node = markup.get_node( 1 );
		while ( node )
		{
			if ( node->get_tag_type() == data_scribe_markup_c::node_c::tag_type_e_open && node->get_value() == "paragraph" )
			{
				text_paragraph_c * paragraph = new text_paragraph_c();
				paragraph->_mother_element_text = this;
				paragraph->_character_start = _plain_text.get_length();
				data_scribe_markup_c::attribute_c const * attribute = node->find_attribute( "key" );
				if ( attribute )
				{
					paragraph->_text_style_reference.set_key( attribute->get_value() );
				}
				data_scribe_markup_c::node_c const * sub_node = node->get_first_daughter();
				while ( sub_node )
				{
					if ( sub_node->get_type() == data_scribe_markup_c::node_c::type_e_text )
					{
						_plain_text += sub_node->get_value();
					}
					else if ( sub_node->get_tag_type() == data_scribe_markup_c::node_c::tag_type_e_open && sub_node->get_value() == "span" )
					{
						sint32_c span_character_start = _plain_text.get_length();
						data_scribe_markup_c::node_c const * sub_sub_node = sub_node->get_first_daughter();
						while ( sub_sub_node )
						{
							if ( sub_sub_node->get_type() == data_scribe_markup_c::node_c::type_e_text )
							{
								_plain_text += sub_sub_node->get_value();
							}
							sub_sub_node = sub_sub_node->get_next_sister();
						}
						sint32_c span_character_end = _plain_text.character_list.get_length() - 2;
						if ( span_character_end >= span_character_start )
						{
							text_span_c * span = new text_span_c();
							span->_mother_paragraph = paragraph;
							span->_character_start = span_character_start;
							span->_character_end = span_character_end;
							paragraph->_span_list.insert_at_end( span );
						}
					}
					sub_node = sub_node->get_next_sister();
				}
				_plain_text += "\n";
				paragraph->_character_end = _plain_text.character_list.get_length() - 2;
				paragraph->_is_glyph_layout_dirty = true;
				_paragraph_list.insert_at_end( paragraph );
			}
			node = node->get_next_sister();
		}
		_is_glyph_layout_dirty = true;

		return true;
	}

	menu_text_align_vertical_e menu_element_text_c::_get_style_text_align_vertical() const
	{
		if ( _text_align_vertical != menu_text_align_vertical_e_inherit_from_style )
		{
			return _text_align_vertical;
		}
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->text_align_vertical_is_defined )
			{
				return _text_style_reference.get_value()->text_align_vertical;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->text_align_vertical_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->text_align_vertical;
	}

	menu_text_align_horizontal_e menu_element_text_c::_get_style_text_align_horizontal() const
	{
		if ( _text_align_horizontal != menu_text_align_horizontal_e_inherit_from_style )
		{
			return _text_align_horizontal;
		}
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->text_align_horizontal_is_defined )
			{
				return _text_style_reference.get_value()->text_align_horizontal;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->text_align_horizontal_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->text_align_horizontal;
	}

	float32_c menu_element_text_c::_get_style_paragraph_spacing() const
	{
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->paragraph_spacing_is_defined )
			{
				return _text_style_reference.get_value()->paragraph_spacing;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->paragraph_spacing_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->paragraph_spacing;
	}

	float32_c menu_element_text_c::_get_style_line_spacing() const
	{
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->line_spacing_is_defined )
			{
				return _text_style_reference.get_value()->line_spacing;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->line_spacing_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->line_spacing;
	}

	float32_c menu_element_text_c::_get_style_glyph_spacing() const
	{
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->glyph_spacing_is_defined )
			{
				return _text_style_reference.get_value()->glyph_spacing;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->glyph_spacing_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->glyph_spacing;
	}

	resource_file_font_c * menu_element_text_c::_get_style_font() const
	{
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->font_is_defined )
			{
				assert( _text_style_reference.get_value()->font.is_reference_set_and_loaded() );
				return _text_style_reference.get_value()->font;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->font_is_defined );
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->font.is_reference_set_and_loaded() );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->font;
	}

	vector32x4_c menu_element_text_c::_get_style_color() const
	{
		if ( _text_style_reference.get_value() )
		{
			/*if ( _text_style_reference.get_value()->color_style_is_defined && _text_style_reference.get_value()->color_style.get_value() )
			{
				return _text_style_reference.get_value()->color_style.get_value()->value;
			}
			else */if ( _text_style_reference.get_value()->color_is_defined )
			{
				return _text_style_reference.get_value()->color;
			}
		}
		/*if ( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->color_style_is_defined )
		{
			assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->color_style.get_value() );
			return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->color_style.get_value()->value;
		}*/
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->color_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->color;
	}

	float32_c menu_element_text_c::_get_style_size() const
	{
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->size_is_defined )
			{
				return _text_style_reference.get_value()->size;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->size_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->size;
	}

	float32_c menu_element_text_c::_get_style_skew() const
	{
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->skew_is_defined )
			{
				return _text_style_reference.get_value()->skew;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->skew_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->skew;
	}

	float32_c menu_element_text_c::_get_style_weight() const
	{
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->weight_is_defined )
			{
				return _text_style_reference.get_value()->weight;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->weight_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->weight;
	}

	float32_c menu_element_text_c::_get_style_softness() const
	{
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->softness_is_defined )
			{
				return _text_style_reference.get_value()->softness;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->softness_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->softness;
	}

	box32x2_c menu_element_text_c::_get_style_margin() const
	{
		if ( _text_style_reference.get_value() )
		{
			if ( _text_style_reference.get_value()->margin_is_defined )
			{
				return _text_style_reference.get_value()->margin;
			}
		}
		assert( engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->margin_is_defined );
		return engine_c::get_instance()->get_menu_style_manager()->get_default_text_style()->margin;
	}

	menu_element_text_c::menu_element_text_c()
		: menu_element_c()
		, _plain_text()
		, _paragraph_list()
		, _text_format_mode( menu_text_format_mode_e_plain )
		, _text_interact_mode( menu_text_interact_mode_e_static )
		, _text_filter_mode( menu_text_filter_mode_e_none )
		, _text_filter_string()
		, _text_select_mode( menu_text_select_mode_e_character )
		, _text_select_anchor_index_start( 0 )
		, _text_select_anchor_index_end( 0 )
		, _text_style_reference()
		, _text_align_horizontal( menu_text_align_horizontal_e_inherit_from_style )
		, _text_align_vertical( menu_text_align_vertical_e_inherit_from_style )
		, _text_value_length_limit( -1 )
		, _multi_line( true )
		, _word_wrap( true )
		, _content_offset()
		, _cursor_index( 0 )
		, _cursor_is_on_previous_line( false )
		, _cursor_sticky_x( 0.0f )
		, _cursor_time( 0.0f )
		, _is_text_focused( false )
		, _is_text_value_modified( false )
		, _is_glyph_layout_dirty( false )
		, _laid_out_width( 0.0f )
		, _content_width( 0.0f )
		, _content_height( 0.0f )
		, _global_list_node( this )
		, _text_glyph_style_cache()
	{
		_global_list.insert_at_end( &_global_list_node );
		_text_style_reference.on_refreshed.subscribe( this, &menu_element_text_c::_handle_text_style_reference_on_refreshed );
		_clear_text_value();
	}

	menu_element_text_c::~menu_element_text_c()
	{
		_global_list.remove( &_global_list_node );
		_paragraph_list.remove_and_delete_all();
	}

	menu_text_style_c::reference_c const & menu_element_text_c::get_style_reference() const
	{
		return _text_style_reference;
	}

	menu_text_style_c::reference_c & menu_element_text_c::get_style_reference()
	{
		return _text_style_reference;
	}

	menu_text_format_mode_e menu_element_text_c::get_text_format_mode() const
	{
		return _text_format_mode;
	}

	void_c menu_element_text_c::set_text_format_mode( menu_text_format_mode_e value )
	{
		_clear_text_value();
		if ( value == menu_text_format_mode_e_rich )
		{
			assert( _text_interact_mode == menu_text_interact_mode_e_static );
		}
		_text_format_mode = value;
	}

	menu_text_interact_mode_e menu_element_text_c::get_text_interact_mode() const
	{
		return _text_interact_mode;
	}

	void_c menu_element_text_c::set_text_interact_mode( menu_text_interact_mode_e value )
	{
		if ( _text_format_mode == menu_text_format_mode_e_rich )
		{
			assert( value == menu_text_interact_mode_e_static );
		}
		_text_interact_mode = value;
	}

	menu_text_filter_mode_e menu_element_text_c::get_text_filter_mode() const
	{
		return _text_filter_mode;
	}

	void_c menu_element_text_c::set_text_filter_mode( menu_text_filter_mode_e value )
	{
		_text_filter_mode = value;
	}

	void_c menu_element_text_c::set_text_filter_string( string16_c const & value )
	{
		_text_filter_string = value;
	}

	sint32_c menu_element_text_c::get_text_value_length_limit() const
	{
		return _text_value_length_limit;
	}

	void_c menu_element_text_c::set_text_value_length_limit( sint32_c value )
	{
		_text_value_length_limit = value < -1 ? -1 : value;
		if ( _text_value_length_limit >= 0 && _plain_text.character_list.get_length() - 2 > _text_value_length_limit )
		{
			sint32_c count = _plain_text.character_list.get_length() - 2 - _text_value_length_limit;
			sint32_c start = _plain_text.character_list.get_length() - 2 - count;
			_delete_text( start, count );
			_plain_text.character_list.set_length( _text_value_length_limit );
			_plain_text.character_list.insert_at_end( L'\n' );
			_plain_text.character_list.insert_at_end( L'\0' );
			_invalidate_glyph_layout_in_all_paragraphs();
		}
	}

	menu_text_align_horizontal_e menu_element_text_c::get_text_align_horizontal() const
	{
		return _text_align_horizontal;
	}

	void_c menu_element_text_c::set_text_align_horizontal( menu_text_align_horizontal_e value )
	{
		if ( _text_align_horizontal != value )
		{
			_text_align_horizontal = value;
			if ( _is_glyph_layout_dirty )
			{
				_do_glyph_layout();
			}
			else
			{
				_update_horizontal_layout_of_all_lines();
			}
		}
	}

	menu_text_align_vertical_e menu_element_text_c::get_text_align_vertical() const
	{
		return _text_align_vertical;
	}

	void_c menu_element_text_c::set_text_align_vertical( menu_text_align_vertical_e value )
	{
		if ( _text_align_vertical != value )
		{
			_text_align_vertical = value;
			if ( _is_glyph_layout_dirty )
			{
				_do_glyph_layout();
			}
			else
			{
				_update_vertical_layout_of_all_paragraphs();
			}
		}
	}

	boolean_c menu_element_text_c::get_word_wrap() const
	{
		return _word_wrap;
	}

	void_c menu_element_text_c::set_word_wrap( boolean_c value )
	{
		if ( _word_wrap != value )
		{
			_word_wrap = value;
			_invalidate_glyph_layout_in_all_paragraphs();
		}
	}

	boolean_c menu_element_text_c::get_multi_line() const
	{
		return _multi_line;
	}

	void_c menu_element_text_c::set_multi_line( boolean_c value )
	{
		_clear_text_value();
		_multi_line = value;
	}

	vector32x2_c const & menu_element_text_c::get_content_offset() const
	{
		return _content_offset;
	}

	void_c menu_element_text_c::set_content_offset( vector32x2_c const & value )
	{
		_content_offset = value;
	}

	float32_c menu_element_text_c::get_content_width() const
	{
		if ( _is_glyph_layout_dirty )
		{
			const_cast< menu_element_text_c * >( this )->_do_glyph_layout();
		}
		return _content_width;
	}

	float32_c menu_element_text_c::get_content_height() const
	{
		if ( _is_glyph_layout_dirty )
		{
			const_cast< menu_element_text_c * >( this )->_do_glyph_layout();
		}
		return _content_height;
	}

	sint32_c menu_element_text_c::get_cursor_index() const
	{
		return _cursor_index;
	}

	void_c menu_element_text_c::set_cursor_index( sint32_c value )
	{
		if ( value < 0 )
		{
			value = 0;
		}
		else if ( value > _plain_text.character_list.get_length() )
		{
			value = _plain_text.character_list.get_length();
		}
		_cursor_index = value;
		_cursor_is_on_previous_line = false;
		_text_select_anchor_index_start = value;
		_text_select_anchor_index_end = value;
	}

	void_c menu_element_text_c::get_cursor_information( sint32_c & result_paragraph_index, sint32_c & result_line_index ) const
	{
		if ( _is_glyph_layout_dirty )
		{
			const_cast< menu_element_text_c * >( this )->_do_glyph_layout();
		}

		text_paragraph_c const * paragraph = _get_paragraph_at_character_index( _cursor_index, &result_paragraph_index );
		paragraph->_get_line_at_character_index( _cursor_index, &result_line_index );
		if ( _cursor_is_on_previous_line )
		{
			assert( result_line_index > 0 );
			result_line_index--;
		}
		result_line_index += paragraph->_line_index_base;
	}

	boolean_c menu_element_text_c::has_selected_text_range() const
	{
		return ( _text_select_anchor_index_start < _text_select_anchor_index_end ) || ( _cursor_index != _text_select_anchor_index_start ) || ( _cursor_index != _text_select_anchor_index_end );
	}

	boolean_c menu_element_text_c::get_selected_text_range( sint32_c & character_start, sint32_c & character_count ) const
	{
		character_start = ( _cursor_index < _text_select_anchor_index_start ? _cursor_index : _text_select_anchor_index_start );
		character_count = ( _cursor_index > _text_select_anchor_index_end ? _cursor_index : _text_select_anchor_index_end ) - character_start;
		return character_count > 0;
	}

	string16_c menu_element_text_c::get_plain_text_value() const
	{
		assert( _plain_text.character_list.get_length() >= 2 ); // plain text must at minimum be 2 long because it must end with a new line character and a null character.
		assert( _plain_text.character_list[ _plain_text.character_list.get_length() - 2 ] == L'\n' );
		assert( _plain_text.character_list[ _plain_text.character_list.get_length() - 1 ] == L'\0' );
		string16_c result;
		result = _plain_text;
		result.character_list.remove_at_index( result.character_list.get_length() - 2 ); // remove the terminating new line character.
		return result;
	}

	void_c menu_element_text_c::set_plain_text_value( string8_c const & value )
	{
		_set_plain_text_value( string16_c( value ) );
	}

	void_c menu_element_text_c::set_plain_text_value( string16_c const & value )
	{
		_set_plain_text_value( value );
	}

	void_c menu_element_text_c::set_rich_text_value( string8_c const & value )
	{
		assert( _text_format_mode == menu_text_format_mode_e_rich );
		_set_rich_text_value( value );
	}

	void_c menu_element_text_c::set_rich_text_value( string16_c const & value )
	{
		assert( _text_format_mode == menu_text_format_mode_e_rich );
		_set_rich_text_value( string8_c( value ) );
	}

	void_c menu_element_text_c::clear_text_value()
	{
		_clear_text_value();
	}

	void_c menu_element_text_c::append_line( string16_c const & plain_text )
	{
		_plain_text.character_list.remove_at_end(); // remove terminating null character.
		_plain_text.character_list.remove_at_end(); // remove terminating new line character.
		_plain_text.character_list.insert_at_end( plain_text.character_list.get_internal_array(), plain_text.character_list.get_length() - 1 ); // append new text excluding the terminating null character.
		_plain_text.character_list.insert_at_end( L'\n' ); // append terminating new line character.
		_plain_text.character_list.insert_at_end( L'\0' ); // append terminating null character.

		assert( _paragraph_list.get_length() > 0 );
		text_paragraph_c * paragraph = _paragraph_list[ _paragraph_list.get_length() - 1 ];
		paragraph->_is_glyph_layout_dirty = true;
		paragraph->_character_end += plain_text.get_length();

		_paragraph_list[ _paragraph_list.get_length() - 1 ]->_do_glyph_layout(); // instead of deferring layout, we'll just do it here.
		_update_vertical_layout_of_all_paragraphs();
	}

	void_c menu_element_text_c::append_paragraph( string16_c const & plain_text )
	{
		_plain_text.character_list.remove_at_end(); // remove terminating new line character.
		sint32_c new_paragraph_character_start = _plain_text.character_list.get_length();
		_plain_text.character_list.insert_at_end( plain_text.character_list.get_internal_array(), plain_text.character_list.get_length() - 1 ); // append new text excluding the terminating null character.
		_plain_text.character_list.insert_at_end( L'\n' ); // append terminating new line character.
		_plain_text.character_list.insert_at_end( L'\0' ); // append terminating null character.
		sint32_c new_paragraph_character_end = _plain_text.get_length();

		text_paragraph_c * new_paragraph = new text_paragraph_c();
		new_paragraph->_mother_element_text = this;
		new_paragraph->_character_start = new_paragraph_character_start;
		new_paragraph->_character_end = new_paragraph_character_end;
		new_paragraph->_is_glyph_layout_dirty = true;
		_paragraph_list.insert_at_end( new_paragraph );

		_is_glyph_layout_dirty = true;

		//_reflow_glyphs_in_paragraph( _paragraph_list.get_length() - 1 ); // instead of deferring layout, we'll just do it here.
		//_update_vertical_layout_of_all_paragraphs();
		//_is_glyph_layout_dirty = false;
	}

	void_c menu_element_text_c::input_character( char16_c character )
	{
		assert( _text_interact_mode == menu_text_interact_mode_e_editable );
		assert( ops::char16_is_printable( character ) );

		_cursor_time = 0.0f;

		if ( has_selected_text_range() )
		{
			_delete_selected_text();
			on_text_value_changed_preview.invoke( this );
		}

		string16_c value_string;
		value_string += character;
		_input_plain_text( value_string );
	}

	void_c menu_element_text_c::input_return( boolean_c shift )
	{
		_cursor_time = 0.0f;
		if ( _multi_line )
		{
			_delete_selected_text();
			if ( _text_format_mode == menu_text_format_mode_e_rich && !shift )
			{
				_insert_new_paragraph();
			}
			else
			{
				_insert_plain_text( L"\n", 1, -1 );
			}
			on_text_value_changed_preview.invoke( this );
		}
		else
		{
			if ( _is_text_value_modified )
			{
				_is_text_value_modified = false;
				on_text_value_changed.invoke( this );
			}
		}
	}

	void_c menu_element_text_c::input_delete_fore()
	{
		_cursor_time = 0.0f;
		if ( has_selected_text_range() )
		{
			_delete_selected_text();
			on_text_value_changed_preview.invoke( this );
		}
		else if ( _cursor_index < _plain_text.character_list.get_length() - 2 )
		{
			_text_select_anchor_index_start = _cursor_index;
			_text_select_anchor_index_end = _cursor_index;
			_delete_text( _cursor_index, 1 );
			on_text_value_changed_preview.invoke( this );
		}
	}

	void_c menu_element_text_c::input_delete_back()
	{
		_cursor_time = 0.0f;
		if ( has_selected_text_range() )
		{
			_delete_selected_text();
			on_text_value_changed_preview.invoke( this );
		}
		else if ( _cursor_index > 0 )
		{
			_cursor_index--;
			_text_select_anchor_index_start = _cursor_index;
			_text_select_anchor_index_end = _cursor_index;
			_delete_text( _cursor_index, 1 );
			on_text_value_changed_preview.invoke( this );
		}
	}

	void_c menu_element_text_c::input_left( boolean_c shift, boolean_c ctrl )
	{
		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		_cursor_time = 0.0f;

		if ( _cursor_index > 0 )
		{
			if ( ctrl )
			{
				if ( _plain_text.character_list[ _cursor_index - 1 ] == L'\n' )
				{
					_cursor_index--;
				}

				// skip space.
				while ( _cursor_index > 0 && ops::char16_is_space( _plain_text.character_list[ _cursor_index - 1 ] ) )
				{
					_cursor_index--;
				}

				// skip block.
				if ( _cursor_index > 0 )
				{
					if ( ops::char16_is_letter( _plain_text.character_list[ _cursor_index - 1 ] ) || _plain_text.character_list[ _cursor_index - 1 ] == '_' )
					{
						while ( _cursor_index > 0 && ( ops::char16_is_letter( _plain_text.character_list[ _cursor_index - 1 ] ) || _plain_text.character_list[ _cursor_index - 1 ] == '_' ) )
						{
							_cursor_index--;
						}
					}
					else if ( ops::char16_is_punctuation( _plain_text.character_list[ _cursor_index - 1 ] ) )
					{
						while ( _cursor_index > 0 && ops::char16_is_punctuation( _plain_text.character_list[ _cursor_index - 1 ] ) )
						{
							_cursor_index--;
						}
					}
				}
			}
			else
			{
				sint32_c selected_character_start;
				sint32_c selected_character_count;
				get_selected_text_range( selected_character_start, selected_character_count );
				if ( !shift && selected_character_count > 0 )
				{
					_cursor_index = selected_character_start; // snap cursor to start of selection.
				}
				else
				{
					_cursor_index--;
				}
			}
		}

		if ( !shift )
		{
			_text_select_anchor_index_start = _cursor_index;
			_text_select_anchor_index_end = _cursor_index;
		}

		_cursor_is_on_previous_line = false;
		_update_cursor_sticky_x();
	}

	void_c menu_element_text_c::input_right( boolean_c shift, boolean_c ctrl )
	{
		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		_cursor_time = 0.0f;

		sint32_c end = _plain_text.character_list.get_length() - 2; // minus 2 to exclude "\n\0" characters at end.
		if ( _cursor_index < end )
		{
			if ( ctrl )
			{
				if ( _cursor_index == end )
				{
					return;
				}

				// skip new-line at end of line if needed.
				if ( _plain_text.character_list[ _cursor_index ] == L'\n' )
				{
					_cursor_index++;
				}

				// skip block.
				if ( ops::char16_is_letter( _plain_text.character_list[ _cursor_index ] ) || ops::char16_is_number( _plain_text.character_list[ _cursor_index ] ) || _plain_text.character_list[ _cursor_index ] == '_' )
				{
					while ( ( _cursor_index < end ) && ( ops::char16_is_letter( _plain_text.character_list[ _cursor_index ] ) || ops::char16_is_number( _plain_text.character_list[ _cursor_index ] ) || _plain_text.character_list[ _cursor_index ] == '_' ) )
					{
						_cursor_index++;
					}
				}
				else if ( ops::char16_is_punctuation( _plain_text.character_list[ _cursor_index ] ) )
				{
					while ( ( _cursor_index < end ) && ( ops::char16_is_punctuation( _plain_text.character_list[ _cursor_index ] ) ) )
					{
						_cursor_index++;
					}
				}

				// skip space.
				while ( ( _cursor_index < end ) && ( ops::char16_is_space( _plain_text.character_list[ _cursor_index ] ) ) && ( _plain_text.character_list[ _cursor_index ] != '\n' ) )
				{
					_cursor_index++;
				}
			}
			else
			{
				sint32_c selected_character_start;
				sint32_c selected_character_count;
				get_selected_text_range( selected_character_start, selected_character_count );
				if ( !shift && selected_character_count > 0 )
				{
					_cursor_index = selected_character_start + selected_character_count; // snap cursor to end of selection.
				}
				else
				{
					_cursor_index++;
				}
			}
		}

		if ( !shift )
		{
			_text_select_anchor_index_start = _cursor_index;
			_text_select_anchor_index_end = _cursor_index;
		}

		_cursor_is_on_previous_line = false;
		_update_cursor_sticky_x();
	}

	void_c menu_element_text_c::input_up( boolean_c shift )
	{
		assert( _text_interact_mode == menu_text_interact_mode_e_static_selectable || _text_interact_mode == menu_text_interact_mode_e_editable );
		
		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		_cursor_time = 0.0f;

		// get the current cursor line.
		sint32_c line_index = -1;
		sint32_c paragraph_index = -1;
		get_cursor_information( paragraph_index, line_index );

		// if there is a range of text that is selected, then favor the higher line.
		sint32_c selected_text_character_start = 0;
		sint32_c selected_text_character_count = 0;
		if ( !shift && get_selected_text_range( selected_text_character_start, selected_text_character_count ) )
		{
			text_paragraph_c const * paragraph = _get_paragraph_at_character_index( selected_text_character_start, nullptr );
			sint32_c selected_text_line_index = -1;
			paragraph->_get_line_at_character_index( selected_text_character_start, &selected_text_line_index );
			if ( selected_text_line_index < line_index )
			{
				line_index = selected_text_line_index;
			}
		}

		_place_cursor_at_line_index( line_index - 1, shift );
	}

	void_c menu_element_text_c::input_down( boolean_c shift )
	{
		assert( _text_interact_mode == menu_text_interact_mode_e_static_selectable || _text_interact_mode == menu_text_interact_mode_e_editable );
		
		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		_cursor_time = 0.0f;

		// get the current cursor line.
		sint32_c line_index = -1;
		sint32_c paragraph_index = -1;
		get_cursor_information( paragraph_index, line_index );

		// if there is a range of characters selected, then favor the lower line.
		sint32_c selected_text_character_start = 0;
		sint32_c selected_text_character_count = 0;
		if ( !shift && get_selected_text_range( selected_text_character_start, selected_text_character_count ) )
		{
			sint32_c selected_text_character_end = selected_text_character_start + selected_text_character_count;
			text_paragraph_c const * paragraph = _get_paragraph_at_character_index( selected_text_character_end, nullptr );
			sint32_c selected_text_line_index = -1;
			paragraph->_get_line_at_character_index( selected_text_character_end, &selected_text_line_index );
			if ( selected_text_line_index > line_index )
			{
				line_index = selected_text_line_index;
			}
		}

		_place_cursor_at_line_index( line_index + 1, shift );
	}

	void_c menu_element_text_c::input_home( boolean_c shift )
	{
		assert( _text_interact_mode == menu_text_interact_mode_e_static_selectable || _text_interact_mode == menu_text_interact_mode_e_editable );

		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		_cursor_time = 0.0f;

		// find paragraph and line that cursor is currently on.
		text_paragraph_c const * paragraph = _get_paragraph_at_character_index( _cursor_index, nullptr );
		sint32_c line_index = -1;
		text_line_c const * line = paragraph->_get_line_at_character_index( _cursor_index, &line_index );
		if ( _cursor_is_on_previous_line )
		{
			assert( line_index > 0 );
			line_index--;
			line = &paragraph->_line_list[ line_index ];
		}

		// count how many white space characters are at start of line.
		sint32_c space_count = 0;
		for ( sint32_c i = line->_character_start; i < line->_character_end; i++ )
		{
			char16_c character = _plain_text.character_list[ i ];
			if ( character == L' ' || character == L'\t' || character == L'\n' )
			{
				space_count++;
			}
			else
			{
				break;
			}
		}

		// toggle location of cursor to start of text content or start of line.
		_cursor_is_on_previous_line = false;
		if ( space_count == 0 || _cursor_index - line->_character_start == space_count || line->_character_end - line->_character_start == space_count )
		{
			// move cursor to start of line.
			_cursor_index = line->_character_start;
			_cursor_sticky_x = line->_left;
		}
		else if ( line->_character_end - line->_character_start > space_count )
		{
			// move cursor past starting whitespace to start of actual text content.
			_cursor_index = line->_character_start + space_count;
			text_glyph_c const * glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start - 1 ];
			_cursor_sticky_x = line->_left + glyph->_left + glyph->_horizontal_advance;
		}

		if ( !shift )
		{
			_text_select_anchor_index_start = _cursor_index;
			_text_select_anchor_index_end = _cursor_index;
		}
	}

	void_c menu_element_text_c::input_end( boolean_c shift )
	{
		assert( _text_interact_mode == menu_text_interact_mode_e_static_selectable || _text_interact_mode == menu_text_interact_mode_e_editable );
		
		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		_cursor_time = 0.0f;

		if ( _cursor_is_on_previous_line )
		{
			return;
		}

		// find paragraph and line that cursor is currently on.
		sint32_c paragraph_index = -1;
		text_paragraph_c const * paragraph = _get_paragraph_at_character_index( _cursor_index, &paragraph_index );
		sint32_c line_index = -1;
		text_line_c const * line = paragraph->_get_line_at_character_index( _cursor_index, &line_index );
		boolean_c line_is_virtual = _plain_text.character_list[ line->_character_end ] != L'\n' && line_index - paragraph->_line_index_base < paragraph->_line_list.get_length() - 1;

		// move cursor to end of line.
		_cursor_index = line->_character_end;
		_cursor_is_on_previous_line = false;
		if ( line_is_virtual )
		{
			_cursor_index++;
			_cursor_is_on_previous_line = true;
		}
		if ( _cursor_index == line->_character_start )
		{
			_cursor_sticky_x = line->_left;
		}
		else
		{
			text_glyph_c const * glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start - 1 ];
			_cursor_sticky_x = line->_left + glyph->_left + glyph->_horizontal_advance;
		}

		if ( !shift )
		{
			_text_select_anchor_index_start = _cursor_index;
			_text_select_anchor_index_end = _cursor_index;
		}
	}

	boolean_c menu_element_text_c::get_text_style_key_at_cursor( string8_c & result_text_style_key )
	{
		sint32_c selected_character_start = 0;
		sint32_c selected_character_count = 0;
		get_selected_text_range( selected_character_start, selected_character_count );
		text_paragraph_c * selected_paragraph = nullptr;
		text_span_c * selected_span = nullptr;
		_get_thing_at_character_range( selected_character_start, selected_character_count <= 0 ? 1 : selected_character_count, selected_paragraph, selected_span );
		if ( selected_paragraph )
		{
			result_text_style_key = selected_paragraph->_text_style_reference.get_key();
			return true;
		}
		else if ( selected_span )
		{
			result_text_style_key = selected_span->_text_style_reference.get_key();
			return true;
		}
		return false;
	}

	boolean_c menu_element_text_c::set_text_style_key_at_cursor( string8_c const & text_style_key )
	{
		sint32_c selected_character_start = 0;
		sint32_c selected_character_count = 0;
		get_selected_text_range( selected_character_start, selected_character_count );
		if ( selected_character_count == 0 )
		{
			// no selection, this means that we want to replace the text style key of the single paragraph or span that already exists at the cursor index.
			assert( selected_character_start == _cursor_index );
			text_paragraph_c * selected_paragraph = nullptr;
			text_span_c * selected_span = nullptr;
			_get_thing_at_character_range( selected_character_start, 1, selected_paragraph, selected_span );
			if ( selected_paragraph )
			{
				selected_paragraph->_text_style_reference.set_key( text_style_key );
				return true;
			}
			else if ( selected_span )
			{
				selected_span->_text_style_reference.set_key( text_style_key );
				return true;
			}
			assert( false ); // shouldn't be possible for the cursor to not intersect with something.
		}
		else
		{
			// a range of characters are selected, this means that we want to clip out any spans in that range.
			_remove_spans_in_character_range( selected_character_start, selected_character_count );
			if ( text_style_key.get_length() > 0 )
			{
				// if the text style key is set then we also want to create new spans with the given text style.
				_create_spans_in_character_range( selected_character_start, selected_character_count, text_style_key );
			}
		}
		return true;
	}

	boolean_c menu_element_text_c::copy_text_to_clip_board()
	{
		assert( _text_interact_mode == menu_text_interact_mode_e_static_selectable || _text_interact_mode == menu_text_interact_mode_e_editable );
		sint32_c selected_character_start;
		sint32_c selected_character_count;
		if ( get_selected_text_range( selected_character_start, selected_character_count ) )
		{
			string16_c clip = ops::string16_sub_string( _plain_text, selected_character_start, selected_character_count );
			return engine_c::get_instance()->get_input_manager()->clip_board_set_plain_text( clip );
		}
		return false;
	}

	boolean_c menu_element_text_c::paste_text_from_clip_board()
	{
		assert( _text_interact_mode == menu_text_interact_mode_e_editable );
		string16_c clip;
		if ( engine_c::get_instance()->get_input_manager()->clip_board_get_plain_text( clip ) )
		{
			if ( has_selected_text_range() )
			{
				_delete_selected_text();
			}
			_input_plain_text( clip );
			_cursor_index += clip.get_length();
			return true;
		}
		return false;
	}

	void_c menu_element_text_c::_on_local_box_modified()
	{
		vector32x2_c size = _local_box.get_size();
		if ( _laid_out_width != size.a )
		{
			_invalidate_glyph_layout_in_all_paragraphs();
			_laid_out_width = size.a;
		}
		else
		{
			_update_vertical_layout_of_all_paragraphs();
		}
	}

	void_c menu_element_text_c::update_animations( float32_c time_step )
	{
		if ( _is_glyph_layout_dirty )
		{
			_do_glyph_layout();
		}

		_cursor_time += static_cast< float32_c >( time_step );

		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			for( sint32_c j = 0; j < paragraph->_entity_list.get_length(); j++ )
			{
				menu_text_entity_c * entity = paragraph->_entity_list[ j ];
				if ( entity->type_code == menu_text_entity_sprite_c::type_code_static() )
				{
					menu_text_entity_sprite_c * entity_sprite = static_cast< menu_text_entity_sprite_c * >( entity );
					entity_sprite->value.update( time_step );
				}
			}
		}	
	}

	void_c menu_element_text_c::set_style_key( string8_c const & text_style_key )
	{
		_text_style_reference.set_key( text_style_key );
	}

	void_c menu_element_text_c::handle_on_character_focus_gained()
	{
		_is_text_value_modified = false;
		_is_text_focused = true;
	}

	void_c menu_element_text_c::handle_on_character_focus_lost()
	{
		if ( _is_text_value_modified )
		{
			_is_text_value_modified = false;
			on_text_value_changed.invoke( this );
		}
		_is_text_focused = false;
	}

	boolean_c menu_element_text_c::handle_on_input( input_event_c * input_event )
	{
		if ( _text_interact_mode == menu_text_interact_mode_e_static )
		{
			return false;
		}

		if ( input_event->type == input_event_c::type_e_character )
		{
			if ( _text_interact_mode == menu_text_interact_mode_e_editable )
			{
				input_character( input_event->character );
				return true;
			}
		}
		else if ( input_event->type == input_event_c::type_e_keyboard_key_pressed )
		{
			if ( input_event->keyboard_key == input_keyboard_key_e_left )
			{
				input_left( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0, ( input_event->modifier_keys_state[ input_modifier_key_e_ctrl ] & input_key_state_bit_e_on ) != 0 );
				return true;
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_right )
			{
				input_right( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0, ( input_event->modifier_keys_state[ input_modifier_key_e_ctrl ] & input_key_state_bit_e_on ) != 0 );
				return true;
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_up )
			{
				input_up( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0 );
				return true;
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_down )
			{
				input_down( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0 );
				return true;
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_home )
			{
				input_home( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0 );
				return true;
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_end )
			{
				input_end( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0 );
				return true;
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_back_space )
			{
				if ( _text_interact_mode == menu_text_interact_mode_e_editable )
				{
					input_delete_back();
					return true;
				}
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_delete )
			{
				if ( _text_interact_mode == menu_text_interact_mode_e_editable )
				{
					input_delete_fore();
					return true;
				}
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_enter )
			{
				if ( _text_interact_mode == menu_text_interact_mode_e_editable && _multi_line )
				{
					input_return( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0 );
					return true;
				}
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_key_pressed )
		{
			assert( _mother_control );
			vector32x2_c local_mouse_position = _mother_control->transform_global_point_to_local_point( input_event->menu_global_mouse_position ); 
			if ( input_event->mouse_key == input_mouse_key_e_left && ( _text_interact_mode == menu_text_interact_mode_e_static_selectable || _text_interact_mode == menu_text_interact_mode_e_editable ) )
			{
				if ( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0 )
				{
					_text_select_mode = menu_text_select_mode_e_character;
					_place_cursor_at_local_point( local_mouse_position, true );
				}
				else
				{
					if ( input_event->mouse_key_multi_click_count == 1 )
					{
						_text_select_mode = menu_text_select_mode_e_character;
					}
					else if ( input_event->mouse_key_multi_click_count == 2 )
					{
						_text_select_mode = menu_text_select_mode_e_word;
					}
					else if ( input_event->mouse_key_multi_click_count == 3 )
					{
						_text_select_mode = menu_text_select_mode_e_paragraph;
					}
					_place_cursor_at_local_point( local_mouse_position, false );
					_cursor_time = 0.0f;
				}
				return true;
			}
			else if ( input_event->mouse_key == input_mouse_key_e_right )
			{
				// if the mouse pointer is not intersecting with the currently selected range of text, then place the cursor where the mouse pointer is.

				_place_cursor_at_local_point( local_mouse_position, false );
				return true;
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_key_released )
		{
			if ( input_event->mouse_key == input_mouse_key_e_right )
			{
				// open context menu.
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_move )
		{
			if ( ( input_event->mouse_keys_state[ input_mouse_key_e_left ] & input_key_state_bit_e_on ) != 0 )
			{
				assert( _mother_control );
				vector32x2_c local_mouse_position = _mother_control->transform_global_point_to_local_point( input_event->menu_global_mouse_position ); 
				_place_cursor_at_local_point( local_mouse_position, true );
				return true;
			}
		}

		return false;
	}

}
