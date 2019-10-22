#pragma once

#include "cheonsa_menu_types.h"

namespace cheonsa
{

	// defines how to format and render text in a text element.
	// these style can be applied to the whole text element, or to sub-ranges of text in the text element.
	// text styles can be defined by "styles.xml".
	class menu_style_for_text_c
	{
	public:
		class reference_c
		{
		private:
			string8_c _key; // persistent key, remains the same between reloading of style files.
			menu_style_for_text_c const * _value; // volatile reference, changes between reloading of style files.

		public:
			reference_c();
			reference_c( reference_c const & ) = delete;
			reference_c & operator = ( reference_c const & ) = delete;

			void_c refresh();

			string8_c const & get_key() const;
			void_c set_key( string8_c const & value );

			menu_style_for_text_c const * get_value() const;
			void_c set_value( menu_style_for_text_c const * value ); // a little more optimal than set_key() since it avoids doing a look up.

			core_event_c< void_c, reference_c const * > on_refreshed; // is invoked whenever _value changes. text elements that reference the style should invalidate their text layout.

		};

		// element appearance properties that can be defined for each visual state.
		class state_c
		{
		public:
			menu_shared_color_c::reference_c shared_color; // color tint, multiplied with texture. if resolved then it takes precedence over color.
			vector32x4_c color; // color tint, multiplied with texture.
			float32_c saturation; // multiplicative color saturation.
			float32_c apparent_scale; // multiplicative apparent element scale.

		public:
			state_c();

			void_c reset();
			vector32x4_c get_expressed_color() const; // if element_color_style is resolved then returns that color, otherwise returns element_color.

		};

	public:
		string8_c key; // unique key to identify this text style by.

		boolean_c font_is_defined;
		resource_object_font_c::reference_c font;

		boolean_c size_is_defined;
		float32_c size; // size of font in pixels.

		boolean_c shared_color_is_defined;
		menu_shared_color_c::reference_c shared_color; // color style defined color of font.

		boolean_c color_is_defined;
		vector32x4_c color; // color of font.
		
		boolean_c skew_is_defined;
		float32_c skew; // skew of font.
		
		boolean_c weight_is_defined;
		float32_c weight; // weight of font.

		boolean_c softness_is_defined;
		float32_c softness;

		boolean_c underline_is_defined;
		float32_c underline; // thickness of underline in pixels.

		boolean_c paragraph_spacing_is_defined;
		float32_c paragraph_spacing; // extra vertical spacing in pixels to place between paragraphs.

		boolean_c line_spacing_is_defined;
		float32_c line_spacing; // extra vertical spacing in pixels to place between lines.

		boolean_c glyph_spacing_is_defined;
		float32_c glyph_spacing; // extra horizontal spacing in pixels to place between glyphs.

		boolean_c text_align_horizontal_is_defined;
		menu_text_align_horizontal_e text_align_horizontal; // how to align text in the block (it can only be applied to block types like headers and paragraphs).

		boolean_c text_align_vertical_is_defined;
		menu_text_align_vertical_e text_align_vertical;

		state_c state_list[ menu_state_e_count_ ];

	public:
		menu_style_for_text_c();

		void_c reset();
		void_c load( data_scribe_markup_c::node_c const * node );

		//vector32x4_c get_color() const; // evaluates color style reference if it is set.

	};

}
