#pragma once

#include "cheonsa__types.h"
#include "cheonsa_resource_file.h"
#include "cheonsa_resource_file_texture.h"
#include "cheonsa_glyph_manager.h"

namespace cheonsa
{

	// font formats conventionally place the coordinate frame in bottom-left, but cheonsa tranlsates this coordinate frame to top-left.
	// font formats conventionally describe the ascender and descender as distance above the base line, so that descender would usually be a negative value, but cheonsa converts descender to distance below the base line so that it will usually be a positive value too.
	class resource_file_font_c
		: public resource_file_c
	{
	public:
		typedef resource_file_reference_c< resource_file_font_c > reference_c;

	public:
		static char8_c const * get_resource_file_type_static() { return "font"; }
		virtual char8_c const * get_resource_file_type() const override { return get_resource_file_type_static(); }

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

		size_metrics_c _quantized_size_metrics[ glyph_manager_c::quantized_count ];

		virtual void_c _load( data_stream_c * stream ) override;
		virtual void_c _unload() override;

	public:
		resource_file_font_c( string16_c const & file_path );
		resource_file_font_c( resource_file_font_c const & ) = delete;
		~resource_file_font_c();
		resource_file_font_c & operator = ( resource_file_font_c const & ) = delete;

		uint32_c get_file_hash() const;

		void_c * get_free_type_face_handle() const;

		float32_c get_unquantized_ascender( float32_c font_size ) const; // gets the ascender in pixels for the given size which represents the distance away from the baseline, positive values go up, negative values go down (be mindful because this is backwards from cheonsa's 2d convention).
		float32_c get_unquantized_descender( float32_c font_size ) const; // gets the descender in pixels for the given size which represents the distance away from the baseline, positive values go up, negative values go down (be mindful because this is backwards from cheonsa's 2d convention).
		float32_c get_unquantized_horizontal_advance_for_space( float32_c font_size ) const; // gets the distance in pixels to advance the cursor horizontally for a space.

		float32_c find_kern_advance( float32_c left_font_size, char16_c left_code_point, float32_c right_font_size, char16_c right_code_point );

		size_metrics_c const * get_quantized_size_metrics( float32_c size ) const; // gets quantized size metrics for this font for the given unquantized size.

	};

}
