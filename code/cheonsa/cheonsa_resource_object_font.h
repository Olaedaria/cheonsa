#pragma once

#include "cheonsa_resource_object.h"
#include "cheonsa_resource_object_texture.h"
#include "cheonsa_core_dictionary.h"
#include "cheonsa_core_linked_list.h"
#include "cheonsa_math_types.h"

namespace cheonsa
{

#pragma pack( push, 1 )
	// used to index glyphs in the glyph dictionary.
	// needs to be packed a certain way to prevent the compiler from adding invisible bytes to the structure.
	// this is because the dictionary's hash function is byte-wise, and those invisible bytes could break it.
	class glyph_key_c
	{
	public:
		uint32_c font_file_hash; // we use this to identify the font instead of a pointer, because it will be persistent across executions and it can be saved to and loaded from the disk cache. it is possible for "dangling" glyphs to accumulate in the disk cache, in the case that the fonts used change between executions.
		char16_c code_point; // code point is just a more technical term for character.
		uint8_c quantized_size; // the size in pixels that this glyph was rendered at (not exact). quantized with get_quantized_size().
		uint8_c unused; // to make 8 bytes. should always be 0.

	public:
		glyph_key_c();
		glyph_key_c( uint32_c font_file_hash, float32_c font_size, char16_c code_point ); // "code point" is just another name for "character". code_point is the character, size is the font size in IDK, pixels?

		boolean_c operator == ( glyph_key_c const & other ) const;

	};
	static_assert( sizeof( glyph_key_c ) == 8, "size of video_renderer_light_c is wrong." );

	// represents a glyph that has been rendered to the glyph atlas and has had its measurements calculated.
	// coordinate frame is top-left.
	class glyph_c
	{
	public:
		box32x2_c box; // xy coordinates of textured quad in pixels (including extra margin to contain extent of SDF). relative to cursor origin of rendered glyph. this value is always going to be an integer, but we store it as a float so that we don't have to cast it to a float later when building the vertices.
		box32x2_c map; // uv coordinates of texture map in normalized texture coordinates (including extra margin to contain extent of SDF).
		glyph_key_c key;
		float32_c horizontal_advance; // for horizontal layout, this is how far the glyph advances the glyph run horizontally in pixels.
		uint8_c atlas_index; // the index of the texture array slice of the glyph atlas texture that this glyph's image lives in.
		uint8_c unused[ 19 ];

	public:
		glyph_c();

		glyph_c & operator = ( glyph_c const & ) = delete;

	};
	static_assert( sizeof( glyph_c ) == 64, "size of glyph_c is wrong." );
#pragma pack( pop )

	// font formats conventionally place the coordinate frame in bottom-left, but cheonsa tranlsates this coordinate frame to top-left.
	// font formats conventionally describe the ascender and descender as distance above the base line, so that descender would usually be a negative value, but cheonsa converts descender to distance below the base line so that it will usually be a positive value too.
	class resource_object_font_c : public resource_object_c
	{
	public:
		typedef resource_object_reference_c< resource_object_font_c > reference_c;

	public:
		static char8_c const * get_type_static() { return "font"; }
		virtual char8_c const * get_type() const override { return get_type_static(); }

	public:
		static core_linked_list_c< resource_object_font_c * > _global_list;
		core_linked_list_c< resource_object_font_c * >::node_c _global_list_node;

	public:
		// caches some info about a given font size...
		// we render and cache glyphs at descrete quantized sizes so that we don't have to render a glyph an infinite number of times if it is to be rendered at an infinite number of different scales.
		class size_metrics_c
		{
		public:
			void_c * free_type_size_handle;
			uint16_c quantized_size;
			float32_c ascender;
			float32_c descender;
			float32_c space_horizontal_advance;

		public:
			size_metrics_c();

			boolean_c _load( void_c * free_type_face_handle, uint8_c quantized_size );
			void_c _unload();

		};
		
	protected:
		uint8_c * _file; // an in-memory copy of the font file, which needs to remain in memory for as long as free type is using it.
		sint32_c _file_size; // size in bytes of _font_file.
		uint32_c _file_hash; // used to identify this font, when saving the glyph cache and loading it.
		void_c * _free_type_face_handle;

		static sint32_c const _quantized_count = 4;
		static uint8_c const _quantized_sizes[ _quantized_count ];
		size_metrics_c _quantized_size_metrics[ _quantized_count ];

		virtual boolean_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_object_font_c();
		resource_object_font_c( resource_object_font_c const & ) = delete;
		~resource_object_font_c();
		resource_object_font_c & operator = ( resource_object_font_c const & ) = delete;

		uint32_c get_file_hash() const;

		void_c * get_free_type_face_handle() const;

		float32_c get_unquantized_ascender( float32_c font_size ) const; // gets the ascender in pixels for the given size which represents the distance away from the baseline, positive values go up, negative values go down (be mindful because this is backwards from cheonsa's 2d convention).
		float32_c get_unquantized_descender( float32_c font_size ) const; // gets the descender in pixels for the given size which represents the distance away from the baseline, positive values go up, negative values go down (be mindful because this is backwards from cheonsa's 2d convention).
		float32_c get_unquantized_horizontal_advance_for_space( float32_c font_size ) const; // gets the distance in pixels to advance the cursor horizontally for a space.

		float32_c find_kern_advance( float32_c left_font_size, char16_c left_code_point, float32_c right_font_size, char16_c right_code_point );

		size_metrics_c const * get_quantized_size_metrics( float32_c size ) const;
		static uint8_c get_quantized_size( float32_c size ); // quantizes an arbitrary font size in pixels to a descrete value, which the size that the glyph is rendered and cached at (baked). this is done to increase probability of reuse, such that if the game wants to render a glyph at many different arbitrary sizes, it will render from the cached descrete size and then scale it to the required arbitrary size.
		static float32_c get_scale_to_unquantize_size( float32_c size ); // gets the corrective scale factor to de-quantize/arbitrarily scale a previously quantized/descretized coordinate that was created for the given get_quantized_size( size ).

	};

}
