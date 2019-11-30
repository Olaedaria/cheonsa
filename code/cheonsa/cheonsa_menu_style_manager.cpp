#include "cheonsa_menu_style_manager.h"
#include "cheonsa__ops.h"
#include "cheonsa_menu_control.h"
#include "cheonsa_engine.h"
#include "cheonsa_menu_style_file.h"

namespace cheonsa
{

	menu_style_manager_c::menu_style_manager_c()
		: _shared_transition_speed( 6.0f )
		, _shared_color_style_list()
		, _shared_color_style_dictionary()
		, _default_font()
		, _default_frame_style()
		, _default_text_style()
		, _engine_styles( string16_c( mode_e_static, L"[e]menus/styles.xml" ) )
		, _game_styles( string16_c( mode_e_static, L"[g]menus/styles.xml" ) )
	{	
	}

	menu_style_manager_c::~menu_style_manager_c()
	{
	}

	boolean_c menu_style_manager_c::start()
	{
		_shared_color_style_list[ menu_shared_color_e_window_normal_base ].key = string8_c( mode_e_static, "e_window_normal_base" );
		_shared_color_style_list[ menu_shared_color_e_window_normal_base ].index = menu_shared_color_e_window_normal_base;
		_shared_color_style_list[ menu_shared_color_e_window_normal_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_window_normal_accent ].key = string8_c( mode_e_static, "e_window_normal_accent" );
		_shared_color_style_list[ menu_shared_color_e_window_normal_accent ].index = menu_shared_color_e_window_normal_accent;
		_shared_color_style_list[ menu_shared_color_e_window_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_window_selected_base ].key = string8_c( mode_e_static, "e_window_selected_base" );
		_shared_color_style_list[ menu_shared_color_e_window_selected_base ].index = menu_shared_color_e_window_selected_base;
		_shared_color_style_list[ menu_shared_color_e_window_selected_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_window_selected_accent ].key = string8_c( mode_e_static, "e_window_selected_accent" );
		_shared_color_style_list[ menu_shared_color_e_window_selected_accent ].index = menu_shared_color_e_window_selected_accent;
		_shared_color_style_list[ menu_shared_color_e_window_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_window_pressed_base ].key = string8_c( mode_e_static, "e_window_pressed_base" );
		_shared_color_style_list[ menu_shared_color_e_window_pressed_base ].index = menu_shared_color_e_window_pressed_base;
		_shared_color_style_list[ menu_shared_color_e_window_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_window_pressed_accent ].key = string8_c( mode_e_static, "e_window_pressed_accent" );
		_shared_color_style_list[ menu_shared_color_e_window_pressed_accent ].index = menu_shared_color_e_window_pressed_accent;
		_shared_color_style_list[ menu_shared_color_e_window_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_window_disabled_base ].key = string8_c( mode_e_static, "e_window_disabled_base" );
		_shared_color_style_list[ menu_shared_color_e_window_disabled_base ].index = menu_shared_color_e_window_disabled_base;
		_shared_color_style_list[ menu_shared_color_e_window_disabled_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_window_disabled_accent ].key = string8_c( mode_e_static, "e_window_disabled_accent" );
		_shared_color_style_list[ menu_shared_color_e_window_disabled_accent ].index = menu_shared_color_e_window_disabled_accent;
		_shared_color_style_list[ menu_shared_color_e_window_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		_shared_color_style_list[ menu_shared_color_e_button_normal_base ].key = string8_c( mode_e_static, "e_button_normal_base" );
		_shared_color_style_list[ menu_shared_color_e_button_normal_base ].index = menu_shared_color_e_button_normal_base;
		_shared_color_style_list[ menu_shared_color_e_button_normal_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_normal_accent ].key = string8_c( mode_e_static, "e_button_normal_accent" );
		_shared_color_style_list[ menu_shared_color_e_button_normal_accent ].index = menu_shared_color_e_button_normal_accent;
		_shared_color_style_list[ menu_shared_color_e_button_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_selected_base ].key = string8_c( mode_e_static, "e_button_selected_base" );
		_shared_color_style_list[ menu_shared_color_e_button_selected_base ].index = menu_shared_color_e_button_selected_base;
		_shared_color_style_list[ menu_shared_color_e_button_selected_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_selected_accent ].key = string8_c( mode_e_static, "e_button_selected_accent" );
		_shared_color_style_list[ menu_shared_color_e_button_selected_accent ].index = menu_shared_color_e_button_selected_accent;
		_shared_color_style_list[ menu_shared_color_e_button_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_pressed_base ].key = string8_c( mode_e_static, "e_button_pressed_base" );
		_shared_color_style_list[ menu_shared_color_e_button_pressed_base ].index = menu_shared_color_e_button_pressed_base;
		_shared_color_style_list[ menu_shared_color_e_button_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_pressed_accent ].key = string8_c( mode_e_static, "e_button_pressed_accent" );
		_shared_color_style_list[ menu_shared_color_e_button_pressed_accent ].index = menu_shared_color_e_button_pressed_accent;
		_shared_color_style_list[ menu_shared_color_e_button_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_disabled_base ].key = string8_c( mode_e_static, "e_button_disabled_base" );
		_shared_color_style_list[ menu_shared_color_e_button_disabled_base ].index = menu_shared_color_e_button_disabled_base;
		_shared_color_style_list[ menu_shared_color_e_button_disabled_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_disabled_accent ].key = string8_c( mode_e_static, "e_button_disabled_accent" );
		_shared_color_style_list[ menu_shared_color_e_button_disabled_accent ].index = menu_shared_color_e_button_disabled_accent;
		_shared_color_style_list[ menu_shared_color_e_button_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		_shared_color_style_list[ menu_shared_color_e_button_prime_normal_base ].key = string8_c( mode_e_static, "e_button_prime_normal_base" );
		_shared_color_style_list[ menu_shared_color_e_button_prime_normal_base ].index = menu_shared_color_e_button_prime_normal_base;
		_shared_color_style_list[ menu_shared_color_e_button_prime_normal_base ].value = vector32x4_c( 0.863f, 1.0f, 0.467f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_prime_normal_accent ].key = string8_c( mode_e_static, "e_button_prime_normal_accent" );
		_shared_color_style_list[ menu_shared_color_e_button_prime_normal_accent ].index = menu_shared_color_e_button_prime_normal_accent;
		_shared_color_style_list[ menu_shared_color_e_button_prime_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_prime_selected_base ].key = string8_c( mode_e_static, "e_button_prime_selected_base" );
		_shared_color_style_list[ menu_shared_color_e_button_prime_selected_base ].index = menu_shared_color_e_button_prime_selected_base;
		_shared_color_style_list[ menu_shared_color_e_button_prime_selected_base ].value = vector32x4_c( 0.757f, 1.0f, 0.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_prime_selected_accent ].key = string8_c( mode_e_static, "e_button_prime_selected_accent" );
		_shared_color_style_list[ menu_shared_color_e_button_prime_selected_accent ].index = menu_shared_color_e_button_prime_selected_accent;
		_shared_color_style_list[ menu_shared_color_e_button_prime_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_prime_pressed_base ].key = string8_c( mode_e_static, "e_button_prime_pressed_base" );
		_shared_color_style_list[ menu_shared_color_e_button_prime_pressed_base ].index = menu_shared_color_e_button_prime_pressed_base;
		_shared_color_style_list[ menu_shared_color_e_button_prime_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 0.725f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_prime_pressed_accent ].key = string8_c( mode_e_static, "e_button_prime_pressed_accent" );
		_shared_color_style_list[ menu_shared_color_e_button_prime_pressed_accent ].index = menu_shared_color_e_button_prime_pressed_accent;
		_shared_color_style_list[ menu_shared_color_e_button_prime_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_prime_disabled_base ].key = string8_c( mode_e_static, "e_button_prime_disabled_base" );
		_shared_color_style_list[ menu_shared_color_e_button_prime_disabled_base ].index = menu_shared_color_e_button_prime_disabled_base;
		_shared_color_style_list[ menu_shared_color_e_button_prime_disabled_base ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_button_prime_disabled_accent ].key = string8_c( mode_e_static, "e_button_prime_disabled_accent" );
		_shared_color_style_list[ menu_shared_color_e_button_prime_disabled_accent ].index = menu_shared_color_e_button_prime_disabled_accent;
		_shared_color_style_list[ menu_shared_color_e_button_prime_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		_shared_color_style_list[ menu_shared_color_e_field_normal_base ].key = string8_c( mode_e_static, "e_field_normal_base" );
		_shared_color_style_list[ menu_shared_color_e_field_normal_base ].index = menu_shared_color_e_field_normal_base;
		_shared_color_style_list[ menu_shared_color_e_field_normal_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_field_normal_accent ].key = string8_c( mode_e_static, "e_field_normal_accent" );
		_shared_color_style_list[ menu_shared_color_e_field_normal_accent ].index = menu_shared_color_e_field_normal_accent;
		_shared_color_style_list[ menu_shared_color_e_field_normal_accent ].value = vector32x4_c( 0.467f, 0.953f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_field_selected_base ].key = string8_c( mode_e_static, "e_field_selected_base" );
		_shared_color_style_list[ menu_shared_color_e_field_selected_base ].index = menu_shared_color_e_field_selected_base;
		_shared_color_style_list[ menu_shared_color_e_field_selected_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_field_selected_accent ].key = string8_c( mode_e_static, "e_field_selected_accent" );
		_shared_color_style_list[ menu_shared_color_e_field_selected_accent ].index = menu_shared_color_e_field_selected_accent;
		_shared_color_style_list[ menu_shared_color_e_field_selected_accent ].value = vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_field_pressed_base ].key = string8_c( mode_e_static, "e_field_pressed_base" );
		_shared_color_style_list[ menu_shared_color_e_field_pressed_base ].index = menu_shared_color_e_field_pressed_base;
		_shared_color_style_list[ menu_shared_color_e_field_pressed_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_field_pressed_accent ].key = string8_c( mode_e_static, "e_field_pressed_accent" );
		_shared_color_style_list[ menu_shared_color_e_field_pressed_accent ].index = menu_shared_color_e_field_pressed_accent;
		_shared_color_style_list[ menu_shared_color_e_field_pressed_accent ].value = vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_field_disabled_base ].key = string8_c( mode_e_static, "e_field_disabled_base" );
		_shared_color_style_list[ menu_shared_color_e_field_disabled_base ].index = menu_shared_color_e_field_disabled_base;
		_shared_color_style_list[ menu_shared_color_e_field_disabled_base ].value = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_shared_color_style_list[ menu_shared_color_e_field_disabled_accent ].key = string8_c( mode_e_static, "e_field_disabled_accent" );
		_shared_color_style_list[ menu_shared_color_e_field_disabled_accent ].index = menu_shared_color_e_field_disabled_accent;
		_shared_color_style_list[ menu_shared_color_e_field_disabled_accent ].value = vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f );

		for ( sint32_c i = 0; i < menu_shared_color_e_count_; i++ )
		{
			assert( _shared_color_style_list[ i ].index == i );
			_shared_color_style_dictionary.insert( _shared_color_style_list[ i ].key, &_shared_color_style_list[ i ] );
		}

		_default_font = engine_c::get_instance()->get_resource_manager()->load_font( string16_c( mode_e_static, L"[e]fonts/DXLBaB-KSCpc-EUC-H.ttf" ) );
		if ( !_default_font.is_reference_set_and_loaded() )
		{
			debug_annoy( L"error", L"default_font could not be loaded." );
			return false;
		}

		_default_frame_style.key = string8_c( mode_e_static, "default" );
		_default_frame_style.texture = engine_c::get_instance()->get_resource_manager()->load_texture( string16_c( mode_e_static, L"[e]menus/atlas.png" ) );
		if ( !_default_frame_style.texture.is_reference_set_and_loaded() )
		{
			debug_annoy( L"error", L"default_frame_style.texture could not be loaded." );
			return false;
		}
		_default_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_nine_slice_stretch;
		_default_frame_style.texture_map_fill_middle = true;
		_default_frame_style.texture_map_fill_middle = true;
		_default_frame_style.state_list[ 0 ].texture_map[ 0 ] = 1;
		_default_frame_style.state_list[ 0 ].texture_map[ 1 ] = 1;
		_default_frame_style.state_list[ 0 ].texture_map[ 2 ] = 24;
		_default_frame_style.state_list[ 0 ].texture_map[ 3 ] = 24;
		_default_frame_style.state_list[ 0 ].texture_map_edges[ 0 ] = 6;
		_default_frame_style.state_list[ 0 ].texture_map_edges[ 1 ] = 6;
		_default_frame_style.state_list[ 0 ].texture_map_edges[ 2 ] = 6;
		_default_frame_style.state_list[ 0 ].texture_map_edges[ 3 ] = 6;
		_default_frame_style.state_list[ 1 ].texture_map[ 0 ] = 1;
		_default_frame_style.state_list[ 1 ].texture_map[ 1 ] = 1;
		_default_frame_style.state_list[ 1 ].texture_map[ 2 ] = 24;
		_default_frame_style.state_list[ 1 ].texture_map[ 3 ] = 12;
		_default_frame_style.state_list[ 1 ].texture_map_edges[ 0 ] = 6;
		_default_frame_style.state_list[ 1 ].texture_map_edges[ 1 ] = 6;
		_default_frame_style.state_list[ 1 ].texture_map_edges[ 2 ] = 6;
		_default_frame_style.state_list[ 1 ].texture_map_edges[ 3 ] = 6;
		_default_frame_style.state_list[ 2 ].texture_map[ 0 ] = 1;
		_default_frame_style.state_list[ 2 ].texture_map[ 1 ] = 1;
		_default_frame_style.state_list[ 2 ].texture_map[ 2 ] = 24;
		_default_frame_style.state_list[ 2 ].texture_map[ 3 ] = 24;
		_default_frame_style.state_list[ 2 ].texture_map_edges[ 0 ] = 6;
		_default_frame_style.state_list[ 2 ].texture_map_edges[ 1 ] = 6;
		_default_frame_style.state_list[ 2 ].texture_map_edges[ 2 ] = 6;
		_default_frame_style.state_list[ 2 ].texture_map_edges[ 3 ] = 6;
		_default_frame_style.state_list[ 3 ].texture_map[ 0 ] = 1;
		_default_frame_style.state_list[ 3 ].texture_map[ 1 ] = 1;
		_default_frame_style.state_list[ 3 ].texture_map[ 2 ] = 24;
		_default_frame_style.state_list[ 3 ].texture_map[ 3 ] = 24;
		_default_frame_style.state_list[ 3 ].texture_map_edges[ 0 ] = 6;
		_default_frame_style.state_list[ 3 ].texture_map_edges[ 1 ] = 6;
		_default_frame_style.state_list[ 3 ].texture_map_edges[ 2 ] = 6;
		_default_frame_style.state_list[ 3 ].texture_map_edges[ 3 ] = 6;

		_default_text_style.key = string8_c( mode_e_static, "default" );
		_default_text_style.font_is_defined = true;
		_default_text_style.font = _default_font;
		_default_text_style.size_is_defined = true;
		_default_text_style.size = 16;
		_default_text_style.color_is_defined = true;
		_default_text_style.color = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		_default_text_style.skew_is_defined = true;
		_default_text_style.skew = 0;
		_default_text_style.weight_is_defined = true;
		_default_text_style.weight = 0;
		_default_text_style.softness_is_defined = true;
		_default_text_style.softness = 0;
		_default_text_style.underline_is_defined = true;
		_default_text_style.underline = 0;
		_default_text_style.paragraph_spacing_is_defined = true;
		_default_text_style.paragraph_spacing = 10.0f;
		_default_text_style.line_spacing_is_defined = true;
		_default_text_style.line_spacing = 0.0f;
		_default_text_style.glyph_spacing_is_defined = true;
		_default_text_style.glyph_spacing = 0.0f;
		_default_text_style.text_align_horizontal_is_defined = true;
		_default_text_style.text_align_horizontal = menu_text_align_horizontal_e_center;
		_default_text_style.text_align_vertical_is_defined = true;
		_default_text_style.text_align_vertical = menu_text_align_vertical_e_center;

		return true;
	}

