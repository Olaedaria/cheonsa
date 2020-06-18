#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_stream.h"

namespace cheonsa
{

	// opens a file for reading or writing.
	// the destructor closes the underlying stream if it happens to be open.
	class data_stream_file_c
		: public data_stream_c
	{
	private:
		void_c * _file;

	public:
		data_stream_file_c();
		virtual ~data_stream_file_c() override; // closes the underlying stream if it happens to be open.

		// opens a file for reading or writing.
		// the file path must be absolute and must be formatted with the operating system's native path format.
		// if stream_mode == data_stream_mode_e_read, then it will try to open an existing file.
		// if stream_mode == data_stream_mode_e_write, then a new file will be created. if it already exists, then it will be over written.
		boolean_c open( string16_c const & file_path, data_stream_mode_e const stream_mode );

		// closes the underlying stream.
		void_c close();

		virtual sint32_c get_size() const override; // gets the size of the file that is currently open.
		virtual sint32_c get_position() const override; // gets the position of where the next read or write will occur.
		virtual boolean_c set_position( sint32_c const position ) override; // sets the position of where the next read or write will occur.
		virtual boolean_c load( void_c * const data, sint32_c const size ) override; // reads data from the stream and advances the position. returns false if size could not be read.
		virtual boolean_c save( void_c const * const data, sint32_c const size ) override; // writes data to the stream and advances the position. returns false if size could not be written. when writing bytes to the middle of the stream, existing bytes in the stream will be overwritten (it does not insert the bytes). when writing bytes to the end of the stream, the size of the file will be extended to contain them.

	};

}
