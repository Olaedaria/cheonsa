#include "cheonsa_menu_style_manager.h"
#include "cheonsa__ops.h"
#include "cheonsa_menu_control.h"
#include "cheonsa_engine.h"
#include "cheonsa_menu_style_for_frame.h"
#include "cheonsa_menu_style_for_text.h"
#include "cheonsa_menu_style_file.h"

namespace cheonsa
{

	menu_style_manager_c::menu_style_manager_c()
		: _engine_styles( string16_c( mode_e_static, L"[e]menus/styles.xml" ) )
		, _game_styles( string16_c( mode_e_static, L"[g]menus/styles.xml" ) )
		, _default_font()
		, _default_style_for_frame()
		, _default_style_for_text()
		, _shared_colors()
		, _shared_colors_map()
		, _shared_transition_speed( 6.0f )
	{	
	}

	menu_style_manager_c::~menu_style_manager_c()
	{
	}

	boolean_c menu_style_manager_c::start()
	{
		_shared_colors[ menu_shared_color_e_window_normal_base ].key = string8_c( mode_e_static, "e_window_normal_base" );
		_shared_colors[ menu_shared_color_e_window_normal_base ].index = menu_shared_color_e_window_normal_base;
		_shared_colors[ menu_shared_color_e_window_normal_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_window_normal_accent ].key = string8_c( mode_e_static, "e_window_normal_accent" );
		_shared_colors[ menu_shared_color_e_window_normal_accent ].index = menu_shared_color_e_window_normal_accent;
		_shared_colors[ menu_shared_color_e_window_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_window_selected_base ].key = string8_c( mode_e_static, "e_window_selected_base" );
		_shared_colors[ menu_shared_color_e_window_selected_base ].index = menu_shared_color_e_window_selected_base;
		_shared_colors[ menu_shared_color_e_window_selected_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_window_selected_accent ].key = string8_c( mode_e_static, "e_window_selected_accent" );
		_shared_colors[ menu_shared_color_e_window_selected_accent ].index = menu_shared_color_e_window_selected_accent;
		_shared_colors[ menu_shared_color_e_window_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		_shared_colors[ menu_shared_color_e_window_pressed_base ].key = string8_c( mode_e_static, "e_window_pressed_base" );
		_shared_colors[ menu_shared_color_e_window_pressed_base ].index = menu_shared_color_e_window_pressed_base;
		_shared_colors[ menu_shared_color_e_window_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_window_pressed_accent ].key = string8_c( mode_e_static, "e_window_pressed_accent" );
		_shared_colors[ menu_shared_color_e_window_pressed_accent ].index = menu_shared_color_e_window_pressed_accent;
		_shared_colors[ menu_shared_color_e_window_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_window_disabled_base ].key = string8_c( mode_e_static, "e_window_disabled_base" );
		_shared_colors[ menu_shared_color_e_window_disabled_base ].index = menu_shared_color_e_window_disabled_base;
		_shared_colors[ menu_shared_color_e_window_disabled_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_window_disabled_accent ].key = string8_c( mode_e_static, "e_window_disabled_accent" );
		_shared_colors[ menu_shared_color_e_window_disabled_accent ].index = menu_shared_color_e_window_disabled_accent;
		_shared_colors[ menu_shared_color_e_window_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		_shared_colors[ menu_shared_color_e_button_normal_base ].key = string8_c( mode_e_static, "e_button_normal_base" );
		_shared_colors[ menu_shared_color_e_button_normal_base ].index = menu_shared_color_e_button_normal_base;
		_shared_colors[ menu_shared_color_e_button_normal_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_normal_accent ].key = string8_c( mode_e_static, "e_button_normal_accent" );
		_shared_colors[ menu_shared_color_e_button_normal_accent ].index = menu_shared_color_e_button_normal_accent;
		_shared_colors[ menu_shared_color_e_button_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_selected_base ].key = string8_c( mode_e_static, "e_button_selected_base" );
		_shared_colors[ menu_shared_color_e_button_selected_base ].index = menu_shared_color_e_button_selected_base;
		_shared_colors[ menu_shared_color_e_button_selected_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_selected_accent ].key = string8_c( mode_e_static, "e_button_selected_accent" );
		_shared_colors[ menu_shared_color_e_button_selected_accent ].index = menu_shared_color_e_button_selected_accent;
		_shared_colors[ menu_shared_color_e_button_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_pressed_base ].key = string8_c( mode_e_static, "e_button_pressed_base" );
		_shared_colors[ menu_shared_color_e_button_pressed_base ].index = menu_shared_color_e_button_pressed_base;
		_shared_colors[ menu_shared_color_e_button_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_pressed_accent ].key = string8_c( mode_e_static, "e_button_pressed_accent" );
		_shared_colors[ menu_shared_color_e_button_pressed_accent ].index = menu_shared_color_e_button_pressed_accent;
		_shared_colors[ menu_shared_color_e_button_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_disabled_base ].key = string8_c( mode_e_static, "e_button_disabled_base" );
		_shared_colors[ menu_shared_color_e_button_disabled_base ].index = menu_shared_color_e_button_disabled_base;
		_shared_colors[ menu_shared_color_e_button_disabled_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_disabled_accent ].key = string8_c( mode_e_static, "e_button_disabled_accent" );
		_shared_colors[ menu_shared_color_e_button_disabled_accent ].index = menu_shared_color_e_button_disabled_accent;
		_shared_colors[ menu_shared_color_e_button_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		_shared_colors[ menu_shared_color_e_button_prime_normal_base ].key = string8_c( mode_e_static, "e_button_prime_normal_base" );
		_shared_colors[ menu_shared_color_e_button_prime_normal_base ].index = menu_shared_color_e_button_prime_normal_base;
		_shared_colors[ menu_shared_color_e_button_prime_normal_base ].value = vector32x4_c( 0.863f, 1.0f, 0.467f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_prime_normal_accent ].key = string8_c( mode_e_static, "e_button_prime_normal_accent" );
		_shared_colors[ menu_shared_color_e_button_prime_normal_accent ].index = menu_shared_color_e_button_prime_normal_accent;
		_shared_colors[ menu_shared_color_e_button_prime_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_prime_selected_base ].key = string8_c( mode_e_static, "e_button_prime_selected_base" );
		_shared_colors[ menu_shared_color_e_button_prime_selected_base ].index = menu_shared_color_e_button_prime_selected_base;
		_shared_colors[ menu_shared_color_e_button_prime_selected_base ].value = vector32x4_c( 0.757f, 1.0f, 0.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_prime_selected_accent ].key = string8_c( mode_e_static, "e_button_prime_selected_accent" );
		_shared_colors[ menu_shared_color_e_button_prime_selected_accent ].index = menu_shared_color_e_button_prime_selected_accent;
		_shared_colors[ menu_shared_color_e_button_prime_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_prime_pressed_base ].key = string8_c( mode_e_static, "e_button_prime_pressed_base" );
		_shared_colors[ menu_shared_color_e_button_prime_pressed_base ].index = menu_shared_color_e_button_prime_pressed_base;
		_shared_colors[ menu_shared_color_e_button_prime_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 0.725f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_prime_pressed_accent ].key = string8_c( mode_e_static, "e_button_prime_pressed_accent" );
		_shared_colors[ menu_shared_color_e_button_prime_pressed_accent ].index = menu_shared_color_e_button_prime_pressed_accent;
		_shared_colors[ menu_shared_color_e_button_prime_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_prime_disabled_base ].key = string8_c( mode_e_static, "e_button_prime_disabled_base" );
		_shared_colors[ menu_shared_color_e_button_prime_disabled_base ].index = menu_shared_color_e_button_prime_disabled_base;
		_shared_colors[ menu_shared_color_e_button_prime_disabled_base ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );
		_shared_colors[ menu_shared_color_e_button_prime_disabled_accent ].key = string8_c( mode_e_static, "e_button_prime_disabled_accent" );
		_shared_colors[ menu_shared_color_e_button_prime_disabled_accent ].index = menu_shared_color_e_button_prime_disabled_accent;
		_shared_colors[ menu_shared_color_e_button_prime_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		_shared_colors[ menu_shared_color_e_field_normal_base ].key = string8_c( mode_e_static, "e_field_normal_base" );
		_shared_colors[ menu_shared_color_e_field_normal_base ].index = menu_shared_color_e_field_normal_base;
		_shared_colors[ menu_shared_color_e_field_normal_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_field_normal_accent ].key = string8_c( mode_e_static, "e_field_normal_accent" );
		_shared_colors[ menu_shared_color_e_field_normal_accent ].index = menu_shared_color_e_field_normal_accent;
		_shared_colors[ menu_shared_color_e_field_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_field_selected_base ].key = string8_c( mode_e_static, "e_field_selected_base" );
		_shared_colors[ menu_shared_color_e_field_selected_base ].index = menu_shared_color_e_field_selected_base;
		_shared_colors[ menu_shared_color_e_field_selected_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_field_selected_accent ].key = string8_c( mode_e_static, "e_field_selected_accent" );
		_shared_colors[ menu_shared_color_e_field_selected_accent ].index = menu_shared_color_e_field_selected_accent;
		_shared_colors[ menu_shared_color_e_field_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		_shared_colors[ menu_shared_color_e_field_pressed_base ].key = string8_c( mode_e_static, "e_field_pressed_base" );
		_shared_colors[ menu_shared_color_e_field_pressed_base ].index = menu_shared_color_e_field_pressed_base;
		_shared_colors[ menu_shared_color_e_field_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_field_pressed_accent ].key = string8_c( mode_e_static, "e_field_pressed_accent" );
		_shared_colors[ menu_shared_color_e_field_pressed_accent ].index = menu_shared_color_e_field_pressed_accent;
		_shared_colors[ menu_shared_color_e_field_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_field_disabled_base ].key = string8_c( mode_e_static, "e_field_disabled_base" );
		_shared_colors[ menu_shared_color_e_field_disabled_base ].index = menu_shared_color_e_field_disabled_base;
		_shared_colors[ menu_shared_color_e_field_disabled_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_colors[ menu_shared_color_e_field_disabled_accent ].key = string8_c( mode_e_static, "e_field_disabled_accent" );
		_shared_colors[ menu_shared_color_e_field_disabled_accent ].index = menu_shared_color_e_field_disabled_accent;
		_shared_colors[ menu_shared_color_e_field_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		for ( sint32_c i = 0; i < menu_shared_color_e_count_; i++ )
		{
			assert( _shared_colors[ i ].index == i );
			_shared_colors_map.insert( _shared_colors[ i ].key, &_shared_colors[ i ] );
		}

		_default_font = engine_c::get_instance()->get_resource_manager()->load_font( string16_c( mode_e_static, L"[e]fonts/DXLBaB-KSCpc-EUC-H.ttf" ) );
		if ( !_default_font.is_reference_set_and_loaded() )
		{
			debug_annoy( L"error", L"default_font could not be loaded." );
			return false;
		}

		_default_style_for_frame.key = "default";
		_default_style_for_frame.texture = engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( mode_e_static, L"[e]menus/atlas.png" ) );
		if ( !_default_style_for_frame.texture.is_reference_set_and_loaded() )
		{
			debug_annoy( L"error", L"default_frame_style.texture could not be loaded." );
			return false;
		}
		_default_style_for_frame.texture_map_mode = menu_style_for_frame_c::texture_map_mode_e_nine_slice_stretch;
		_default_style_for_frame.texture_map_fill_middle = true;
		_default_style_for_frame.texture_map_fill_middle = true;
		_default_style_for_frame.state_list[ 0 ].texture_map[ 0 ] = 1;
		_default_style_for_frame.state_list[ 0 ].texture_map[ 1 ] = 1;
		_default_style_for_frame.state_list[ 0 ].texture_map[ 2 ] = 24;
		_default_style_for_frame.state_list[ 0 ].texture_map[ 3 ] = 24;
		_default_style_for_frame.state_list[ 0 ].texture_map_edges[ 0 ] = 6;
		_default_style_for_frame.state_list[ 0 ].texture_map_edges[ 1 ] = 6;
		_default_style_for_frame.state_list[ 0 ].texture_map_edges[ 2 ] = 6;
		_default_style_for_frame.state_list[ 0 ].texture_map_edges[ 3 ] = 6;
		_default_style_for_frame.state_list[ 1 ].texture_map[ 0 ] = 1;
		_default_style_for_frame.state_list[ 1 ].texture_map[ 1 ] = 1;
		_default_style_for_frame.state_list[ 1 ].texture_map[ 2 ] = 24;
		_default_style_for_frame.state_list[ 1 ].texture_map[ 3 ] = 12;
		_default_style_for_frame.state_list[ 1 ].texture_map_edges[ 0 ] = 6;
		_default_style_for_frame.state_list[ 1 ].texture_map_edges[ 1 ] = 6;
		_default_style_for_frame.state_list[ 1 ].texture_map_edges[ 2 ] = 6;
		_default_style_for_frame.state_list[ 1 ].texture_map_edges[ 3 ] = 6;
		_default_style_for_frame.state_list[ 2 ].texture_map[ 0 ] = 1;
		_default_style_for_frame.state_list[ 2 ].texture_map[ 1 ] = 1;
		_default_style_for_frame.state_list[ 2 ].texture_map[ 2 ] = 24;
		_default_style_for_frame.state_list[ 2 ].texture_map[ 3 ] = 24;
		_default_style_for_frame.state_list[ 2 ].texture_map_edges[ 0 ] = 6;
		_default_style_for_frame.state_list[ 2 ].texture_map_edges[ 1 ] = 6;
		_default_style_for_frame.state_list[ 2 ].texture_map_edges[ 2 ] = 6;
		_default_style_for_frame.state_list[ 2 ].texture_map_edges[ 3 ] = 6;
		_default_style_for_frame.state_list[ 3 ].texture_map[ 0 ] = 1;
		_default_style_for_frame.state_list[ 3 ].texture_map[ 1 ] = 1;
		_default_style_for_frame.state_list[ 3 ].texture_map[ 2 ] = 24;
		_default_style_for_frame.state_list[ 3 ].texture_map[ 3 ] = 24;
		_default_style_for_frame.state_list[ 3 ].texture_map_edges[ 0 ] = 6;
		_default_style_for_frame.state_list[ 3 ].texture_map_edges[ 1 ] = 6;
		_default_style_for_frame.state_list[ 3 ].texture_map_edges[ 2 ] = 6;
		_default_style_for_frame.state_list[ 3 ].texture_map_edges[ 3 ] = 6;

		_default_style_for_text.key = "default";
		_default_style_for_text.font_is_defined = true;
		_default_style_for_text.font = _default_font;
		_default_style_for_text.size_is_defined = true;
		_default_style_for_text.size = 16;
		_default_style_for_text.color_is_defined = true;
		_default_style_for_text.color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_default_style_for_text.skew_is_defined = true;
		_default_style_for_text.skew = 0;
		_default_style_for_text.weight_is_defined = true;
		_default_style_for_text.weight = 0;
		_default_style_for_text.softness_is_defined = true;
		_default_style_for_text.softness = 0;
		_default_style_for_text.underline_is_defined = true;
		_default_style_for_text.underline = 0;
		_default_style_for_text.paragraph_spacing_is_defined = true;
		_default_style_for_text.paragraph_spacing = 10.0f;
		_default_style_for_text.line_spacing_is_defined = true;
		_default_style_for_text.line_spacing = 0.0f;
		_default_style_for_text.glyph_spacing_is_defined = true;
		_default_style_for_text.glyph_spacing = 0.0f;
		_default_style_for_text.text_align_horizontal_is_defined = true;
		_default_style_for_text.text_align_horizontal = menu_text_align_horizontal_e_center;
		_default_style_for_text.text_align_vertical_is_defined = true;
		_default_style_for_text.text_align_vertical = menu_text_align_vertical_e_center;

		return true;
	}

	void_c menu_style_manager_c::refresh()
	{
		_engine_styles.refresh();
		_game_styles.refresh();
		menu_control_c::_global_resolve_style_maps();
	}

	menu_style_for_frame_c const * menu_style_manager_c::find_style_for_frame( string8_c const & key )
	{
		if ( key == "[none]" )
		{
			return nullptr;
		}
		else if ( key == "[default]" )
		{
			return engine_c::get_instance()->get_menu_style_manager()->get_default_style_for_frame();
		}
		if ( key.get_length() > 0 )
		{
			menu_style_file_c * style_file_list[ 2 ] = { &_game_styles, &_engine_styles };
			for ( sint32_c i = 0; i < 2; i++ )
			{
				menu_style_for_frame_c const * result = style_file_list[ i ]->_frame_style_dictionary.find_value_else_nullptr( key );
				if ( result )
				{
					return result;
				}
			}
		}
		return nullptr;
	}

	menu_style_for_text_c const * menu_style_manager_c::find_style_for_text( string8_c const & key )
	{
		if ( key.character_list.get_length() > 1 )
		{
			menu_style_file_c * style_file_list[ 2 ] = { &_game_styles, &_engine_styles };
			for ( sint32_c i = 0; i < 2; i++ )
			{
				menu_style_for_text_c const * result = style_file_list[ i ]->_text_style_dictionary.find_value_else_nullptr( key );
				if ( result )
				{
					return result;
				}
			}
		}
		return nullptr;
	}

	menu_style_map_c const * menu_style_manager_c::find_style_map( string8_c const & key )
	{
		if ( key.character_list.get_length() > 1 )
		{
			menu_style_file_c * style_file_list[ 2 ] = { &_game_styles, &_engine_styles };
			for ( sint32_c i = 0; i < 2; i++ )
			{
				menu_style_map_c const * result = style_file_list[ i ]->_style_map_dictionary.find_value_else_nullptr( key );
				if ( result )
				{
					return result;
				}
			}
		}
		return nullptr;
	}

}
