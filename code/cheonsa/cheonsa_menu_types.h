#pragma once

/*

a bunch of shared menu types are declared in this one file.

about the menu transform and layout model:

the 2D coordinate system convention is that the origin is in the top left of the screen, and each coordinate point represents the center of a pixel.
the top left corner of the screen is (0, 0).
the bottom right corner of the screen is (screen_width, screen_height).

transform controls where a control is positioned on the 2D screen or in the 3D scene.
transform only needs to be recalculated when a local transform property changes.

layout consists of 2D rectangles.
layout only needs to be recalculated when the rectangular area changes.
layout recalculation propagates down the daughter chain as needed.

transform and layout properties are independent of each other, so changing one does not require an update to the other.

scroll bars control the transform properties of a container, so when a scroll occurs then container transform needs to be recalculated while container layout does not need to be recalculated.

*/

#include "cheonsa__types.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_string8.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_scribe_markup.h"
#include "cheonsa_input_manager.h"
#include "cheonsa_video_renderer_shader_manager.h"
#include "cheonsa_video_renderer_types.h"
#include "cheonsa_resource_file_texture.h"
#include "cheonsa_resource_file_font.h"
#include "cheonsa_sprite.h"

namespace cheonsa
{

	class user_interface_c;
	class menu_control_c;
	class menu_element_c;
	class menu_element_frame_c;
	class menu_element_text_c;
	class menu_style_for_frame_c;
	class menu_style_for_text_c;
	class menu_style_for_text_glyph_c;
	class menu_style_map_c;
	class menu_style_file_c;
	class menu_style_manager_c;

	// these values are used with non-client hit testing.
	// non-client hit testing works in tandem with the operating system's window system to allow for us to define areas that the operating system will treat as resize handles or title bars etc.
	// input events that are determined to intersect the non-client areas defined by the menu controls in the menu context will be consumed/blocked, they will not be routed through the normal path.
	// the game can set up a menu context for which non client hit tests can be performed against:
	// menu_non_client_area_e user_interface_c::non_client_hit_test().
	// the menu context can be configured to have its edges serve as resizable handles and its top edge serve as a title bar handle.
	// controls within the menu context can be configured to act as handles as well.
	enum menu_non_client_type_e : uint8_c
	{
		menu_non_client_type_e_none,
		menu_non_client_type_e_client, // if this is set on a control that gets picked during the non-client hit test, then it tells the engine to ignore the non-client hit test. in this way, we can have for example a button overlapping with the title bar which takes priority over the title bar.
		menu_non_client_type_e_size_left,
		menu_non_client_type_e_size_right,
		menu_non_client_type_e_size_top,
		menu_non_client_type_e_size_top_left,
		menu_non_client_type_e_size_top_right,
		menu_non_client_type_e_size_bottom,
		menu_non_client_type_e_size_bottom_left,
		menu_non_client_type_e_size_bottom_right,
		menu_non_client_type_e_title_bar,
		menu_non_client_type_e_button_close,
		menu_non_client_type_e_button_maximize_restore,
		menu_non_client_type_e_button_minimize,
	};

	// determines priority of hit testing, and ordering of rendering.
	enum menu_layer_e : sint8_c
	{
		menu_layer_e_undefined = -1, // when a control's layer is undefined, it will inherit to having the same layer of the nearest mother that defines a layer, otherwise it will default to base.
		menu_layer_e_base = 0, // base level.
		menu_layer_e_popup = 1, // higher level than base, for combo box drop downs and right click context menus and menu bar menus, etc.
		menu_layer_e_tooltip = 2, // highest level, for tool tips.
		menu_layer_e_count_
	};

	// all controls are going to be in any one of these states at a time.
	// these in turn can drive the visual state of the control.
	enum menu_state_e : uint8_c
	{
		menu_state_e_normal,
		menu_state_e_selected,
		menu_state_e_pressed,
		menu_state_e_disabled,
		menu_state_e_count_
	};

	// size mode defines how size of a control is determined.
	// size mode can be defined independently for x and y axes.
	// for list items, we use size mode fill for width and fit_fill for height.
	enum menu_size_mode_e : uint8_c
	{
		menu_size_mode_e_fixed = 0x00, // width or height is set directly and is fixed.
		//menu_size_mode_e_fill_mother = 0x01, // width or height will be set to width or height of mother container.
		menu_size_mode_e_fit_content = 0x02, // width or height will be set to width or height of contents.
	};

	enum menu_popup_type_e
	{
		menu_popup_type_e_bottom, // prioritizes opening towards the bottom, then towards the right. used for combo lists, menu bar menus, right click context menus.
		menu_popup_type_e_right, // prioritizes opening towards the right, then towards the bottom. used for menu sub menus.
	};

