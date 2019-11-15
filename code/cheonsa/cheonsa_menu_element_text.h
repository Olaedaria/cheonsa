#pragma once

#include "cheonsa_menu_element.h"
#include "cheonsa_resource_file_font.h"
#include "cheonsa_input_manager.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_core_linked_list.h"
#include "cheonsa_sprite.h"
#include "cheonsa_menu_style_for_text.h"
#include "cheonsa_menu_style_for_text_glyph.h"

namespace cheonsa
{

	enum menu_text_format_mode_e
	{
		menu_text_format_mode_e_plain, // all text uses the same style, does not support rich formatting.
		menu_text_format_mode_e_rich, // text supports rich formatting. currently, does not work with editable 
	};

	enum menu_text_edit_mode_e
	{
		menu_text_edit_mode_e_static, // text is static, can't be interacted with by the user.
		menu_text_edit_mode_e_static_selectable, // text is static, but can be selected and copied by the user.
		menu_text_edit_mode_e_editable, // text is editable.
	};

	// defines which characters are allowed to be inputted into a text element.
	enum menu_text_filter_mode_e
	{
		menu_text_filter_mode_e_none, // text input will not filter input and will accept all displayable characters.
		menu_text_filter_mode_e_numeric_integer, // text input will be forced to single line mode and will only accept numbers, pressing '-' will toggle negative sign at start of the string, pressing '+' will remove negative sign at start of string.
		menu_text_filter_mode_e_numeric_real, // text input will be forced to single line mode and only accept numbers and at most one decimal point, pressing '-' will toggle negative sign at start of string, pressing '+' will remove negative sign at start of string.
		menu_text_filter_mode_e_custom, // text input will only accept characters defined by the filter string.
	};

	// text element.
	//
	// for now, only plain text mode supports editing by the user.
	//
	// usage:
	// when you create a menu_element_text_c instance, call the following functions in the following order:
	// 1. call set_mode() to set plain text mode or rich text mode.
	// 2. optionally, call set_style_key() to set the base text style, which is the style that will be used if no style is explicitly defined by rich text markup.
	// 3. call set_plain_text() if plain text mode was set or set_rich_text() if rich text mode was set.
	//
	// rich text uses the data_scribe_markup_c class to parse markup, so it's xml-like.
	// rich text markup uses square brackets instead of angled brackets.
	//
	// [p style="p"]text [s style="p_bold"]with [s style="p_itallic"]nested[/] styles[/] and [string key="player_name"].[/]
	// "p" or "paragraph" tag is paragraph, all text that you want to display must be contained in these, any number can be contained at root, they can not be nested.
	// "s" or "span" tag is span, must be nested another paragraph or sprite tag.
	// "style" attribute is a text style key.
	//
	// "sprite" tag is reference to game defined sprite.
	// "key" attribute is key of sprite.
	//
	// "string" tag is reference to game defined string.
	// "key" attribute is key of string.
	//
	// [p text_style_key="" font="" size="" weight="" skew="" color=""][/]
	// paragraph.
	// [s][/]
	// span.
	class menu_element_text_c
		: public menu_element_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "element_text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		static core_linked_list_c< menu_element_text_c * > _global_list;
		static void_c invalidate_glyph_layout_of_all_instances(); // invalidates lay out of all text elements, this also causes sprite references and string references to be updated.
		static void_c invalidate_glyph_layout_of_all_instances_with_font( resource_file_font_c * font ); // invalidates lay out of all text elements that have paragraphs that reference font.

