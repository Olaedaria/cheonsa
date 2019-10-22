#include "cheonsa_menu_style_for_text_glyph.h"

namespace cheonsa
{

	menu_style_for_text_glyph_c::menu_style_for_text_glyph_c()
		: font( nullptr )
		, color( 1.0f, 1.0f, 1.0f, 1.0f )
		, size( 1.0f )
		, weight( 0.0f )
		, skew( 0.0f )
		, softness( 0.0f )
		, underline( 0.0f )
	{
	}

	menu_style_for_text_glyph_c::menu_style_for_text_glyph_c( menu_style_for_text_glyph_c const & other )
	{
		*this = other;
	}

	menu_style_for_text_glyph_c & menu_style_for_text_glyph_c::operator = ( menu_style_for_text_glyph_c const & other )
	{
		font = other.font;
		color = other.color;
		size = other.size;
		weight = other.weight;
		skew = other.skew;
		softness = other.softness;
		underline = other.underline;
		return *this;
	}

	boolean_c menu_style_for_text_glyph_c::operator == ( menu_style_for_text_glyph_c const & other ) const
	{
		return ( font == other.font ) && ( size == other.size ) && ( color == other.color ) && ( skew == other.skew ) && ( weight == other.weight ) && ( softness == other.softness ) && ( underline == other.underline );
	}

}
