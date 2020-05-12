#pragma once

#include "cheonsa_types.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_stream.h"

namespace cheonsa
{

	class data_stream_file_c
		: public data_stream_c
	{
	private:
		void_c * _file;

	public:
		data_stream_file_c();
		virtual ~data_stream_file_c() override;

		// the file path must be absolute and must be formatted with the operating system's native path format.
		// if stream_mode == data_stream_mode_e_write, then a new file will be created. if it already exists, then it will be over written.
		// if stream_mode == data_stream_mode_e_read, then it will try to open an existing file.
		boolean_c open( string16_c const & file_path, data_stream_mode_e const stream_mode );
		void_c close();

		virtual sint32_c get_size() const override;
		virtual sint32_c get_position() const override;
		virtual boolean_c set_position( sint32_c const position ) override;
		virtual boolean_c load( void_c * const data, sint32_c const size ) override;
		virtual boolean_c save( void_c const * const data, sint32_c const size ) override;

	};

}
