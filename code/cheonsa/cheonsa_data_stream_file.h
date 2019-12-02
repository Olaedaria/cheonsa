#pragma once

#include "cheonsa__types.h"
#include "cheonsa_string16.h"
#include "cheonsa_data_stream.h"

namespace cheonsa
{

	class data_stream_file_c : public data_stream_c
	{
	private:
		void_c * _file;

	public:
		data_stream_file_c();
		virtual ~data_stream_file_c() override;

		// on windows the file_path can be relative to the directory of the executable, but cheonsa's convention is to always use an absolute path as best practice.
		// if stream_mode == stream_mode_write_e, then a new file will be created, or the existing file will be replaced with an empty file.
		boolean_c open( string16_c const & file_path, data_stream_mode_e const stream_mode );
		void_c close();

		virtual sint32_c get_size() const override;
		virtual sint32_c get_position() const override;
		virtual boolean_c set_position( sint32_c const position ) override;
		virtual boolean_c load( void_c * const data, sint32_c const size ) override;
		virtual boolean_c save( void_c const * const data, sint32_c const size ) override;

	};

}
