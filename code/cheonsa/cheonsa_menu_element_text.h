#pragma once

#include "cheonsa_menu_element.h"
#include "cheonsa_resource_file_font.h"
#include "cheonsa_input_manager.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_core_linked_list.h"
#include "cheonsa_sprite.h"

namespace cheonsa
{

	// text element.
	//
	// usage:
	// when you create a menu_element_text_c instance, call the following functions in the following order:
	// 1. call set_mode() to set plain text mode or rich text mode.
	// 2. optionally, call set_style_key() to set the base text style, which is the style that will be used if no style is explicitly defined by rich text markup.
	// 3. call set_plain_text() if plain text mode was set or set_rich_text() if rich text mode was set.
	//
	// rich text uses the data_scribe_markup_c class to parse markup, so it's xml-like.
	// rich text markup uses square brackets instead of angled brackets, so that rich text values can be saved in xml files without breaking the xml.
	//
	// [paragraph key="h1"]header style[/]
	// [paragraph key="p"]paragraph style. [style key=""]span[/] text.[/]
	// 
	// [paragraph key=""][/] tag is paragraph, all text that you want to display must be contained in these, any number can be contained at root, they can not be nested. they may contain "s" tags.
	//   "key" attribute is a text style key that references a text style in the menu style manager.
	//
	// [span key=""][/] tag is span, it must be nested within a [paragraph][/] tag, it contains a range of text to format a certain way.
	//   "key" attribute is a text style key that references a text style in the menu style manager.
	//
	// [sprite key=""/] tag is reference to game defined sprite.
	//   "key" attribute is a sprite key that references a game defined sprite.
	//
	// [string key=""/] tag is reference to game defined string.
	//   "key" attribute is a string key that references a game defined string.
	//
	// undo/redo system?
	// might just brute force it for now, a delta snapshot system will be complicated.
	// to store delta snapshots for plain text changes is easy enough.
	// to store delta snahshots for paragraph state changes is more difficult.
	//
	// todo:
	// add inlineable images (emojis).
	// add clickable links (uris).
	// resolve references (formatted like "$string_key" in the original plain text value) to localized string values.
	//
	// if the user is supposed to be able to interact with the text content in the text element, then the control needs to channel events to the text element's event handler functions:
	//   handle_on_character_focus_gained()
	//   handle_on_character_focus_lost()
	//   handle_on_input()
	class menu_element_text_c
		: public menu_element_c
	{
	public:
		static inline char8_c const * get_type_name_static() { return "element_text"; }
		virtual inline char8_c const * get_type_name() const override { return get_type_name_static(); }

	public:
		static core_linked_list_c< menu_element_text_c * > _global_list; // hold all text element instances, so that they can be notified in order to respond to changes in shared menu styles or shared string values.
		static void_c invalidate_glyph_layout_of_all_instances(); // invalidates lay out of all text elements, this also causes sprite references and string references to be updated.
		static void_c invalidate_glyph_layout_of_all_instances_with_font( resource_file_font_c * font ); // invalidates lay out of all text elements that have paragraphs that reference font.

	public:
		// stores the formatted layout of a line of text.
		// does not contain the list of laid out glyphs, those are stored in the paragraph that the line is also stored in.
		class text_line_c
		{
		public:
			sint32_c _character_start; // index of first character within _plain_text that belongs to this text line.
			sint32_c _character_end; // index of last character within _plain_text that belongs to this text line. if this is the last line in the paragraph then this is the index of the paragraph's terminating new line character.
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

		// stores layout of a glyph or entity within a paragraph.
		// glyphs are stored in their own list in the text element.
		// there is always one glyph allocated for each character in the text element's _plain_text, even for non-printable/displayable characters, so as to maintain the one to one relationship.
		// for a given character, some times a sprite or string entity is used in place of the glyph, in which case the glyph is ignored even though it is still allocated.
		class text_glyph_c
		{
		public:
			glyph_c const * _glyph; // pointer to the glyph in the font that this formatted glyph is displaying, and for which we hold a reference count to. this can not be nullptr.

			uint16_c _style_index; // references a style in the text element's style cache.
			uint16_c _uri_index; // references a uri in the text element's uri cache.

			box32x2_c _box; // offset of this laid out glyph's quad mesh geometry (including margin for SDF), relative to the laid out line's origin.
			float32_c _left; // horizontal offset of this laid out glyph's origin relative to the left edge of the laid out line's origin or baseline. used by animated sprites, which are drawn relative to this origin. vertical origin can be calculated from the laid out line's _top and the laid out paragraph's _top.
			float32_c _ascender; // distance above the base line, will probably always be positive.
			float32_c _descender; // distance above the base line, will probably always be negative.
			float32_c _horizontal_advance; // this glyph's horizontal advance towards the right in pixels, including extra glyph spacing.

		public:
			text_glyph_c();
			text_glyph_c & operator = ( text_glyph_c const & other );

		};

		class text_paragraph_c;

		// a span defines a text style to apply to a range of characters in the text element.
		// a span must not include the paragraph's terminating new-line character.
		// spans are generally not responsible for togglable bold, italic, and underline states.
		class text_span_c
		{
		private:
			friend class menu_element_text_c;
			friend class core_list_c< text_span_c >;

			text_paragraph_c * _mother_paragraph; // will be set if the immediate mother of this span is a paragraph, otherwise it will be nullptr.

			sint32_c _character_start; // index of first character within _plain_text that is formatted by this span.
			sint32_c _character_end; // index of last character within _plain_text that is formatted by this span.

			menu_text_style_c::reference_c _text_style_reference; // the format to apply to the range of characters defined by this span. can be left unset, which means that the style for this span will fall back to using the _style_reference defined in the text element.
			void_c _handle_text_style_reference_on_refreshed( menu_text_style_c * text_style );

		private:
			text_span_c();

			// called in response to insertion of text in _plain_text.
			// this will extend spans that intersect with the given character range.
			void_c _handle_insert_character_range( sint32_c character_start, sint32_c character_count );

			resource_file_font_c * _get_style_font() const; // goes up the inheritance tree to find the font for this span.
			vector32x4_c _get_style_color() const; // goes up the inheritance tree to find the color for this span.
			float32_c _get_style_size() const; // goes up the inheritance tree to find the size for this span.
			float32_c _get_style_skew() const; // goes up the inheritance tree to find the skew for this span.
			float32_c _get_style_weight() const; // goes up the inheritance tree to find the weight for this span.
			float32_c _get_style_softness() const; // goes up the inheritance tree to find the softness for this span.
			float32_c _get_style_glyph_spacing() const; // goes up the inheritance tree to find the glyph spacing for this span.
			menu_text_glyph_style_c _get_style() const;

		};

		// a paragraph is a section or block of text that shares the same horizontal text alignment.
		// a text element must always contain at least one paragraph, even if it is empty of text.
		// a paragraph must always end with a new-line character, even if the paragraph is otherwise empty of text.
		// a paragraph may also contain in-line new-line characters within it, these will create line breaks within in the paragraph.
		// a paragraph contains a list of spans, which defines ranges of text within the paragraph to apply different text styles to.
		// the last span in the paragraph should not enclose the paragraph's terminating new line character.
		class text_paragraph_c
		{
		private:
			friend class menu_element_text_c;

			menu_element_text_c * _mother_element_text; // the mother text element that owns this paragraph.

			sint32_c _character_start; // index of first character within _plain_text that belongs to this paragraph.
			sint32_c _character_end; // index of last character within _plain_text that belongs to this paragraph, including this paragraph's terminating new line character. in the case that the paragraph only contains the (bare minimum) new line character, then _character_start will equal _character_end.

			menu_text_style_c::reference_c _text_style_reference; // a reference of the text style to apply to text in this paragraph. if it is not defined, then the _style_reference from the text element will be used.
			void_c _handle_text_style_reference_on_refreshed( menu_text_style_c::reference_c const * text_style_reference );

			core_list_c< text_span_c * > _span_list; // all of the root level spans in this paragraph. will always be empty if in plain text mode (so that we don't have to worry about maintaining them, because only plain text mode can be edited).
			text_span_c const * _find_span( sint32_c character_index ) const;

			float32_c _top; // offset from top of document to top edge of this paragraph after taking into account vertical text alignment of the text element.
			float32_c _content_width; // width of the widest laid out line in this paragraph.
			float32_c _content_height; // height of all of the lines in this paragraph.
			sint32_c _line_index_base; // the line index of the first line in this paragrpah relative to all of the lines in the whole document.
			core_list_c< text_line_c > _line_list; // all of the lines in this paragraph, this can be a mix of real and virtual (broken by automatic word wrap algorithm) lines of text.
			core_list_c< text_glyph_c > _glyph_list; // all of the laid out glyphs in this paragraph.
			core_list_c< menu_text_entity_c * > _entity_list; // all of the entities inlined in the text of this paragraph.

			boolean_c _is_glyph_layout_dirty; // will be set to true if the glyphs in this paragraph needs to be reflowed.

		private:
			text_paragraph_c();

		public:
			~text_paragraph_c();

		private:
			// reflows all the glyphs in this paragraph.
			void_c _do_glyph_layout();

			// called by _do_glyph_layout to add laid out lines to the paragraph.
			// takes into account horizontal text alignment and translates horizontal position of glyphs that were already added to the paragraph's _glyph_list.
			// inserts a new laid out line at the end of the paragraph, after word wrapping algorithm has determined the start and end characters of the line.
			// calculates the width, height, layout, etc. of the line.
			void_c _do_glyph_layout_finish_line( sint32_c character_start, sint32_c character_end, float32_c laid_out_width, float32_c & laid_out_top );

			// called in response to deletion of text from _plain_text.
			// this will clip/resize this paragraph, and clip/resize/delete daughter spans.
			// character_start is the index of the first character that was deleted.
			// character_count is the number of characters that were deleted.
			// returns -1 if this paragraph is entirely enclosed by the range of text that was deleted and so wants to be deleted itself.
			// returns 0 if everything went okay.
			// returns 1 if the tail end of the paragraph was clipped by the delete range, which means that this paragraph will need to be merged with the next one. 
			sint32_c _handle_delete_character_range( sint32_c character_start, sint32_c character_count );

			// called in response to insertion of text in _plain_text.
			// this will extend spans that intersect with the given character range.
			void_c _handle_insert_character_range( sint32_c character_start, sint32_c character_count );

			// recursively scans daughter spans for any single span that fully contains the given character range and returns it.
			// if the given character range lies on the boundaries of any span then this returns nullptr.
			text_span_c * _get_span_at_character_range( sint32_c character_start, sint32_c character_count );

			// returns index of line in this paragraph's line list that contains the given character index.
			//sint32_c _get_line_index_at_character_index( sint32_c character_index );
			text_line_c const * _get_line_at_character_index( sint32_c character_index, sint32_c * result_line_index ) const;
			text_line_c * _get_line_at_character_index( sint32_c character_index, sint32_c * result_line_index );

			menu_text_align_horizontal_e _get_style_text_align_horizontal() const; // goes up the inheritance tree to find the horizontal text align for this paragraph.
			float32_c _get_style_paragraph_spacing() const;
			float32_c _get_style_line_spacing() const;
			float32_c _get_style_glyph_spacing() const;
			resource_file_font_c * _get_style_font() const; // goes up the inheritance tree to find the font for this paragraph.
			vector32x4_c _get_style_color() const; // goes up the inheritance tree to find the color for this paragraph.
			float32_c _get_style_size() const; // goes up the inheritance tree to find the size for this paragraph.
			float32_c _get_style_skew() const; // goes up the inheritance tree to find the skew for this paragraph.
			float32_c _get_style_weight() const; // goes up the inheritance tree to find the weight for this paragraph.
			float32_c _get_style_softness() const; // goes up the inheritance tree to find the softness for this paragraph.
			menu_text_glyph_style_c _get_style() const;

		};

		// result of query methods to check information of glyph associated with character at a given index or at a given local point.
		struct glyph_information_c
		{
			sint32_c character_index; // character index of the glyph.
			sint32_c paragraph_index; // paragraph within the text element's paragraph list that character is in.
			sint32_c paragraph_span_index; // span within paragraph's span list that the character is in, or -1 if character is not in a span.
			sint32_c paragraph_line_index; // line within the paragraph's line list that the character is in.
			//boolean_c is_at_end_of_virtual_line; // will be true if the cursor index is at the end of a virtual line and at the start of another line. this means that the cursor could be on line_index or it could be on line_index - 1.

			sint32_c cursor_index;
			boolean_c cursor_is_on_previous_line;
			float32_c cursor_x; // local x coordinate of cursor.

			glyph_information_c()
				: character_index( -1 )
				, paragraph_index( -1 )
				, paragraph_span_index( -1 )
				, paragraph_line_index( -1 )
				, cursor_index( -1 )
				, cursor_is_on_previous_line( false )
				, cursor_x( 0.0f )
			{
			}
		};

		// for code reusability, for both paragraphs and spans.
		static void_c _remove_spans_in_character_range( core_list_c< text_span_c * > & span_list, sint32_c character_start, sint32_c character_count );

		// for code reusability, for both paragraphs and spans.
		static sint32_c _handle_delete_character_range( sint32_c character_start, sint32_c character_count, sint32_c & _character_start, sint32_c & _character_end );

	private:
		string16_c _plain_text;
		core_list_c< text_paragraph_c * > _paragraph_list;
		menu_text_format_mode_e _text_format_mode;
		menu_text_interact_mode_e _text_interact_mode;
		menu_text_filter_mode_e _text_filter_mode;
		string16_c _text_filter_string; // if format mode is filter string, then this string contains the set of characters that are allowed to be inputted.
		menu_text_select_mode_e _text_select_mode; // determines how the selection updates as the mouse moves.
		sint32_c _text_select_anchor_index_start; // anchored selected range, inclusive.
		sint32_c _text_select_anchor_index_end; // anchored selected range, exclusive.
		menu_text_style_c::reference_c _text_style_reference; // the base text style to apply to text in this text element when no other text style is defined (via paragraph and/or span).
		menu_text_align_horizontal_e _text_align_horizontal; // horizontal text alignment to apply to all of the text, if set then it overrides the value defined by the text _style_reference.
		menu_text_align_vertical_e _text_align_vertical; // vertical text alignment to apply to all of the text, if set then it overrides the value defined by the text _style_reference.
		sint32_c _text_value_length_limit; // if greater than or equal to 0 then a character limit enforced on the _plain_text string (this does not include the terminating '\n' and '\0' characters).
		boolean_c _multi_line; // if true, then pressing enter will insert a new line. if false, and if _is_text_value_modified is true, then pressing enter will invoke on_value_committed and _is_text_value_modified will be set to false.
		boolean_c _word_wrap; // if true, then automatic word wrapping will be performed.
		vector32x2_c _content_offset; // translation (usually from scroll bars) that offsets where text is placed within the text element.
		sint32_c _cursor_index; // the index in _plain_text that the cursor is located at.
		boolean_c _cursor_is_on_previous_line; // because the character index at the end of a virtual (word wrapped) line is the same as the character index at the start of the next line, this determines if the cursor should be drawn at the end of the previous line or not. this is only really needed when clicking and dragging to select a range of characters that includes the end of a virtual line of text, as it will force the cursor to appear at the end of the selected line of text rather than on the next line of text.
		float32_c _cursor_sticky_x; // cursor will try to stick to this x coordinate when up and down arrows are pressed.
		float32_c _cursor_time; // accumulates time to drive cursor blink animation.
		boolean_c _is_text_focused; // if true then cursor will be rendered, except for label mode then it is always hidden.
		boolean_c _is_text_value_modified; // tracks if text was modified by the user to determine if it needs to be committed or not when the user presses enter or the element loses input focus.
		boolean_c _is_glyph_layout_dirty; // will be set to true if any paragraphs need to reflow their glyph layout.
		float32_c _laid_out_width; // used to detect changes to _local_box width in order to set _is_glyph_layout_dirty to true.
		float32_c _content_width; // widest width of text glyph contents after glyph reflow layout. is dirty when _is_glyph_layout_dirty is true.
		float32_c _content_height; // height of text glyph contents after glyph reflow layout. is dirty when _is_glyph_layout_dirty is true.
		core_linked_list_c< menu_element_text_c * >::node_c _global_list_node;
		core_list_c< menu_text_glyph_style_c const * > _text_glyph_style_cache; // text glyph styles are cached in here, so that they can be referenced by any number of glyphs in the text element.

		void_c _clear_cached_data(); // resets the glyph style cache. frees data that's no longer in use but cache will have to be rebuilt with the data that is in use.
		void_c _cache_text_glyph_style( menu_text_glyph_style_c const & value, menu_text_glyph_style_c const * & result_pointer, uint16_c & result_index ); // caches a text glyph style in the text glyph style cache, or gets the equivalent one that was already cached.
		void_c _handle_text_style_reference_on_refreshed( menu_text_style_c::reference_c const * value ); // is invoked by _style_reference when it refreshes(), just sets _is_glyph_layout_dirty to true.
		void_c _invalidate_glyph_layout_in_all_paragraphs(); // sets _is_glyph_layout_dirty to true on this text element and on all paragraphs.
		void_c _invalidate_glyph_layout_in_paragraphs_with_font( resource_file_font_c * font ); // sets _is_glyph_layout_dirty to true on any paragraphs that reference the given font. this is called in response to a font being reloaded.
		void_c _do_glyph_layout(); // does glyph reflow layout, sets _is_glyph_layout_dirty is false.
		void_c _update_horizontal_layout_of_all_lines(); // needs to be called when horizontal text alignment is changed.
		void_c _update_vertical_layout_of_all_paragraphs(); // called at end of reflowing text in paragraphs that need it.
		void_c _place_cursor_at_line_index( sint32_c line_index, boolean_c shift ); // places the cursor on the given line, and tries to keeps its x location as close to _cursor_sticky_x as possible.
		void_c _place_cursor_at_local_point( vector32x2_c const & local_point, boolean_c shift ); // places the cursor at the glyph that intersects with the given point.
		void_c _get_word_at_character_index( sint32_c character_index, sint32_c & character_start, sint32_c & character_end ); // scans around character index for a word.
		text_paragraph_c const * _get_paragraph_at_character_index( sint32_c character_index, sint32_c * result_paragraph_index ) const ; // finds the paragraph that contains the given character index.
		text_paragraph_c * _get_paragraph_at_character_index( sint32_c character_index, sint32_c * result_paragraph_index ); // finds the paragraph that contains the given character index.
		text_glyph_c * _get_text_glyph_at_character_index( sint32_c character_index ); // finds the text glyph at the given character index.
		text_glyph_c const * _get_text_glyph_at_character_index( sint32_c character_index ) const; // finds the text glyph at the given character index.
		void_c _update_cursor_sticky_x(); // recalculates _cursor_sticky_x based on current _cursor_index.
		void_c _input_plain_text( string16_c const & value ); // inputs the given text value into the _plain_text value at the current _cursor_index, applies _text_filter_mode.
		void_c _insert_plain_text( char16_c const * values, sint32_c values_count, sint32_c at_character_index ); // inserts the given plain text value at the given at_character_index, and then advances the _cursor_index if applicable, applies _text_value_length_limit.
		void_c _delete_text( sint32_c character_start, sint32_c character_count ); // deletes a range of characters, and automatically handles deleting and merging affected paragraphs and spans.
		void_c _delete_selected_text(); // deletes currently selected range of text.
		glyph_information_c _get_glyph_information( sint32_c character_index ); // gets information about the glyph associated with the given character_index.
		glyph_information_c _get_glyph_information( vector32x2_c const & local_point ); // gets information about the glyph closest to local_point, also sets the cursor properties of the result.
		void_c _get_thing_at_character_range( sint32_c character_start, sint32_c character_count, text_paragraph_c * & result_paragraph, text_span_c * & result_span ); // if the character range intersects with a single thing (paragraph or span), then the result paragraph or span will be set to that thing, otherwise they will both be set to nullptr.
		void_c _remove_spans_in_character_range( sint32_c character_start, sint32_c character_count ); // clips spans that intersect with the given character range, used to remove span formatting from a given range of text.
		void_c _create_spans_in_character_range( sint32_c character_start, sint32_c character_count, string8_c const & text_style_key ); // creates span(s) in the given character range. if the given character range intersects with boundaries of a paragraph, then one span needs to be created for each paragraph. make sure that _remove_spans_in_character_range() was called prior in order to cut out room for new spans to be created.
		void_c _insert_new_paragraph(); // inserts a new paragraph at the current _cursor_index, and advnaces _cursor_index.
		void_c _clear_text_value();
		void_c _set_plain_text_value( string16_c const & value ); // value should be plain text without mark up.
		boolean_c _set_rich_text_value( string8_c const & value ); // value should be plain text with mark up.

		menu_text_align_vertical_e _get_style_text_align_vertical() const;
		menu_text_align_horizontal_e _get_style_text_align_horizontal() const;
		float32_c _get_style_paragraph_spacing() const;
		float32_c _get_style_line_spacing() const;
		float32_c _get_style_glyph_spacing() const;
		resource_file_font_c * _get_style_font() const;
		vector32x4_c _get_style_color() const;
		float32_c _get_style_size() const;
		float32_c _get_style_skew() const;
		float32_c _get_style_weight() const;
		float32_c _get_style_softness() const;
		box32x2_c _get_style_margin() const;

		// handles invalidation of text layout when size of local rectangle changes.
		// is more expensive if the width changes, is not as expensive if the height changes.
		// text reflow will be deferred until later, once we encounter a time when it is needed (if hit detection is about to be performed, if element is about to be rendered, etc).
		virtual void_c _on_local_box_modified() override;

		virtual void_c _build_draw_list() override;

	public:
		menu_element_text_c( string8_c const & name );
		virtual ~menu_element_text_c();

		// updates visual state animations, cursor animation, and any in-lined sprite animations.
		// reflows text if text layout is dirty.
		virtual void_c update_animations( float32_c time_step ) override;

		// sets the base text style of this text element, this is the text style that will be applied to plain text or otherwise unstyled text.
		// text_style_key is a text style key that references a text style instance in the menu style manager.
		virtual void_c set_style_key( string8_c const & text_style_key ) override;

		string16_c get_plain_text_value() const; // returns a copy of the plain text contents of this text element (with the very last internal use terminating new line character omitted).
		void_c set_plain_text_value( string8_c const & value );
		void_c set_plain_text_value( string16_c const & value );
		void_c set_rich_text_value( string8_c const & value );
		void_c set_rich_text_value( string16_c const & value );
		void_c clear_text_value();

		menu_text_format_mode_e get_text_format_mode() const;
		void_c set_text_format_mode( menu_text_format_mode_e value ); // this also has the side effect of clearing the text value.

		menu_text_interact_mode_e get_text_interact_mode() const;
		void_c set_text_interact_mode( menu_text_interact_mode_e value );

		menu_text_filter_mode_e get_text_filter_mode() const;
		void_c set_text_filter_mode( menu_text_filter_mode_e value );
		void_c set_text_filter_string( string16_c const & value ); // when text filter mode is set to string, this will define the finite set of characters that will be allowed to be inputted.

		boolean_c has_selected_text_range() const; // returns true if a range of text is selected.
		boolean_c get_selected_text_range( sint32_c & character_start, sint32_c & character_count ) const; // returns true of a range of text is selected.

		menu_text_style_c::reference_c const & get_style_reference() const;
		menu_text_style_c::reference_c & get_style_reference();

		menu_text_align_horizontal_e get_text_align_horizontal() const;
		void_c set_text_align_horizontal( menu_text_align_horizontal_e value );

		menu_text_align_vertical_e get_text_align_vertical() const;
		void_c set_text_align_vertical( menu_text_align_vertical_e value );

		sint32_c get_text_value_length_limit() const;
		void_c set_text_value_length_limit( sint32_c value ); // if needed then this will truncate the existing text value to fit within the new character limit.

		boolean_c get_multi_line() const;
		void_c set_multi_line( boolean_c value ); // if needed then this will trucnate the existing text value to a single line.

		boolean_c get_word_wrap() const;
		void_c set_word_wrap( boolean_c value ); // if word wrap setting is set to true, then multi line setting should also be set to true in order for it to be meaningful.

		float32_c get_default_size() const;

		vector32x2_c const & get_content_offset() const;
		void_c set_content_offset( vector32x2_c const & value );

		float32_c get_content_width() const;
		float32_c get_content_height() const;

		sint32_c get_cursor_index() const;
		void_c set_cursor_index( sint32_c cursor_index );

		void_c get_cursor_information( sint32_c & result_paragraph_index, sint32_c & result_line_index ) const;

		// if no range of text is selected, then this tries to get the text style key of the current span or paragraph that the cursor is in.
		// if a range of text is selected, then this tries to get the text style key of the current span or paragraph that the selected range of text is in.
		// if the selected range of text overlaps with the boundaries of a span or paragraph, then this returns false.
		// if the selected range of text is entirely within a single span, then this returns the text style key of that span.
		// if the selected range of text is entirely within a paragraph and does not intersect any span, then this returns the text style key of that paragraph.
		// in general, returns false if text style key could not be resolved or if none is set.
		boolean_c get_text_style_key_at_cursor( string8_c & result_text_style_key );
		// if no range of text is selected, then this applies the given text style key to the existing span or paragraph that the cursor is in.
		// if a range of text is selected, then this applies the given text style key to that range of text.
		// if text_style_key is not set then it has the effect of clipping and deleting the affected span(s).
		boolean_c set_text_style_key_at_cursor( string8_c const & text_style_key );

		// copies the currently selected plain text to the operating system clip board.
		boolean_c copy_text_to_clip_board();
		// if the operating system clip board contains plain text, then deletes currently selected text and pastes plain text at current cursor index.
		boolean_c paste_text_from_clip_board();

		core_event_c< void_c, menu_element_text_c * > on_text_value_changed_preview; // occurs whenever the text value changes as the user types.
		core_event_c< void_c, menu_element_text_c * > on_text_value_changed; // occurs if the text value was modified and the user presses enter or the element loses text input focus.

	public:
		// should be called by text box control when it gains or loses text focus.
		// when gaining text focus:
		//     sets _is_text_value_modified to false.
		//     sets _is_text_focused to true, which enables display of the cursor.
		// when losing text focus:
		//     if _is_text_value_modified is true then
		//         invokes on_value_changed_commit event.
		//     sets _is_text_focused to false, which disables display of the cursor.
		void_c handle_on_is_text_focused_changed( boolean_c value );

		// should be called by text box control when it receives an on multi clicked event.
		boolean_c handle_on_multi_clicked( input_event_c * input_event );
		// should be called by text box control when it receives an on input event.
		// returns true if input event is handled (consumed), false if otherwise.
		boolean_c handle_on_input( input_event_c * input_event );

		void_c append_line( string16_c const & plain_text ); // appends a line of plain text to the end of the last paragraph. costs more computation time if the last paragraph has a lot of text in it, since all of the text in that paragraph will need to be reflowed.
		void_c append_paragraph( string16_c const & plain_text ); // appends a line of plain text as a new paragraph. should cost less comptation time than append_line(), but might cost a little more memory than append_line().
		void_c input_character( char16_c character ); // deletes the currently selected range of text and inserts a printable character (which may be a space or tab). don't use this to insert new lines or paragraphs, use input_return for that instead.
		void_c input_return( boolean_c shift ); // deletes the currently selected range of text, if shift is false then inserts a new text paragraph else inserts a new line in the existing paragraph/span.
		void_c input_delete_fore(); // deletes the currently selected range of text or character after the cursor, and merges text paragraphs if needed.
		void_c input_delete_back(); // deletes the currently selected range of text or deletes the character before the cursor, and merges text paragraphs if needed.
		void_c input_left( boolean_c shift, boolean_c ctrl ); // moves the cursor to the left one character.
		void_c input_right( boolean_c shift, boolean_c ctrl ); // moves the cursor to the right one character.
		void_c input_up( boolean_c shift ); // moves the cursor up one line.
		void_c input_down( boolean_c shift ); // moves the cursor down one character.
		void_c input_home( boolean_c shift ); // moves the cursor to the end of the current line.
		void_c input_end( boolean_c shift ); // moves the cursor to the start of the current line.

	};

}