	enum menu_layout_mode_e : uint8_c
	{
		menu_layout_mode_e_simple, // _local_box is what it is.
		menu_layout_mode_e_box_anchor, // _local_box is calculated from _local_anchor and _local_anchor_measures, then _local_origin set to center of _local_box, then _local_box is translated by negative _local_origin.
		menu_layout_mode_e_point_anchor // _local_box is what it is. _local_origin is calculated from _local_anchor and _local_anchor_measures.minimum.
	};

	// anchor bit flags tell the layout logic which edges of a control should be anchored to the same edge of the mother control.
	enum menu_anchor_e : uint8_c
	{
		menu_anchor_e_none = 0x00,
		menu_anchor_e_left = 0x01,
		menu_anchor_e_top = 0x02,
		menu_anchor_e_right = 0x04,
		menu_anchor_e_bottom = 0x08,
	};

	inline menu_anchor_e operator | ( menu_anchor_e a, menu_anchor_e b )
	{
		return static_cast< menu_anchor_e >( static_cast< uint8_c >( a ) | static_cast< uint8_c >( b ) );
	}

	inline menu_anchor_e & operator |= ( menu_anchor_e & a, menu_anchor_e const & b )
	{
		a = static_cast< menu_anchor_e >( static_cast< uint8_c >( a ) | static_cast< uint8_c >( b ) );
		return a;
	}

	// determines how a control may be selected by the user.
	enum menu_select_mode_e : uint8_c
	{
		menu_select_mode_e_none,	// this control is not selectable by mouse or directional input, the context will ignore this control when it comes to mouse selection, the context will not set mouse_hit_was_blocked even if the mouse happens to intersect with this control's area.
		menu_select_mode_e_mouse, // this control is selectable by mouse only, the context will set mouse_hit_was_blocked to true if the mouse intersects with this control.
		menu_select_mode_e_mouse_and_directional, // this control is selectable, the context will set mouse_hit_was_blocked to true if the mouse intersects with this control, the context will also use directional inputs to select neighboring controls.
	};

	// used to drive logic for automatic show/hide or new/delete of daughter controls.
	// usually used with scroll bars.
	enum menu_visibility_mode_e
	{
		menu_visibility_mode_e_never,
		menu_visibility_mode_e_always,
		menu_visibility_mode_e_automatic,
	};

	enum menu_text_align_horizontal_e
	{
		menu_text_align_horizontal_e_inherit_from_style, // can be set on the text element only, if it wants the horizontal text alignment to be inherited from the menu_style_for_text_c instead.
		menu_text_align_horizontal_e_left,
		menu_text_align_horizontal_e_center,
		menu_text_align_horizontal_e_right,
		menu_text_align_horizontal_e_justify,
	};

	enum menu_text_align_vertical_e
	{
		menu_text_align_vertical_e_inherit_from_style, // can be set on the text element only, if it wants the horizontal text alignment to be inherited from the menu_style_for_text_c instead.
		menu_text_align_vertical_e_top,
		menu_text_align_vertical_e_center,
		menu_text_align_vertical_e_bottom,
	};

	enum menu_text_selection_mode_e
	{
		menu_text_selection_mode_e_character, // extends the selection around the character under the cursor. can be invoked via click and drag, or shift + directional input.
		menu_text_selection_mode_e_word, // extends the selection around the word under the cursor. can be invoked via double-click and drag, or shift + ctrl + directional input.
		//menu_text_selection_mode_e_line, // extends the selection around the line under the cursor. can be invoked via triple-click and drag.
		menu_text_selection_mode_e_paragraph, // extends the selection around the paragraph under the cursor. can be invoked via triple-click and drag.
	};

	// built in shared styles for color.
	// used by shared_colors in style manager.
	// combines different types of controls with states and two colors.
	// types of controls:
	// "window" is intended for windows, panels, labels.
	// "button" is intended for most buttons, scroll bars.
	// "button_prime" is for buttons that you want to draw the user's attention to, the default choice in a multiple-choice dialog, the close button on a window, etc. they should pop out more than the other buttons that use the normal "button" colors.
	// "field" is for text boxes, combo boxes, list boxes, check boxes, etc.
	// types of visual states:
	// "normal"
	// "selected"
	// "pressed"
	// "disabled"
	// types of colors:
	// "base" is intended for backgrounds.
	// "accent" is intended for borders, title bars, and text.
	enum menu_shared_color_e
	{
		menu_shared_color_e_window_normal_base,
		menu_shared_color_e_window_normal_accent,
		menu_shared_color_e_window_selected_base,
		menu_shared_color_e_window_selected_accent,
		menu_shared_color_e_window_pressed_base,
		menu_shared_color_e_window_pressed_accent,
		menu_shared_color_e_window_disabled_base,
		menu_shared_color_e_window_disabled_accent,

