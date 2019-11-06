#pragma once

#include "cheonsa__types.h"
#include "cheonsa_data_stream.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	// if you set the position to the middle of the stream and then write to the stream, then it overwrites the existing bytes rather than inserts.
	class data_stream_memory_c : public data_stream_c
	{
	private:
		core_list_c< uint8_c > _buffer; // the underlying memory buffer.
		sint32_c _position; // current position of the stream in the buffer.

	public:
		data_stream_memory_c();
		data_stream_memory_c( data_stream_memory_c const & other ); // initializes as copy of other stream.
		virtual ~data_stream_memory_c() override;

		boolean_c get_is_open();
		boolean_c open(); // opens this stream for reading and writing and automatically resizes internal array as needed.
		boolean_c open_static( void_c const * data, sint32_c data_size ); // wraps data buffer for reading and writing, size is fixed.
		boolean_c close(); // closes this stream and frees any resources allocated by this stream.

		core_list_c< uint8_c > & get_internal_buffer(); // gets a reference to the internal buffer.

		virtual sint32_c get_size() const override;
		void_c set_size( sint32_c size );
		virtual sint32_c get_position() const override;
		virtual void_c set_position( sint32_c const position ) override;
		virtual boolean_c load( void_c * const data, sint32_c const size ) override;
		virtual boolean_c save( void_c const * const data, sint32_c const size ) override;

	};

}
