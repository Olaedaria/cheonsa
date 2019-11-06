#include "cheonsa_menu_element_text.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	core_linked_list_c< menu_element_text_c * > menu_element_text_c::_global_list;

	void_c menu_element_text_c::invalidate_glyph_layout_of_all_instances()
	{
		core_linked_list_c< menu_element_text_c * >::node_c const * menu_element_text_list_node = _global_list.get_first();
		while ( menu_element_text_list_node != nullptr )
		{
			menu_element_text_list_node->get_value()->_invalidate_glyph_flow_in_all_paragraphs();
			menu_element_text_list_node = menu_element_text_list_node->get_next();
		}
	}

	void_c menu_element_text_c::invalidate_glyph_layout_of_all_instances_with_font( resource_file_font_c * font )
	{
		core_linked_list_c< menu_element_text_c * >::node_c const * menu_element_text_list_node = _global_list.get_first();
		while ( menu_element_text_list_node != nullptr )
		{
			menu_element_text_list_node->get_value()->_invalidate_glyph_flow_in_all_paragraphs_with_font( font );
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
		, _style( nullptr )
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
		_style = other._style;
		_box = other._box;
		_left = other._left;
		_ascender = other._ascender;
		_descender = other._descender;
		_horizontal_advance = other._horizontal_advance;
		return *this;
	}

	void_c menu_element_text_c::text_span_c::_resolve_style_reference()
	{
		//if ( _mother_paragraph )
		//{
		//	_mother_paragraph->_text_layout_is_dirty = true; // is not needed because we are only called by our mother (and she already knows).
		//}
		_style_reference.refresh();
		for ( sint32_c i = 0; i < _span_list.get_length(); i++ )
		{
			_span_list[ i ]->_resolve_style_reference();
		}
	}

	menu_element_text_c::text_span_c const * menu_element_text_c::text_span_c::_find_span( sint32_c with_start_character_index ) const
	{
		for ( sint32_c i = 0; i < _span_list.get_length(); i++ )
		{
			text_span_c const * span = _span_list[ i ];
			if ( span->_character_start >= with_start_character_index )
			{
				if ( span->_character_start == with_start_character_index )
				{
					return span;
				}
				break;
			}
		}
		return nullptr;
	}

	menu_element_text_c::text_span_c::text_span_c()
		: _mother_paragraph( nullptr )
		, _mother_span( nullptr )
		, _span_list()
		, _character_start( -1 )
		, _character_end( -1 )
		, _style_reference()
	{
	}

	menu_element_text_c::text_span_c & menu_element_text_c::text_span_c::operator = ( text_span_c const & other )
	{
		_style_reference.set_value( other._style_reference.get_value() );
		_character_start = other._character_start;
		_character_end = other._character_end;
		return *this;
	}

	resource_file_font_c * menu_element_text_c::text_span_c::get_style_font() const
	{
		text_span_c const * span = this;
		while ( true )
		{
			if ( span->_style_reference.get_value() )
			{
				if ( span->_style_reference.get_value()->font_is_defined )
				{
					assert( span->_style_reference.get_value()->font.is_reference_set_and_loaded() );
					return span->_style_reference.get_value()->font;
				}
			}
			if ( span->_mother_span )
			{
				span = span->_mother_span;
			}
			else
			{
				break;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->get_style_font();
	}

	vector32x4_c menu_element_text_c::text_span_c::get_style_color() const
	{
		text_span_c const * span = this;
		while ( span )
		{
			if ( span->_style_reference.get_value() )
			{
				if ( span->_style_reference.get_value()->shared_color_is_defined || span->_style_reference.get_value()->shared_color.get_value() )
				{
					return span->_style_reference.get_value()->shared_color.get_value()->value;
				}
				else if ( span->_style_reference.get_value()->color_is_defined )
				{
					return span->_style_reference.get_value()->color;
				}
			}
			span = span->_mother_span;
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->get_style_color();
	}

	float32_c menu_element_text_c::text_span_c::get_style_size() const
	{
		text_span_c const * span = this;
		while ( true )
		{
			if ( span->_style_reference.get_value() )
			{
				if ( span->_style_reference.get_value()->size_is_defined )
				{
					return span->_style_reference.get_value()->size;
				}
			}
			if ( span->_mother_span )
			{
				span = span->_mother_span;
			}
			else
			{
				break;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->get_style_size();
	}

	float32_c menu_element_text_c::text_span_c::get_style_skew() const
	{
		text_span_c const * span = this;
		while ( true )
		{
			if ( span->_style_reference.get_value() )
			{
				if ( span->_style_reference.get_value()->skew_is_defined )
				{
					return span->_style_reference.get_value()->skew;
				}
			}
			if ( span->_mother_span )
			{
				span = span->_mother_span;
			}
			else
			{
				break;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->get_style_skew();
	}

	float32_c menu_element_text_c::text_span_c::get_style_weight() const
	{
		text_span_c const * span = this;
		while ( true )
		{
			if ( span->_style_reference.get_value() )
			{
				if ( span->_style_reference.get_value()->weight_is_defined )
				{
					return span->_style_reference.get_value()->weight;
				}
			}
			if ( span->_mother_span )
			{
				span = span->_mother_span;
			}
			else
			{
				break;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->get_style_weight();
	}

	float32_c menu_element_text_c::text_span_c::get_style_softness() const
	{
		text_span_c const * span = this;
		while ( true )
		{
			if ( span->_style_reference.get_value() )
			{
				if ( span->_style_reference.get_value()->softness_is_defined )
				{
					return span->_style_reference.get_value()->softness;
				}
			}
			if ( span->_mother_span )
			{
				span = span->_mother_span;
			}
			else
			{
				break;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->get_style_softness();
	}

	float32_c menu_element_text_c::text_span_c::get_style_underline() const
	{
		text_span_c const * span = this;
		while ( true )
		{
			if ( span->_style_reference.get_value() )
			{
				if ( span->_style_reference.get_value()->underline_is_defined )
				{
					return span->_style_reference.get_value()->underline;
				}
			}
			if ( span->_mother_span )
			{
				span = span->_mother_span;
			}
			else
			{
				break;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->get_style_underline();
	}

	float32_c menu_element_text_c::text_span_c::get_style_glyph_spacing() const
	{
		text_span_c const * span = this;
		while ( true )
		{
			if ( span->_style_reference.get_value() )
			{
				if ( span->_style_reference.get_value()->glyph_spacing_is_defined )
				{
					return span->_style_reference.get_value()->glyph_spacing;
				}
			}
			if ( span->_mother_span )
			{
				span = span->_mother_span;
			}
			else
			{
				break;
			}
		}
		assert( span->_mother_paragraph );
		return span->_mother_paragraph->get_style_glyph_spacing();
	}

	menu_style_for_text_glyph_c menu_element_text_c::text_span_c::get_style() const
	{
		menu_style_for_text_glyph_c result;
		result.font = get_style_font();
		result.color = get_style_color();
		result.size = get_style_size();
		result.weight = get_style_weight();
		result.skew = get_style_skew();
		result.softness = get_style_softness();
		result.underline = get_style_underline();
		result.quantized_size = glyph_manager_c::get_quantized_size( result.size );
		result.sdf_range = glyph_manager_c::get_sdf_range( result.quantized_size );
		return result;
	}

	void_c menu_element_text_c::text_paragraph_c::_resolve_style_reference()
	{
		//if ( _mother_element_text )
		//{
		//	_mother_element_text->_text_layout_is_dirty = true; // is not be needed because we are only called from our mother (and she already knows).
		//}
		_text_layout_is_dirty = true;
		_style_reference.refresh();
		for ( sint32_c i = 0; i < _span_list.get_length(); i++ )
		{
			_span_list[ i ]->_resolve_style_reference();
		}
	}

	menu_element_text_c::text_span_c const * menu_element_text_c::text_paragraph_c::_find_span( sint32_c with_start_character_index ) const
	{
		for ( sint32_c i = 0; i < _span_list.get_length(); i++ )
		{
			text_span_c const * span = _span_list[ i ];
			if ( span->_character_start >= with_start_character_index )
			{
				if ( span->_character_start == with_start_character_index )
				{
					return span;
				}
				break;
			}
		}
		return nullptr;
	}

	menu_element_text_c::text_paragraph_c::text_paragraph_c( menu_element_text_c * mother_element_text )
		: _mother_element_text( mother_element_text )
		, _character_start( -1 )
		, _character_end( -1 )
		, _entity_list()
		, _span_list()
		, _top( 0.0f )
		, _content_width( 0.0f )
		, _content_height( 0.0f )
		, _line_index_base( 0 )
		, _line_list()
		, _glyph_list()
		, _text_layout_is_dirty( false )
	{
	}

	menu_element_text_c::text_paragraph_c::~text_paragraph_c()
	{
		for ( sint32_c i = 0; i < _entity_list.get_length(); i++ )
		{
			delete _entity_list[ i ];
		}
		for ( sint32_c i = 0; i < _span_list.get_length(); i++ )
		{
			delete _span_list[ i ];
		}
	}

	void_c menu_element_text_c::text_paragraph_c::_delete_character_range( sint32_c start, sint32_c end )
	{
		assert( _mother_element_text->_text_edit_mode == menu_text_edit_mode_e_editable );
		assert( end >= start );
		sint32_c count = end - start + 1;


		for ( sint32_c j = 0; j < _span_list.get_length(); j++ )
		{
			text_span_c * span = _span_list[ j ];
			if ( end < span->_character_start )
			{
				// delete range was before this span range.
				span->_character_start -= count;
				span->_character_end -= count;
			}
			else if ( start <= span->_character_start && end >= span->_character_end )
			{
				// delete range contains this entire span range.
				delete span;
				j--;
			}
			else if ( start > span->_character_start && end < span->_character_end )
			{
				// delete range is entirely within this span range.
				span->_character_end -= count;
			}
			else if ( start <= span->_character_start && end > span->_character_start && end < span->_character_end )
			{
				// delete range contains head of this span range.
				span->_character_end -= end - span->_character_start + 1;
			}
			else if ( start > span->_character_start && start < span->_character_end && end >= span->_character_end )
			{
				// delete range contains tail of this span range.
				span->_character_end = start - 1;
			}
		}
	}

	sint32_c menu_element_text_c::text_paragraph_c::get_line_index( sint32_c character_index )
	{
		for ( sint32_c i = 0; i < _line_list.get_length(); i++ )
		{
			text_line_c * line = &_line_list[ i ];
			if ( character_index >= line->_character_start && character_index <= line->_character_end )
			{
				return i;
			}
		}
		return -1;
	}

	menu_text_align_horizontal_e menu_element_text_c::text_paragraph_c::get_style_text_align_horizontal() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value())
		{
			if ( _style_reference.get_value()->text_align_horizontal_is_defined )
			{
				return _style_reference.get_value()->text_align_horizontal;
			}
		}
		return _mother_element_text->get_style_text_align_horizontal();
	}

	float32_c menu_element_text_c::text_paragraph_c::get_style_paragraph_spacing() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->paragraph_spacing_is_defined )
			{
				return _style_reference.get_value()->paragraph_spacing;
			}
		}
		return _mother_element_text->get_style_paragraph_spacing();
	}

	float32_c menu_element_text_c::text_paragraph_c::get_style_line_spacing() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->line_spacing_is_defined )
			{
				return _style_reference.get_value()->line_spacing;
			}
		}
		return _mother_element_text->get_style_line_spacing();
	}

	float32_c menu_element_text_c::text_paragraph_c::get_style_glyph_spacing() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->glyph_spacing_is_defined )
			{
				return _style_reference.get_value()->glyph_spacing;
			}
		}
		return _mother_element_text->get_style_glyph_spacing();
	}

	resource_file_font_c * menu_element_text_c::text_paragraph_c::get_style_font() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->font_is_defined )
			{
				assert( _style_reference.get_value()->font.is_reference_set_and_loaded() );
				return _style_reference.get_value()->font;
			}
		}
		return _mother_element_text->get_style_font();
	}

	vector32x4_c menu_element_text_c::text_paragraph_c::get_style_color() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->shared_color_is_defined && _style_reference.get_value()->shared_color.get_value() )
			{
				return _style_reference.get_value()->shared_color.get_value()->value;
			}
			else if ( _style_reference.get_value()->color_is_defined )
			{
				return _style_reference.get_value()->color;
			}
		}
		return _mother_element_text->get_style_color();
	}

	float32_c menu_element_text_c::text_paragraph_c::get_style_size() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->size_is_defined )
			{
				return _style_reference.get_value()->size;
			}
		}
		return _mother_element_text->get_style_size();
	}

	float32_c menu_element_text_c::text_paragraph_c::get_style_skew() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->skew_is_defined )
			{
				return _style_reference.get_value()->skew;
			}
		}
		return _mother_element_text->get_style_skew();
	}

	float32_c menu_element_text_c::text_paragraph_c::get_style_weight() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->weight_is_defined )
			{
				return _style_reference.get_value()->weight;
			}
		}
		return _mother_element_text->get_style_weight();
	}

	float32_c menu_element_text_c::text_paragraph_c::get_style_softness() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->softness_is_defined )
			{
				return _style_reference.get_value()->softness;
			}
		}
		return _mother_element_text->get_style_softness();
	}

	float32_c menu_element_text_c::text_paragraph_c::get_style_underline() const
	{
		assert( _mother_element_text );
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->underline_is_defined )
			{
				return _style_reference.get_value()->underline;
			}
		}
		return _mother_element_text->get_style_underline();
	}

	menu_style_for_text_glyph_c menu_element_text_c::text_paragraph_c::get_style() const
	{
		menu_style_for_text_glyph_c result;
		result.font = get_style_font();
		result.color = get_style_color();
		result.size = get_style_size();
		result.weight = get_style_weight();
		result.skew = get_style_skew();
		result.softness = get_style_softness();
		result.underline = get_style_underline();
		result.quantized_size = glyph_manager_c::get_quantized_size( result.size );
		result.sdf_range = glyph_manager_c::get_sdf_range( result.quantized_size );
		return result;
	}

	void_c menu_element_text_c::_handle_style_reference_on_refreshed( menu_style_for_text_c::reference_c const * value )
	{
		_style.font = get_style_font();
		_style.color = get_style_color();
		_style.size = get_style_size();
		_style.weight = get_style_weight();
		_style.skew = get_style_skew();
		_style.softness = get_style_softness();
		_style.underline = get_style_underline();
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			_paragraph_list[ i ]->_resolve_style_reference();
		}
		_text_layout_is_dirty = true;
	}

	void_c menu_element_text_c::_handle_string_reference_on_refreshed( string_c::reference_c const * value )
	{
		if ( _string_reference.get_value() )
		{
			if ( _text_format_mode == menu_text_format_mode_e_plain )
			{
				set_plain_text( string16_c( _string_reference.get_value()->get_value() ) );
			}
			else
			{
				set_rich_text( _string_reference.get_value()->get_value() );
			}
		}
		else
		{
			string16_c plain_text;
			plain_text += "[";
			plain_text += _string_reference.get_key();
			plain_text += "]";
			set_plain_text( plain_text );
		}
	}

	menu_style_for_text_glyph_c const * menu_element_text_c::_cache_text_glyph_style( menu_style_for_text_glyph_c const & text_glyph_style )
	{
		for ( sint32_c i = 0; i < _text_glyph_style_cache.get_length(); i++ )
		{
			if ( *_text_glyph_style_cache[ i ] == text_glyph_style )
			{
				return _text_glyph_style_cache[ i ];
			}
		}
		menu_style_for_text_glyph_c const * result = new menu_style_for_text_glyph_c( text_glyph_style );
		_text_glyph_style_cache.insert_at_end( result );
		return result;
	}

	void_c menu_element_text_c::_finalize_glyph_flow_for_line( text_paragraph_c * paragraph, sint32_c character_start, sint32_c character_end, float32_c laid_out_width, float32_c & laid_out_top )
	{
		// advance by extra line spacing if needed.
		if ( paragraph->_line_list.get_length() > 0 )
		{
			laid_out_top += paragraph->get_style_line_spacing();
		}

		// create new laid out line.
		text_line_c * line = paragraph->_line_list.emplace_at_end();
		line->_character_start = paragraph->_character_start + character_start;
		line->_character_end = paragraph->_character_start + character_end;
		line->_width = laid_out_width;
		if ( paragraph->_content_width < laid_out_width )
		{
			paragraph->_content_width = laid_out_width;
		}

		// take into account horizontal text alignment.
		line->_update_horizontal_layout( paragraph->get_style_text_align_horizontal(), _local_box.get_width() );

		// this is straight forward because it's relative to the paragraph's top, so we don't need to take into account vertical text alignment here, that is done in the paragraph.
		line->_top = laid_out_top;

		// find largest ascender and descender so that we can find out where to place the base line and how tall the line is.
		// and finalize the horizontal position the glyphs.
		float32_c cursor_x = 0.0f; //line->_left;
		float32_c ascender = 0.0f; // accumulates the largest positive value above the baseline.
		float32_c descender = 0.0f; // accumulates the largest negative value below the baseline.
		for ( sint32_c i = character_start; i < character_end; i++ )
		{
			text_glyph_c * glyph = &paragraph->_glyph_list[ i ];
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

	void_c menu_element_text_c::_invalidate_glyph_flow_in_all_paragraphs()
	{
		_text_layout_is_dirty = true;
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
			paragraph->_text_layout_is_dirty = true;
		}
	}

	void_c menu_element_text_c::_invalidate_glyph_flow_in_all_paragraphs_with_font( resource_file_font_c * font )
	{
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
			if ( paragraph->get_style_font() == font )
			{
				paragraph->_text_layout_is_dirty = true;
			}
			else
			{
				for ( sint32_c span_index = 0; span_index < paragraph->_span_list.get_length(); span_index++ )
				{
					text_span_c * span = paragraph->_span_list[ span_index ];
					if ( span->get_style_font() == font )
					{
						_text_layout_is_dirty = true;
						paragraph->_text_layout_is_dirty = true;
						break;
					}
				}
			}
		}
	}

	void_c menu_element_text_c::_reflow_glyphs_in_all_paragraphs_that_need_it()
	{
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
			if ( paragraph->_text_layout_is_dirty )
			{
				_reflow_glyphs_in_paragraph( paragraph_index );
			}
		}
		_text_layout_is_dirty = false;
		_update_vertical_layout_of_all_paragraphs();
	}

	void_c menu_element_text_c::_reflow_glyphs_in_paragraph( sint32_c paragraph_index )
	{
		assert( paragraph_index >= 0 && paragraph_index < _paragraph_list.get_length() );

		text_paragraph_c * current_paragraph = _paragraph_list[ paragraph_index ];
		assert( current_paragraph->_character_end > current_paragraph->_character_start );
		current_paragraph->_content_width = 0.0f;
		current_paragraph->_line_list.remove_all();
		current_paragraph->_glyph_list.remove_all();

		float32_c word_wrap_width = _local_box.get_width();
		boolean_c word_wrap_has_word = false; // will be set to true if we are parsing a string of displayable characters.
		boolean_c word_wrap_has_space = false; // will be set to true if we are parsing a string of invisible characters.
		boolean_c word_wrap_just_wrapped = true; // will be set to true if a virtual line break was just inserted to prevent a perpetual insertion of virtual line breaks if a word is wider than word_wrap_width. is initially set to true to prevent automatic word wrap from invoking on the first word if the first word is wider than the word_wrap_width.
		sint32_c current_line_start = 0; // index of character in _plain_text that is the first character of the current line.
		float32_c current_word_width = 0.0f; // current accumulated width of consecutive displayable non-line-breaking characters.
		sint32_c current_word_start = 0; // index of character in _plain_text that is the first character of the current word.
		float32_c current_space_width = 0.0f; // current accumulated width of consecutive spaces. space width does not contribute to the width of the line until it is followed up by a word.
		float32_c current_line_top = 0.0f; // current offset of top of line from top of paragraph.
		float32_c current_line_width = 0.0f; // current accumulated width of line.
		float32_c current_line_spacing = current_paragraph->get_style_line_spacing(); // extra space to place between lines is defined by the paragraph.
		float32_c current_laid_out_ascender; // current unquantized ascender for the current style of the current character.
		float32_c current_laid_out_descender; // current unquantized descender for the current style of the current character.
		float32_c current_laid_out_glyph_spacing; // current unquantized glyph spacing (extra horizontal advance) for the current style of the current character.
		float32_c current_scale_to_unquantize_size; // corrective scale, to scale glyph from size it was baked at (quantized) to arbitrary size that the style wants it to be rendered at (dequantized).

		text_span_c const * last_span = nullptr; // span of the previous character, may be nullptr.
		text_span_c const * current_span = nullptr; // span of the current character, may be nullptr.

		//menu_style_for_text_glyph_c const * last_style = nullptr; // style of the previous character, may be nullptr (if current character is the first character).
		//menu_style_for_text_glyph_c const * current_style_p = nullptr; // style of the current character, may not be nullptr (will be pulled from current_span or current_paragraph).
		menu_style_for_text_glyph_c current_text_glyph_style; // used to build compiled text glyph style based on element's style, paragraph's style, and any span's style (of which may be nested).
		menu_style_for_text_glyph_c const * current_text_glyph_style_in_cache = nullptr; // cached equivalent of current_style_for_text_glyph, 
		menu_style_for_text_glyph_c const * last_text_glyph_style_in_cache = nullptr;

		char16_c last_code_point = 0; // previous character.
		char16_c current_code_point = 0; // current character.

		for ( sint32_c j = current_paragraph->_character_start; j < current_paragraph->_character_end; j++ )
		{
			text_span_c const * last_span = current_span;

			// exit nested spans.
			while ( current_span != nullptr && j == current_span->_character_end )
			{
				current_span = current_span->_mother_span;
			}
			
			// enter nested spans.
			if ( current_span == nullptr )
			{
				current_span = current_paragraph->_find_span( j );
			}
			if ( current_span != nullptr )
			{
				text_span_c const * current_span_sub = current_span->_find_span( j );
				while ( current_span_sub != nullptr )
				{
					if ( current_span_sub != nullptr )
					{
						current_span = current_span_sub;
					}
					current_span_sub = current_span->_find_span( j );
				}
			}

			// handle change over of current span.
			last_text_glyph_style_in_cache = current_text_glyph_style_in_cache;
			if ( current_span != last_span || last_text_glyph_style_in_cache == nullptr )
			{
				if ( current_span != nullptr )
				{
					current_text_glyph_style = current_span->get_style();
					current_laid_out_glyph_spacing = current_span->get_style_glyph_spacing() * current_text_glyph_style.size;
				}
				else
				{
					assert( current_paragraph != nullptr );
					current_text_glyph_style = current_paragraph->get_style();
					current_laid_out_glyph_spacing = current_paragraph->get_style_glyph_spacing() * current_text_glyph_style.size;
				}
				
				current_scale_to_unquantize_size = current_text_glyph_style.size / static_cast< float32_c >( current_text_glyph_style.quantized_size );
				current_text_glyph_style.softness += 1.0f / current_scale_to_unquantize_size * 0.025f; // softness bias, so that edges are base-line level of soft by default.
				current_laid_out_ascender = current_text_glyph_style.font->get_unquantized_ascender( current_text_glyph_style.size );
				current_laid_out_descender = current_text_glyph_style.font->get_unquantized_descender( current_text_glyph_style.size );
				current_space_width = current_text_glyph_style.font->get_unquantized_horizontal_advance_for_space( current_text_glyph_style.size );
				current_text_glyph_style_in_cache = _cache_text_glyph_style( current_text_glyph_style );
			}

			assert( current_text_glyph_style.color.d != 0.0f );

			// process the current character.
			boolean_c process_word_wrap = true;
			boolean_c current_character_is_space = false;
			last_code_point = current_code_point;
			current_code_point = _plain_text.character_list[ j ];
			text_glyph_c * current_glyph = current_paragraph->_glyph_list.emplace_at_end(); // create the laid out glyph instance that maps to the current character.
			current_glyph->_glyph = nullptr;
			current_glyph->_left = 0.0f; // will be set properly when _finalize_glyph_flow_for_line() is called.
			current_glyph->_ascender = current_laid_out_ascender;
			current_glyph->_descender = current_laid_out_descender;
			current_glyph->_style = current_text_glyph_style_in_cache;
			if ( current_code_point != '\n' && current_code_point != ' ' && current_code_point != '\t' && ( current_code_point & 0xF000 ) != 0xF000 )
			{
				// this character is displayable.
				glyph_c const * glyph = global_engine_instance.interfaces.glyph_manager->load_quantized_glyph( current_text_glyph_style_in_cache->font, current_text_glyph_style_in_cache->size, current_code_point );
				if ( glyph == nullptr )
				{
					// the glyph cache filled up, the glyph cache will request the engine to reset it at the start of the next update, and then a reflow will be triggered for us again and we can try again.
					return;
				}

				assert( glyph->key.code_point == current_code_point || glyph->key.code_point == '?' );

				// create laid out glyph for the current character.
				current_glyph->_glyph = glyph;
				current_glyph->_box.minimum.a = glyph->box.minimum.a * current_scale_to_unquantize_size;
				current_glyph->_box.minimum.b = glyph->box.minimum.b * current_scale_to_unquantize_size;
				current_glyph->_box.maximum.a = glyph->box.maximum.a * current_scale_to_unquantize_size;
				current_glyph->_box.maximum.b = glyph->box.maximum.b * current_scale_to_unquantize_size;
				current_glyph->_horizontal_advance = ( glyph->horizontal_advance * current_scale_to_unquantize_size ) + current_laid_out_glyph_spacing;

				// apply kerning if possible.
				if ( last_text_glyph_style_in_cache != nullptr && last_text_glyph_style_in_cache->font == current_text_glyph_style_in_cache->font )
				{
					current_glyph->_horizontal_advance += current_text_glyph_style_in_cache->font->find_kern_advance( last_text_glyph_style_in_cache->size, last_code_point, current_text_glyph_style_in_cache->size, current_code_point );
				}
			}
			else if ( current_code_point == ' ' )
			{
				current_character_is_space = true;

				// create laid out glyph of width of space.
				current_glyph->_horizontal_advance = ( current_space_width * current_scale_to_unquantize_size ) + current_laid_out_glyph_spacing;
			}
			else if ( current_code_point == '\t' )
			{
				current_character_is_space = true;

				// create laid out glyph of width of tab, which is dynamic.
				current_glyph->_horizontal_advance = ( current_space_width * current_scale_to_unquantize_size ) + current_laid_out_glyph_spacing;
			}
			else if ( current_code_point == '\n' )
			{
				// create laid out glyph of same size as space character.
				current_glyph->_horizontal_advance = ( current_space_width * current_scale_to_unquantize_size ) + current_laid_out_glyph_spacing;

				_finalize_glyph_flow_for_line( current_paragraph, current_line_start, current_paragraph->_glyph_list.get_length(), current_line_width + current_word_width + current_space_width, current_line_top );
				current_line_start = j - current_paragraph->_character_start + 1;
				assert( current_line_start >= 0 );
				current_word_start = j - current_paragraph->_character_start + 1;
				current_line_width = 0.0f;
				current_word_width = 0.0f;
				current_space_width = 0.0f;
				word_wrap_just_wrapped = true;
				process_word_wrap = false;
			}
			else if ( ( current_code_point & 0xF000 ) == 0xF000 )
			{
				// current character is referencing an entity.
				current_glyph->_glyph = nullptr;
				current_glyph->_box.minimum.a = 0.0f; // box is not used by sprite entities.
				current_glyph->_box.minimum.b = -current_laid_out_ascender;
				current_glyph->_box.maximum.a = 0.0f;
				current_glyph->_box.maximum.b = -current_laid_out_descender;
				current_glyph->_left = 0.0f;
				current_glyph->_ascender = current_laid_out_ascender;
				current_glyph->_descender = current_laid_out_descender;
				current_glyph->_style = current_text_glyph_style_in_cache;

				// the entity instance exists in the paragraph's entity list from when the rich text markup was processed, retreive it.
				uint16_c entity_index = 0x0FFF & current_code_point;
				menu_text_entity_c * entity = current_paragraph->_entity_list[ entity_index ];
				if ( entity->type_code == menu_text_entity_sprite_c::type_code_static() )
				{
					menu_text_entity_sprite_c * entity_sprite = static_cast< menu_text_entity_sprite_c * >( entity );
					if ( entity_sprite->value.get_is_ready() )
					{
						current_glyph->_horizontal_advance = entity_sprite->value.get_sprite()->font_horizontal_advance * ( current_text_glyph_style_in_cache->size / entity_sprite->value.get_sprite()->font_size );
					}
					else
					{
						current_glyph->_horizontal_advance = current_text_glyph_style_in_cache->size;
					}
				}
			}

			// perform word wrap if needed.
			if ( _word_wrap && _multi_line && process_word_wrap )
			{
				if ( current_character_is_space == false )
				{
					if ( word_wrap_has_word == false )
					{
						word_wrap_has_word = true;
						current_word_start = j - current_paragraph->_character_start;
						current_word_width = 0.0f;
					}
					current_word_width += current_glyph->_horizontal_advance;
					if ( word_wrap_has_space == true )
					{
						word_wrap_has_space = false;
					}
					if ( word_wrap_just_wrapped == false && current_line_width + current_space_width + current_word_width - current_laid_out_glyph_spacing > word_wrap_width )
					{
						_finalize_glyph_flow_for_line( current_paragraph, current_line_start, current_word_start, current_line_width, current_line_top ); // note that automatic word wrapping does not factor in the current accumulated width of white space at the end of the line, this lets the line ignore this trailing white space when text is horizontally aligned center or right.
						current_line_start = current_word_start;
						current_line_width = 0.0f;
						current_space_width = 0.0f;
						current_word_start = j - current_paragraph->_character_start;
						word_wrap_just_wrapped = true;
					}
				}
				else
				{
					if ( word_wrap_has_word == true )
					{
						word_wrap_has_word = false;
						current_line_width += current_space_width + current_word_width;
						current_word_width = 0.0f;
					}
					if ( word_wrap_has_space == false )
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
		current_paragraph->_top = -1.0f; // invalidate this value, it will be recalculated via subsequent call to _update_vertical_layout_of_paragraphs_at_and_after().
		current_paragraph->_content_height = current_line_top - current_line_spacing; // subtract extra line spacing that would have been added by previous call to _finalize_glyph_flow_for_line().
		current_paragraph->_text_layout_is_dirty = false;
	}

	void_c menu_element_text_c::_update_horizontal_layout_of_all_lines()
	{
		assert( _text_layout_is_dirty == false );
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			assert( paragraph->_text_layout_is_dirty == false );
			for ( sint32_c j = 0; j < paragraph->_line_list.get_length(); j++ )
			{
				text_line_c * line = &paragraph->_line_list[ j ];
				line->_update_horizontal_layout( paragraph->get_style_text_align_horizontal(), _local_box.get_width() );
			}
		}
	}

	void_c menu_element_text_c::_update_vertical_layout_of_all_paragraphs()
	{
		//assert( _text_layout_is_dirty == false ); // this function is only supposed to be called once glyphs have been reflowed.
		_content_width = 0.0f;
		_content_height = 0.0f;
		float32_c paragraph_spacing = get_style_paragraph_spacing();
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
			//menu_text_align_vertical_e text_align_vertical = get_style_text_align_vertical();
			//if ( text_align_vertical == menu_text_align_vertical_e_center )
			//{
			//	paragraph_top_offset = ( _local_box.get_height() * 0.5f ) - ( _content_height * 0.5f );
			//}
			//else if ( text_align_vertical == menu_text_align_vertical_e_bottom )
			//{
			//	paragraph_top_offset = _local_box.get_height() - _content_height;
			//}
			//if ( paragraph_top_offset != 0.0f ) // translate paragraphs if needed.
			//{
			//	for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
			//	{
			//		paragraph = _paragraph_list[ i ];
			//		paragraph->_top += paragraph_top_offset;
			//	}
			//}
		}
	}

	void_c menu_element_text_c::_place_cursor_at_line_index( sint32_c line_index, boolean_c shift )
	{
		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		// cap input line within valid range.
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

		// find paragraph and line of line_index.
		text_paragraph_c * paragraph = nullptr;
		text_line_c * line = nullptr;
		boolean_c line_is_virtual_line = false; // virtual lines are generated by word wrapping, and so they don't have a new line character at the end. in a paragraph, all of the formatted lines are virtual lines except for the last one.
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			paragraph = _paragraph_list[ paragraph_index ];
			if ( line_index >= paragraph->_line_index_base && line_index < paragraph->_line_index_base + paragraph->_line_list.get_length() )
			{
				line = &paragraph->_line_list[ line_index - paragraph->_line_index_base ];
				line_is_virtual_line = line_index - paragraph->_line_index_base < paragraph->_line_list.get_length() - 1;
				break;
			}
		}

		// place cursor on target line and preserve sticky x.
		sint32_c j = line->_character_start;
		sint32_c j_end = line->_character_end;
		if ( line_is_virtual_line == false )
		{
			j_end--; // exclude terminating new line character of paragraph.
		}
		for ( ; j <= j_end; j++ )
		{
			text_glyph_c * laid_out_glyph = &paragraph->_glyph_list[ j - paragraph->_character_start ];
			if ( _cursor_sticky_x < line->_left + laid_out_glyph->_left + ( laid_out_glyph->_horizontal_advance * 0.5f ) )
			{
				break;
			}
		}
		_cursor_index = j;
		_cursor_is_at_end_of_virtual_line = _cursor_index == line->_character_end && line_is_virtual_line;

		if ( shift == false )
		{
			_selection_anchor_index_start = _cursor_index;
			_selection_anchor_index_end = _cursor_index;
		}
	}

	void_c menu_element_text_c::_place_cursor_at_local_point( vector32x2_c const & local_point, boolean_c shift )
	{
		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		float32_c vertically_algined_y = 0.0f;
		menu_text_align_vertical_e text_align_vertical = get_style_text_align_vertical();
		if ( text_align_vertical == menu_text_align_vertical_e_center )
		{
			vertically_algined_y = ( _local_box.get_height() * 0.5f ) - ( get_content_height() * 0.5f );
		}
		else if ( text_align_vertical == menu_text_align_vertical_e_bottom )
		{
			vertically_algined_y = _local_box.get_height() - get_content_height();
		}

		boolean_c picked = false;
		sint32_c new_cursor_index = 0;
		float32_c new_cursor_sticky_x = 0.0f;
		boolean_c new_cursor_is_at_end_of_virtual_line = false;

		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];

			float32_c paragraph_left = _local_box.minimum.a + _content_offset.a;
			float32_c paragraph_right = paragraph_left + _local_box.get_width();
			float32_c paragraph_top = _local_box.minimum.b + _content_offset.b + paragraph->_top + vertically_algined_y;
			float32_c paragraph_bottom = paragraph_top + paragraph->_content_height;

			if ( local_point.b < paragraph_top )
			{
				new_cursor_index = paragraph->_character_start;
				new_cursor_sticky_x = paragraph_left;
				goto finish;
			}

			if ( local_point.b >= paragraph_top && local_point.b <= paragraph_bottom )
			{
				for ( sint32_c j = 0; j < paragraph->_line_list.get_length(); j++ )
				{
					text_line_c const * line = &paragraph->_line_list[ j ];

					float32_c line_left = paragraph_left + line->_left; // baseline offset from left.
					float32_c line_top = paragraph_top; // force top of hit box of first line to be flush with paragraph_top.
					if ( j > 0 ) // tops of hit boxes of subsequent lines will be placed correctly.
					{
						line_top += line->_top;
					}
					float32_c line_bottom = paragraph_top + line->_top + line->_ascender - line->_descender; // bottoms of hit boxes of lines will be placed correctly.
					if ( j == paragraph->_line_list.get_length() - 1 ) // force bottom of hit box of last line to be flush with paragraph bottom.
					{
						line_bottom = paragraph_bottom;
					}

					if ( local_point.b < line_top )
					{
						new_cursor_index = line->_character_start;
						new_cursor_sticky_x = line->_left;
						goto finish;
					}

					if ( local_point.b >= line_top && local_point.b <= line_bottom )
					{
						new_cursor_index = line->_character_start;
						new_cursor_is_at_end_of_virtual_line = false;
						new_cursor_sticky_x = line->_left;

						float32_c line_width_with_trailing_white_space = paragraph->_glyph_list[ line->_character_end - paragraph->_character_start - 1 ]._left; // because line->_width does not include traling white space.
						if ( local_point.a < line_left )
						{
							// point lies to the left of formatted line.
							new_cursor_index = line->_character_start;
							new_cursor_is_at_end_of_virtual_line = false;
							new_cursor_sticky_x = line->_left;
						}
						else if ( local_point.a > line_left + line_width_with_trailing_white_space )
						{
							// point lies to the right of formatted line.
							new_cursor_is_at_end_of_virtual_line = j < paragraph->_line_list.get_length() - 1;
							new_cursor_index = new_cursor_is_at_end_of_virtual_line ? line->_character_end : line->_character_end - 1;
							text_glyph_c const * last_glyph = &paragraph->_glyph_list[ line->_character_end - paragraph->_character_start - 1 ];
							new_cursor_sticky_x = line->_left + last_glyph->_left + last_glyph->_horizontal_advance;
						}
						else
						{
							// point lies somewhere within formatted line.
							for ( ; new_cursor_index < line->_character_end; new_cursor_index++ )
							{
								text_glyph_c * glyph = &paragraph->_glyph_list[ new_cursor_index - paragraph->_character_start ];
								if ( local_point.a < line_left + glyph->_left + ( glyph->_horizontal_advance * 0.5f ) )
								{
									new_cursor_sticky_x = line->_left + glyph->_left;
									goto finish;
								}
							}
						}
						goto finish;
					}
				}
			}
		}

		// point lies below everything.
		// place cursor at last index.
		text_paragraph_c const * last_paragraph = _paragraph_list[ _paragraph_list.get_length() - 1 ];
		new_cursor_index = last_paragraph->_character_end - 1;
		text_line_c const * last_line = &last_paragraph->_line_list[ last_paragraph->_line_list.get_length() - 1 ];
		text_glyph_c const * last_glyph = &last_paragraph->_glyph_list[ last_line->_character_end - last_paragraph->_character_start - 1 ];
		new_cursor_sticky_x = last_line->_left + last_glyph->_left + last_glyph->_horizontal_advance;
		new_cursor_is_at_end_of_virtual_line = false;

	finish:
		if ( _text_selection_mode == menu_text_selection_mode_e_character )
		{
			_cursor_index = new_cursor_index;
			_cursor_is_at_end_of_virtual_line = new_cursor_is_at_end_of_virtual_line;
			if ( shift == false )
			{
				_selection_anchor_index_start = new_cursor_index;
				_selection_anchor_index_end = new_cursor_index;
			}
			_cursor_sticky_x = new_cursor_sticky_x;
		}
		else if ( _text_selection_mode == menu_text_selection_mode_e_word )
		{
			sint32_c word_start;
			sint32_c word_end;
			_get_word_at_character_index( new_cursor_index, word_start, word_end );
			if ( shift == false )
			{
				_selection_anchor_index_start = word_start;
				_selection_anchor_index_end = word_end;
				_cursor_index = word_end;
			}
			else
			{
				if ( word_start < _selection_anchor_index_start )
				{
					_cursor_index = word_start;
				}
				else
				{
					_cursor_index = word_end;
				}
			}
			_cursor_is_at_end_of_virtual_line = false;
			_update_cursor_sticky_x();
		}
		else if ( _text_selection_mode == menu_text_selection_mode_e_paragraph )
		{
			text_paragraph_c * paragraph = _get_paragraph_at_character_index( new_cursor_index );
			assert( paragraph != nullptr );
			if ( shift == false )
			{
				_selection_anchor_index_start = paragraph->_character_start;
				_selection_anchor_index_end = paragraph->_character_end;
				_cursor_index = paragraph->_character_end + 1;
			}
			else
			{
				if ( paragraph->_character_start < _selection_anchor_index_start )
				{
					_cursor_index = paragraph->_character_start;
				}
				else
				{
					_cursor_index = paragraph->_character_end + 1;
				}
			}
			//_cursor_is_at_end_of_virtual_line = false;
			_update_cursor_sticky_x();
		}
	}

	void_c menu_element_text_c::_get_word_at_character_index( sint32_c character_index, sint32_c & character_start, sint32_c & character_end )
	{
		sint8_c word_type = 0; // letter.
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

		// scan ahind.
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
		return;
	}

	sint32_c menu_element_text_c::_get_paragraph_index_at_character_index( sint32_c at_character_index )
	{
		for ( sint32_c paragraph_index = 0; paragraph_index < _paragraph_list.get_length(); paragraph_index++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
			if ( at_character_index >= paragraph->_character_start && at_character_index <= paragraph->_character_end )
			{
				return paragraph_index;
			}
		}
		return -1;
	}

	menu_element_text_c::text_paragraph_c * menu_element_text_c::_get_paragraph_at_character_index( sint32_c character_index )
	{
		sint32_c paragraph_index = _get_paragraph_index_at_character_index( character_index );
		if ( paragraph_index >= 0 )
		{
			return _paragraph_list[ paragraph_index ];
		}
		return nullptr;
	}

	void_c menu_element_text_c::_update_cursor_sticky_x()
	{
		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		sint32_c paragraph_index = _get_paragraph_index_at_character_index( _cursor_index );
		text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
		sint32_c line_index = paragraph->get_line_index( _cursor_index );
		if ( _cursor_is_at_end_of_virtual_line )
		{
			line_index--; // get_character_information would have returned the incorrect line index for our situation, fix it.
		}
		text_line_c * laid_out_line = &paragraph->_line_list[ line_index ];
		if ( _cursor_is_at_end_of_virtual_line )
		{
			text_glyph_c * laid_out_glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start - 1 ];
			_cursor_sticky_x = laid_out_line->_left + laid_out_glyph->_left + laid_out_glyph->_horizontal_advance;
		}
		else
		{
			text_glyph_c * laid_out_glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start ];
			_cursor_sticky_x = laid_out_line->_left + laid_out_glyph->_left;
		}
	}

	void_c menu_element_text_c::_insert_plain_text_at_cursor( string16_c const & value )
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_editable );

		sint32_c paragraph_index = _get_paragraph_index_at_character_index( _cursor_index );

		// filter input if needed.
		if ( _filter_mode != menu_text_filter_mode_e_none )
		{
			string16_c value_filtered;
			boolean_c negate = false;
			boolean_c already_has_decimal_point = false;
			if ( _filter_mode == menu_text_filter_mode_e_numeric_real )
			{
				already_has_decimal_point = _plain_text.character_list.find_index_of( '.' ) != -1;
			}
			for ( sint32_c i = 0; i < value.get_length(); i++ )
			{
				char16_c value_character = value.character_list[ i ];
				if ( _multi_line == false && value_character == '\n' )
				{
					continue;
				}
				if ( _filter_mode == menu_text_filter_mode_e_numeric_integer )
				{
					assert( _character_limit > 1 );
					if ( value.get_length() + value_filtered.get_length() >= _character_limit - 1 ) // minus 1 to exclude optional leading - sign.
					{
						break;
					}
					if ( value_character == '-' )
					{
						negate = !negate;
					}
					else if ( value_character < '0' || value_character > '9' )
					{
						continue;
					}
				}
				else if ( _filter_mode == menu_text_filter_mode_e_numeric_real )
				{
					assert( _character_limit > 1 );
					if ( value.get_length() + value_filtered.get_length() >= _character_limit - 1 ) // minus 1 to exclude optional leading - sign.
					{
						break;
					}
					if ( value_character == '.' )
					{
						if ( already_has_decimal_point == true )
						{
							continue;
						}
						already_has_decimal_point = true;
					}
					else if ( value_character == '-' )
					{
						negate = !negate;
					}
					else if ( value_character < '0' || value_character > '9' )
					{
						continue;
					}
				}
				else if ( _filter_mode == menu_text_filter_mode_e_custom )
				{
					if ( _filter_string.character_list.find_index_of( value_character ) == -1 )
					{
						continue;
					}
				}
				value_filtered += value_character;
			}

			// insert or remove minus sign at start of string.
			if ( negate )
			{
				assert( _filter_mode == menu_text_filter_mode_e_numeric_integer || _filter_mode == menu_text_filter_mode_e_numeric_real );
				if ( _plain_text.character_list[ 0 ] == '-' )
				{
					_plain_text.character_list.remove_at_index( 0 );
					_shift_character_offset( _cursor_index, -1 );
					_cursor_index--;
				}
				else
				{
					_plain_text.character_list.insert_at_index( 0, '-' );
					_shift_character_offset( _cursor_index, 1 );
					_cursor_index++;
				}
			}

			// insert filtered values.
			_plain_text.character_list.insert_range_at_index( _cursor_index, value_filtered.get_length(), value_filtered.character_list.get_internal_array() );
			_shift_character_offset( _cursor_index, value_filtered.get_length() );
			_cursor_index += value_filtered.get_length();
		}
		else
		{
			// insert unfiltered values.
			_plain_text.character_list.insert_range_at_index( _cursor_index, value.get_length(), value.character_list.get_internal_array() );
			_shift_character_offset( _cursor_index, value.get_length() );
			_cursor_index += value.get_length();
		}

		_selection_anchor_index_start = _cursor_index;
		_selection_anchor_index_end = _cursor_index;

		// assuming that we have only 1 paragraph.
		_text_is_modified = true;
		_text_layout_is_dirty = true;
		_reflow_glyphs_in_paragraph( paragraph_index );
		_text_layout_is_dirty = false;
		_update_vertical_layout_of_all_paragraphs();

		_selection_anchor_index_start = _cursor_index;
		_selection_anchor_index_end = _cursor_index;

		_cursor_is_at_end_of_virtual_line = false;
		_update_cursor_sticky_x();

		on_value_changed_preview.invoke( this );		
	}

	void_c menu_element_text_c::_shift_character_offset( sint32_c at_character_index, sint32_c count )
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_editable );
		assert( count > 0 );
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			assert( paragraph->_span_list.get_length() == 0 );
			if ( at_character_index < paragraph->_character_start )
			{
				paragraph->_character_start += count;
			}
			if ( at_character_index <= paragraph->_character_end )
			{
				paragraph->_character_end += count;
			}
		}
	}

	void_c menu_element_text_c::_delete_character_range( sint32_c start, sint32_c end )
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_editable );

		// ensure that we aren't trying to delete anything out of range, or the last new line character, or the terminating null character.
		assert( start >= 0 && end >= start && end < _plain_text.character_list.get_length() - 2 );

		// remove characters from plain text.
		_plain_text.character_list.remove_range_at_index( start, end - start + 1 );

		sint32_c paragraph_to_reflow = -1;
		sint32_c paragraph_to_merge = -1;
		sint32_c i = 0;
		for ( ; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];

			if ( start <= paragraph->_character_start && end >= paragraph->_character_end )
			{
				// delete range contains this entire paragraph range.
				boolean_c done = end == paragraph->_character_end;
				_paragraph_list.remove_at_index( i );
				delete paragraph;
				if ( done )
				{
					break;
				}
				i--;
			}
			else if ( start >= paragraph->_character_start && end < paragraph->_character_end )
			{
				// delete range is contained entirely within this paragraph range (excluding the terminating new line character).
				paragraph->_character_end -= end - start;
				paragraph->_delete_character_range( start, end );
				_reflow_glyphs_in_paragraph( i );
				if ( paragraph_to_reflow == -1 )
				{
					paragraph_to_reflow = i;
				}
				i++;
				break;
			}
			else if ( start <= paragraph->_character_start && end >= paragraph->_character_start && end < paragraph->_character_end )
			{
				// delete range contains head of this paragraph range.
				paragraph->_character_start = start;
				paragraph->_character_end -= end - start + 1;
				paragraph->_delete_character_range( start, end );
				_reflow_glyphs_in_paragraph( i );
				if ( paragraph_to_reflow == -1 )
				{
					paragraph_to_reflow = i;
				}
				i++;
				break;
			}
			else if ( start > paragraph->_character_start && start < paragraph->_character_end && end >= paragraph->_character_end )
			{
				// delete range contains the tail end of this paragraph range.
				boolean_c done = end == paragraph->_character_end;
				paragraph->_character_end = start;
				paragraph->_delete_character_range( start, end );
				paragraph_to_merge = i;
				if ( paragraph_to_reflow == -1 )
				{
					paragraph_to_reflow = i;
				}
				if ( done )
				{
					i++;
					break;
				}
			}
		}

		// adjust offsets in remaining paragraphs.
		for ( ; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			paragraph->_character_start -= end - start;
			paragraph->_character_end -= end - start;
			paragraph->_delete_character_range( start, end );
		}

		// merge paragraph_to_merge if it is set and if there is another paragraph that follows after it.
		if ( paragraph_to_merge >= 0 && paragraph_to_merge + 1 < _paragraph_list.get_length() )
		{
			text_paragraph_c * paragraph = _paragraph_list[ paragraph_to_merge ];
			text_paragraph_c * next_paragraph = _paragraph_list[ paragraph_to_merge + 1 ];

			paragraph->_character_end = next_paragraph->_character_end;
			//paragraph->_span_list.insert_range_at_end( next_paragraph->_span_list.get_array(), next_paragraph->_span_list.get_count() );
			//next_paragraph->_span_list.remove_all();

			delete next_paragraph;

			_paragraph_list.remove_at_index( paragraph_to_merge + 1 );

			//_clean_spans_in_paragraph_range( paragraph_to_merge, paragraph_to_merge + 1 );
		}

		// reflow text in affected paragraph and reflow paragraphs.
		if ( paragraph_to_reflow != -1 )
		{
			_reflow_glyphs_in_paragraph( paragraph_to_reflow );
			_update_vertical_layout_of_all_paragraphs();
		}
	}

	void_c menu_element_text_c::_delete_selected_text()
	{
		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		sint32_c selection_index_start;
		sint32_c selection_index_end;
		get_selected_text_range( selection_index_start, selection_index_end );
		if ( selection_index_start < selection_index_end )
		{
			_delete_character_range( selection_index_start, selection_index_end - 1 );
			_cursor_index = selection_index_start;
			_selection_anchor_index_start = selection_index_start;
			_selection_anchor_index_end = selection_index_start;
			_text_is_modified = true;
			on_value_changed_preview.invoke( this );
		}
	}

	void_c menu_element_text_c::_get_character_information( sint32_c character_index, sint32_c * paragraph_index, sint32_c * span_index, sint32_c * line_index, boolean_c * is_at_end_of_virtual_line )
	{
		assert( _text_format_mode == menu_text_format_mode_e_plain );

		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			text_paragraph_c * paragraph = _paragraph_list[ i ];
			if ( _cursor_index >= paragraph->_character_start && _cursor_index <= paragraph->_character_end )
			{
				if ( paragraph_index != nullptr )
				{
					*paragraph_index = i;
				}

				if ( span_index != nullptr )
				{
					for ( sint32_c j = 0; j < paragraph->_span_list.get_length(); j++ )
					{
						text_span_c * span = paragraph->_span_list[ j ];
						if ( character_index >= span->_character_start && character_index <= span->_character_end )
						{
							*span_index = j;
							break;
						}
					}
				}

				if ( line_index != nullptr || is_at_end_of_virtual_line != nullptr )
				{
					for ( sint32_c j = 0; j < paragraph->_line_list.get_length(); j++ )
					{
						text_line_c * line = &paragraph->_line_list[ j ];
						if ( _cursor_index >= line->_character_start && _cursor_index <= line->_character_end )
						{
							if ( line_index != nullptr )
							{
								*line_index = paragraph->_line_index_base + j;
							}
							if ( is_at_end_of_virtual_line != nullptr )
							{
								*is_at_end_of_virtual_line = j - paragraph->_line_index_base < paragraph->_line_list.get_length() - 1;
							}
							break;
						}
					}
				}
			}
		}
	}

	sint32_c menu_element_text_c::_insert_new_paragraph( sint32_c at_character_index, string16_c const & plain_text )
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_editable );
		assert( _text_format_mode == menu_text_format_mode_e_plain ); // i haven't written logic yet to edit rich text.

		string16_c const & content = plain_text;
		sint32_c content_length = content.character_list.get_length() - 1; // get length of content (excluding null terminator).
		sint32_c content_length_inserted = 0;

		assert( at_character_index >= 0 && at_character_index <= _plain_text.character_list.get_length() - 1 ); // insertion point must within the range of current _value.

		text_paragraph_c * new_paragraph = new text_paragraph_c( this );
		new_paragraph->_resolve_style_reference();

		sint32_c result = -1; // will be set to index of inserted paragraph.
		sint32_c k = -1; // will be set to index of first paragraph that needs to be re laid out.

		if ( at_character_index == _plain_text.character_list.get_length() - 1 )
		{
			// insert new paragraph at the end, after terminating new line character.
			result = _paragraph_list.get_length();
			_paragraph_list.insert_at_end( new_paragraph );
			if ( content_length > 0 )
			{
				_plain_text.character_list.insert_range_at_index( at_character_index, content_length, content.character_list.get_internal_array() );
			}
			_plain_text.character_list.insert_at_index( _plain_text.character_list.get_length() - 1, L'\n' );
		}
		else
		{
			// insert new paragraph somewhere in the middle.
			for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
			{
				text_paragraph_c * paragraph = _paragraph_list[ i ];

				if ( at_character_index == paragraph->_character_start )
				{
					// no need to split a paragraph, just insert the new paragraph before the current paragraph.

					// insert changes into _plain_text.
					if ( content_length > 0 )
					{
						_plain_text.character_list.insert_range_at_index( at_character_index, content_length, content.character_list.get_internal_array() );
					}
					_plain_text.character_list.insert_at_index( at_character_index + content_length, L'\n' );
					content_length_inserted = content_length + 1;

					// process new_paragraph.
					new_paragraph->_character_start = at_character_index;
					new_paragraph->_character_end = at_character_index + content_length + 1; // +1 to account for new line character to close new_paragraph.
					_paragraph_list.insert_at_index( i, new_paragraph );

					//
					result = i;
					k = i + 1;
					break;
				}
				else if ( at_character_index > paragraph->_character_start && at_character_index <= paragraph->_character_end )
				{
					// split this paragraph.

					// insert changes into _plain_text.
					_plain_text.character_list.insert_at_index( at_character_index, L'\n' ); // insert new line to close current paragraph.
					if ( content_length > 0 )
					{
						_plain_text.character_list.insert_range_at_index( at_character_index + 1, content_length, content.character_list.get_internal_array() );
					}
					_plain_text.character_list.insert_at_index( at_character_index + 1 + content_length, L'\n' ); // insert new line to close new_paragraph.
					content_length_inserted = content_length + 2;

					// process current paragraph.
					sint32_c before_character_end = paragraph->_character_end;
					paragraph->_character_end = at_character_index; // this is the index of the new line that we inserted earlier to close this paragraph.
					paragraph->_text_layout_is_dirty = true; // needs to be laid out later.

					// process new_paragraph.
					new_paragraph->_style_reference.set_value( paragraph->_style_reference.get_value() );
					new_paragraph->_character_start = at_character_index + 1; // +1 to account for inserted new line to close current paragraph.
					new_paragraph->_character_end = at_character_index + 1 + content_length; // +1 to account for inserted new line to close current paragraph.
					_paragraph_list.insert_at_index( i + 1, new_paragraph );

					// process new_after_paragraph.
					text_paragraph_c * new_after_paragraph = new text_paragraph_c( this );
					new_after_paragraph->_character_start = new_paragraph->_character_end + 1; // +2, 1 to account for inserted new line to close out current paragraph, 1 to account for inserted new line to close out new_paragraph.
					new_after_paragraph->_character_end = before_character_end + content_length_inserted; // +2, 1 to account for inserted new line to close out current paragraph, 1 to account for inserted new line to close out new_paragraph.
					new_after_paragraph->_resolve_style_reference();
					_paragraph_list.insert_at_index( i + 2, new_after_paragraph );

					//
					result = i + 1;
					k = i + 3; // point to paragraph after new_after_paragraph.
					break;
				}
			}
		}

		// adjust character ranges of subsequent paragraphs that were offset by the insertion.
		if ( k >= 0 )
		{
			for ( ; k < _paragraph_list.get_length(); k++ )
			{
				text_paragraph_c * subsequent_paragraph = _paragraph_list[ k ];
				subsequent_paragraph->_character_start += content_length_inserted;
				subsequent_paragraph->_character_end += content_length_inserted;
			}
		}

		_text_layout_is_dirty = true;

		return result;
	}

	void_c menu_element_text_c::_on_local_box_modified()
	{
		vector32x2_c size = _local_box.get_size();
		if ( _laid_out_width != size.a )
		{
			_invalidate_glyph_flow_in_all_paragraphs();
			_laid_out_width = size.a;
		}
		else
		{
			_update_vertical_layout_of_all_paragraphs();
		}
	}

	void_c menu_element_text_c::_build_draw_list()
	{
		assert( _mother_control != nullptr );

		_draw_list.reset();

		if ( _is_showing_from_style == false || _is_showing == false || _local_color.d <= 0.0f || _plain_text.get_length() <= 1 )
		{
			return;
		}

		// get text style, the text element must have one in order to be rendered.
		menu_style_for_text_c const * text_style = _style_reference.get_value();
		if ( text_style == nullptr )
		{
			text_style = &global_engine_instance.interfaces.menu_style_manager->default_text_style;
		}

		// last moment text reflow.
		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		// state color and saturation.
		menu_state_e state = get_state();
		vector32x4_c element_color = text_style->state_list[ state ].get_expressed_color() * _local_color;
		float32_c element_saturation = text_style->state_list[ state ].saturation;

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

		sint32_c selection_index_start;
		sint32_c selection_index_end;
		get_selected_text_range( selection_index_start, selection_index_end );

		float32_c vertically_algined_y = 0.0f;
		menu_text_align_vertical_e text_align_vertical = get_style_text_align_vertical();
		if ( text_align_vertical == menu_text_align_vertical_e_center )
		{
			vertically_algined_y = ( _local_box.get_height() * 0.5f ) - ( get_content_height() * 0.5f );
		}
		else if ( text_align_vertical == menu_text_align_vertical_e_bottom )
		{
			vertically_algined_y = _local_box.get_height() - get_content_height();
		}

		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			menu_element_text_c::text_paragraph_c const * paragraph = _paragraph_list[ i ];

			float32_c paragraph_left = _local_box.minimum.a + _content_offset.a;
			float32_c paragraph_right = paragraph_left + _local_box.get_width();
			float32_c paragraph_top = _local_box.minimum.b + _content_offset.b + paragraph->_top + vertically_algined_y;
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
						if ( ( _text_edit_mode == menu_text_edit_mode_e_static_selectable || _text_edit_mode == menu_text_edit_mode_e_editable ) && selection_index_start != selection_index_end && selection_index_end > line->_character_start && selection_index_start < line->_character_end )
						{
							menu_element_text_c::text_glyph_c const * glyph_left = &paragraph->_glyph_list[ ops::math_maximum( selection_index_start, line->_character_start ) - paragraph->_character_start ];
							float32_c selection_left = line_left + glyph_left->_left;
							menu_element_text_c::text_glyph_c const * glyph_right = &paragraph->_glyph_list[ ops::math_minimum( selection_index_end, line->_character_end ) - 1 - paragraph->_character_start ];
							float32_c selection_right = line_left + glyph_right->_left + glyph_right->_horizontal_advance;
							float32_c selection_top = line_top;
							float32_c selection_bottom = line_bottom;

							vector32x4_c selection_color = global_engine_instance.interfaces.menu_style_manager->shared_colors[ menu_shared_color_e_field_normal_accent ].value;
							selection_color.d *= 0.5f;

							video_renderer_vertex_menu_c * vertex = vertex_list_for_selection.emplace_at_end();
							vertex->position.a = selection_left;
							vertex->position.b = selection_top;
							vertex->color = selection_color;

							vertex = vertex_list_for_selection.emplace_at_end();
							vertex->position.a = selection_left;
							vertex->position.b = selection_bottom;
							vertex->color = selection_color;

							vertex = vertex_list_for_selection.emplace_at_end();
							vertex->position.a = selection_right;
							vertex->position.b = selection_top;
							vertex->color = selection_color;

							vertex = vertex_list_for_selection.emplace_at_end();
							vertex->position.a = selection_right;
							vertex->position.b = selection_bottom;
							vertex->color = selection_color;
						}

						if ( _is_text_focused && _text_edit_mode != menu_text_edit_mode_e_static )
						{
							boolean_c do_cursor = false;
							float32_c cursor_left;
							vector32x4_c cursor_color = global_engine_instance.interfaces.menu_style_manager->shared_colors[ menu_shared_color_e_field_normal_accent ].value;
							if ( _cursor_is_at_end_of_virtual_line == false && _cursor_index >= line->_character_start && _cursor_index < line->_character_end )
							{
								menu_element_text_c::text_glyph_c const * laid_out_glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start ];
								cursor_left = line_left + laid_out_glyph->_left;
								do_cursor = true;
							}
							else if ( _cursor_is_at_end_of_virtual_line == true && _cursor_index == line->_character_end )
							{
								menu_element_text_c::text_glyph_c const * laid_out_glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start - 1 ];
								cursor_left = line_left + laid_out_glyph->_left + laid_out_glyph->_horizontal_advance;
								do_cursor = true;
							}
							if ( do_cursor == true )
							{
								cursor_left -= _cursor_width * 0.5f;
								float32_c cursor_right = cursor_left + _cursor_width;
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

						sint32_c glyph_start = line->_character_start - paragraph->_character_start;
						sint32_c glyph_end = line->_character_end - paragraph->_character_start;
						for ( sint32_c k = glyph_start; k < glyph_end; k++ )
						{
							char16_c character = _plain_text.character_list[ paragraph->_character_start + k ];
							menu_element_text_c::text_glyph_c const * laid_out_glyph = &paragraph->_glyph_list[ k ];
							assert( laid_out_glyph->_style != nullptr );
							vector32x4_c laid_out_glyph_color = laid_out_glyph->_style->color * element_color;
							if ( laid_out_glyph_color.d != 0.0f )
							{
								if ( ( character & 0xF000 ) != 0xF000 )
								{
									if ( laid_out_glyph->_glyph != nullptr )
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
											float32_c skew = laid_out_glyph->_style->skew;
											vector32x4_c parameters = vector32x4_c( laid_out_glyph->_style->skew, laid_out_glyph->_style->weight, laid_out_glyph->_style->softness, static_cast< float32_c >( k + paragraph->_character_start ) );

											video_renderer_vertex_menu_c * vertex = vertex_list_for_glyphs.emplace_at_end(); // top left.
											vertex->position.a = glyph_left - ( laid_out_glyph->_box.minimum.b * skew );
											vertex->position.b = glyph_top;
											vertex->position.c = 0.0f;
											vertex->texture.a = laid_out_glyph->_glyph->map.minimum.a;
											vertex->texture.b = laid_out_glyph->_glyph->map.minimum.b;
											vertex->texture.c = glyph_map_texture_slice_index;
											vertex->color = laid_out_glyph_color;
											vertex->parameters = parameters;

											vertex = vertex_list_for_glyphs.emplace_at_end(); // bottom left.
											vertex->position.a = glyph_left + ( laid_out_glyph->_box.maximum.b * skew );
											vertex->position.b = glyph_bottom;
											vertex->position.c = 0.0f;
											vertex->texture.a = laid_out_glyph->_glyph->map.minimum.a;
											vertex->texture.b = laid_out_glyph->_glyph->map.maximum.b;
											vertex->texture.c = glyph_map_texture_slice_index;
											vertex->color = laid_out_glyph_color;
											vertex->parameters = parameters;

											vertex = vertex_list_for_glyphs.emplace_at_end(); // top right.
											vertex->position.a = glyph_right - ( laid_out_glyph->_box.minimum.b * skew );
											vertex->position.b = glyph_top;
											vertex->position.c = 0.0f;
											vertex->texture.a = laid_out_glyph->_glyph->map.maximum.a;
											vertex->texture.b = laid_out_glyph->_glyph->map.minimum.b;
											vertex->texture.c = glyph_map_texture_slice_index;
											vertex->color = laid_out_glyph_color;
											vertex->parameters = parameters;

											vertex = vertex_list_for_glyphs.emplace_at_end(); // bottom right.
											vertex->position.a = glyph_right + ( laid_out_glyph->_box.maximum.b * skew );
											vertex->position.b = glyph_bottom;
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

											float32_c scale_to_size = laid_out_glyph->_style->size / entity_sprite->value.get_sprite()->font_size;

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

	stitch:
		if ( vertex_list_for_selection.get_length() > 0 )
		{
			_draw_list.append_rectangle_list( vertex_list_for_selection, global_engine_instance.interfaces.video_renderer_shader_manager->menu_ps_solid_color, nullptr );
		}

		if ( vertex_list_for_glyphs.get_length() > 0 )
		{
			_draw_list.append_rectangle_list( vertex_list_for_glyphs, global_engine_instance.interfaces.video_renderer_shader_manager->menu_ps_text, nullptr );
		}

		if ( vertex_list_for_cursor.get_length() > 0 )
		{
			_draw_list.append_rectangle_list( vertex_list_for_cursor, global_engine_instance.interfaces.video_renderer_shader_manager->menu_ps_solid_color, nullptr );
		}

		vertex_list_for_selection.remove_all();
		vertex_list_for_glyphs.remove_all();
		vertex_list_for_cursor.remove_all();

		_draw_list_is_dirty = false;
	}

	menu_element_text_c::menu_element_text_c()
		: menu_element_c()
		, _global_instance_list_node( this )
		, _style_reference()
		, _string_reference()
		, _text_format_mode( menu_text_format_mode_e_plain )
		, _text_edit_mode( menu_text_edit_mode_e_static )
		, _filter_mode( menu_text_filter_mode_e_none )
		, _filter_string()
		, _plain_text()
		, _text_is_modified( false )
		, _multi_line( true )
		, _word_wrap( true )
		, _character_limit( 0 )
		, _content_offset( 0.0f, 0.0f )
		, _cursor_index( 0 )
		, _cursor_is_at_end_of_virtual_line( false )
		, _text_selection_mode( menu_text_selection_mode_e_character )
		, _selection_anchor_index_start( 0 )
		, _selection_anchor_index_end( 0 )
		, _cursor_sticky_x( 0.0f )
		, _text_align_horizontal( menu_text_align_horizontal_e_inherit_from_style )
		, _text_align_vertical( menu_text_align_vertical_e_inherit_from_style )
		, _is_text_focused( false )
		, _cursor_width( 2.0f )
		, _cursor_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, _cursor_time( 0.0f )
		, _text_layout_is_dirty( false )
		, _laid_out_width( 0.0f )
		, _content_width( 0.0f )
		, _content_height( 0.0f )
		, _paragraph_list()
	{
		_global_list.insert_at_end( &_global_instance_list_node );
		_style_reference.on_refreshed.subscribe( this, &menu_element_text_c::_handle_style_reference_on_refreshed );
		_string_reference.on_refreshed.subscribe( this, &menu_element_text_c::_handle_string_reference_on_refreshed );
		clear_text();
	}

	menu_element_text_c::~menu_element_text_c()
	{
		_global_list.remove( &_global_instance_list_node );
		_paragraph_list.remove_and_delete_all();
	}

	void_c menu_element_text_c::update_animations( float32_c time_step )
	{
		menu_element_c::update_animations( time_step );

		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
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

	void_c menu_element_text_c::set_style_key( string8_c const & value )
	{
		_style_reference.set_key( value );
	}

	void_c menu_element_text_c::update_text_layout()
	{
		_reflow_glyphs_in_all_paragraphs_that_need_it();
	}

	menu_style_for_text_c::reference_c const & menu_element_text_c::get_style_reference() const
	{
		return _style_reference;
	}

	menu_style_for_text_c::reference_c & menu_element_text_c::get_style_reference()
	{
		return _style_reference;
	}

	string_c::reference_c const & menu_element_text_c::get_string_reference() const
	{
		return _string_reference;
	}

	string_c::reference_c & menu_element_text_c::get_string_reference()
	{
		return _string_reference;
	}

	menu_text_format_mode_e menu_element_text_c::get_text_format_mode() const
	{
		return _text_format_mode;
	}

	void_c menu_element_text_c::set_text_format_mode( menu_text_format_mode_e value )
	{
		if ( value == menu_text_format_mode_e_rich )
		{
			assert( _text_edit_mode == menu_text_edit_mode_e_static );
		}
		_text_format_mode = value;
	}

	menu_text_edit_mode_e menu_element_text_c::get_text_edit_mode() const
	{
		return _text_edit_mode;
	}

	void_c menu_element_text_c::set_text_edit_mode( menu_text_edit_mode_e value )
	{
		if ( _text_format_mode == menu_text_format_mode_e_rich )
		{
			assert( value == menu_text_edit_mode_e_static );
		}
		_text_edit_mode = value;
	}

	menu_text_filter_mode_e menu_element_text_c::get_text_filter_mode() const
	{
		return _filter_mode;
	}

	void_c menu_element_text_c::set_text_filter_mode( menu_text_filter_mode_e filter_mode, string16_c const & filter_string )
	{
		_filter_mode = filter_mode;
		if ( _filter_mode == menu_text_filter_mode_e_numeric_integer || _filter_mode == menu_text_filter_mode_e_numeric_real )
		{
			_multi_line = false;
			_character_limit = 31; // 30 digits with decimal point, and 1 optional negative sign.
		}
		else if ( _filter_mode == menu_text_filter_mode_e_custom )
		{
			_filter_string = filter_string;
		}
		clear_text();
	}

	void_c menu_element_text_c::clear_text()
	{
		if ( _plain_text == "\n" )
		{
			// we should be clear already.
			return;
		}

		_plain_text = "\n";
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			delete _paragraph_list[ i ];
		}
		_paragraph_list.remove_all();

		text_paragraph_c * paragraph = new text_paragraph_c( this );
		paragraph->_character_start = 0;
		paragraph->_character_end = 1;
		paragraph->_resolve_style_reference();
		_paragraph_list.insert_at_end( paragraph );

		_text_layout_is_dirty = true;
	}

	string16_c menu_element_text_c::get_plain_text() const
	{
		assert( _plain_text.character_list.get_length() >= 2 ); // plain text must at minimum be 2 long because it must end with a new line character and a null character.
		assert( _plain_text.character_list[ _plain_text.character_list.get_length() - 2 ] == '\n' );
		assert( _plain_text.character_list[ _plain_text.character_list.get_length() - 1 ] == '\0' );
		string16_c result;
		result = _plain_text;
		result.character_list.remove_at_index( result.character_list.get_length() - 2 ); // remove the terminating new line character.
		return result;
	}

	void_c menu_element_text_c::set_plain_text( string16_c const & value )
	{
		// clear current text.
		for ( sint32_c i = 0; i < _paragraph_list.get_length(); i++ )
		{
			delete _paragraph_list[ i ];
		}
		_paragraph_list.remove_all();
		_plain_text.character_list.remove_all();

		// scan through text character by character and copy it.
		// if we are set to single line and it contains a new line, then we will break upon encountering the new line.
		// if the length of the value is longer than our character limit, then we will break upon reaching the limit.
		sint32_c value_length = value.get_length();
		if ( _character_limit > 0 && value_length > _character_limit )
		{
			value_length = _character_limit;
		}
		for ( sint32_c i = 0; i < value_length; i++ )
		{
			char16_c c = value.character_list[ i ];
			_plain_text.character_list.insert_at_end( c );
			if ( c == '\n' && !_multi_line )
			{
				break;
			}
		}
		_plain_text.character_list.insert_at_end( '\n' ); // insert terminating new line (still needed even if in single line mode, to streamline hit detection logic and such).
		_plain_text.character_list.insert_at_end( '\0' ); // insert terminating null character.

		// put the new text value into a single paragraph, even if it contains new lines.
		text_paragraph_c * paragraph = new text_paragraph_c( this );
		paragraph->_character_start = 0;
		paragraph->_character_end = _plain_text.get_length();
		paragraph->_resolve_style_reference(); // also sets _text_layout_is_dirty to true.
		_paragraph_list.insert_at_end( paragraph );
		_text_layout_is_dirty = true;
	}

	void_c menu_element_text_c::set_rich_text( string8_c const & plain_text_with_markup )
	{
		assert( _text_format_mode == menu_text_format_mode_e_rich );

		clear_text();

		data_scribe_markup_c markup( '[' );
		if ( !markup.parse( plain_text_with_markup ) )
		{
			return;
		}

		// used to store stack state so we can recurse in a flat loop rather than recurse with the call stack.
		struct node_walker_c
		{
			data_scribe_markup_c::node_c const * node; // tag type node.
			data_scribe_markup_c::node_c const * next_node;
			text_paragraph_c * paragraph; // will be set if tag corresponds to a paragraph.
			text_span_c * span; // will be set if tag corresponds to a span.
		};

		// emtpy text string.
		_plain_text = string16_c();
		_text_layout_is_dirty = true;

		// create root level tag walker.
		core_list_c< node_walker_c > node_walker_stack;
		node_walker_c * node_walker = node_walker_stack.emplace_at_end();
		node_walker->node = markup.get_node( 0 );
		node_walker->next_node = node_walker->node->get_first_daughter();
		node_walker->paragraph = nullptr;
		node_walker->span = nullptr;

		// iterate through each tag recursively.
		data_scribe_markup_c::attribute_c const * attribute = nullptr;
		text_paragraph_c * paragraph = nullptr;
		while ( node_walker_stack.get_length() > 0 )
		{
			node_walker = &node_walker_stack[ node_walker_stack.get_length() - 1 ];
			if ( node_walker->next_node )
			{
				data_scribe_markup_c::node_c const * node = node_walker->next_node;
				node_walker->next_node = node->get_next_sister();

				if ( node->get_type() == data_scribe_markup_c::node_c::type_e_tag )
				{
					if ( node->get_tag_type() == data_scribe_markup_c::node_c::tag_type_e_open )
					{
						if ( node->get_value() == "p" || node->get_value() == "paragraph" )
						{
							// encountered paragraph tag.
							// paragraphs can only be at the root level.
							// ignore if not at root level.
							if ( node_walker_stack.get_length() == 1 )
							{
								paragraph = new text_paragraph_c( this );

								_paragraph_list.insert_at_end( paragraph );

								paragraph->_character_start = _plain_text.get_length();
								
								// load and resolve base style.
								attribute = node->find_attribute( "text_style_key" );
								if ( attribute )
								{
									paragraph->_style_reference.set_key( attribute->get_value() );
								}
								paragraph->_resolve_style_reference();

								// load style overrides.
								//paragraph->_text_style.load( node );

								node_walker_c * daughter_node_walker = node_walker_stack.emplace_at_end();
								daughter_node_walker->node = node;
								daughter_node_walker->next_node = daughter_node_walker->node->get_first_daughter();
								daughter_node_walker->paragraph = paragraph;
								daughter_node_walker->span = nullptr;
							}
						}
						else if ( node->get_value() == "s" || node->get_value() == "span" )
						{
							// encountered span tag.
							// spans can only be at sub root level.
							// ignore if at root level.
							if ( node_walker_stack.get_length() > 1 )
							{
								text_span_c * span = new text_span_c();

								if ( node_walker->paragraph )
								{
									span->_mother_paragraph = node_walker->paragraph;
									node_walker->paragraph->_span_list.insert_at_end( span );
								}
								else
								{
									span->_mother_span = node_walker->span;
									node_walker->span->_span_list.insert_at_end( span );
								}

								span->_character_start = _plain_text.get_length();

								// load and resolve base style.
								attribute = node->find_attribute( "text_style_key" );
								if ( attribute )
								{
									span->_style_reference.set_key( attribute->get_value() );
								}
								span->_resolve_style_reference();

								// load style overrides.
								//span->_text_style.load( node );

								node_walker_c * daughter_node_walker = node_walker_stack.emplace_at_end();
								daughter_node_walker->node = node;
								daughter_node_walker->next_node = daughter_node_walker->node->get_first_daughter();
								daughter_node_walker->paragraph = nullptr;
								daughter_node_walker->span = span;
							}
						}
					}
					else if ( node->get_tag_type() == data_scribe_markup_c::node_c::tag_type_e_open_close )
					{
						// encountered open close tag.
						if ( node->get_value() == "sprite" )
						{
							// encountered sprite tag.
							if ( paragraph != nullptr && paragraph->_entity_list.get_length() < 0x0FFF )
							{
								char16_c entity_index = 0xF000 & static_cast< uint16_c >( paragraph->_entity_list.get_length() );
								_plain_text.character_list.insert_at_end( entity_index );

								menu_text_entity_sprite_c * entity_sprite = new menu_text_entity_sprite_c();
								attribute = node->find_attribute( "key" );
								if ( attribute )
								{
									core_list_c< string8_c > splits;
									ops::string8_split_at_delimiter( attribute->get_value(), string8_c( mode_e_static, ":" ), splits );
									if ( splits.get_length() == 2 )
									{
										entity_sprite->value.set_sprite_set_resource( global_engine_instance.interfaces.resource_manager->load_sprite_set( string16_c( splits[ 0 ] ) ) );
										entity_sprite->value.set_sprite_name( splits[ 1 ] );
									}
								}
								paragraph->_entity_list.insert_at_end( entity_sprite );
							}
						}
						else if ( node->get_value() == "string" )
						{
							// encountered string reference, resolve the string reference and insert its value into the plain text.
							attribute = node->find_attribute( "key" );
							if ( attribute )
							{
								string_c const * string = global_engine_instance.interfaces.content_manager->find_string( attribute->get_value() );
								if ( string )
								{
									_plain_text += string->get_value();
								}
								else
								{
									_plain_text += '[';
									_plain_text += attribute->get_value();
									_plain_text += ']';
								}
							}
						}
					}
				}
				else if ( node->get_type() == data_scribe_markup_c::node_c::type_e_text )
				{
					// encountered text tag.
					// append its value to our plain text.
					_plain_text += node->get_value();
				}
			}
			else
			{
				// (virtually) encountered close tag.
				// virtual because we didn't encounter a close tag so much as we encountered the end of the sister list.
				if ( node_walker->paragraph )
				{
					_plain_text.character_list.insert_at_end( '\n' ); // insert new line character at end of paragraph.
					_plain_text.character_list.insert_at_end( '\0' );
					node_walker->paragraph->_character_end = _plain_text.get_length();
				}
				else
				{
					node_walker->span->_character_end = _plain_text.get_length();
				}
				node_walker_stack.remove_at_end();
			}
		}
	}

	void_c menu_element_text_c::append_line( string16_c const & plain_text )
	{
		_plain_text.character_list.remove_at_end(); // remove terminating null character.
		_plain_text.character_list.remove_at_end(); // remove terminating new line character.
		_plain_text.character_list.insert_range_at_end( plain_text.character_list.get_internal_array(), plain_text.character_list.get_length() - 1 ); // append new text excluding the terminating null character.
		_plain_text.character_list.insert_at_end( '\n' ); // append terminating new line character.
		_plain_text.character_list.insert_at_end( '\0' ); // append terminating null character.

		assert( _paragraph_list.get_length() > 0 );
		text_paragraph_c * paragraph = _paragraph_list[ _paragraph_list.get_length() - 1 ];
		paragraph->_text_layout_is_dirty = true;
		paragraph->_character_end += plain_text.get_length();

		_reflow_glyphs_in_paragraph( _paragraph_list.get_length() - 1 ); // instead of deferring layout, we'll just do it here.
		_update_vertical_layout_of_all_paragraphs();
	}

	void_c menu_element_text_c::append_paragraph( string16_c const & plain_text )
	{
		_plain_text.character_list.remove_at_end(); // remove terminating new line character.
		sint32_c new_paragraph_character_start = _plain_text.character_list.get_length();
		_plain_text.character_list.insert_range_at_end( plain_text.character_list.get_internal_array(), plain_text.character_list.get_length() - 1 ); // append new text excluding the terminating null character.
		_plain_text.character_list.insert_at_end( '\n' ); // append terminating new line character.
		_plain_text.character_list.insert_at_end( '\0' ); // append terminating null character.
		sint32_c new_paragraph_character_end = _plain_text.get_length();

		text_paragraph_c * new_paragraph = new text_paragraph_c( this );
		new_paragraph->_text_layout_is_dirty = true;
		new_paragraph->_character_start = new_paragraph_character_start;
		new_paragraph->_character_end = new_paragraph_character_end;
		_paragraph_list.insert_at_end( new_paragraph );

		_reflow_glyphs_in_paragraph( _paragraph_list.get_length() - 1 ); // instead of deferring layout, we'll just do it here.
		_update_vertical_layout_of_all_paragraphs();
	}

	sint32_c menu_element_text_c::get_character_limit() const
	{
		return _character_limit;
	}

	void_c menu_element_text_c::set_character_limit( sint32_c value )
	{
		_character_limit = value < 0 ? 0 : value;
		if ( _plain_text.get_length() > _character_limit )
		{
			_plain_text.character_list.set_length( _character_limit );
			_plain_text.character_list.insert_at_end( '\n' );
			_plain_text.character_list.insert_at_end( '\0' );
			_invalidate_glyph_flow_in_all_paragraphs();
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
			if ( _text_layout_is_dirty )
			{
				_reflow_glyphs_in_all_paragraphs_that_need_it();
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
			if ( _text_layout_is_dirty )
			{
				_reflow_glyphs_in_all_paragraphs_that_need_it();
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
			_invalidate_glyph_flow_in_all_paragraphs();
		}
	}

	boolean_c menu_element_text_c::get_multi_line() const
	{
		return _multi_line;
	}

	void_c menu_element_text_c::set_multi_line( boolean_c value )
	{
		if ( _multi_line != value )
		{
			if ( _multi_line && _plain_text.get_length() > 1 )
			{
				// if going from multi-line to single-line then we need to trim the text.
				assert( _paragraph_list.get_length() > 0 );
				for ( sint32_c i = 1; i < _paragraph_list.get_length(); i++ )
				{
					delete _paragraph_list[ i ];
				}
				_paragraph_list.remove_range_at_end( _paragraph_list.get_length() - 1 );
				text_paragraph_c * paragraph = _paragraph_list[ 0 ];
				assert( paragraph->_character_start == 0 );
				for ( sint32_c i = paragraph->_character_start; i <= paragraph->_character_end; i++ )
				{
					if ( _plain_text.character_list[ i ] == '\n' )
					{
						paragraph->_character_end = i + 1;
						_plain_text.character_list.remove_range_at_end( _plain_text.character_list.get_length() - paragraph->_character_end - 1 );
						_plain_text.character_list.insert_at_end( '\0' );
						break;
					}
				}
				if ( _cursor_index > paragraph->_character_end )
				{
					_cursor_index = paragraph->_character_end;
				}
				if ( _selection_anchor_index_start > paragraph->_character_end )
				{
					_selection_anchor_index_start = paragraph->_character_end;
				}
				if ( _selection_anchor_index_end > paragraph->_character_end )
				{
					_selection_anchor_index_end = paragraph->_character_end;
				}
			}
			_multi_line = value;
		}
	}

	vector32x4_c menu_element_text_c::get_cursor_color() const
	{
		return _cursor_color;
	}

	void_c menu_element_text_c::set_cursor_color( vector32x4_c const & value )
	{
		_cursor_color = value;
	}

	float32_c menu_element_text_c::get_cursor_width() const
	{
		return _cursor_width;
	}

	void_c menu_element_text_c::set_cursor_width( float32_c value )
	{
		_cursor_width = value;
	}

	menu_text_align_vertical_e menu_element_text_c::get_style_text_align_vertical() const
	{
		if ( _text_align_vertical != menu_text_align_vertical_e_inherit_from_style )
		{
			return _text_align_vertical;
		}
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->text_align_vertical_is_defined )
			{
				return _style_reference.get_value()->text_align_vertical;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.text_align_vertical_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.text_align_vertical;
	}

	menu_text_align_horizontal_e menu_element_text_c::get_style_text_align_horizontal() const
	{
		if ( _text_align_horizontal != menu_text_align_horizontal_e_inherit_from_style )
		{
			return _text_align_horizontal;
		}
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->text_align_horizontal_is_defined )
			{
				return _style_reference.get_value()->text_align_horizontal;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.text_align_horizontal_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.text_align_horizontal;
	}

	float32_c menu_element_text_c::get_style_paragraph_spacing() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->paragraph_spacing_is_defined )
			{
				return _style_reference.get_value()->paragraph_spacing;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.paragraph_spacing_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.paragraph_spacing;
	}

	float32_c menu_element_text_c::get_style_line_spacing() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->line_spacing_is_defined )
			{
				return _style_reference.get_value()->line_spacing;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.line_spacing_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.line_spacing;
	}

	float32_c menu_element_text_c::get_style_glyph_spacing() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->glyph_spacing_is_defined )
			{
				return _style_reference.get_value()->glyph_spacing;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.glyph_spacing_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.glyph_spacing;
	}

	resource_file_font_c * menu_element_text_c::get_style_font() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->font_is_defined )
			{
				assert( _style_reference.get_value()->font.is_reference_set_and_loaded() );
				return _style_reference.get_value()->font;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.font_is_defined );
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.font.is_reference_set_and_loaded() );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.font;
	}

	vector32x4_c menu_element_text_c::get_style_color() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->shared_color_is_defined && _style_reference.get_value()->shared_color.get_value() )
			{
				return _style_reference.get_value()->shared_color.get_value()->value;
			}
			else if ( _style_reference.get_value()->color_is_defined )
			{
				return _style_reference.get_value()->color;
			}
		}
		if ( global_engine_instance.interfaces.menu_style_manager->default_text_style.shared_color_is_defined )
		{
			assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.shared_color.get_value() );
			return global_engine_instance.interfaces.menu_style_manager->default_text_style.shared_color.get_value()->value;
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.color_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.color;
	}

	float32_c menu_element_text_c::get_style_size() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->size_is_defined )
			{
				return _style_reference.get_value()->size;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.size_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.size;
	}

	float32_c menu_element_text_c::get_style_skew() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->skew_is_defined )
			{
				return _style_reference.get_value()->skew;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.skew_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.skew;
	}

	float32_c menu_element_text_c::get_style_weight() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->weight_is_defined )
			{
				return _style_reference.get_value()->weight;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.weight_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.weight;
	}

	float32_c menu_element_text_c::get_style_softness() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->softness_is_defined )
			{
				return _style_reference.get_value()->softness;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.softness_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.softness;
	}

	float32_c menu_element_text_c::get_style_underline() const
	{
		if ( _style_reference.get_value() )
		{
			if ( _style_reference.get_value()->underline_is_defined )
			{
				return _style_reference.get_value()->underline;
			}
		}
		assert( global_engine_instance.interfaces.menu_style_manager->default_text_style.underline_is_defined );
		return global_engine_instance.interfaces.menu_style_manager->default_text_style.underline;
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
		return _content_width;
	}

	float32_c menu_element_text_c::get_content_height() const
	{
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
		_selection_anchor_index_start = value;
		_selection_anchor_index_end = value;
	}

	boolean_c menu_element_text_c::has_selected_text()
	{
		return _cursor_index != _selection_anchor_index_start || _cursor_index != _selection_anchor_index_end;
	}

	void_c menu_element_text_c::get_selected_text_range( sint32_c & selection_index_start, sint32_c & selection_index_end ) const
	{
		selection_index_start = _cursor_index < _selection_anchor_index_start ? _cursor_index : _selection_anchor_index_start;
		selection_index_end = _cursor_index > _selection_anchor_index_end ? _cursor_index : _selection_anchor_index_end;
	}

	void_c menu_element_text_c::handle_on_character_focus_gain()
	{
		_text_is_modified = false;
		_is_text_focused = true;
	}

	void_c menu_element_text_c::handle_on_character_focus_lose()
	{
		if ( _text_is_modified )
		{
			_text_is_modified = false;
			on_value_changed_commit.invoke( this );
		}
		_is_text_focused = false;
	}

	boolean_c menu_element_text_c::handle_input( input_event_c * input_event )
	{
		if ( _text_edit_mode == menu_text_edit_mode_e_static )
		{
			return false;
		}

		if ( input_event->type == input_event_c::type_e_character )
		{
			if ( _text_edit_mode == menu_text_edit_mode_e_editable )
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
				if ( _text_edit_mode == menu_text_edit_mode_e_editable )
				{
					input_delete_back();
					return true;
				}
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_delete )
			{
				if ( _text_edit_mode == menu_text_edit_mode_e_editable )
				{
					input_delete_fore();
					return true;
				}
			}
			else if ( input_event->keyboard_key == input_keyboard_key_e_enter )
			{
				if ( _text_edit_mode == menu_text_edit_mode_e_editable && _multi_line == true )
				{
					input_return( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0 );
					return true;
				}
			}
		}
		else if ( input_event->type == input_event_c::type_e_mouse_key_pressed )
		{
			assert( _mother_control != nullptr );
			vector32x2_c local_mouse_position = _mother_control->transform_global_point_to_local_point( input_event->menu_global_mouse_position ); 
			if ( input_event->mouse_key == input_mouse_key_e_left && ( _text_edit_mode == menu_text_edit_mode_e_static_selectable || _text_edit_mode == menu_text_edit_mode_e_editable ) )
			{
				if ( ( input_event->modifier_keys_state[ input_modifier_key_e_shift ] & input_key_state_bit_e_on ) != 0 )
				{
					_text_selection_mode = menu_text_selection_mode_e_character;
					_place_cursor_at_local_point( local_mouse_position, true );
				}
				else
				{
					if ( input_event->mouse_key_multi_click_count == 1 )
					{
						_text_selection_mode = menu_text_selection_mode_e_character;
					}
					else if ( input_event->mouse_key_multi_click_count == 2 )
					{
						_text_selection_mode = menu_text_selection_mode_e_word;
					}
					else if ( input_event->mouse_key_multi_click_count == 3 )
					{
						_text_selection_mode = menu_text_selection_mode_e_paragraph;
					}
					_place_cursor_at_local_point( local_mouse_position, false );
					_cursor_time = 0.0f;
				}
				//_input_lmb_down = true;
				return true;
			}
			else if ( input_event->mouse_key == input_mouse_key_e_right )
			{
				// if the * is not in the selected text, then move the cursor to the point under the *.
				_place_cursor_at_local_point( local_mouse_position, false );
				// open context menu.
				return true;
			}
		}
		//else if ( input_event->event_type == input_event_type_e_mouse_key_released )
		//{
		//	//if ( input_event->mouse_key == input_mouse_key_e_left )
		//	//{
		//	//	_input_lmb_down = false;
		//	//}
		//}
		else if ( input_event->type == input_event_c::type_e_mouse_move )
		{
			if ( ( input_event->mouse_keys_state[ input_mouse_key_e_left ] & input_key_state_bit_e_on ) != 0 )
			{
				assert( _mother_control != nullptr );
				vector32x2_c local_mouse_position = _mother_control->transform_global_point_to_local_point( input_event->menu_global_mouse_position ); 
				_place_cursor_at_local_point( local_mouse_position, true );
				return true;
			}
		}

		return false;
	}

	void_c menu_element_text_c::input_character( char16_c character )
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_editable );

		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		_cursor_time = 0.0f;

		if ( has_selected_text() )
		{
			_delete_selected_text();
		}

		string16_c value_string;
		value_string += character;

		_insert_plain_text_at_cursor( value_string );
	}

	void_c menu_element_text_c::input_delete_fore()
	{
		_cursor_time = 0.0f;

		if ( has_selected_text() )
		{
			_delete_selected_text();
			on_value_changed_preview.invoke( this );
		}
		else if ( _cursor_index < _plain_text.character_list.get_length() - 2 )
		{
			_selection_anchor_index_start = _cursor_index;
			_selection_anchor_index_end = _cursor_index;

			_delete_character_range( _cursor_index, _cursor_index );

			_text_is_modified = true;

			_cursor_is_at_end_of_virtual_line = false;

			_update_cursor_sticky_x();

			on_value_changed_preview.invoke( this );
		}
	}

	void_c menu_element_text_c::input_delete_back()
	{
		_cursor_time = 0.0f;

		if ( has_selected_text() )
		{
			_delete_selected_text();
			_text_is_modified = true;
			on_value_changed_preview.invoke( this );
		}
		else if ( _cursor_index > 0 )
		{
			_cursor_index--;
			_selection_anchor_index_start = _cursor_index;
			_selection_anchor_index_end = _cursor_index;
			_delete_character_range( _cursor_index, _cursor_index );
			_cursor_is_at_end_of_virtual_line = false;
			_update_cursor_sticky_x();
			_text_is_modified = true;
			on_value_changed_preview.invoke( this );
		}
	}

	void_c menu_element_text_c::input_return( boolean_c shift )
	{
		_cursor_time = 0.0f;

		if ( _multi_line )
		{
			_delete_selected_text();
			_insert_new_paragraph( _cursor_index, string16_c( mode_e_static, L"\n" ) );
			_text_is_modified = true;
			on_value_changed_preview.invoke( this );
		}
		else
		{
			if ( _text_is_modified )
			{
				_text_is_modified = false;
				on_value_changed_commit.invoke( this );
			}
		}
	}

	void_c menu_element_text_c::input_left( boolean_c shift, boolean_c ctrl )
	{
		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		_cursor_time = 0.0f;

		if ( _cursor_index > 0 )
		{
			if ( ctrl )
			{
				if ( _plain_text.character_list[ _cursor_index - 1 ] == '\n' )
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
				sint32_c selection_start;
				sint32_c selection_end;
				get_selected_text_range( selection_start, selection_end );
				if ( shift == false && selection_start < selection_end )
				{
					_cursor_index = _selection_anchor_index_start; // snap cursor to start of selection.
				}
				else
				{
					_cursor_index--;
					//_cursor_is_at_end_of_virtual_line = false;

					//if ( _cursor_is_at_end_of_virtual_line )
					//{
					//	_cursor_index--;
					//	_cursor_is_at_end_of_virtual_line = false;
					//}
					//else
					//{
					//	sint32_c block_index;
					//	sint32_c line_index;
					//	sint32_c column_index;
					//	get_character_information( _cursor_index, &block_index, nullptr, &line_index, &column_index, nullptr );
					//	text_paragraph_c * block = _paragraph_list[block_index];
					//	text_line_c * laid_out_line = &block->formatted_line_list[line_index - block->formatted_line_index_base];
					//	if ( _cursor_index == laid_out_line->character_start && line_index > block->formatted_line_index_base )
					//	{
					//		_cursor_is_at_end_of_virtual_line = true;
					//	}
					//	else
					//	{
					//		_cursor_index--;
					//	}
					//}
				}
			}
		}

		if ( shift == false )
		{
			_selection_anchor_index_start = _cursor_index;
			_selection_anchor_index_end = _cursor_index;
		}

		_update_cursor_sticky_x();
	}

	void_c menu_element_text_c::input_right( boolean_c shift, boolean_c ctrl )
	{
		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
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
				if ( _plain_text.character_list[ _cursor_index ] == '\n' )
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
				sint32_c selection_start;
				sint32_c selection_end;
				get_selected_text_range( selection_start, selection_end );
				if ( shift == false && selection_start < selection_end )
				{
					_cursor_index = selection_end; // snap cursor to end of selection.
				}
				else
				{
					_cursor_index++;
					_cursor_is_at_end_of_virtual_line = false;

					//if ( _cursor_is_at_end_of_virtual_line )
					//{
					//	_cursor_is_at_end_of_virtual_line = false;
					//}
					//else
					//{
					//	_cursor_index++;
					//	sint32_c block_index;
					//	sint32_c line_index;
					//	sint32_c column_index;
					//	get_character_information( _cursor_index, &block_index, nullptr, &line_index, &column_index, nullptr );
					//	text_paragraph_c * block = _paragraph_list[block_index];
					//	text_line_c * laid_out_line = &block->formatted_line_list[line_index - block->formatted_line_index_base];
					//	if ( _cursor_index == laid_out_line->character_start && line_index - block->formatted_line_index_base > 0 )
					//	{
					//		_cursor_is_at_end_of_virtual_line = true;
					//	}
					//}
				}
			}
		}

		if ( shift == false )
		{
			_selection_anchor_index_start = _cursor_index;
			_selection_anchor_index_end = _cursor_index;
		}

		_update_cursor_sticky_x();
	}

	void_c menu_element_text_c::input_up( boolean_c shift )
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_static_selectable || _text_edit_mode == menu_text_edit_mode_e_editable );

		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		_cursor_time = 0.0f;

		sint32_c line_index;
		sint32_c selection_index_start;
		sint32_c selection_index_end;
		get_selected_text_range( selection_index_start, selection_index_end );
		if ( shift == false && selection_index_end > selection_index_start ) // snap cursor to line above start of selection.
		{
			_get_character_information( selection_index_start, nullptr, nullptr, &line_index, nullptr );
		}
		else
		{
			_get_character_information( _cursor_index, nullptr, nullptr, &line_index, nullptr );
		}
		line_index--;
		_place_cursor_at_line_index( line_index, shift );
	}

	void_c menu_element_text_c::input_down( boolean_c shift )
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_static_selectable || _text_edit_mode == menu_text_edit_mode_e_editable );

		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		_cursor_time = 0.0f;

		sint32_c line_index;
		sint32_c selection_index_start;
		sint32_c selection_index_end;
		get_selected_text_range( selection_index_start, selection_index_end );
		if ( shift == false && selection_index_end > selection_index_start ) // snap cursor to line below end of selection.
		{
			_get_character_information( selection_index_end, nullptr, nullptr, &line_index, nullptr );
		}
		else
		{
			_get_character_information( _cursor_index, nullptr, nullptr, &line_index, nullptr );
		}
		line_index++;
		_place_cursor_at_line_index( line_index, shift );
	}

	void_c menu_element_text_c::input_home( boolean_c shift )
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_static_selectable || _text_edit_mode == menu_text_edit_mode_e_editable );

		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		_cursor_time = 0.0f;

		sint32_c cursor_paragraph_index;
		sint32_c cursor_line_index;
		_get_character_information( _cursor_index, &cursor_paragraph_index, nullptr, &cursor_line_index, nullptr );
		if ( _cursor_is_at_end_of_virtual_line )
		{
			cursor_line_index--;
		}

		text_paragraph_c * cursor_paragraph = _paragraph_list[ cursor_paragraph_index ];
		text_line_c * cursor_line = &cursor_paragraph->_line_list[ cursor_line_index - cursor_paragraph->_line_index_base ];

		// count how much white space is at start of line.
		sint32_c space_count = 0;
		for ( sint32_c i = cursor_line->_character_start; i < cursor_line->_character_end; i++ )
		{
			char16_c character = _plain_text.character_list[ i ];
			if ( character == ' ' || character == '\t' )
			{
				space_count++;
			}
			else
			{
				break;
			}
		}

		// toggle location of cursor to start of text content or start of line.
		if ( space_count == 0 || _cursor_index - cursor_line->_character_start == space_count )
		{
			// move character to start of line.
			_cursor_index = cursor_line->_character_start;
			_cursor_is_at_end_of_virtual_line = false;
		}
		else
		{
			// move character to start of text content.
			_cursor_index = cursor_line->_character_start + space_count;
			if ( cursor_line_index - cursor_paragraph->_line_index_base < cursor_paragraph->_line_list.get_length() - 1 )
			{
				_cursor_is_at_end_of_virtual_line = _cursor_index == cursor_line->_character_end;
			}
			else
			{
				_cursor_is_at_end_of_virtual_line = false;
			}
		}

		_update_cursor_sticky_x();

		if ( shift == false )
		{
			_selection_anchor_index_start = _cursor_index;
			_selection_anchor_index_end = _cursor_index;
		}
	}

	void_c menu_element_text_c::input_end( boolean_c shift )
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_static_selectable || _text_edit_mode == menu_text_edit_mode_e_editable );

		if ( _text_layout_is_dirty )
		{
			_reflow_glyphs_in_all_paragraphs_that_need_it();
		}

		_cursor_time = 0.0f;

		if ( _cursor_is_at_end_of_virtual_line )
		{
			return;
		}

		sint32_c paragraph_index = _get_paragraph_index_at_character_index( _cursor_index );
		text_paragraph_c * paragraph = _paragraph_list[ paragraph_index ];
		sint32_c line_index = paragraph->get_line_index( _cursor_index );
		text_line_c * line = &paragraph->_line_list[ line_index ];
		_cursor_index = line->_character_end;
		_cursor_is_at_end_of_virtual_line = line_index < paragraph->_line_list.get_length() - 1;
		if ( _cursor_is_at_end_of_virtual_line )
		{
			text_glyph_c * glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start - 1 ];
			_cursor_sticky_x = line->_left + glyph->_left + glyph->_horizontal_advance;
		}
		else
		{
			_cursor_index--; // exclude terminating new line character.
			text_glyph_c * glyph = &paragraph->_glyph_list[ _cursor_index - paragraph->_character_start ];
			_cursor_sticky_x = line->_left + glyph->_left;
		}

		if ( shift == false )
		{
			_selection_anchor_index_start = _cursor_index;
			_selection_anchor_index_end = _cursor_index;
		}
	}

	boolean_c menu_element_text_c::copy_text_to_clip_board()
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_static_selectable || _text_edit_mode == menu_text_edit_mode_e_editable );

		sint32_c selection_index_start;
		sint32_c selection_index_end;
		get_selected_text_range( selection_index_start, selection_index_end );
		if ( selection_index_start < selection_index_end )
		{
			sint32_c character_start;
			sint32_c character_end;
			get_selected_text_range( character_start, character_end );
			string16_c clip = ops::string16_sub_string( _plain_text, character_start, character_end - character_start );
			return global_engine_instance.interfaces.input_manager->clip_board_set_plain_text( clip );
		}
		return false;
	}

	boolean_c menu_element_text_c::paste_text_from_clip_board()
	{
		assert( _text_edit_mode == menu_text_edit_mode_e_editable );

		string16_c clip;
		if ( global_engine_instance.interfaces.input_manager->clip_board_get_plain_text( clip ) )
		{
			sint32_c selection_index_start;
			sint32_c selection_index_end;
			get_selected_text_range( selection_index_start, selection_index_end );
			if ( selection_index_start < selection_index_end )
			{
				_delete_character_range( selection_index_start, selection_index_end - 1 );
				_cursor_index = selection_index_start;
				_selection_anchor_index_start = selection_index_start;
				_selection_anchor_index_end = selection_index_start;
			}
			_insert_plain_text_at_cursor( clip );
			return true;
		}
		return false;
	}

}
