#pragma once

#include "cheonsa_menu_types.h"
#include "cheonsa_menu_style_file.h"
#include "cheonsa_core_dictionary.h"

namespace cheonsa
{

	// manages loading of styles from xml files and serves as a registry for all loaded control style maps and element styles.
	// supports hot-loading of style files, which will trigger all frame and text elements to re-resolve their style references, and will trigger reflow/relayout of all glyphs in text elements.
	// there is one global shared instance, declared in "cheonsa_engine.h", defined in "cheonsa_engine.cpp", it's fully qualified name is "global_engine_instance.interfaces.menu_style_manager".
	class menu_style_manager_c
	{
	private:
		float32_c _shared_transition_speed; // speed in units per second of visual state transitions. a speed of 1 will take 1 second, a speed of 100 will take 1/100th of a second.
		
		menu_shared_color_base_notes_c _shared_color_base_notes;
		menu_color_style_c _shared_color_style_list[ menu_shared_color_class_e_count_ * menu_state_e_count_ * menu_shared_color_slot_e_count_ ]; // engine and/or game can set these programatically to customize the color scheme. style files may reference these with keys "[n]", where n is a number between 0 and 7.
		core_dictionary_c< string8_c, menu_color_style_c * > _shared_color_style_dictionary;

		resource_file_font_c::reference_c _default_font; // global default font to use.
		menu_frame_style_c _default_frame_style; // global frame style to use when the specific frame style can't be found.
		menu_text_style_c _default_text_style; // global text style to use when the specific text style can't be found.

		menu_style_file_c _engine_styles; // built-in styles used by engine menus, loaded from "[e]menus/styles.xml".
		menu_style_file_c _game_styles; // game defined styles used by game menus, loaded from "[g]menus/styles.xml".

	public:
		menu_style_manager_c();
		~menu_style_manager_c();

		boolean_c start();

		void_c refresh(); // reloads engine and game style files.

		void_c set_shared_color_base_notes( menu_shared_color_base_notes_c const & values );
		void_c get_shared_color_base_notes( menu_shared_color_base_notes_c & values );

		menu_color_style_c * find_shared_color_style( menu_shared_color_class_e color_class, menu_state_e color_state, menu_shared_color_slot_e color_slot );
		menu_color_style_c * find_shared_color_style( string8_c key );

		menu_color_style_c const * find_color_style( string8_c const & key ) const;
		menu_frame_style_c const * find_frame_style( string8_c const & key ) const; // searches for a loaded frame style with key, returns it if it exsists, or returns nullptr if it does not exist.
		menu_text_style_c const * find_text_style( string8_c const & key ) const; // searches for a loaded text style with key, returns it if it exists, or returns nullptr if it does not exist.
		menu_style_map_c const * find_style_map( string8_c const & key ) const;

		inline resource_file_font_c const * get_default_font() const { return _default_font; }
		inline menu_frame_style_c const * get_default_frame_style() const { return &_default_frame_style; }
		inline menu_text_style_c const * get_default_text_style() const { return &_default_text_style; }

		inline float32_c get_shared_transition_speed() const { return _shared_transition_speed; }

	};

}