	void_c menu_style_manager_c::refresh()
	{
		_engine_styles.refresh();
		_game_styles.refresh();
		menu_control_c::_global_resolve_style_maps();
	}

	menu_color_style_c * menu_style_manager_c::find_shared_color_style( menu_shared_color_e index )
	{
		assert( index >= 0 && index < menu_shared_color_e_count_ );
		return &_shared_color_style_list[ index ];
	}

	menu_color_style_c * menu_style_manager_c::find_shared_color_style( string8_c key )
	{
		return _shared_color_style_dictionary.find_value_else_nullptr( key );
	}

	menu_color_style_c const * menu_style_manager_c::find_color_style( string8_c const & key ) const
	{
		if ( key != "" )
		{
			menu_style_file_c const * style_file_list[ 2 ] = { &_game_styles, &_engine_styles };
			for ( sint32_c i = 0; i < 2; i++ )
			{
				menu_color_style_c const * result = style_file_list[ i ]->_color_style_dictionary.find_value_else_nullptr( key );
				if ( result )
				{
					return result;
				}
			}
			return _shared_color_style_dictionary.find_value_else_nullptr( key );
		}
		return nullptr;
	}

	menu_frame_style_c const * menu_style_manager_c::find_frame_style( string8_c const & key ) const
	{
		if ( key == "" || key == "[none]" )
		{
			return nullptr;
		}
		else if ( key == "[default]" )
		{
			return &_default_frame_style;
		}
		if ( key.get_length() > 0 )
		{
			menu_style_file_c const * style_file_list[ 2 ] = { &_game_styles, &_engine_styles };
			for ( sint32_c i = 0; i < 2; i++ )
			{
				menu_frame_style_c const * result = style_file_list[ i ]->_frame_style_dictionary.find_value_else_nullptr( key );
				if ( result )
				{
					return result;
				}
			}
		}
		return nullptr;
	}

	menu_text_style_c const * menu_style_manager_c::find_text_style( string8_c const & key ) const
	{
		if ( key == "" || key == "[none]" )
		{
			return nullptr;
		}
		else if ( key == "[default]" )
		{
			return &_default_text_style;
		}
		else
		{
			if ( key.character_list.get_length() > 1 )
			{
				menu_style_file_c const * style_file_list[ 2 ] = { &_game_styles, &_engine_styles };
				for ( sint32_c i = 0; i < 2; i++ )
				{
					menu_text_style_c const * result = style_file_list[ i ]->_text_style_dictionary.find_value_else_nullptr( key );
					if ( result )
					{
						return result;
					}
				}
			}
		}
		return nullptr;
	}

	menu_style_map_c const * menu_style_manager_c::find_style_map( string8_c const & key ) const
	{
		if ( key.character_list.get_length() > 1 )
		{
			menu_style_file_c const * style_file_list[ 2 ] = { &_game_styles, &_engine_styles };
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