	public:
		// once text style lay out has been calculated for a line of text, it is remembered in one of these.
		// does not contain the list of laid out glyphs, those are stored in the paragraph that the line is also stored in.
		class text_line_c
		{
		public:
			sint32_c _character_start; // index of first character within _plain_text that belongs to this formatted line.
			sint32_c _character_end; // index of last character within _plain_text that belongs to this formatted line.
			float32_c _width; // total measured width of the contents of this line, which is used to calculate _left base on horizontal text alignment.
			float32_c _left; // offset of the left edge of this line from the left edge of the paragraph or text element, after taking into account the horizontal text alignment.
			float32_c _top; // offset from top edge of mother paragraph to top edge of this line. this includes style line spacing.
			float32_c _ascender; // largest (positive) ascender in the line. base line is located at _top + _ascender.
			float32_c _descender; // largest (negative) descender in the line. bottom edge of line is located at _top + _ascender - _descender.

		public:
			text_line_c();

			text_line_c & operator = ( text_line_c const & other );

			void_c _update_horizontal_layout( menu_text_align_horizontal_e text_align_horizontal, float32_c element_width );

		};

		// stores layout of a glyph (or entity) within a paragraph.
		// glyphs are stored in their own list in the text element.
		// there is always 1 glyph allocated for each character in the text element's _plain_text.
		// for a given character, some times a sprite or string entity is used in place of the glyph, in which case the glyph is ignored even though it is still allocated.
		class text_glyph_c
		{
		public:
			glyph_c const * _glyph; // pointer to the glyph in the font that this formatted glyph is displaying, and for which we hold a reference count to. this can not be nullptr.
			menu_style_for_text_glyph_c const * _style; // references a style in the style cache.
			box32x2_c _box; // offset of this laid out glyph's quad mesh geometry (including margin for SDF), relative to the laid out line's origin.
			float32_c _left; // horizontal offset of this laid out glyph's origin relative to the left edge of the laid out line's origin or baseline. used by animated sprites, which are drawn relative to this origin. vertical origin can be calculated from the laid out line's _top and the laid out paragraph's _top.
			float32_c _ascender; // this glyph's font's ascender in pixels, positive values are above the baseline, negative values are below the baseline, which is backwrds from cheonsa's 2d coordinate system, so just be sure to negate it if needed.
			float32_c _descender; // this glyph's font's descender in pixels, positive values are above the baseline, negative values are below the baseline, which is backwards from cheonsa's 2d coordinate system, so just be sure to negate it if needed.
			float32_c _horizontal_advance; // this glyph's horizontal advance towards the right in pixels, including extra glyph spacing.

		public:
			text_glyph_c();
			text_glyph_c & operator = ( text_glyph_c const & other );

		};

		class text_paragraph_c;

		// a span defines a text style to apply to a range of characters in the text element.
		// the last format span in a paragraph contains the paragraph's terminating new line character, however the new line character is ignored when doing hit detection.
		class text_span_c
		{
		public:
			text_paragraph_c * _mother_paragraph; // the immediate mother of this span, if this span's mother is a paragraph.
			text_span_c * _mother_span; // the immediate mother of this span, if this span's mother is a span.
			core_list_c< text_span_c * > _span_list; // the immediate daughters of this span.
			menu_element_text_c::text_span_c const * _find_span( sint32_c with_start_character_index ) const;

			sint32_c _character_start; // index of first character within _raw_text that is formatted by this span.
			sint32_c _character_end; // index of last character within _raw_text that is formatted by this span, including the terminating new line character of the paragraph (if this span is the last span in the paragraph). in the case that the paragraph only consists of the new line character, then _character_end == _character_start.

			menu_style_for_text_c::reference_c _style_reference; // the format to apply to the range of characters defined by this span. can be left unset, which means that the style for this span will fall back to using the _style_reference defined in the text element.
			void_c _resolve_style_reference(); // resolves style reference for this span and all daughter spans.

		public:
			text_span_c();

			text_span_c & operator = ( text_span_c const & other );

