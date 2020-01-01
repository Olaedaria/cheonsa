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
		// load default shared color styles.
		sint32_c shared_color_style_index = 0;
		// window
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_normal, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_normal_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_normal, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_normal_secondary" ), vector32x4_c( 0.5f, 0.5f, 0.5f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_normal, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_normal_accent" ), vector32x4_c( 0.5f, 0.5f, 0.5f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_selected, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_selected_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_selected, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_selected_secondary" ), vector32x4_c( 0.25f, 0.25f, 0.25f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_selected, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_selected_accent" ), vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_pressed, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_pressed_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_pressed, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_pressed_secondary" ), vector32x4_c( 0.2f, 0.2f, 0.2f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_pressed, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_pressed_accent" ), vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_disabled, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_disabled_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_disabled, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_disabled_secondary" ), vector32x4_c( 0.75f, 0.75f, 0.75f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_disabled, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_disabled_accent" ), vector32x4_c( 0.25f, 0.25f, 0.25f, 1.0f ) );
		// button
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_normal, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_normal_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_normal, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_normal_secondary" ), vector32x4_c( 0.5f, 0.5f, 0.5f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_normal, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_normal_accent" ), vector32x4_c( 0.863f, 1.0f, 0.467f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_selected, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_selected_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_selected, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_selected_secondary" ), vector32x4_c( 0.25f, 0.25f, 0.25f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_selected, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_selected_accent" ), vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_pressed, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_pressed_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_pressed, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_pressed_secondary" ), vector32x4_c( 0.2f, 0.2f, 0.2f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_pressed, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_pressed_accent" ), vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_disabled, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_disabled_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_disabled, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_disabled_secondary" ), vector32x4_c( 0.75f, 0.75f, 0.75f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_disabled, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_disabled_accent" ), vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f ) );
		// field
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_normal, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_normal_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_normal, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_normal_secondary" ), vector32x4_c( 0.5f, 0.5f, 0.5f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_normal, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_normal_accent" ), vector32x4_c( 0.863f, 1.0f, 0.467f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_selected, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_selected_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_selected, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_selected_secondary" ), vector32x4_c( 0.25f, 0.25f, 0.25f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_selected, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_selected_accent" ), vector32x4_c( 0.0f, 0.863f, 0.941f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_pressed, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_pressed_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_pressed, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_pressed_secondary" ), vector32x4_c( 0.2f, 0.2f, 0.2f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_pressed, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_pressed_accent" ), vector32x4_c( 0.725f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_disabled, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_disabled_primary" ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_disabled, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_disabled_secondary" ), vector32x4_c( 0.75f, 0.75f, 0.75f, 1.0f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_disabled, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_disabled_accent" ), vector32x4_c( 0.64f, 0.64f, 0.64f, 1.0f ) );
		// insert into dictionary.
		sint32_c shared_color_count = menu_shared_color_class_e_count_ * menu_state_e_count_ * menu_shared_color_slot_e_count_;
		for ( sint32_c i = 0; i < shared_color_count; i++ )
		{
			assert( _shared_color_style_list[ i ].index == i );
			_shared_color_style_dictionary.insert( _shared_color_style_list[ i ].key, &_shared_color_style_list[ i ] );
		}
		//
		menu_shared_color_base_notes_c shared_color_base_notes;
		shared_color_base_notes.window_primary = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_color_base_notes.window_secondary = vector32x4_c( 0.5f, 0.5f, 0.5f, 1.0f );
		shared_color_base_notes.window_accent = vector32x4_c( 0.64f, 1.0f, 0.15f, 1.0f );
		shared_color_base_notes.button_primary = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_color_base_notes.button_secondary = vector32x4_c( 0.5f, 0.5f, 0.5f, 1.0f );
		shared_color_base_notes.button_accent = vector32x4_c( 0.64f, 1.0f, 0.15f, 1.0f );
		shared_color_base_notes.field_primary = vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f );
		shared_color_base_notes.field_secondary = vector32x4_c( 0.5f, 0.5f, 0.5f, 1.0f );
		shared_color_base_notes.field_accent = vector32x4_c( 0.64f, 1.0f, 0.15f, 1.0f );
		set_shared_color_base_notes( shared_color_base_notes );

		// load default font.
		_default_font = engine_c::get_instance()->get_resource_manager()->load_font( string16_c( mode_e_static, L"[e]fonts/DXLBaB-KSCpc-EUC-H.ttf" ) );
		if ( !_default_font.is_reference_set_and_loaded() )
		{
			debug_annoy( L"error", L"default_font could not be loaded." );
			return false;
		}

		// load default frame style.
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

		// load default text style.
		_default_text_style.key = string8_c( mode_e_static, "default" );
		_default_text_style.font_is_defined = true;
		_default_text_style.font = _default_font;
		_default_text_style.size_is_defined = true;
		_default_text_style.size = 16;
		_default_text_style.color_is_defined = true;
		_default_text_style.color = vector32x4_c( 0.5f, 0.5f, 0.5f, 1.0f );
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
		core_linked_list_c< menu_color_style_c::reference_c * >::node_c * color_style_list_node = menu_color_style_c::reference_c::_global_list.get_first();
		while ( color_style_list_node )
		{
			color_style_list_node->get_value()->release_value();
			color_style_list_node = color_style_list_node->get_next();
		}
		core_linked_list_c< menu_frame_style_c::reference_c * >::node_c * frame_style_list_node = menu_frame_style_c::reference_c::_global_list.get_first();
		while ( frame_style_list_node )
		{
			frame_style_list_node->get_value()->release_value();
			frame_style_list_node = frame_style_list_node->get_next();
		}
		core_linked_list_c< menu_text_style_c::reference_c * >::node_c * text_style_list_node = menu_text_style_c::reference_c::_global_list.get_first();
		while ( text_style_list_node )
		{
			text_style_list_node->get_value()->release_value();
			text_style_list_node = text_style_list_node->get_next();
		}

		_engine_styles._refresh();
		_game_styles._refresh();

		//menu_control_c::_global_resolve_style_maps();

		color_style_list_node = menu_color_style_c::reference_c::_global_list.get_first();
		while ( color_style_list_node )
		{
			color_style_list_node->get_value()->resolve_value();
			color_style_list_node = color_style_list_node->get_next();
		}
		frame_style_list_node = menu_frame_style_c::reference_c::_global_list.get_first();
		while ( frame_style_list_node )
		{
			frame_style_list_node->get_value()->resolve_value();
			frame_style_list_node = frame_style_list_node->get_next();
		}
		text_style_list_node = menu_text_style_c::reference_c::_global_list.get_first();
		while ( text_style_list_node )
		{
			text_style_list_node->get_value()->resolve_value();
			text_style_list_node = text_style_list_node->get_next();
		}
	}

	void_c menu_style_manager_c::set_shared_color_base_notes( menu_shared_color_base_notes_c const & values )
	{
		_shared_color_base_notes = values;

		sint32_c shared_color_style_index = 0;
		// window
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_normal, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_normal_primary" ), values.window_primary );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_normal, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_normal_secondary" ), values.window_secondary );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_normal, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_normal_accent" ), values.window_secondary );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_selected, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_selected_primary" ), ops::adjust_color_contrast( values.window_primary, 0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_selected, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_selected_secondary" ), ops::adjust_color_contrast( values.window_secondary, 0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_selected, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_selected_accent" ), ops::adjust_color_contrast( values.window_accent, 0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_pressed, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_pressed_primary" ), ops::adjust_color_contrast( values.window_primary, -0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_pressed, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_pressed_secondary" ), ops::adjust_color_contrast( values.window_secondary, -0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_pressed, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_pressed_accent" ), ops::adjust_color_contrast( values.window_accent, -0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_disabled, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_disabled_primary" ), ops::adjust_color_contrast( ops::adjust_color_saturation( values.window_primary, 0.2f ), -0.25f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_disabled, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_disabled_secondary" ), ops::adjust_color_contrast( ops::adjust_color_saturation( values.window_secondary, 0.2f ), -0.25f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_window, menu_state_e_disabled, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_window_disabled_accent" ), ops::adjust_color_contrast( ops::adjust_color_saturation( values.window_secondary, 0.2f ), -0.25f ) );
		// button
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_normal, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_normal_primary" ), values.button_primary );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_normal, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_normal_secondary" ), values.button_secondary );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_normal, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_normal_accent" ), values.button_secondary );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_selected, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_selected_primary" ), ops::adjust_color_brightness( values.button_primary, 0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_selected, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_selected_secondary" ), ops::adjust_color_brightness( values.button_secondary, 0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_selected, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_selected_accent" ), ops::adjust_color_brightness( values.button_accent, 0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_pressed, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_pressed_primary" ), ops::adjust_color_brightness( values.button_primary, -0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_pressed, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_pressed_secondary" ), ops::adjust_color_brightness( values.button_secondary, -0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_pressed, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_pressed_accent" ), ops::adjust_color_brightness( values.button_accent, -0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_disabled, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_disabled_primary" ), ops::adjust_color_contrast( ops::adjust_color_saturation( values.button_primary, 0.2f ), -0.25f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_disabled, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_disabled_secondary" ), ops::adjust_color_contrast( ops::adjust_color_saturation( values.button_secondary, 0.2f ), -0.25f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_button, menu_state_e_disabled, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_button_disabled_accent" ), ops::adjust_color_contrast( ops::adjust_color_saturation( values.button_secondary, 0.2f ), -0.25f ) );
		// field
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_normal, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_normal_primary" ), values.field_primary );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_normal, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_normal_secondary" ), values.field_secondary );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_normal, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_normal_accent" ), values.field_secondary );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_selected, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_selected_primary" ), ops::adjust_color_brightness( values.field_primary, 0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_selected, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_selected_secondary" ), ops::adjust_color_brightness( values.field_secondary, 0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_selected, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_selected_accent" ), ops::adjust_color_brightness( values.field_accent, 0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_pressed, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_pressed_primary" ), ops::adjust_color_brightness( values.field_primary, -0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_pressed, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_pressed_secondary" ), ops::adjust_color_brightness( values.field_secondary, -0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_pressed, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_pressed_accent" ), ops::adjust_color_brightness( values.field_accent, -0.1f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_disabled, menu_shared_color_slot_e_primary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_disabled_primary" ), ops::adjust_color_contrast( ops::adjust_color_saturation( values.field_primary, 0.2f ), -0.25f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_disabled, menu_shared_color_slot_e_secondary );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_disabled_secondary" ), ops::adjust_color_contrast( ops::adjust_color_saturation( values.field_secondary, 0.2f ), -0.25f ) );
		shared_color_style_index = get_shared_color_index( menu_shared_color_class_e_field, menu_state_e_disabled, menu_shared_color_slot_e_accent );
		_shared_color_style_list[ shared_color_style_index ].initialize( shared_color_style_index, string8_c( mode_e_static, "e_field_disabled_accent" ), ops::adjust_color_contrast( ops::adjust_color_saturation( values.field_secondary, 0.2f ), -0.25f ) );
	}

	void_c menu_style_manager_c::get_shared_color_base_notes( menu_shared_color_base_notes_c & values )
	{
		values = _shared_color_base_notes;
	}

	menu_color_style_c * menu_style_manager_c::find_shared_color_style( menu_shared_color_class_e color_class, menu_state_e color_state, menu_shared_color_slot_e color_slot )
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
