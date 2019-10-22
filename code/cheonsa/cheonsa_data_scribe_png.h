#pragma once

#include "cheonsa_data_stream.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	// this is only for loading the PNG file format chunk structure into memory so that it can be processed.
	// it doesn't decode the PNG image or let you save PNGs.
	// i am using a third party library called lodepng for that.
	class data_scribe_png_c
	{
	public:
		static uint8_c const png_file_signature[ 8 ];

		struct chunk_c
		{
			uint32_c data_size; // converted to native endianness.
			uint8_c type[ 4 ]; // converted to native endianness.
			uint8_c * data; // points to an offset within _file_data.
			uint32_c crc; // converted to native endianness.
		};

	private:
		core_list_c< uint8_c > _file_data; // the whole file loaded into memory.
		core_list_c< chunk_c > _chunk_list; // chunk instances from the file.

	public:
		boolean_c open( data_stream_c * stream );

	};

}
