#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_list.h"
#include "cheonsa_data_stream.h"

namespace cheonsa
{

	//char8_c const * image_png_signature() { return "\211PNG\r\n\032\n"; }
	extern uint8_c const image_png_signature[ 8 ];

	// lets us define extra custom chunks to save with the png file that are not image related.
	// these chunks will probably be ignored by image viewers, but it lets us do cool things like save the game state with the png file.
	struct image_png_chunk_c
	{
		char8_c type[ 5 ]; // actual type code is only 4 bytes, but extra chararcter is for null terminator, so it can be safely treated like a string.
		uint32_c data_size;
		uint8_c * data;
		boolean_c data_is_ours;
		uint32_c crc;

		image_png_chunk_c();
		~image_png_chunk_c();

		// transfers ownership. zeros out other.
		image_png_chunk_c & operator = ( image_png_chunk_c & other );

	};

	// 2d image, bitmap, texture, etc.
	// this works as the intermediate exchange format between video api textures and various image file formats like png and jpg.
	struct image_c
	{
		sint32_c width; // width of image in pixels, can be 1 to 4096.
		sint32_c height; // height of image in pixels, can be 1 to 4096.
		sint32_c channel_count; // number of color channels, can be 1, 2, 3, or 4.
		sint32_c channel_bit_depth; // unsigned int bit-depth, can be 8 or 16.
		//core_list_c< uint8_c > palette; // optional, may only be populated if channel_count == 1 and channel_bit_depth == 8. if populated, it must contain 256 entries formatted as r8g8b8a8 (1024 bytes total size).
		core_list_c< uint8_c > data; // the raw pixel data of the image, formatted as a flat list of pixels, going row-by-row from left-to-right and top-to-bottom.
		image_c();
	};

	// converts an image from in to out, and converts between formats if needed.
	// out must define channel_count between 1 and 4.
	// out must define channel_bit_depth as 8 or 16.
	// out must define width and height to be same as in's width and height.

	// returns true if conversion is successful, and out will contain the converted 
	boolean_c image_copy_and_convert( image_c const & in, image_c & out );

	// loads from a png file just the chunks that are not related to the image.
	boolean_c image_load_from_png_just_the_extra_chunks(
		data_stream_c * png_file_stream,
		core_list_c< image_png_chunk_c > & extra_chunk_list
	);

	// decodes an image from png file format.
	boolean_c image_load_from_png(
		core_list_c< uint8_c > const & png_file_data, // the png file data to decode.
		image_c & image, // the resulting decoded image. to get the original format then set channel_count and channel_bit_depth to 0. to request a specific format then set channel_count and channel_bit_depth, then the result will be converted to fit this format.
		core_list_c< image_png_chunk_c > * extra_chunk_list // optional, if provided then will hold extra chunks that were found. these are chunks that are placed after IDAT and before IEND.
	);

	// encodes an image to png file format.
	boolean_c image_save_to_png(
		image_c const & image, // the image to encode.
		core_list_c< image_png_chunk_c > const * extra_chunk_list, // optional, if provided then will define extra chunks to save. these are chunks that are placed after IDAT and before IEND.
		core_list_c< uint8_c > & png_file_data // the resulting png file data.
	);

	// decodes an image from jpg file format.
	boolean_c image_load_from_jpg(
		core_list_c< uint8_c > const & jpg_file_data, // the jpg file data to decode.
		image_c & image // the resulting decoded image. to get the original format then set channel_count and channel_bit_depth to 0. to request a specific format then set channel_count and channel_bit_depth, then the result will be converted to fit this format.
	);

	// encodes an image to jpg file format.
	boolean_c image_save_to_jpg(
		image_c const & image, // the image to encode.
		sint32_c jpg_quality, // jpg encoding quality, between 1 and 100.
		core_list_c< uint8_c > & jpg_file_data // a pointer to a buffer that will store the result of the resulting jpeg file encode.
	);

}