			resource_file_font_c * get_style_font() const; // goes up the inheritance tree to find the font for this span.
			vector32x4_c get_style_color() const; // goes up the inheritance tree to find the color for this span.
			float32_c get_style_size() const; // goes up the inheritance tree to find the size for this span.
			float32_c get_style_skew() const; // goes up the inheritance tree to find the skew for this span.
			float32_c get_style_weight() const; // goes up the inheritance tree to find the weight for this span.
			float32_c get_style_softness() const; // goes up the inheritance tree to find the softness for this span.
			float32_c get_style_underline() const; // goes up the inheritance tree to find the underline for this span.
			float32_c get_style_glyph_spacing() const; // goes up the inheritance tree to find the glyph spacing for this span.
			menu_style_for_text_glyph_c get_style() const;

		};

		// a paragraph is a section or block of text that shares the same horizontal text alignment.
		// a paragraph must always end with a new-line character, even if the paragraph is otherwise empty of text.
		// a paragraph may also contain more in-line new-line characters in addition to the one at the end, these will create new lines in the paragraph without breaking into a new paragraph.
		// a paragraph contains a list of spans, which defines ranges of text within the block to apply different formats to.
		// - a paragraph must always contain at least one span.
		// the last span in the paragraph should not enclose the paragraph's terminating new line character.
		class text_paragraph_c
		{
		public:
			menu_element_text_c * _mother_element_text; // the mother text element that owns this paragraph.

			sint32_c _character_start; // index of first character within _plain_text that belongs to this paragraph.
			sint32_c _character_end; // index of last character within _plain_text that belongs to this paragraph, including this paragraph's terminating new line character. in the case that the paragraph only contains the new line character, then _character_end == _character_start.

			menu_style_for_text_c::reference_c _style_reference; // a reference of the text style to apply to text in this paragraph. if it is not defined, then the _style_reference from the text element will be used.
			void_c _resolve_style_reference(); // resolves style reference for this paragraph and all daughter spans. sets _text_layout_is_dirty to true.

			core_list_c< menu_text_entity_c * > _entity_list; // all of the entities inlined in the text of this paragraph.

			core_list_c< text_span_c * > _span_list; // all of the root level spans in this paragraph. will always be empty if in plain text mode (so that we don't have to worry about maintaining them, because only plain text mode can be edited).
			text_span_c const * _find_span( sint32_c with_start_character_index ) const;

			float32_c _top; // offset from top of document to top edge of this paragraph after taking into account vertical text alignment of the text element.
			float32_c _content_width; // width of the widest line in this paragraph.
			float32_c _content_height; // height of all the lines in this paragraph.
			sint32_c _line_index_base; // the line index of the first line in this paragrpah relative to all of the lines in the whole document.
			core_list_c< text_line_c > _line_list; // all of the lines in this paragraph, this can be a mix of real and virtual (broken by automatic word wrap algorithm) lines of text.
			core_list_c< text_glyph_c > _glyph_list; // all of the laid out glyphs in this paragraph.

			boolean_c _text_layout_is_dirty; // will be set to true if the glyphs in this paragraph needs to be reflowed.

		public:
			text_paragraph_c( menu_element_text_c * mother_element_text );
			~text_paragraph_c();

			void_c _delete_character_range( sint32_c start, sint32_c end ); // deletes range of characters defined by start and end, inclusive (meaning that if start == end then 1 character will be deleted).

			sint32_c get_line_index( sint32_c character_index );

			menu_text_align_horizontal_e get_style_text_align_horizontal() const; // goes up the inheritance tree to find the horizontal text align for this paragraph.
			float32_c get_style_paragraph_spacing() const;
			float32_c get_style_line_spacing() const;
			float32_c get_style_glyph_spacing() const;
			resource_file_font_c * get_style_font() const; // goes up the inheritance tree to find the font for this paragraph.
			vector32x4_c get_style_color() const; // goes up the inheritance tree to find the color for this paragraph.
			float32_c get_style_size() const; // goes up the inheritance tree to find the size for this paragraph.
			float32_c get_style_skew() const; // goes up the inheritance tree to find the skew for this paragraph.
			float32_c get_style_weight() const; // goes up the inheritance tree to find the weight for this paragraph.
			float32_c get_style_softness() const; // goes up the inheritance tree to find the softness for this paragraph.
			float32_c get_style_underline() const; // goes up the inheritance tree to find the underline for this paragraph.
			menu_style_for_text_glyph_c get_style() const;

		};

