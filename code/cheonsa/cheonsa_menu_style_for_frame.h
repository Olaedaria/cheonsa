#pragma once

#include "cheonsa_menu_types.h"

namespace cheonsa
{

	// defines how to render a frame element.
	// frame styles can be defined by "styles.xml".
	class menu_style_for_frame_c
	{
	public:
		class reference_c
		{
		private:
			string8_c _key; // persistent key, remains the same between reloading of style files.
			menu_style_for_frame_c const * _value; // volatile reference, changes between reloading of style files.

		public:
			reference_c();
			reference_c( reference_c const & ) = delete;
			reference_c & operator = ( reference_c const & ) = delete;

			void_c refresh(); // looks up _value with _key, then invokes on_refreshed.

			string8_c const & get_key() const;
			void_c set_key( string8_c const & value );

			menu_style_for_frame_c const * get_value() const;
			void_c set_value( menu_style_for_frame_c const * value ); // a little more optimal than set_key() since it avoids doing a look up.

			core_event_c< void_c, reference_c const * > on_refreshed; // is invoked whenever _value changes.

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
			menu_shared_color_c::reference_c shared_color; // color tint, multiplied with texture. if resolved then it takes precedence over color.
			vector32x4_c color; // color tint, multiplied with texture.
			float32_c saturation; // color saturation, applied to ( element_color.rgb * texture.rgb ).
			float32_c apparent_scale; // when rendered, the frame is scaled by this factor around its center point. this is an apparent scale, meaning that it affects the visual representation only, and does not affect the actual scale of the control itself or hit detection.
			sint16_c texture_map[ 4 ]; // left, top, right, and bottom coordinates in pixels of edges of texture map rectangle.
			sint16_c texture_map_edges[ 4 ]; // left, top, right, and bottom insets in pixels of texture map rectangle.

		public:
			state_c();

			void_c reset();
			vector32x4_c get_expressed_color() const; // if style_for_color is resolved then returns the color defined by the color style, otherwise returns the fall back color.

		};

	public:
		string8_c key; // unique key to identify this frame style by.
		resource_object_texture_c::reference_c texture; // texture to map to the frame.
		texture_map_mode_e texture_map_mode; // how we want to map the texture to the frame.
		boolean_c texture_map_fill_middle; // for nine slice texture mapping modes, this says whether to draw the middle part or not.
		video_renderer_pixel_shader_c::reference_c pixel_shader_reference; // pixel shader override, if not set then the built-in frame pixel shader will be used.
		state_c state_list[ menu_state_e_count_ ];

	public:
		menu_style_for_frame_c();

		void_c reset();
		void_c load( data_scribe_markup_c::node_c const * node );

	};

}