		menu_shared_color_e_button_normal_base,
		menu_shared_color_e_button_normal_accent,
		menu_shared_color_e_button_selected_base,
		menu_shared_color_e_button_selected_accent,
		menu_shared_color_e_button_pressed_base,
		menu_shared_color_e_button_pressed_accent,
		menu_shared_color_e_button_disabled_base,
		menu_shared_color_e_button_disabled_accent,

		menu_shared_color_e_button_prime_normal_base,
		menu_shared_color_e_button_prime_normal_accent,
		menu_shared_color_e_button_prime_selected_base,
		menu_shared_color_e_button_prime_selected_accent,
		menu_shared_color_e_button_prime_pressed_base,
		menu_shared_color_e_button_prime_pressed_accent,
		menu_shared_color_e_button_prime_disabled_base,
		menu_shared_color_e_button_prime_disabled_accent,

		menu_shared_color_e_field_normal_base,
		menu_shared_color_e_field_normal_accent,
		menu_shared_color_e_field_selected_base,
		menu_shared_color_e_field_selected_accent,
		menu_shared_color_e_field_pressed_base,
		menu_shared_color_e_field_pressed_accent,
		menu_shared_color_e_field_disabled_base,
		menu_shared_color_e_field_disabled_accent,

		menu_shared_color_e_count_
	};

	class menu_shared_color_c
	{
	public:
		class reference_c
		{
		private:
			string8_c _key;
			menu_shared_color_c const * _value;

		public:
			string8_c const & get_key() const;
			void_c set_key( string8_c const & value );

			menu_shared_color_c const * get_value() const;

		};

	public:
		string8_c key;
		sint32_c index;
		vector32x4_c value;

	};

	// a text entity is a thing that is may not be text but is inlined in the text, like a sprite or emoji, or another string.
	// each entity has a corresponding text_glyph_c instance, which may store additional info about how to lay out the entity.
	// that is why we don't store too much info in the entity.
	class menu_text_entity_c
	{
	public:
		sint32_c type_code;

	public:
		menu_text_entity_c( sint32_c type_code );

	};

	// stores layout of a sprite entity in a paragraph.
	class menu_text_entity_sprite_c
		: public menu_text_entity_c
	{
	public:
		static sint32_c type_code_static() { return 2; }

	public:
		sprite_c value; // sprite instance.

	public:
		menu_text_entity_sprite_c();

	};

	//// stores layout of a string entity in a paragraph.
	//class menu_text_entity_string_c
	//	: public menu_text_entity_c
	//{
	//	static sint32_c type_code_static() { return 3; }
	//	string8_c _key; // key that is used to reference and look up a string.
	//	string16_c _value; // string instance, resolved from key.
	//	menu_text_entity_string_c();
	//};


	// used to pass additional info about input events to event handlers.
	struct menu_event_info_c
	{
	public:
		menu_control_c * control; // the control that is receiving the event.
		input_event_c * event; // the input event that caused the menu control event to trigger. currently only set for on_clicked and on_double_clicked events. otherwise always nullptr.

	public:
		menu_event_info_c();
		menu_event_info_c( menu_control_c * control, input_event_c * event );

	};

	// elements use this to build and draw their stuff.
	struct menu_draw_list_c
	{
	public:
		struct draw_c
		{
		public:
			video_pixel_shader_c * pixel_shader; // optional pixel shader to use. if it's nullptr then the renderer will pick a default given the type of the mother_element.
			resource_file_texture_c * texture; // optional texture to use. if it's nullptr then the renderer will pick a default given the type of the mother_element. (only used by frame type elements though).
			uint32_c vertex_start;
			uint32_c vertex_count;
			uint32_c index_start;
			uint32_c index_count;

		public:
			draw_c();

		};

		//menu_element_c * mother_element;
		core_list_c< video_renderer_vertex_menu_c > vertex_list;
		core_list_c< uint16_c > index_list; // formatted as triangle list.
		core_list_c< draw_c > draw_list;
		sint32_c vertex_base; // after the renderer compiles the vertex_list into the monolith vertex_list, this will be the offset to this unit's vertices.
		sint32_c index_base; // after the renderer compiles the index_list into the monolith index_list, this will be the offset to this unit's indices.

	public:
		menu_draw_list_c();

		// resets state, clears all the draw lists.
		void_c reset();

		// interprets the input_vertex_list as a list of quads to build the index_list.
		void_c append_rectangle_list( core_list_c< video_renderer_vertex_menu_c > const & input_vertex_list, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture );

		// generates vertices and indices for a box.
		void_c append_rectangle( box32x2_c const & box, box32x2_c const & map, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture, vector32x4_c const & color );

	};

}
