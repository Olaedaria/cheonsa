#pragma once

#include "cheonsa_types.h"
#include "cheonsa_data_types.h"

namespace cheonsa
{

	// abstract base stream interface.
	class data_stream_c
	{
	public:
		virtual ~data_stream_c() = 0;
		virtual sint32_c get_size() const = 0; // gets the size in bytes of this stream.
		virtual sint32_c get_position() const = 0; // gets the current position of the read/write pointer within the stream, relative to the start of the stream.
		virtual boolean_c set_position( sint32_c const position ) = 0; // sets the position of the read/write pointer within the stream, relative to the start of the stream.
		virtual boolean_c load( void_c * const data, sint32_c const size ) = 0; // attempts to read size bytes from the stream into data buffer. returns true if the read was successful, false if otherwise.
		virtual boolean_c save( void_c const * const data, sint32_c const size ) = 0; // attempts to write size bytes from data buffer into the stream. returns true if the write was successful, false if otherwise. if the current position of the stream is somewhere in the middle of the stream, then the data that is written will override what data already exists at that position. if the current position of the stream plus the size of the data to write is larger than the current size of the stream, then the size of the stream will be increased (in most implementations anyway).

	};

}