		struct character_information_c
		{
			sint32_c character_index; // the character that we want information on.
			sint32_c paragraph_index; // paragraph within the text element's paragraph list that character is in.
			sint32_c span_index; // span within paragraph's span list that the character is in, or -1 if character is not in a span.
			sint32_c line_index; // line within the paragraph's line list that the character is in.
			boolean_c is_at_end_of_virtual_line; // will be true if the cursor index is at the end of a virtual line and at the start of another line. this means that the cursor could be on line_index or it could be on line_index - 1.
		};

	private:
		friend class menu_render_procedure_c;

		string16_c _plain_text; // plain text without markup. if set_rich_text() is called, markup is stripped and this is the resulting text.

		core_linked_list_c< menu_element_text_c * >::node_c _global_instance_list_node;

		menu_style_for_text_c::reference_c _style_reference;
		void_c _handle_style_reference_on_refreshed( menu_style_for_text_c::reference_c const * value ); // is invoked by _style_reference when it refreshes().

		string_c::reference_c _string_reference; // when this is set then it will define the text value of this text element.
		void_c _handle_string_reference_on_refreshed( string_c::reference_c const * value ); // is invoked by _string_reference when it refreshes();

		menu_style_for_text_glyph_c _style; // resolved format of this text element, laid out glyphs may reference this with a pointer.

		menu_text_format_mode_e _text_format_mode;
		menu_text_edit_mode_e _text_edit_mode;

		menu_text_filter_mode_e _filter_mode; // if set, determines how user input is selectively filtered.
		string16_c _filter_string; // if filter mode is custom, determines which characters can be inputted.

		boolean_c _text_is_modified; // tracks if text was modified by the user to determine if it needs to be committed or not when the user presses enter or the element loses input focus.
		boolean_c _multi_line; // if true, then pressing enter will insert a new line. if false, and if _text_is_modified is true, then pressing enter will invoke on_value_committed and _text_is_modified will be set to false.
		boolean_c _word_wrap; // if true, then automatic word wrapping will be performed.
		sint32_c _character_limit; // if > 0 then there will be a finite character limit enforced.
		vector32x2_c _content_offset; // offset (usually from scroll bars)   this affects the position that text is rendered at within the element, and it affects hit detection too. this is basically what the text box control routes its scroll bar values into in order to scroll the text around.
		sint32_c _cursor_index; // the index in _plain_text that the cursor is located at.
		boolean_c _cursor_is_at_end_of_virtual_line; // the _cursor_index is the same at the end of a virtual line and at the start of the next line, so when this value is true it tells the renderer to render the cursor at the end of the previous line rather than at the start of the current line.
		menu_text_selection_mode_e _text_selection_mode; // determines how the selection updates as the mouse moves.
		sint32_c _selection_anchor_index_start; // anchored selected range, inclusive.
		sint32_c _selection_anchor_index_end; // anchored selected range, exclusive.
		float32_c _cursor_sticky_x; // cursor will try to stick to this x coordinate when up and down arrows are pressed.
		menu_text_align_horizontal_e _text_align_horizontal; // horizontal text alignment to apply to all of the text, if set then it overrides the value defined by the text _style_reference.
		menu_text_align_vertical_e _text_align_vertical; // vertical text alignment to apply to all of the text, if set then it overrides the value defined by the text _style_reference.
		boolean_c _is_text_focused; // if true then cursor will be rendered, except for label mode then it is always hidden.
		float32_c _cursor_width; // width of cursor in pixels.
		vector32x4_c _cursor_color; // color of cursor.
		float32_c _cursor_time; // accumulates time to drive cursor blink animation.
		boolean_c _text_layout_is_dirty; // will be set to true if glyphs in any paragraphs need reflow, which can happen if any property changes that would potentially modify the layouts of the glyphs within the paragraph (a font is reloaded, the style reference key changes, the string reference key changes, the text value changes, etc.).
		float32_c _laid_out_width; // used to track changes to _local_box width to determine if all glyphs need to be reflowed.
		float32_c _content_width;
		float32_c _content_height;
		core_list_c< text_paragraph_c * > _paragraph_list; // the list of paragraphs.
		core_list_c< menu_style_for_text_glyph_c const * > _text_glyph_style_cache;
		menu_style_for_text_glyph_c const * _cache_text_glyph_style( menu_style_for_text_glyph_c const & text_glyph_style );

