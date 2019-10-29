#include "cheonsa_menu_style_manager.h"
#include "cheonsa_ops.h"
#include "cheonsa_menu_control.h"
#include "cheonsa_engine.h"
#include "cheonsa_menu_style_for_frame.h"
#include "cheonsa_menu_style_for_text.h"
#include "cheonsa_menu_style_file.h"

namespace cheonsa
{

	menu_style_manager_c::menu_style_manager_c()
		: default_font()
		, default_frame_style()
		, default_text_style()
		, _engine_styles( string16_c( mode_e_static, L"[e]menus/styles.xml" ) )
		, _game_styles( string16_c( mode_e_static, L"[g]menus/styles.xml" ) )
		, shared_colors()
		, shared_transition_speed( 6.0f )
	{	
	}

	menu_style_manager_c::~menu_style_manager_c()
	{
	}

	boolean_c menu_style_manager_c::start()
	{
		shared_colors[ menu_color_e_window_normal_base ].key = string8_c( mode_e_static, "e_window_normal_base" );
		shared_colors[ menu_color_e_window_normal_base ].index = menu_color_e_window_normal_base;
		shared_colors[ menu_color_e_window_normal_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_window_normal_accent ].key = string8_c( mode_e_static, "e_window_normal_accent" );
		shared_colors[ menu_color_e_window_normal_accent ].index = menu_color_e_window_normal_accent;
		shared_colors[ menu_color_e_window_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_window_selected_base ].key = string8_c( mode_e_static, "e_window_selected_base" );
		shared_colors[ menu_color_e_window_selected_base ].index = menu_color_e_window_selected_base;
		shared_colors[ menu_color_e_window_selected_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_window_selected_accent ].key = string8_c( mode_e_static, "e_window_selected_accent" );
		shared_colors[ menu_color_e_window_selected_accent ].index = menu_color_e_window_selected_accent;
		shared_colors[ menu_color_e_window_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		shared_colors[ menu_color_e_window_pressed_base ].key = string8_c( mode_e_static, "e_window_pressed_base" );
		shared_colors[ menu_color_e_window_pressed_base ].index = menu_color_e_window_pressed_base;
		shared_colors[ menu_color_e_window_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_window_pressed_accent ].key = string8_c( mode_e_static, "e_window_pressed_accent" );
		shared_colors[ menu_color_e_window_pressed_accent ].index = menu_color_e_window_pressed_accent;
		shared_colors[ menu_color_e_window_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_window_disabled_base ].key = string8_c( mode_e_static, "e_window_disabled_base" );
		shared_colors[ menu_color_e_window_disabled_base ].index = menu_color_e_window_disabled_base;
		shared_colors[ menu_color_e_window_disabled_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_window_disabled_accent ].key = string8_c( mode_e_static, "e_window_disabled_accent" );
		shared_colors[ menu_color_e_window_disabled_accent ].index = menu_color_e_window_disabled_accent;
		shared_colors[ menu_color_e_window_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		shared_colors[ menu_color_e_button_normal_base ].key = string8_c( mode_e_static, "e_button_normal_base" );
		shared_colors[ menu_color_e_button_normal_base ].index = menu_color_e_button_normal_base;
		shared_colors[ menu_color_e_button_normal_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_button_normal_accent ].key = string8_c( mode_e_static, "e_button_normal_accent" );
		shared_colors[ menu_color_e_button_normal_accent ].index = menu_color_e_button_normal_accent;
		shared_colors[ menu_color_e_button_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_button_selected_base ].key = string8_c( mode_e_static, "e_button_selected_base" );
		shared_colors[ menu_color_e_button_selected_base ].index = menu_color_e_button_selected_base;
		shared_colors[ menu_color_e_button_selected_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_button_selected_accent ].key = string8_c( mode_e_static, "e_button_selected_accent" );
		shared_colors[ menu_color_e_button_selected_accent ].index = menu_color_e_button_selected_accent;
		shared_colors[ menu_color_e_button_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		shared_colors[ menu_color_e_button_pressed_base ].key = string8_c( mode_e_static, "e_button_pressed_base" );
		shared_colors[ menu_color_e_button_pressed_base ].index = menu_color_e_button_pressed_base;
		shared_colors[ menu_color_e_button_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_button_pressed_accent ].key = string8_c( mode_e_static, "e_button_pressed_accent" );
		shared_colors[ menu_color_e_button_pressed_accent ].index = menu_color_e_button_pressed_accent;
		shared_colors[ menu_color_e_button_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_button_disabled_base ].key = string8_c( mode_e_static, "e_button_disabled_base" );
		shared_colors[ menu_color_e_button_disabled_base ].index = menu_color_e_button_disabled_base;
		shared_colors[ menu_color_e_button_disabled_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_button_disabled_accent ].key = string8_c( mode_e_static, "e_button_disabled_accent" );
		shared_colors[ menu_color_e_button_disabled_accent ].index = menu_color_e_button_disabled_accent;
		shared_colors[ menu_color_e_button_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		shared_colors[ menu_color_e_button_prime_normal_base ].key = string8_c( mode_e_static, "e_button_prime_normal_base" );
		shared_colors[ menu_color_e_button_prime_normal_base ].index = menu_color_e_button_prime_normal_base;
		shared_colors[ menu_color_e_button_prime_normal_base ].value = vector32x4_c( 0.863f, 1.0f, 0.467f, 1.0f );
		shared_colors[ menu_color_e_button_prime_normal_accent ].key = string8_c( mode_e_static, "e_button_prime_normal_accent" );
		shared_colors[ menu_color_e_button_prime_normal_accent ].index = menu_color_e_button_prime_normal_accent;
		shared_colors[ menu_color_e_button_prime_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_button_prime_selected_base ].key = string8_c( mode_e_static, "e_button_prime_selected_base" );
		shared_colors[ menu_color_e_button_prime_selected_base ].index = menu_color_e_button_prime_selected_base;
		shared_colors[ menu_color_e_button_prime_selected_base ].value = vector32x4_c( 0.757f, 1.0f, 0.0f, 1.0f );
		shared_colors[ menu_color_e_button_prime_selected_accent ].key = string8_c( mode_e_static, "e_button_prime_selected_accent" );
		shared_colors[ menu_color_e_button_prime_selected_accent ].index = menu_color_e_button_prime_selected_accent;
		shared_colors[ menu_color_e_button_prime_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		shared_colors[ menu_color_e_button_prime_pressed_base ].key = string8_c( mode_e_static, "e_button_prime_pressed_base" );
		shared_colors[ menu_color_e_button_prime_pressed_base ].index = menu_color_e_button_prime_pressed_base;
		shared_colors[ menu_color_e_button_prime_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 0.725f, 1.0f );
		shared_colors[ menu_color_e_button_prime_pressed_accent ].key = string8_c( mode_e_static, "e_button_prime_pressed_accent" );
		shared_colors[ menu_color_e_button_prime_pressed_accent ].index = menu_color_e_button_prime_pressed_accent;
		shared_colors[ menu_color_e_button_prime_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_button_prime_disabled_base ].key = string8_c( mode_e_static, "e_button_prime_disabled_base" );
		shared_colors[ menu_color_e_button_prime_disabled_base ].index = menu_color_e_button_prime_disabled_base;
		shared_colors[ menu_color_e_button_prime_disabled_base ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );
		shared_colors[ menu_color_e_button_prime_disabled_accent ].key = string8_c( mode_e_static, "e_button_prime_disabled_accent" );
		shared_colors[ menu_color_e_button_prime_disabled_accent ].index = menu_color_e_button_prime_disabled_accent;
		shared_colors[ menu_color_e_button_prime_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		shared_colors[ menu_color_e_field_normal_base ].key = string8_c( mode_e_static, "e_field_normal_base" );
		shared_colors[ menu_color_e_field_normal_base ].index = menu_color_e_field_normal_base;
		shared_colors[ menu_color_e_field_normal_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_field_normal_accent ].key = string8_c( mode_e_static, "e_field_normal_accent" );
		shared_colors[ menu_color_e_field_normal_accent ].index = menu_color_e_field_normal_accent;
		shared_colors[ menu_color_e_field_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_field_selected_base ].key = string8_c( mode_e_static, "e_field_selected_base" );
		shared_colors[ menu_color_e_field_selected_base ].index = menu_color_e_field_selected_base;
		shared_colors[ menu_color_e_field_selected_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_field_selected_accent ].key = string8_c( mode_e_static, "e_field_selected_accent" );
		shared_colors[ menu_color_e_field_selected_accent ].index = menu_color_e_field_selected_accent;
		shared_colors[ menu_color_e_field_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		shared_colors[ menu_color_e_field_pressed_base ].key = string8_c( mode_e_static, "e_field_pressed_base" );
		shared_colors[ menu_color_e_field_pressed_base ].index = menu_color_e_field_pressed_base;
		shared_colors[ menu_color_e_field_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_field_pressed_accent ].key = string8_c( mode_e_static, "e_field_pressed_accent" );
		shared_colors[ menu_color_e_field_pressed_accent ].index = menu_color_e_field_pressed_accent;
		shared_colors[ menu_color_e_field_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_field_disabled_base ].key = string8_c( mode_e_static, "e_field_disabled_base" );
		shared_colors[ menu_color_e_field_disabled_base ].index = menu_color_e_field_disabled_base;
		shared_colors[ menu_color_e_field_disabled_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_colors[ menu_color_e_field_disabled_accent ].key = string8_c( mode_e_static, "e_field_disabled_accent" );
		shared_colors[ menu_color_e_field_disabled_accent ].index = menu_color_e_field_disabled_accent;
		shared_colors[ menu_color_e_field_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		for ( sint32_c i = 0; i < menu_color_e_count_; i++ )
		{
			assert( shared_colors[ i ].index == i );
			shared_colors_map.insert( shared_colors[ i ].key, &shared_colors[ i ] );
		}

		default_font = global_engine_instance.interfaces.resource_manager->load_font( string16_c( mode_e_static, L"[e]fonts/DXLBaB-KSCpc-EUC-H.ttf" ) );
		if ( !default_font.is_reference_set_and_loaded() )
		{
			cheonsa_annoy( L"error", L"default_font could not be loaded." );
			return false;
		}

		default_frame_style.key = "default";
		default_frame_style.texture = global_engine_instance.interfaces.resource_manager->load_texture( string16_c( mode_e_static, L"[e]menus/atlas.png" ) );
		if ( !default_frame_style.texture.is_reference_set_and_loaded() )
		{
			cheonsa_annoy( L"error", L"default_frame_style.texture could not be loaded." );
			return false;
		}
		default_frame_style.texture_map_mode = menu_style_for_frame_c::texture_map_mode_e_nine_slice_stretch;
		default_frame_style.texture_map_fill_middle = true;
		default_frame_style.texture_map_fill_middle = true;
		default_frame_style.state_list[ 0 ].texture_map[ 0 ] = 1;
		default_frame_style.state_list[ 0 ].texture_map[ 1 ] = 1;
		default_frame_style.state_list[ 0 ].texture_map[ 2 ] = 24;
		default_frame_style.state_list[ 0 ].texture_map[ 3 ] = 24;
		default_frame_style.state_list[ 0 ].texture_map_edges[ 0 ] = 6;
		default_frame_style.state_list[ 0 ].texture_map_edges[ 1 ] = 6;
		default_frame_style.state_list[ 0 ].texture_map_edges[ 2 ] = 6;
		default_frame_style.state_list[ 0 ].texture_map_edges[ 3 ] = 6;
		default_frame_style.state_list[ 1 ].texture_map[ 0 ] = 1;
		default_frame_style.state_list[ 1 ].texture_map[ 1 ] = 1;
		default_frame_style.state_list[ 1 ].texture_map[ 2 ] = 24;
		default_frame_style.state_list[ 1 ].texture_map[ 3 ] = 12;
		default_frame_style.state_list[ 1 ].texture_map_edges[ 0 ] = 6;
		default_frame_style.state_list[ 1 ].texture_map_edges[ 1 ] = 6;
		default_frame_style.state_list[ 1 ].texture_map_edges[ 2 ] = 6;
		default_frame_style.state_list[ 1 ].texture_map_edges[ 3 ] = 6;
		default_frame_style.state_list[ 2 ].texture_map[ 0 ] = 1;
		default_frame_style.state_list[ 2 ].texture_map[ 1 ] = 1;
		default_frame_style.state_list[ 2 ].texture_map[ 2 ] = 24;
		default_frame_style.state_list[ 2 ].texture_map[ 3 ] = 24;
		default_frame_style.state_list[ 2 ].texture_map_edges[ 0 ] = 6;
		default_frame_style.state_list[ 2 ].texture_map_edges[ 1 ] = 6;
		default_frame_style.state_list[ 2 ].texture_map_edges[ 2 ] = 6;
		default_frame_style.state_list[ 2 ].texture_map_edges[ 3 ] = 6;
		default_frame_style.state_list[ 3 ].texture_map[ 0 ] = 1;
		default_frame_style.state_list[ 3 ].texture_map[ 1 ] = 1;
		default_frame_style.state_list[ 3 ].texture_map[ 2 ] = 24;
		default_frame_style.state_list[ 3 ].texture_map[ 3 ] = 24;
		default_frame_style.state_list[ 3 ].texture_map_edges[ 0 ] = 6;
		default_frame_style.state_list[ 3 ].texture_map_edges[ 1 ] = 6;
		default_frame_style.state_list[ 3 ].texture_map_edges[ 2 ] = 6;
		default_frame_style.state_list[ 3 ].texture_map_edges[ 3 ] = 6;

		default_text_style.key = "default";
		default_text_style.font_is_defined = true;
		default_text_style.font = default_font;
		default_text_style.size_is_defined = true;
		default_text_style.size = 16;
		default_text_style.color_is_defined = true;
		default_text_style.color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		default_text_style.skew_is_defined = true;
		default_text_style.skew = 0;
		default_text_style.weight_is_defined = true;
		default_text_style.weight = 0;
		default_text_style.softness_is_defined = true;
		default_text_style.softness = 0;
		default_text_style.underline_is_defined = true;
		default_text_style.underline = 0;
		default_text_style.paragraph_spacing_is_defined = true;
		default_text_style.paragraph_spacing = 10.0f;
		default_text_style.line_spacing_is_defined = true;
		default_text_style.line_spacing = 0.0f;
		default_text_style.glyph_spacing_is_defined = true;
		default_text_style.glyph_spacing = 0.0f;
		default_text_style.text_align_horizontal_is_defined = true;
		default_text_style.text_align_horizontal = menu_text_align_horizontal_e_center;
		default_text_style.text_align_vertical_is_defined = true;
		default_text_style.text_align_vertical = menu_text_align_vertical_e_center;

		//refresh();

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
			return &global_engine_instance.interfaces.menu_style_manager->default_frame_style;
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
