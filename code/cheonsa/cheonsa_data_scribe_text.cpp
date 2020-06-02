#include "cheonsa_data_scribe_text.h"
#include <cassert>

namespace cheonsa
{

	data_scribe_text_c::data_scribe_text_c()
		: _stream( nullptr )
	{
	}

	void_c data_scribe_text_c::open( data_stream_c * stream )
	{
		assert( _stream == nullptr );
		assert( stream );
		_stream = stream;
	}

	void_c data_scribe_text_c::close()
	{
		assert( _stream );
		_stream = nullptr;
	}

	boolean_c data_scribe_text_c::load_next_line( string8_c & result )
	{
		// loads chunks of bytes at a time, and scans those bytes until a '\n' is encountered.
		// we do it this way because it's probably faster than loading bytes from a file stream one by one, it's certainly less calls.
		// even though the operating system can probably deal with loading bytes one by one from a file without much performance hit.
		static sint32_c const chunk_size = 100;

		assert( _stream );

		result.character_list.construct_mode_dynamic( 0 );
		sint32_c stream_size = _stream->get_size();
		sint32_c stream_position = _stream->get_position();
		sint32_c stream_position_original = stream_position;
		sint32_c bytes_to_load = 0;
		sint32_c result_length = 0;

		if ( stream_position == stream_size )
		{
			result.character_list.insert( -1, '\0' );
			return false;
		}

	load_chunk:
		bytes_to_load = stream_size - stream_position;
		if ( bytes_to_load > 0 )
		{
			if ( bytes_to_load > chunk_size )
			{
				bytes_to_load = chunk_size;
			}
			char8_c * result_buffer = result.character_list.emplace( -1, bytes_to_load );
			_stream->load( result_buffer, bytes_to_load );
			stream_position += bytes_to_load;
			for ( sint32_c i = 0; i < bytes_to_load; i++ )
			{
				if ( result_buffer[ i ] == '\n' )
				{
					result_length += i;
					_stream->set_position( stream_position_original + result_length + 1 );
					if ( result_buffer > result.character_list.get_internal_array() )
					{
						if ( result_buffer[ i - 1 ] == '\r' )
						{
							result_length--;
						}
					}
					result.character_list.set_length( result_length + 1 ); 
					result.character_list[ result_length ] = '\0';
					return true;
				}
			}
			result_length += bytes_to_load;
		}
		else
		{
			result.character_list.insert( -1, '\0' );
			return true;
		}
		goto load_chunk;
	}

}