		//void_c _process_markup(); // after _markup has parsed a document, it will contain rich text data, this will process it.

		// takes into account horizontal text alignment and translates horizontal position of glyphs that were already added to the paragraph's _glyph_list.
		// inserts a new laid out line at the end of the paragraph, after word wrapping algorithm has determined the start and end characters of the line.
		// calculates the width, height, layout, etc. of the line.
		void_c _finalize_glyph_flow_for_line( text_paragraph_c * paragraph, sint32_c character_start, sint32_c character_end, float32_c laid_out_width, float32_c & laid_out_top );

		// sets _text_layout_is_dirty to true on all paragraphs, which triggers deferred lay out on all paragraphs in this text element.
		void_c _invalidate_glyph_flow_in_all_paragraphs();

		// goes through each paragraph and checks if any spans reference a style which references the given font, and if so then sets _text_layout_is_dirty to true for that paragraph.
		// this is called in response to a font being reloaded.
		void_c _invalidate_glyph_flow_in_all_paragraphs_with_font( resource_file_font_c * font );

		// needs to be called when _layout_is_dirty is true.
		// reflows text in all paragraphs that have _text_layout_is_dirty set to true. calls _reflow_glyphs_in_paragraph() for each affected paragraph and then calls _update_vertical_layout_of_paragraphs_at_and_after() at the end to finish it up.
		void_c _reflow_glyphs_in_all_paragraphs_that_need_it();

		// needs to be called when _layout_is_dirty is true and because text in the paragraph was modified.
		// reflows the text in the given paragraph and sets _text_layout_is_dirty to false for that paragraph.
		// you will need to call _update_vertical_layout_of_all_paragraphs() to update vertical layout of affected paragraphs.
		void_c _reflow_glyphs_in_paragraph( sint32_c paragraph_index );

		// needs to be called when horizontal text alignment is changed and _layout_is_dirty is false.
		// because horizontal text alignment only affects the _left coordinate of each line, we don't need to reflow all the glyphs, so this is cheaper than calling _reflow_glyphs_in_all_paragraphs_that_need_it().
		// if needs to be called when horizontal text alignment is changed and _layout_is_dirty is true, then _reflow_glyphs_in_all_paragraphs_that_need_it() should be called instead.
		void_c _update_horizontal_layout_of_all_lines();

		// recalculates _content_size and updates vertical layout of all paragraphs.
		void_c _update_vertical_layout_of_all_paragraphs();

		// tries to place the cursor on the given line, and keeps its x location as close to _cursor_sticky_x as possible.
		void_c _place_cursor_at_line_index( sint32_c line_index, boolean_c shift );

		// places the cursor at the given point if possible.
		void_c _place_cursor_at_local_point( vector32x2_c const & local_point, boolean_c shift );
		// scans around character index for a word.
		void_c _get_word_at_character_index( sint32_c character_index, sint32_c & character_start, sint32_c & character_end );
		// scans for the paragraph that contains character_index.
		sint32_c _get_paragraph_index_at_character_index( sint32_c at_character_index );
		// scans for the paragraph that contains character_index.
		text_paragraph_c * _get_paragraph_at_character_index( sint32_c character_index );

		// recalculates _cursor_sticky_x.
		void_c _update_cursor_sticky_x();

