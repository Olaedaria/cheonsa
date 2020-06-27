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
		, _engine_styles( string16_c( L"menus/styles.xml", core_list_mode_e_static ) )
		, _game_styles( string16_c( L"menus/styles.xml", core_list_mode_e_static ) )
	{
		_engine_styles.set_search_engine_data( true );
		_engine_styles.set_search_game_data( false );
		_game_styles.set_search_engine_data( false );
		_game_styles.set_search_game_data( true );
	}

	menu_style_manager_c::~menu_style_manager_c()
	{
	}

	boolean_c menu_style_manager_c::start()
	{
		// initialize shared color styles.
		sint32_c shared_color_style_index = 0;
		// window
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_normal, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_normal_primary", core_list_mode_e_static ), vector32x4_c( 1.0000f, 1.0000f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_normal, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_normal_secondary", core_list_mode_e_static ), vector32x4_c( 0.7500f, 0.7500f, 0.7500f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_normal, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_normal_accent", core_list_mode_e_static ), vector32x4_c( 0.7500f, 0.7500f, 0.7500f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_selected, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_selected_primary", core_list_mode_e_static ), vector32x4_c( 1.0000f, 1.0000f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_selected, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_selected_secondary", core_list_mode_e_static ), vector32x4_c( 0.0000f, 0.8676f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_selected, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_selected_accent", core_list_mode_e_static ), vector32x4_c( 0.0000f, 0.8676f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_pressed, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_pressed_primary", core_list_mode_e_static ), vector32x4_c( 1.0000f, 1.0000f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_pressed, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_pressed_secondary", core_list_mode_e_static ), vector32x4_c( 0.2500f, 0.9007f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_pressed, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_pressed_accent", core_list_mode_e_static ), vector32x4_c( 0.2500f, 0.9007f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_disabled, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_disabled_primary", core_list_mode_e_static ), vector32x4_c( 0.9900f, 0.9900f, 0.9900f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_disabled, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_disabled_secondary", core_list_mode_e_static ), vector32x4_c( 0.9000f, 0.9000f, 0.9000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_disabled, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_window_disabled_accent", core_list_mode_e_static ), vector32x4_c( 0.9000f, 0.9000f, 0.9000f, 1.0000f ) );
		// button
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_normal, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_normal_primary", core_list_mode_e_static ), vector32x4_c( 1.0000f, 1.0000f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_normal, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_normal_secondary", core_list_mode_e_static ), vector32x4_c( 0.7500f, 0.7500f, 0.7500f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_normal, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_normal_accent", core_list_mode_e_static ), vector32x4_c( 0.7500f, 0.7500f, 0.7500f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_selected, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_selected_primary", core_list_mode_e_static ), vector32x4_c( 1.0000f, 1.0000f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_selected, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_selected_secondary", core_list_mode_e_static ), vector32x4_c( 0.0000f, 0.8676f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_selected, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_selected_accent", core_list_mode_e_static ), vector32x4_c( 0.0000f, 0.8676f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_pressed, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_pressed_primary", core_list_mode_e_static ), vector32x4_c( 1.0000f, 1.0000f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_pressed, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_pressed_secondary", core_list_mode_e_static ), vector32x4_c( 0.2500f, 0.9007f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_pressed, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_pressed_accent", core_list_mode_e_static ), vector32x4_c( 0.2500f, 0.9007f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_disabled, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_disabled_primary", core_list_mode_e_static ), vector32x4_c( 0.9900f, 0.9900f, 0.9900f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_disabled, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_disabled_secondary", core_list_mode_e_static ), vector32x4_c( 0.9000f, 0.9000f, 0.9000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_disabled, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_button_disabled_accent", core_list_mode_e_static ), vector32x4_c( 0.9000f, 0.9000f, 0.9000f, 1.0000f ) );
		// field
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_normal, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_normal_primary", core_list_mode_e_static ), vector32x4_c( 1.0000f, 1.0000f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_normal, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_normal_secondary", core_list_mode_e_static ), vector32x4_c( 0.7500f, 0.7500f, 0.7500f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_normal, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_normal_accent", core_list_mode_e_static ), vector32x4_c( 0.7500f, 0.7500f, 0.7500f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_selected, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_selected_primary", core_list_mode_e_static ), vector32x4_c( 1.0000f, 1.0000f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_selected, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_selected_secondary", core_list_mode_e_static ), vector32x4_c( 0.7500f, 0.7500f, 0.7500f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_selected, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_selected_accent", core_list_mode_e_static ), vector32x4_c( 0.0000f, 0.8676f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_pressed, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_pressed_primary", core_list_mode_e_static ), vector32x4_c( 1.0000f, 1.0000f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_pressed, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_pressed_secondary", core_list_mode_e_static ), vector32x4_c( 0.8000f, 0.8000f, 0.8000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_pressed, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_pressed_accent", core_list_mode_e_static ), vector32x4_c( 0.2500f, 0.9007f, 1.0000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_disabled, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_disabled_primary", core_list_mode_e_static ), vector32x4_c( 0.9900f, 0.9900f, 0.9900f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_disabled, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_disabled_secondary", core_list_mode_e_static ), vector32x4_c( 0.9000f, 0.9000f, 0.9000f, 1.0000f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_disabled, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( string8_c( "e_field_disabled_accent", core_list_mode_e_static ), vector32x4_c( 0.9000f, 0.9000f, 0.9000f, 1.0000f ) );
		// insert into dictionary.
		sint32_c shared_color_count = menu_shared_color_class_e_count_ * menu_state_e_count_ * menu_shared_color_slot_e_count_;
		for ( sint32_c i = 0; i < shared_color_count; i++ )
		{
			_shared_color_style_dictionary.insert( _shared_color_style_list[ i ].key, &_shared_color_style_list[ i ] );
		}

		// load default font.
		_default_font = engine.get_resource_manager()->load_font( string16_c( L"fonts/DXLBaB-KSCpc-EUC-H.ttf", core_list_mode_e_static ), true, false );
		if ( !_default_font.get_is_value_set_and_loaded() )
		{
			debug_annoy( L"error", L"default_font could not be loaded." );
			return false;
		}

		// load default frame style.
		_default_frame_style.key = string8_c( "default", core_list_mode_e_static );
		_default_frame_style.texture = engine.get_resource_manager()->load_texture( string16_c( L"menus/atlas.png", core_list_mode_e_static ), true, false );
		if ( !_default_frame_style.texture.get_is_value_set_and_loaded() )
		{
			debug_annoy( L"error", L"default_frame_style.texture could not be loaded." );
			return false;
		}
		_default_frame_style.texture_map_mode = menu_frame_style_c::texture_map_mode_e_nine_slice_stretch;
		_default_frame_style.texture_map_fill_middle = true;
		_default_frame_style.state_list[ 0 ].texture_map[ 0 ] = 21;
		_default_frame_style.state_list[ 0 ].texture_map[ 1 ] = 11;
		_default_frame_style.state_list[ 0 ].texture_map[ 2 ] = 29;
		_default_frame_style.state_list[ 0 ].texture_map[ 3 ] = 19;
		_default_frame_style.state_list[ 0 ].texture_map_edges[ 0 ] = 3;
		_default_frame_style.state_list[ 0 ].texture_map_edges[ 1 ] = 3;
		_default_frame_style.state_list[ 0 ].texture_map_edges[ 2 ] = 3;
		_default_frame_style.state_list[ 0 ].texture_map_edges[ 3 ] = 3;
		_default_frame_style.state_list[ 1 ].texture_map[ 0 ] = 21;
		_default_frame_style.state_list[ 1 ].texture_map[ 1 ] = 11;
		_default_frame_style.state_list[ 1 ].texture_map[ 2 ] = 29;
		_default_frame_style.state_list[ 1 ].texture_map[ 3 ] = 19;
		_default_frame_style.state_list[ 1 ].texture_map_edges[ 0 ] = 3;
		_default_frame_style.state_list[ 1 ].texture_map_edges[ 1 ] = 3;
		_default_frame_style.state_list[ 1 ].texture_map_edges[ 2 ] = 3;
		_default_frame_style.state_list[ 1 ].texture_map_edges[ 3 ] = 3;
		_default_frame_style.state_list[ 2 ].texture_map[ 0 ] = 21;
		_default_frame_style.state_list[ 2 ].texture_map[ 1 ] = 11;
		_default_frame_style.state_list[ 2 ].texture_map[ 2 ] = 29;
		_default_frame_style.state_list[ 2 ].texture_map[ 3 ] = 19;
		_default_frame_style.state_list[ 2 ].texture_map_edges[ 0 ] = 3;
		_default_frame_style.state_list[ 2 ].texture_map_edges[ 1 ] = 3;
		_default_frame_style.state_list[ 2 ].texture_map_edges[ 2 ] = 3;
		_default_frame_style.state_list[ 2 ].texture_map_edges[ 3 ] = 3;
		_default_frame_style.state_list[ 3 ].texture_map[ 0 ] = 21;
		_default_frame_style.state_list[ 3 ].texture_map[ 1 ] = 11;
		_default_frame_style.state_list[ 3 ].texture_map[ 2 ] = 29;
		_default_frame_style.state_list[ 3 ].texture_map[ 3 ] = 19;
		_default_frame_style.state_list[ 3 ].texture_map_edges[ 0 ] = 3;
		_default_frame_style.state_list[ 3 ].texture_map_edges[ 1 ] = 3;
		_default_frame_style.state_list[ 3 ].texture_map_edges[ 2 ] = 3;
		_default_frame_style.state_list[ 3 ].texture_map_edges[ 3 ] = 3;

		// load default text style.
		_default_text_style.key = string8_c( "default", core_list_mode_e_static );
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
		_default_text_style.margin_is_defined = true;
		_default_text_style.margin = box32x2_c( 4.0f, 4.0f, 4.0f, 4.0f );

		// done.
		return true;
	}

	void_c menu_style_manager_c::refresh()
	{
		core_linked_list_c< menu_style_map_c::reference_c * >::node_c * style_map_list_node = menu_style_map_c::reference_c::_global_list.get_first();
		while ( style_map_list_node )
		{
			style_map_list_node->get_value()->unresolve();
			style_map_list_node = style_map_list_node->get_next();
		}
		core_linked_list_c< menu_color_style_c::reference_c * >::node_c * color_style_list_node = menu_color_style_c::reference_c::_global_list.get_first();
		while ( color_style_list_node )
		{
			color_style_list_node->get_value()->unresolve();
			color_style_list_node = color_style_list_node->get_next();
		}

		sint32_c i = 0;
		core_linked_list_c< menu_frame_style_c::reference_c * >::node_c * frame_style_list_node = menu_frame_style_c::reference_c::_global_list.get_first();
		while ( frame_style_list_node )
		{
			frame_style_list_node->get_value()->unresolve();
			core_linked_list_c< menu_frame_style_c::reference_c * >::node_c * frame_style_list_node_previous = frame_style_list_node;
			frame_style_list_node = frame_style_list_node->get_next();
			i++;
		}
		core_linked_list_c< menu_text_style_c::reference_c * >::node_c * text_style_list_node = menu_text_style_c::reference_c::_global_list.get_first();
		while ( text_style_list_node )
		{
			text_style_list_node->get_value()->unresolve();
			text_style_list_node = text_style_list_node->get_next();
		}

		_engine_styles.refresh();
		_game_styles.refresh();

		color_style_list_node = menu_color_style_c::reference_c::_global_list.get_first();
		while ( color_style_list_node )
		{
			color_style_list_node->get_value()->resolve();
			color_style_list_node = color_style_list_node->get_next();
		}
		frame_style_list_node = menu_frame_style_c::reference_c::_global_list.get_first();
		while ( frame_style_list_node )
		{
			frame_style_list_node->get_value()->resolve();
			frame_style_list_node = frame_style_list_node->get_next();
		}
		text_style_list_node = menu_text_style_c::reference_c::_global_list.get_first();
		while ( text_style_list_node )
		{
			text_style_list_node->get_value()->resolve();
			text_style_list_node = text_style_list_node->get_next();
		}
		style_map_list_node = menu_style_map_c::reference_c::_global_list.get_first();
		while ( style_map_list_node )
		{
			style_map_list_node->get_value()->resolve();
			style_map_list_node = style_map_list_node->get_next();
		}
	}

	menu_color_style_c * menu_style_manager_c::get_shared_color_style( sint32_c shared_color_index )
	{
		assert( shared_color_index >= 0 && shared_color_index < menu_shared_color_class_e_count_ * menu_state_e_count_ * menu_shared_color_slot_e_count_ );
		return &_shared_color_style_list[ shared_color_index ];
	}

	menu_color_style_c * menu_style_manager_c::get_shared_color_style( menu_shared_color_class_e color_class, menu_state_e color_state, menu_shared_color_slot_e color_slot )
	{
		sint32_c shared_color_index = get_shared_color_index( color_class, color_state, color_slot );
		return &_shared_color_style_list[ shared_color_index ];
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
				menu_color_style_c const * result = style_file_list[ i ]->find_color_style( key );
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
				menu_frame_style_c const * result = style_file_list[ i ]->find_frame_style( key );
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
					menu_text_style_c const * result = style_file_list[ i ]->find_text_style( key );
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
				menu_style_map_c const * result = style_file_list[ i ]->find_style_map( key );
				if ( result )
				{
					return result;
				}
			}
		}
		return nullptr;
	}

}
