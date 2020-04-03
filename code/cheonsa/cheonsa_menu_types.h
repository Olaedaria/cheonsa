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
	class menu_color_style_c;
	class menu_frame_style_c;
	class menu_text_style_c;
	class menu_text_glyph_style_c;
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

	enum menu_pop_up_type_e
	{
		menu_pop_up_type_e_bottom_right, // prioritizes opening towards the bottom, then towards the right. used for combo lists, menu bar menus, right click context menus.
		menu_pop_up_type_e_right_bottom, // prioritizes opening towards the right, then towards the bottom. used for sub menus.
	};

	enum menu_layout_mode_e : uint8_c
	{
		menu_layout_mode_e_simple, // _local_box is what it is.
		menu_layout_mode_e_box_anchor, // _local_box is calculated from _local_anchor and _local_anchor_measures, then _local_origin set to center of _local_box, then _local_box is translated by negative _local_origin.
		menu_layout_mode_e_point_anchor, // _local_box is what it is. _local_origin is calculated from _local_anchor and _local_anchor_measures.minimum.
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
		menu_text_align_horizontal_e_inherit_from_style, // can be set on the text element only, if it wants to inherit horizontal text alignment property from the text style that it has assigned to it.
		menu_text_align_horizontal_e_left,
		menu_text_align_horizontal_e_center,
		menu_text_align_horizontal_e_right,
		menu_text_align_horizontal_e_justify,
	};

	enum menu_text_align_vertical_e
	{
		menu_text_align_vertical_e_inherit_from_style, // can be set on the text element only, if it wants to inherit the vertical text alignment property from the text style that it has assigned to it.
		menu_text_align_vertical_e_top,
		menu_text_align_vertical_e_center,
		menu_text_align_vertical_e_bottom,
	};

	enum menu_text_select_mode_e
	{
		menu_text_select_mode_e_character, // extends the selection around the character under the cursor. can be invoked via click and drag, or shift + directional input.
		menu_text_select_mode_e_word, // extends the selection around the word under the cursor. can be invoked via double-click and drag, or shift + ctrl + directional input.
		menu_text_select_mode_e_paragraph, // extends the selection around the paragraph under the cursor. can be invoked via triple-click and drag.
	};

	enum menu_text_interact_mode_e
	{
		menu_text_interact_mode_e_static, // text is static, can't be interacted with by the user.
		menu_text_interact_mode_e_static_selectable, // text is static, but can be selected and copied by the user.
		menu_text_interact_mode_e_editable, // text is editable by the user.
	};

	enum menu_text_format_mode_e
	{
		menu_text_format_mode_e_plain, // all text uses the same style, does not support rich formatting.
		menu_text_format_mode_e_rich, // text supports rich formatting. currently, does not work with editable 
	};

	enum menu_text_filter_mode_e
	{
		menu_text_filter_mode_e_none,
		menu_text_filter_mode_e_number_integer,
		menu_text_filter_mode_e_number_real,
		menu_text_filter_mode_e_string,
	};

	enum menu_text_flag_e
	{
		menu_text_flag_e_none = 0x00,
		menu_text_flag_e_bold = 0x01,
		menu_text_flag_e_italic = 0x02,
		menu_text_flag_e_underline = 0x04,
	};

	enum menu_shared_color_class_e
	{
		menu_shared_color_class_e_window,
		menu_shared_color_class_e_button,
		menu_shared_color_class_e_field,
		menu_shared_color_class_e_count_,
	};

	// frames can use all three, via rgb channels of texture.
	// texts will only use secondary.
	enum menu_shared_color_slot_e
	{
		menu_shared_color_slot_e_primary, // used for frames. for color keyed shader, this maps to red channel in texture.
		menu_shared_color_slot_e_secondary, // used for texts. for color keyed shader, this maps to green channel in texture.
		menu_shared_color_slot_e_accent, // used for frames (accents). for color keyed shader, this maps to blue channel in texture.
		menu_shared_color_slot_e_count_
	};

	inline sint32_c get_shared_color_index( menu_shared_color_class_e color_class, menu_state_e color_state, menu_shared_color_slot_e color_slot )
	{
		return ( color_class * menu_state_e_count_ * menu_shared_color_slot_e_count_ ) + ( color_state * menu_shared_color_slot_e_count_ ) + color_slot;
	}

	// these define color values that can be shared and used by any number of elements.
	// the menu style manager holds a built in fixed length collection of these, one for each enum menu_shared_color_e, these built in shared colors are intended to be controlled by the program at run time.
	// the menu "styles.xml" files may define (in a data driven way) additional color styles.
	class menu_color_style_c
	{
	public:
		class reference_c
		{
		private:
			friend class menu_style_manager_c;

			static core_linked_list_c< reference_c * > _global_list;
			core_linked_list_c< reference_c * >::node_c _global_list_node;

			string8_c _key;
			menu_color_style_c const * _value;

		public:
			reference_c();
			~reference_c();

			void_c release_value();
			void_c resolve_value();

			string8_c const & get_key() const;
			void_c set_key( string8_c const & value );

			menu_color_style_c const * get_value() const;

		};

	public:
		sint32_c index;
		string8_c key;
		vector32x4_c value;

	public:
		menu_color_style_c();

		void_c reset();
		void_c load( data_scribe_markup_c::node_c const * node );
		void_c initialize( sint32_c index, string8_c const & key );

	};

	// defines how to render a frame element.
	// frame styles can be defined by "styles.xml".
	class menu_frame_style_c
	{
	public:
		class reference_c
		{
		private:
			friend class menu_style_manager_c;

			static core_linked_list_c< reference_c * > _global_list;
			core_linked_list_c< reference_c * >::node_c _global_list_node;

			string8_c _key; // persistent key, remains the same between reloading of style files.
			menu_frame_style_c const * _value; // volatile reference, changes between reloading of style files.

		public:
			reference_c();
			~reference_c();

			void_c release_value(); // releases _value but keeps _key, then involes on_refreshed.
			void_c resolve_value(); // looks up _value with _key, then invokes on_refreshed.

			string8_c const & get_key() const;
			void_c set_key( string8_c const & value );

			menu_frame_style_c const * get_value() const;
			void_c set_value( menu_frame_style_c const * value ); // a little more optimal than set_key() since it avoids doing a look up.

		};

		// used with frames to specify how to map the texture to the frame.
		enum texture_map_mode_e : uint8_c
		{
			texture_map_mode_e_stretch, // stretches a texture over a rectangle, with the size of the edges defined by frame_texture_margins.
			texture_map_mode_e_scale_to_fit, // scales a texture over a rectangle to fit completely within it, preserves aspect ratio of the texture. some of the rectangle might not be covered if its aspect ratio is different from the texture.
			texture_map_mode_e_scale_to_fill, // scales a texture over a rectangle to completely cover it, preserves aspect ratio of the texture. some of the texture might be cut off if its aspect ratio is different from the rectangle.
			texture_map_mode_e_nine_slice_stretch, // nine slice, with the texture stretched over the edges and the middle.
			texture_map_mode_e_nine_slice_tile, // nine slice, with the texture tiled over the edges and the middle.
		};

		// element appearance properties that can be defined for each visual state.
		class state_c
		{
		public:
			//menu_color_style_c::reference_c color_style; // color tint, multiplied with texture. if resolved then it takes precedence over color.
			//vector32x4_c color; // color tint, multiplied with texture.
			float32_c saturation; // color saturation, applied to ( element_color.rgb * texture.rgb ).
			float32_c apparent_scale; // when rendered, the frame is scaled by this factor around its center point. this is an apparent scale, meaning that it affects the visual representation only, and does not affect the actual scale of the control itself or hit detection.
			sint16_c texture_map[ 4 ]; // left, top, right, and bottom coordinates in pixels of edges of texture map rectangle.
			sint16_c texture_map_edges[ 4 ]; // left, top, right, and bottom insets in pixels of texture map rectangle.

		public:
			state_c();

			void_c reset();
			//vector32x4_c get_expressed_color() const; // if style_for_color is resolved then returns the color defined by the color style, otherwise returns the fall back color.

		};

	public:
		string8_c key; // unique key to identify this frame style by.
		resource_file_texture_c::reference_c texture; // texture to map to the frame.
		texture_map_mode_e texture_map_mode; // how we want to map the texture to the frame.
		boolean_c texture_map_fill_middle; // for nine slice texture mapping modes, this says whether to draw the middle part or not.
		video_renderer_pixel_shader_c::reference_c pixel_shader_reference; // pixel shader override, if not set then the built-in frame pixel shader will be used.
		state_c state_list[ menu_state_e_count_ ];

	public:
		menu_frame_style_c();

		void_c reset();
		void_c load( data_scribe_markup_c::node_c const * node );

	};

	// defines how to format and render text in a text element.
	// these style can be applied to the whole text element, or to sub-ranges of text in the text element.
	// text styles can be defined by "styles.xml".
	class menu_text_style_c
	{
	public:
		class reference_c
		{
		private:
			friend class menu_style_manager_c;

			static core_linked_list_c< reference_c * > _global_list;
			core_linked_list_c< reference_c * >::node_c _global_list_node;

			string8_c _key; // persistent key, remains the same between reloading of style files.
			menu_text_style_c const * _value; // volatile reference, changes between reloading of style files.

		public:
			reference_c();
			~reference_c();

			void_c release_value();
			void_c resolve_value();

			string8_c const & get_key() const;
			void_c set_key( string8_c const & value );

			menu_text_style_c const * get_value() const;
			void_c set_value( menu_text_style_c const * value ); // a little more optimal than set_key() since it avoids doing a look up.

			core_event_c< void_c, reference_c const * > on_refreshed; // is invoked whenever _value changes. text elements that reference the style should invalidate their text layout.

		};

		// element appearance properties that can be defined for each visual state.
		class state_c
		{
		public:
			//menu_color_style_c::reference_c color_style; // color tint, multiplied with texture. if resolved then it takes precedence over color.
			//vector32x4_c color; // color tint, multiplied with texture.
			float32_c saturation; // multiplicative color saturation.
			float32_c apparent_scale; // multiplicative apparent element scale.

		public:
			state_c();

			void_c reset();
			//vector32x4_c get_expressed_color() const; // if element_color_style is resolved then returns that color, otherwise returns element_color.

		};

	public:
		string8_c key; // unique key to identify this text style by.

		boolean_c font_is_defined;
		resource_file_font_c::reference_c font;

		boolean_c size_is_defined;
		float32_c size; // size of font in pixels.

		//boolean_c color_style_is_defined;
		//menu_color_style_c::reference_c color_style; // color style defined color of font.

		boolean_c color_is_defined;
		vector32x4_c color; // color of font.
		
		boolean_c skew_is_defined;
		float32_c skew; // skew of font.
		
		boolean_c weight_is_defined;
		float32_c weight; // weight of font.

		boolean_c softness_is_defined;
		float32_c softness;

		boolean_c underline_is_defined;
		float32_c underline; // thickness of underline in pixels.

		boolean_c paragraph_spacing_is_defined;
		float32_c paragraph_spacing; // extra vertical spacing in pixels to place between paragraphs.

		boolean_c line_spacing_is_defined;
		float32_c line_spacing; // extra vertical spacing in pixels to place between lines.

		boolean_c glyph_spacing_is_defined;
		float32_c glyph_spacing; // extra horizontal spacing in pixels to place between glyphs.

		boolean_c text_align_horizontal_is_defined;
		menu_text_align_horizontal_e text_align_horizontal; // how to align text in the block (it can only be applied to block types like headers and paragraphs).

		boolean_c text_align_vertical_is_defined;
		menu_text_align_vertical_e text_align_vertical;

		boolean_c margin_is_defined;
		box32x2_c margin; // space between left, top, right, and bottom edges of text element to where actualy text layout rectangle is.

		state_c state_list[ menu_state_e_count_ ];

	public:
		menu_text_style_c();

		void_c reset();
		void_c load( data_scribe_markup_c::node_c const * node );

	};

	// the style map defines which styles to map to which daughter elements, and which style maps to map to which daughter controls.
	// it also can define named property values that control implementations can use, for example for defining the fixed thickness of scroll bars, or fixed height of text boxes or combo boxes, etc.
	class menu_style_map_c
	{
	public:
		// used by controls to hold a reference to a style map inistance.
		class reference_c
		{
		private:
			friend class menu_style_manager_c;

			static core_linked_list_c< reference_c * > _global_list;
			core_linked_list_c< reference_c * >::node_c _global_list_node;

			string8_c _key; // persistent key, remains the same between reloading of style files.
			menu_style_map_c const * _value; // volatile reference, changes between reloading of style files.

		public:
			reference_c();
			~reference_c();
			
			void_c release_value();
			void_c resolve_value();

			void_c clear();

			string8_c const & get_key() const;
			void_c set_key( string8_c const & value );

			menu_style_map_c const * get_value() const;
			void_c set_value( menu_style_map_c const * value );

			core_event_c< void_c, reference_c const * > on_refreshed;

		};

		// maps a style key assignment to an element in the control.
		class entry_c
		{
		private:
			friend class menu_style_map_c;

			string8_c _element_name; // the name of the element(s) in the control that this entry maps a style to.
			string8_c _style_key; // the key of element style to map to the element(s) targeted by element_key.
			//menu_anchor_e anchor; // used to adjust the insets of the element rectangle relative to the control rectangle.
			//box32x2_c anchor_measures;

		public:
			entry_c();

			//string8_c get_target_type() const; // everything before the ":" in target, should be "frame", "text", or "control".
			//string8_c get_target_name() const; // everything after the ":" in target.

			string8_c const & get_element_name() const;
			string8_c const & get_style_key() const;

		};

		//// defines an arbitrary property that can be used to define colors or layout metrics or anything (depending on if the control is programmed to use the property).
		//class property_c
		//{
		//private:
		//	friend class menu_style_map_c;

		//	string8_c _name; // this property's name.
		//	string8_c _value;

		//public:
		//	property_c();

		//	string8_c const & get_name() const;
		//	string8_c const & get_value() const;

		//};

	private:
		friend class menu_control_c;

		string8_c _key; // unique key to identify this style map by.
		core_list_c< entry_c * > _entry_list; // defines style mappings for sub-elements and sub-controls.
		//core_list_c< property_c * > property_list; // defines property values for this control.

	public:
		menu_style_map_c();
		~menu_style_map_c();

		void_c load( data_scribe_markup_c::node_c const * node );

		string8_c const & get_key() const;

		entry_c const * find_entry( string8_c const & element_name ) const;

		//property_c const * find_property( string8_c const & name ) const;
		//boolean_c find_property_as_float32( string8_c const & name, float32_c & property_value_as_float32 ) const;

	};

	// resolved text glyph style.
	// resolved means that it has inherited all of the styles that are granularly defined by mother span(s), mother paragraph, mother text element, and built in (topmost).
	// within the context of the text element, all of the values will be set to be set to something valid and usable (although maybe not ideal).
	class menu_text_glyph_style_c
	{
	public:
		resource_file_font_c::reference_c font;
		vector32x4_c color;
		float32_c size;
		float32_c weight;
		float32_c skew;
		float32_c softness;

	public:
		menu_text_glyph_style_c();
		menu_text_glyph_style_c( menu_text_glyph_style_c const & other );

		menu_text_glyph_style_c & operator = ( menu_text_glyph_style_c const & other );

		boolean_c operator == ( menu_text_glyph_style_c const & other ) const;

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
	struct menu_event_information_c
	{
	public:
		menu_control_c * control; // the control that is receiving the event.
		input_event_c * event; // the input event that caused the menu control event to trigger. currently only set for on_clicked and on_double_clicked events. otherwise always nullptr.

	public:
		menu_event_information_c();
		menu_event_information_c( menu_control_c * control, input_event_c * event );

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
			vector32x4_c color; // uploads to menu_draw_color.
			vector32x4_c shared_colors[ 3 ]; // uploads to menu_draw_shared_colors.
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
		void_c append_rectangle_list( core_list_c< video_renderer_vertex_menu_c > const & input_vertex_list, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture, vector32x4_c const & color, vector32x4_c const shared_colors[ 3 ] );

		// generates vertices and indices for a box.
		void_c append_rectangle( box32x2_c const & box, box32x2_c const & map, video_pixel_shader_c * pixel_shader, resource_file_texture_c * texture, vector32x4_c const & color, vector32x4_c const shared_colors[ 3 ] );

	};

}