		// inserts plain text at current _cursor_index, the style of the inserted text will be inherited from the span that the text is inserted into.
		void_c _insert_plain_text( string16_c const & value );
		void_c __shift_character_offset( sint32_c at, sint32_c count );

		// deletes a range of characters, and automatically handles deletion and merging of spans and blocks.
		void_c _delete_character_range( sint32_c start, sint32_c end );

		// deletes currently selected range of text.
		void_c _delete_selected_text();

		// calculates detailed information about the cursor.
		// character_index, the index of the character in _plain_text to get information about.
		// paragraph_index, the index of the paragraph that the cursor is in.
		// span_index, the index of the span within the paragraph that the cursor is in.
		// line, the line number that the cursor is on.
		// column, the index of the character within the line that the cursor lies before.
		// position, the x offset from the left edge of the document to the left edge of the selected character, and the y offset from the top edge of the document to the base line of the selected line.
		//void_c get_character_information( sint32_c character_index, sint32_c * paragraph_index, sint32_c * span_index, sint32_c * line_index, boolean_c * is_at_end_of_virtual_line, vector32x2_c * position );
		void_c _get_character_information( sint32_c character_index, character_information_c * result );

		// we don't cache the formatted_left of each formatted_line because it's a bit of work to re-calculate it each time the content width changes.
		//float32_c _get_formatted_line_formatted_left( text_paragraph_c * paragraph, text_line_c * formatted_line );

		// if at_character_index is in the middle of an existing span, then that span will be split in half, and the index of the later half (the new span) will be set as the result span_index.
		// if at_character_index is at the start of an existing span, then the index of the span will be set as the result span_index.
		// paragraph_index will be set to the index of the paragraph that contains at_character_index.
		// span_index will be set to the index of the span that starts at at_character_index.
		//void_c _subdivide_span( sint32_c at_character_index, sint32_c & paragraph_index, sint32_c & span_index );

		// if character_start and/or character_end lie in the middle of existing spans, then those existing spans are split.
		// all of the spans in the range of character_start to character_end are added to span_list.
		// and paragraph_start is set to the index of the first paragraph that contains character_start, and span_start is set to the index of the first span in within paragraph_start that contains character_start.
		// and paragraph_end is set to the index of the first paragraph that contains character_end, and span_end is set to the index of the first span within paragraph_end that contains character_end.
		//void_c _subdivide_spans_in_character_range( sint32_c character_start, sint32_c character_end, sint32_c & paragraph_start, sint32_c & paragraph_end, sint32_c & span_start, sint32_c & span_end );

		// gathers spans that lie from paragraph_start to paragraph_end
		//void_c _gather_spans( core_list_c< text_span_c * > & result, sint32_c paragraph_start, sint32_c paragraph_end, sint32_c span_start, sint32_c span_end );
		//void_c _gather_spans_in_character_range( sint32_c character_start, sint32_c character_end, core_list_c< text_span_c * > & result );

		// merges neighboring spans within each paragraph that share an equivalent style.
		// also deletes any spans that might happen to be empty.
		//void_c _clean_spans_in_paragraph_range( sint32_c paragraph_start, sint32_c paragraph_end );

		// inserts a new paragraph starting at the character offset denoted by at_character_index.
		// if at_character_index points to the last character in _plain_text, then a new paragraph will be inserted at the end.
		// if at_character_index points to the first character in an existing paragraph, then a new paragraph will be inserted before that paragraph.
		// if at_character_index points to a character in the middle of an existing paragraph, then the existing paragraph will be split into two paragraphs.
		// returns the index of the newly inserted paragraph.
		sint32_c _insert_new_paragraph( sint32_c at_character_index, string16_c const & plain_text );

		// handles invalidation of text layout when size of local rectangle changes.
		// is more expensive if the width changes, is not as expensive if the height changes.
		// text reflow will be deferred until later, once we encounter a time when it is needed (if hit detection is about to be performed, if element is about to be rendered, etc).
		virtual void_c _on_local_box_modified() override;

