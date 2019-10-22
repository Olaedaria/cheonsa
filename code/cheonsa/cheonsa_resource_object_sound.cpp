#include "cheonsa_resource_object_sound.h"
#include "cheonsa_data_scribe_binary.h"
#include "cheonsa_audio_buffer.h"
#include "cheonsa_audio_buffer_raw.h"
#include "cheonsa_audio_buffer_ogg.h"
#include "cheonsa_debug.h"

namespace cheonsa
{

	boolean_c resource_object_sound_c::_load( data_stream_c * stream )
	{
		cheonsa_assert( _is_loaded == false );

		data_scribe_binary_c scribe_binary;
		scribe_binary.open( stream, data_endianness_e_little );

		// determine file type.
		char8_c loaded_file_signature[ 4 ];
		stream->load( loaded_file_signature, 4 ); // endian agnostic.

		if ( loaded_file_signature[ 0 ] == 'R' && loaded_file_signature[ 1 ] == 'I' && loaded_file_signature[ 2 ] == 'F' && loaded_file_signature[ 3 ] == 'F' )
		{
			// gah, this is hard to read =.= but idc.
			// we need to extract the raw data from the wav file, because the audio_buffer_raw_c is not programmed to decode file formats.

			// RIFF header.
			uint32_c file_size = scribe_binary.load_uint32(); // little endian.
			char8_c file_format[ 4 ];
			stream->load( file_format, 4 ); // endian agnostic.
			if ( !( file_format[ 0 ] == 'W' && file_format[ 1 ] == 'a' && file_format[ 2 ] == 'v' && file_format[ 3 ] == 'E' ) )
			{
				return false;
			}

			// format chunk.
			boolean_c has_format = false;
			uint16_c audio_format;
			uint16_c channel_count;
			uint32_c sample_rate;
			uint32_c byte_rate;
			uint16_c block_align;
			uint16_c bits_per_sample;

			// data chunk.
			boolean_c has_data = false;
			uint8_c * wave_data = 0;
			uint32_c wave_data_size;

			do
			{
				// load chunk header.
				char8_c chunk_id[ 4 ];
				stream->load( chunk_id, 4 ); // endianness agnostic.
				uint32_c chunk_size = scribe_binary.load_uint32(); // little endian.
				uint32_c next_chunk = stream->get_position() + chunk_size;

				// process chunks that we recognize.
				if ( chunk_id[ 0 ] == 'f' && chunk_id[ 1 ] == 'm' && chunk_id[ 2 ] == 't' && chunk_id[ 3 ] == ' ' )
				{
					// load format chunk
					has_format = true;
					audio_format = scribe_binary.load_uint16();
					channel_count = scribe_binary.load_uint16();
					sample_rate = scribe_binary.load_uint32();
					byte_rate = scribe_binary.load_uint32();
					block_align = scribe_binary.load_uint16();
					bits_per_sample = scribe_binary.load_uint16();

					if ( audio_format != 1 )
					{
						if ( wave_data != nullptr )
						{
							delete[] wave_data;
							wave_data = 0;
						}
						return false;
					}
				}
				else if ( chunk_id[ 0 ] == 'd' && chunk_id[ 1 ] == 'a' && chunk_id[ 2 ] == 't' && chunk_id[ 3 ] == 'a' )
				{
					// load data chunk.
					has_data = true;
					wave_data_size = chunk_size;
					wave_data = new uint8_c[wave_data_size];
					stream->load( wave_data, wave_data_size );
				}

				// go to next chunk.
				stream->set_position( next_chunk );
			} while ( stream->get_position() < stream->get_size() );

			if ( has_format && has_data )
			{
				// initialize audio buffer.
				audio_sample_format_e sample_format;
				if ( bits_per_sample == 8 )
				{
					sample_format = audio_sample_format_e_sint8;
				}
				else if ( bits_per_sample == 16 )
				{
					sample_format = audio_sample_format_e_sint16;
				}

				sint32_c frame_frequency = sample_rate;

				audio_buffer_raw_c * buffer_raw = new audio_buffer_raw_c( sample_format, channel_count, frame_frequency, wave_data, wave_data_size );

				_is_loaded = buffer_raw->get_is_loaded();

				if ( _is_loaded )
				{
					_audio_buffer = buffer_raw;
				}
				else
				{
					buffer_raw->remove_reference();
					buffer_raw = nullptr;
				}
			}

			if ( wave_data != 0 )
			{
				delete[] wave_data;
				wave_data = 0;
				wave_data_size = 0;
			}
		}
		else if ( loaded_file_signature[ 0 ] == 'O' && loaded_file_signature[ 1 ] == 'g' && loaded_file_signature[ 2 ] == 'g' && loaded_file_signature[ 3 ] == 'S' )
		{
			stream->set_position( 0 );

			// just load the whole file into memory, and create a new audio_buffer_ogg_c which is programmed to decode the ogg file format.
			sint32_c file_data_size = stream->get_size();
			uint8_c * file_data = new uint8_c[ file_data_size ];
			stream->load( file_data, file_data_size );

			audio_buffer_ogg_c * buffer_ogg = new audio_buffer_ogg_c( file_data, file_data_size );

			_is_loaded = buffer_ogg->get_is_loaded();

			if ( _is_loaded )
			{
				_audio_buffer = buffer_ogg;
			}
			else
			{
				buffer_ogg->remove_reference();
				buffer_ogg = nullptr;
			}

			delete[] file_data;
		}

		if ( _is_loaded )
		{
			on_load.invoke( this );
		}

		return _is_loaded;
	}

	void_c resource_object_sound_c::_unload()
	{
		cheonsa_assert( _is_loaded == true );
		_is_loaded = false;
		on_unload.invoke( this );
		if ( _audio_buffer )
		{
			_audio_buffer->remove_reference();
			_audio_buffer = nullptr;
		}
	}

	resource_object_sound_c::resource_object_sound_c()
		: resource_object_c()
		, _audio_buffer( nullptr )
	{
	}

	resource_object_sound_c::~resource_object_sound_c()
	{
		cheonsa_assert( _is_loaded == false );
	}

	audio_buffer_c const * resource_object_sound_c::get_audio_buffer() const
	{
		return _audio_buffer;
	}
}
