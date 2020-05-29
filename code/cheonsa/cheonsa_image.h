#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_data_stream.h"

namespace cheonsa
{

	//char8_c const * image_png_signature() { return "\211PNG\r\n\032\n"; }
	extern uint8_c const image_png_signature[ 8 ];

	// lets us define extra custom chunks to save with the png file that are not image related.
	// custom chunks should have signatures with letter case like "xxXx" (search "png chunk naming conventions" for more details).
	// this is used to save glyph data with the glyph atlas cache png when the engine stops.
	// this can be used to do cool things like save game state in a png file.
	// for image loading functions that create these as a result, they are created on the heap, so be sure to delete them when you are done with them.
	class image_png_chunk_c
	{
		friend class image_c;

	private:
		char8_c _type[ 5 ]; // actual type code is only 4 bytes, but extra chararcter is for null terminator, so it can be safely treated like a string.
		core_list_c< uint8_c > _data;
		uint32_c _crc;

	public:
		image_png_chunk_c();

		char8_c const * get_type() const;
		void_c set_type( char8_c const * value ); // for custom chunks, letter casing should always be like "xxXx".

		core_list_c< uint8_c > const & get_data() const;
		core_list_c< uint8_c > & get_data();

		uint32_c get_crc() const;

	};

	// 2d image, bitmap, texture, etc.
	// this works as the intermediate exchange format between video api textures and various image file formats like png and jpg.
	class image_c
	{
	private:
		sint32_c _width; // width of image in pixels, can be 1 to 4096.
		sint32_c _height; // height of image in pixels, can be 1 to 4096.
		sint32_c _channel_count; // number of color channels, can be 1, 2, 3, or 4.
		sint32_c _channel_bit_depth; // unsigned int bit-depth, can be 8 or 16.
		core_list_c< uint8_c > _data; // the raw pixel data of the image, formatted as a flat list of pixels, going row-by-row from left-to-right and top-to-bottom.
		//core_list_c< uint8_c > palette; // optional, may only be populated if channel_count == 1 and channel_bit_depth == 8. if populated, it must contain 256 entries formatted as r8g8b8a8 (1024 bytes total size).

	public:
		image_c();

		image_c & operator = ( image_c const & other ) = delete;

		sint32_c get_width() const;
		void_c set_width( sint32_c value );

		sint32_c get_height() const;
		void_c set_height( sint32_c value );

		sint32_c get_channel_count() const;
		void_c set_channel_count( sint32_c value );

		sint32_c get_channel_bit_depth() const;
		void_c set_channel_bit_depth( sint32_c value );

		core_list_c< uint8_c > const & get_data() const;
		core_list_c< uint8_c > & get_data(); // you may modify this reference directly.

	public:
		// converts an image from in to out, and converts between formats if needed.
		// out must define channel_count between 1 and 4.
		// out must define channel_bit_depth as 8 or 16.
		// out must define width and height to be same as in's width and height.
		// returns true if conversion is successful, and out will contain the converted 
		static boolean_c copy_and_convert( image_c const & in, image_c & out );

		// loads from a png file just the chunks that are not related to the image.
		// png_file_stream is a stream of a png file.
		// extra_chunk_list is the result. these chunks are allocated on the heap, so it is your responsibility to delete them when you're done.
		static boolean_c load_from_png_just_the_extra_chunks(
			data_stream_c * png_file_stream,
			core_list_c< image_png_chunk_c * > & extra_chunk_list
		);

		// decodes an image from png file format.
		// png_file_data is the raw data of a png file.
		// image is the resulting decoded image. to get the image in the same format as the source file, then set channel_count and channel_bit_depth to 0. to get the image in a specific format, then set channel_count and channel_bit_depth to the format that you want.
		// optional, if provided then will hold extra chunks that were found. these are chunks that are placed after IDAT and before IEND. these chunks are allocated on the heap, so it is your responsibility to delete them when you're done.
		static boolean_c load_from_png(
			core_list_c< uint8_c > const & png_file_data,
			image_c & image,
			core_list_c< image_png_chunk_c * > * extra_chunk_list
		);

		// encodes an image to png file format.
		// image is the image to encode.
		// extra_chunk_list is optional, if provided then will define extra chunks to save. these are chunks that are placed after IDAT and before IEND.
		// png_file_data is the result raw data of the png file.
		static boolean_c save_to_png(
			image_c const & image,
			core_list_c< image_png_chunk_c * > const * extra_chunk_list,
			core_list_c< uint8_c > & png_file_data
		);

		// decodes an image from jpg file format.
		// jpg_file_data is the raw file data of a jpg file.
		// image is the resulting decoded image. to get the image in the same format as the source file, then set channel_count and channel_bit_depth to 0. to get the image in a specific format, then set channel_count and channel_bit_depth to the format that you want.
		static boolean_c load_from_jpg(
			core_list_c< uint8_c > const & jpg_file_data,
			image_c & image
		);

		// encodes an image to jpg file format.
		// image is the image to encode.
		// jpg_quality is the jpg encoding quality, a value between 1 and 100.
		// jpg_file_data is the result raw data of the jpg file.
		static boolean_c save_to_jpg(
			image_c const & image,
			sint32_c jpg_quality,
			core_list_c< uint8_c > & jpg_file_data
		);

	};

	

}
