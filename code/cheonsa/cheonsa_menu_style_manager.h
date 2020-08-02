#pragma once

#include "cheonsa_menu_types.h"
#include "cheonsa_menu_style_file.h"
#include "cheonsa_core_dictionary.h"

namespace cheonsa
{

	// manages loading of styles from xml files and serves as a registry for all loaded control style maps and element styles.
	// supports hot-loading of style files. when reloading style files, tells all frame elements and text elements to unresolve their style references, then reoloads style files, then tells all frame elements and text elements to reresolve their style references.
	// there is one global shared instance, declared in "cheonsa_engine.h", defined in "cheonsa_engine.cpp", it's fully qualified name is "global_engine_instance.interfaces.menu_style_manager".
	// also provides a set of internal color themes that can are applied to controls by default.
	class menu_style_manager_c
	{
		friend class engine_c;

	private:
		float32_c _shared_transition_speed; // speed in units per second of visual state transitions. a speed of 1 will take 1 second, a speed of 100 will take 1/100th of a second.
		
		menu_color_theme_c _internal_undefined_color_theme; // built in, key is "internal_undefined". elements will use this when their preferred color style can't be found. it's ugly by design so that it stands out.
		menu_color_theme_c _internal_window_color_theme; // built in, key is "internal_window".
		menu_color_theme_c _internal_button_color_theme; // built in, key is "internal_button".
		menu_color_theme_c _internal_field_color_theme; // built in, key is "internal_field".
		//menu_color_theme_c _default_item_color_theme; // built in, key is "internal_item". color theme for selectable list items, icons, etc, in the non-selected state.
		//menu_color_theme_c _default_item_selected_color_theme; // built in, key is "interna_item_selected", color theme for selectable list items, icons, etc, in the selected state. this works in a different dimension from (in addition to) menu_state_e_selected.
		core_dictionary_c< string8_c, menu_color_theme_c * > _internal_color_theme_dictionary; // built in (programatically added) color themes.

		resource_file_font_c::reference_c _default_font; // built in (sort of, loaded from an external file) font to use, can be used when a particular font can't be loaded.
		menu_frame_style_c _default_frame_style; // built in frame stylem, can be used when a particular frame style can't be loaded.
		menu_text_style_c _default_text_style; // built in text style, can be used when a particular text style can't be loaded.

		menu_style_file_c _engine_styles; // built-in styles used by engine menus, loaded from engine data folder "menus/styles.xml".
		menu_style_file_c _game_styles; // game defined styles used by game menus, loaded from game data folder "menus/styles.xml".

		menu_style_manager_c();
		~menu_style_manager_c();

		boolean_c start();

	public:
		void_c refresh(); // reloads engine and game style files.

		menu_color_theme_c const * find_color_theme( string8_c const & key ) const; // looks up a color theme instance with key. searches both internal (hard-coded) and external (file-defined) instances. returns nullptr if it does not exist.
		menu_frame_style_c const * find_frame_style( string8_c const & key ) const; // looks up a frame style instance with key. returns nullptr if it does not exist.
		menu_text_style_c const * find_text_style( string8_c const & key ) const; // looks up a text style instance with key. returns nullptr if it does not exist.
		menu_style_map_c const * find_style_map( string8_c const & key ) const; // looks up a style map instance with key. returns nullptr if it does not exist.

		inline resource_file_font_c const * get_default_font() const { return _default_font; }
		inline menu_frame_style_c const * get_default_frame_style() const { return &_default_frame_style; }
		inline menu_text_style_c const * get_default_text_style() const { return &_default_text_style; }

		inline menu_color_theme_c * get_internal_undefined_color_theme() { return &_internal_undefined_color_theme; }
		inline menu_color_theme_c * get_internal_window_color_theme() { return &_internal_window_color_theme; }
		inline menu_color_theme_c * get_internal_button_color_theme() { return &_internal_button_color_theme; }
		inline menu_color_theme_c * get_internal_field_color_theme() { return &_internal_field_color_theme; }
		//inline menu_color_theme_c const * get_default_item_color_theme() { return &_default_item_color_theme; }
		//inline menu_color_theme_c const * get_default_item_selected_color_theme() { return &_default_item_selected_color_theme; }

		inline sint32_c get_internal_color_theme_count() const { return 3; }
		menu_color_theme_c * get_internal_color_theme( sint32_c index );
		menu_color_theme_c * find_internal_color_theme( string8_c const & key ); // finds an internal color theme by its key.

		inline float32_c get_shared_transition_speed() const { return _shared_transition_speed; }

	};

}