		virtual void_c _build_draw_list() override;

	public:
		menu_element_text_c();
		virtual ~menu_element_text_c();

		// updates visual state animations, cursor animation, and any in-lined sprite animations.
		// reflows text if text layout is dirty.
		virtual void_c update_animations( float32_c time_step ) override;

		virtual void_c set_style_key( string8_c const & value ) override;

		// reflows text if needed.
		// this happens automatically, this fucntion is provided in situations where it needs to be forced.
		// such as when _content_width and _content_height needs to be known.
		void_c update_text_layout();

		menu_style_for_text_c::reference_c const & get_style_reference() const;
		menu_style_for_text_c::reference_c & get_style_reference();

		string_c::reference_c const & get_string_reference() const;
		string_c::reference_c & get_string_reference();

		menu_text_format_mode_e get_text_format_mode() const;
		void_c set_text_format_mode( menu_text_format_mode_e value );

		menu_text_edit_mode_e get_text_edit_mode() const;
		void_c set_text_edit_mode( menu_text_edit_mode_e value );

		// gets filter mode.
		menu_text_filter_mode_e get_text_filter_mode() const;
		// sets fiter mode, also clears text (sorry).
		// also sets _mutli_line to false if the filter_mode is numeric.
		// also sets _character_limit to something finite if the filter_mode is numeric.
		void_c set_text_filter_mode( menu_text_filter_mode_e filter_mode, string16_c const & filter_string );

		// removes all text in this text element, works in both plain and rich text modes.
		// also clears the string reference key and value if they are set (sorry).
		void_c clear_text();

		// returns a copy of the plain text contents of this text element with the terminating new line character removed.
		string16_c get_plain_text() const;

		// sets the plain text of this text element.
		// the text format mode can be plain or rich.
		// if the text length would go over the character limit, then it will be truncated.
		// if the text contains a new line and we are in single line mode, then it will be truncated.
		// this does not clear the string reference.
		void_c set_plain_text( string16_c const & plain_text );

		// sets the rich text of this text element.
		// the text element text mode must be the rich text mode.
		void_c set_rich_text( string8_c const & plain_text_with_markup );

		// appends a line of plain text to the end of the last paragraph.
		// this is computationally expensive if the last paragraph is already populated with a lot of text in it since it has to reflow all of that text again.
		void_c append_line( string16_c const & plain_text );

		// appends a line of plain text as a new paragraph.
		// this is not computationally expensive if the text element is already populated since it does not have to reflow any existing text.
		void_c append_paragraph( string16_c const & plain_text );

		// gets the maximum number of characters allowed in _plain_text (not including the new line terminator).
		sint32_c get_character_limit() const;
		// sets the maximum number of characters allowed in _plain_text (not including the new line terminator).
		// will truncate _plain_text to length.
		void_c set_character_limit( sint32_c value );

		menu_text_align_horizontal_e get_text_align_horizontal() const;
		void_c set_text_align_horizontal( menu_text_align_horizontal_e value );

		menu_text_align_vertical_e get_text_align_vertical() const;
		void_c set_text_align_vertical( menu_text_align_vertical_e value );

		// gets automatic word wrap setting.
		boolean_c get_word_wrap() const;
		// sets automatic word wrap setting. if true then multi line setting should also be true in order for this setting to be meaningful.
		void_c set_word_wrap( boolean_c value );

		// gets the multi line setting.
		boolean_c get_multi_line() const;
		// sets the multi line setting, which determines if new line characters are allowed to be inputted.
		// also clears the text value (sorry), because i don't want to bother writing special case code if the text value already has new lines in it and then this setting is flipped to true.
		void_c set_multi_line( boolean_c value );

		vector32x4_c get_cursor_color() const;
		void_c set_cursor_color( vector32x4_c const & value );

		float32_c get_cursor_width() const;
		void_c set_cursor_width( float32_c value );

