#pragma once

#include "cheonsa_menu_types.h"

namespace cheonsa
{

	// resolved glyph sytle.
	// resolved means that it has inherited all of the styles that are granularly defined by mother spans, paragraph, text element, and built-in (topmost).
	// all of the values are guaranteed to be set to something valid and usable (although maybe not ideal).
	class menu_style_for_text_glyph_c
	{
	public:
		resource_object_font_c::reference_c font;
		vector32x4_c color;
		float32_c size;
		float32_c weight;
		float32_c skew;
		float32_c softness;
		float32_c underline;

	public:
		menu_style_for_text_glyph_c();
		menu_style_for_text_glyph_c( menu_style_for_text_glyph_c const & other );

		menu_style_for_text_glyph_c & operator = ( menu_style_for_text_glyph_c const & other );

		boolean_c operator == ( menu_style_for_text_glyph_c const & other ) const;

	};

}
