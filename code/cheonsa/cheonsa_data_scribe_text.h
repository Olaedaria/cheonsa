#pragma once

#include "cheonsa__types.h"
#include "cheonsa_data_stream.h"
#include "cheonsa_string8.h"

namespace cheonsa
{

	class data_scribe_text_c
	{
	private:
		data_stream_c * _stream;

	public:
		data_scribe_text_c();

		void_c open( data_stream_c * stream );
		void_c close();

		// extracts the next line of text in the file, minus the new line character(s).
		// works with both unix "\n" and windows style "\r\n" new lines.
		boolean_c load_next_line( string8_c & result );

	};

}