	public:
		menu_text_align_vertical_e get_style_text_align_vertical() const;
		menu_text_align_horizontal_e get_style_text_align_horizontal() const;
		float32_c get_style_paragraph_spacing() const;
		float32_c get_style_line_spacing() const;
		float32_c get_style_glyph_spacing() const;
		resource_file_font_c * get_style_font() const;
		vector32x4_c get_style_color() const;
		float32_c get_style_size() const;
		float32_c get_style_skew() const;
		float32_c get_style_weight() const;
		float32_c get_style_softness() const;
		float32_c get_style_underline() const;

	public:
		// gets the origin point for text in this text element, which is used with scroll bars to translate the position of the text within the element.
		vector32x2_c const & get_content_offset() const;
		// sets the origin point for text in this text element, which is used with scroll bars to translate the position of the text within the element.
		// this does not force reflow.
		void_c set_content_offset( vector32x2_c const & value );

		// gets width of text content, which can be used to set the capacity of horizontal scroll bar.
		float32_c get_content_width() const;
		// gets height of text content, which can be used to set the capacity of vertical scroll bar.
		float32_c get_content_height() const;

		// gets the character index position of the cursor in the string of plain text.
		sint32_c get_cursor_index() const;
		// sets the character index position of the cursor in the string of plain text. automatically clamps the index to be within a valid range.
		void_c set_cursor_index( sint32_c cursor_index );

		// returns true if a range of text is currently selected.
		boolean_c has_selected_text();
		// returns the character index range of currently selected text.
		void_c get_selected_text_range( sint32_c & selection_index_start, sint32_c & selection_index_end ) const;

	public:
		// called by control when it gains character focus. sets _modified to false and sets _is_text_focused to true, which enables display of the cursor.
		void_c handle_on_character_focus_gain();
		// called by control when it loses character focus. sets _is_text_focused to false, which disables display of the cursor.
		void_c handle_on_character_focus_lose();
		// called by mother control when it receives an input event, returns true if input event is handled (consumed), false if otherwise.
		boolean_c handle_input( input_event_c * input_event );

	public:
		// deletes the currently selected range of text and inserts a printable character (which may be a space or tab).
		// don't use this to insert new lines or paragraphs, use input_return for that instead.
		void_c input_character( char16_c character );
		// deletes the currently selected range of text, if shift is false then inserts a new text paragraph else inserts a new line in the existing paragraph/span.
		void_c input_return( boolean_c shift );
		// deletes the currently selected range of text or character after the cursor, and merges text paragraphs if needed.
		void_c input_delete_fore();
		// deletes the currently selected range of text or deletes the character before the cursor, and merges text paragraphs if needed.
		void_c input_delete_back();
		// moves the cursor to the left one character.
		void_c input_left( boolean_c shift, boolean_c ctrl );
		// moves the cursor to the right one character.
		void_c input_right( boolean_c shift, boolean_c ctrl );
		// moves the cursor up one line.
		void_c input_up( boolean_c shift );
		// moves the cursor down one character.
		void_c input_down( boolean_c shift );
		// moves the cursor to the end of the current line.
		void_c input_home( boolean_c shift );
		// moves the cursor to the start of the current line.
		void_c input_end( boolean_c shift );

	public:
		// copies the currently selected plain text to the operating system clip board.
		boolean_c copy_text_to_clip_board();
		// pastes any plain text in the operating system clip board into the document. deletes any currently selected text and inserts new text.
		boolean_c paste_text_from_clip_board();

		// gets total number of lines of text, including virtual ones created by automatic word wrapping.
		// but this isn't super useful to know, so eh.
		//sint32_c get_line_count() const;

	public:
		core_event_c< void_c, menu_element_text_c * > on_value_changed_preview; // occurs whenever the text value changes as the user types.
		core_event_c< void_c, menu_element_text_c * > on_value_changed_commit; // occurs if the text value was modified and the user presses enter or the element loses text input focus.

	};

}
