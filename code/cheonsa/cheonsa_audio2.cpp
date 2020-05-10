#include "cheonsa_audio2.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_data_scribe_binary.h"

#include <cassert>
#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <Mmsystem.h> // for audio output on windows.
#endif

namespace cheonsa
{

	#define cheonsa_audio_minimum_distance 0.01f // 1 centimeter radius, this makes point source a sphere, and prevents loudness from approaching infinity.
	#define cheonsa_audio_defactor_distance 0.1f // 10 centimeter radius, directional influence begins to be factored out when source and listener are less than this distance apart.
	#define cheonsa_audio_minimum_attenuation 0.0001f // if sources are attenuated to be less than this then they will not be mixed to the output.

	void_c audio2_convert_sint16_to_sint8( sint32_c samples_count, sint16_c const * samples_in, sint8_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint8_c >( samples_in[ i ] * 0xFF / 0xFFFF );
		}
	}

	void_c audio2_convert_float32_to_sint8( sint32_c samples_count, float32_c const * samples_in, sint8_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint8_c >( samples_in[ i ] * 127.0f );
		}
	}

	void_c audio2_convert_sint8_to_sint16( sint32_c samples_count, sint8_c const * samples_in, sint16_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint16_c >( samples_in[i] * 0xFFFF / 0xFF );
		}
	}

	void_c audio2_convert_float32_to_sint16( sint32_c samples_count, float32_c const * samples_in, sint16_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint16_c >( samples_in[ i ] * 32767.0f );
		}
	}

	void_c audio2_convert_sint8_to_float32( sint32_c samples_count, sint8_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< float32_c >( samples_in[ i ] ) / 127.0f;
		}
	}

	void_c audio2_convert_sint16_to_float32( sint32_c samples_count, sint16_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< float32_c >( samples_in[ i ] ) / 32767.0f;
		}
	}

	void_c audio2_convert_6_to_1( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 6;
			samples_out[ i ] = ( samples_in[ j ] + samples_in[ j + 1 ] + samples_in[ j + 2 ] + samples_in[ j + 3 ] + samples_in[ j + 4 ] + samples_in[ j + 5 ] ) / 6.0f;
		}
	}

	void_c audio2_convert_2_to_1( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 2;
			samples_out[ i ] = ( samples_in[ j ] + samples_in[ j + 1 ] ) / 2.0f;
		}
	}

	void_c audio2_convert_1_to_2( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 2;
			samples_out[ j + 1 ] = samples_out[ j ] = ( samples_in[ i ] );
		}
	}

	void_c audio2_convert_6_to_2( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 6;
			sint32_c k = i * 2;
			samples_out[ k ] = ( samples_in[ j ] + samples_in[ j + 4 ] ) / 2.0f; // left = front left + back left.
			samples_out[ k + 1 ] = ( samples_in[ j + 1 ] + samples_in[ j + 5 ] ) / 2.0f; // right = front right + back right.
		}
	}

	void_c audio2_convert_1_to_6( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 6;
			samples_out[ j ] = samples_out[ j + 1 ] = samples_out[ j + 4 ] = samples_out[ j + 5 ] = samples_in[ i ];
			samples_out[ j + 2 ] = samples_out[ j + 3 ] = 0.0f;
		}
	}

	void_c audio2_convert_2_to_6( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 2;
			sint32_c k = i * 6;
			samples_out[ k ] = samples_out[ k + 4 ] = samples_in[ j ];
			samples_out[ k + 1 ] = samples_out[ k + 5 ] = samples_in[ j + 1 ];
			samples_out[ k + 2 ] = samples_out[ k + 3 ] = 0.0f;
		}
	}

	void_c audio2_mix(
		audio2_sample_format_e input_sample_format,
		sint32_c input_channel_count,
		sint32_c input_sample_count,
		void_c const * input_sample_buffer,
		sint32_c output_channel_count,
		sint32_c output_sample_count,
		float32_c * output_sample_buffer,
		float32_c * output_channel_volumes )
	{
		// is 0xFFFF (65535) enough samples for a temporary buffer?
		// output sample rate is typically 44100 samples per second.
		// this is 1470 samples per frame at 30 frames per second.
		// this is 735 samples per second at mono.
		// this is 1470 samples per second at stereo.
		// this is 4410 samples per second at 5.1 surround.
		// play back speed will scale the number of samples needed, which could incur buffer over flow, so it will need to be limited.
		// play back speed of 5 with 6 channels is 22050 samples per frame, still well below 65535.
		// at 60 frames per second these values are halved.
		static float32_c float32_temp_buffer_a[ 0xFFFF ];
		static float32_c float32_temp_buffer_b[ 0xFFFF ];

		if ( input_sample_count == 0 || output_sample_count == 0 )
		{
			return;
		}

		assert( input_channel_count == 1 || input_channel_count == 2 || input_channel_count == 6 );
		assert( input_sample_format == audio2_sample_format_e_sint8 || input_sample_format == audio2_sample_format_e_sint16 || input_sample_format == audio2_sample_format_e_float32 );
		assert( input_sample_count > 0 );
		assert( input_sample_buffer != nullptr );
		assert( output_channel_count == 1 || output_channel_count == 2 || output_channel_count == 6 );
		assert( output_sample_count > 0 );
		assert( output_sample_buffer != nullptr );
		assert( output_channel_volumes != nullptr );
		assert( input_sample_count * input_channel_count < 0xFFFF );
		assert( output_sample_count * output_channel_count < 0xFFFF );

		// convert from input sample format to utput sample format (float32_c).
		float32_c const * stage_0 = nullptr;
		if ( input_sample_format == audio2_sample_format_e_sint8 )
		{
			audio2_convert_sint8_to_float32( input_sample_count * input_channel_count, static_cast< sint8_c const * >( input_sample_buffer ), float32_temp_buffer_a );
			stage_0 = float32_temp_buffer_a;
		}
		else if ( input_sample_format == audio2_sample_format_e_sint16 )
		{
			audio2_convert_sint16_to_float32( input_sample_count * input_channel_count, static_cast< sint16_c const * >( input_sample_buffer ), float32_temp_buffer_a );
			stage_0 = float32_temp_buffer_a;
		}
		else if ( input_sample_format == audio2_sample_format_e_float32 )
		{
			stage_0 = static_cast< float32_c const * >( input_sample_buffer );
		}
		assert( stage_0 );

		// convert from input channel count to output channel count.
		float32_c const * stage_1 = nullptr;
		if ( input_channel_count == output_channel_count )
		{
			stage_1 = stage_0;
		}
		else if ( output_channel_count == 1 )
		{
			if ( input_channel_count == 2 )
			{
				audio2_convert_2_to_1( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
			else if ( input_channel_count == 6 )
			{
				audio2_convert_6_to_1( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
		}
		else if ( output_channel_count == 2 )
		{
			if ( input_channel_count == 1 )
			{
				audio2_convert_1_to_2( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
			else if ( input_channel_count == 6 )
			{
				audio2_convert_6_to_2( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
		}
		else if ( output_channel_count == 6 )
		{
			if ( input_channel_count == 1 )
			{
				audio2_convert_1_to_6( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
			else if ( input_channel_count == 2 )
			{
				audio2_convert_2_to_6( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
		}
		assert( stage_1 );

		// convert input sample count to output sample count and apply volume.
		float32_c const * input_samples = stage_1;
		if ( input_sample_count >= output_sample_count )
		{
			// input is at same or higher frequency to output so we will use nearest neighbor sampling.
			for ( sint32_c output_frame_index = 0; output_frame_index < output_sample_count; output_frame_index++ )
			{
				sint32_c output_sample_index = output_frame_index * output_channel_count;
				for ( sint32_c output_channel_index = 0; output_channel_index < output_channel_count; output_channel_index++ )
				{
					output_sample_buffer[ output_sample_index + output_channel_index ] += input_samples[ output_sample_index + output_channel_index ] * output_channel_volumes[ output_channel_index ];
				}
			}
		}
		else
		{
			// input is at lower frequency than output so we will use linearly interpolated sampling.
			for ( sint32_c output_index = 0; output_index < output_sample_count; output_index++ )
			{
				sint32_c output_index_2 = output_index * output_channel_count;
				float64_c input_index_scalar = static_cast< float64_c >( output_index ) / static_cast< float64_c >( output_sample_count ) * static_cast< float64_c >( input_sample_count );
				sint32_c input_index_2_a = static_cast< sint32_c >( input_index_scalar ) * output_channel_count;
				sint32_c input_index_2_b = input_index_2_a + output_channel_count;
				float32_c input_blend = static_cast< float32_c >( ops::math_modulo( input_index_scalar, 1.0 ) );
				for ( sint32_c channel_index = 0; channel_index < output_channel_count; channel_index++ )
				{
					output_sample_buffer[ output_index_2 + channel_index ] += ( ( input_samples[ input_index_2_a + channel_index ] * ( 1.0f - input_blend ) ) + ( input_samples[ input_index_2_b + channel_index ] * input_blend ) ) * output_channel_volumes[ channel_index ];
				}
			}
		}
	}

	audio2_wave_buffer_c::state_c::state_c( data_stream_c * stream )
		: _reference_counter()
		, _format( audio2_wave_buffer_format_e_none )
		, _channel_count( 0 )
		, _sample_format( audio2_sample_format_e_none )
		, _sample_rate( 0 )
		, _sample_count( 0 )
		, _data_buffer( nullptr )
		, _data_buffer_size( 0 )
	{
		assert( stream != nullptr );
		assert( stream->get_position() == 0 );

		data_scribe_binary_c scribe;
		scribe.set_stream( stream );
		scribe.set_byte_order( byte_order_e_little );

		// determine file type.
		char8_c loaded_file_signature[ 4 ];
		stream->load( loaded_file_signature, 4 ); // endian agnostic.

		if ( ops::memory_compare( loaded_file_signature, "RIFF", 4 ) )
		{
			// this is hard to read =.= but idc.
			// load the riff wave file.

			// RIFF header.
			uint32_c file_size;
			if ( !scribe.load_uint32( file_size ) ) // little endian.
			{
				return;
			}
			char8_c file_format[ 4 ];
			stream->load( file_format, 4 ); // endian agnostic.
			if ( !ops::memory_compare( file_format, "WavE", 4 ) )
			{
				return;
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

			do
			{
				// load chunk header.
				char8_c chunk_id[ 4 ];
				stream->load( chunk_id, 4 ); // endianness agnostic.
				uint32_c chunk_size;
				if ( !scribe.load_uint32( chunk_size ) ) // little endian.
				{
					return;
				}
				uint32_c next_chunk = stream->get_position() + chunk_size;

				// process chunks that we recognize.
				if ( ops::memory_compare( chunk_id, "fmt ", 4 ) )
				{
					// load format chunk
					assert( has_format == false );
					if ( has_format == false )
					{
						has_format = true;
						if ( !scribe.load_uint16( audio_format ) )
						{
							return;
						}
						if ( !scribe.load_uint16( channel_count ) )
						{
							return;
						}
						if ( !scribe.load_uint32( sample_rate ) )
						{
							return;
						}
						if ( !scribe.load_uint32( byte_rate ) )
						{
							return;
						}
						if ( !scribe.load_uint16( block_align ) )
						{
							return;
						}
						if ( !scribe.load_uint16( bits_per_sample ) )
						{
							return;
						}
					}
				}
				else if ( ops::memory_compare( chunk_id, "data", 4 ) )
				{
					// load data chunk.
					assert( has_data == false );
					if ( has_data == false )
					{
						has_data = true;
						_data_buffer_size = chunk_size;
						_data_buffer = new uint8_c[ _data_buffer_size ];
						if ( !stream->load( _data_buffer, _data_buffer_size ) )
						{
							delete[] _data_buffer;
							_data_buffer = nullptr;
							_data_buffer_size = 0;
							return;
						}
					}
				}

				// go to next chunk.
				if ( !stream->set_position( next_chunk ) )
				{
					return;
				}
			} while ( stream->get_position() < stream->get_size() );

			if ( !has_format || !has_data )
			{
				if ( _data_buffer )
				{
					delete[] _data_buffer;
					_data_buffer = nullptr;
					_data_buffer_size = 0;
				}
				return;
			}

			// copy info about the raw buffer.
			_channel_count = channel_count;
			if ( bits_per_sample == 8 )
			{
				_sample_format = audio2_sample_format_e_sint8;
				_sample_size = 1;
			}
			else if ( bits_per_sample == 16 )
			{
				_sample_format = audio2_sample_format_e_sint16;
				_sample_size = 2;
			}
			else
			{
				return;
			}
			_sample_rate = sample_rate;
			_sample_count = _data_buffer_size / ( channel_count * bits_per_sample / 8 );

			// done.
			_format = audio2_wave_buffer_format_e_raw;
		}
		else if ( ops::memory_compare( loaded_file_signature, "OggS", 4 ) )
		{
			if ( !stream->set_position( 0 ) )
			{
				return;
			}

			_data_buffer_size = stream->get_size();
			_data_buffer = new uint8_c[ _data_buffer_size ];
			if ( !stream->load( _data_buffer, _data_buffer_size ) )
			{
				delete[] _data_buffer;
				_data_buffer = nullptr;
				_data_buffer_size = 0;
				return;
			}

			// load info about the ogg.
			int error = 0;
			int bytes_used = 0;
			stb_vorbis * temp_stb_vorbis = stb_vorbis_open_memory( _data_buffer, _data_buffer_size, &error, nullptr );
			if ( temp_stb_vorbis == nullptr )
			{
				delete[] _data_buffer;
				_data_buffer = nullptr;
				_data_buffer_size = 0;
				return;
			}
			stb_vorbis_info temp_stb_vorbis_info = stb_vorbis_get_info( temp_stb_vorbis );
			_channel_count = temp_stb_vorbis_info.channels;
			_sample_format = audio2_sample_format_e_float32;
			_sample_size = 4;
			_sample_rate = temp_stb_vorbis_info.sample_rate;
			_sample_count = stb_vorbis_stream_length_in_samples( temp_stb_vorbis );
			stb_vorbis_close( temp_stb_vorbis );

			// done.
			_format = audio2_wave_buffer_format_e_ogg;
		}
	}

	audio2_wave_buffer_c::state_c::~state_c()
	{
		if ( _data_buffer != nullptr )
		{
			delete[] _data_buffer;
			_data_buffer = nullptr;
		}
	}

	audio2_wave_buffer_c::state_c * audio2_wave_buffer_c::state_c::make_new_instance( data_stream_c * stream )
	{
		return new state_c( stream );
	}

	boolean_c audio2_wave_buffer_c::state_c::add_reference()
	{
		return _reference_counter.increment() != 0;
	}

	void_c audio2_wave_buffer_c::state_c::remove_reference()
	{
		if ( _reference_counter.decrement() == 0 )
		{
			delete this;
		}
	}

	core_linked_list_c< audio2_wave_buffer_c * > audio2_wave_buffer_c::_instance_list;

	audio2_wave_buffer_c::audio2_wave_buffer_c()
		: _instance_list_node( this )
		, _reference_count( 0 )
		, _state( nullptr )
	{
		_instance_list.insert_at_end( &_instance_list_node );
	}

	audio2_wave_buffer_c::~audio2_wave_buffer_c()
	{
		if ( _state != nullptr )
		{
			_state->remove_reference();
			_state = nullptr;
		}
		_instance_list.remove( &_instance_list_node );
	}

	audio2_wave_buffer_c * audio2_wave_buffer_c::make_new_instance()
	{
		return new audio2_wave_buffer_c();
	}

	void_c audio2_wave_buffer_c::add_reference()
	{
		_reference_count++;
	}

	void_c audio2_wave_buffer_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	void_c audio2_wave_buffer_c::load_new_state( data_stream_c * stream )
	{
		state_c * old_state = _state;
		_state = state_c::make_new_instance( stream );
		_state->add_reference();
		if ( old_state != nullptr )
		{
			old_state->remove_reference();
		}
	}

	void_c audio2_wave_buffer_c::release_state()
	{
		state_c * old_state = _state;
		_state = nullptr;
		if ( old_state != nullptr )
		{
			old_state->remove_reference();
		}
	}

	boolean_c audio2_wave_player_c::_start()
	{
		// default to not playing.
		_is_playing = false;
		_sample = 0;

		// check that wave player can be played.
		if ( ( _wave_buffer == nullptr ) || ( _wave_buffer->_state == nullptr ) || ( _wave_buffer->_state->_format == audio2_wave_buffer_format_e_none ) )
		{
			return false;
		}

		// replace wave buffer state link if needed.
		if ( _wave_buffer_state == nullptr || _wave_buffer_state != _wave_buffer->_state )
		{
			// unlink wave buffer state with wave player if needed.
			if ( _wave_buffer_state != nullptr )
			{
				if ( _ogg_stb_vorbis != nullptr )
				{
					stb_vorbis_close( _ogg_stb_vorbis );
					_ogg_stb_vorbis = nullptr;
				}
				_wave_buffer_state->remove_reference();
				_wave_buffer_state = nullptr;
			}

			// try to open ogg stream if needed.
			if ( _wave_buffer_state->_format == audio2_wave_buffer_format_e_ogg )
			{
				assert( _ogg_stb_vorbis == nullptr );
				int error = 0;
				_ogg_stb_vorbis = stb_vorbis_open_memory( _wave_buffer_state->_data_buffer, _wave_buffer_state->_data_buffer_size, &error, nullptr );
				if ( _ogg_stb_vorbis == nullptr )
				{
					return false;
				}
			}

			// link wave buffer state with wave player.
			_wave_buffer_state = _wave_buffer->_state;
			_wave_buffer_state->add_reference();
		}

		// play.
		_is_playing = true;
		if ( _randomize_seek )
		{
			_sample = ops::random_sint32( 0, _wave_buffer_state->_sample_count );
		}
		if ( _wave_buffer_state->_format == audio2_wave_buffer_format_e_ogg )
		{
			stb_vorbis_seek( _ogg_stb_vorbis, _sample );
		}

		// done.
		return true;
	}

	boolean_c audio2_wave_player_c::_advance( float32_c time_step )
	{
		return true;
	}

	boolean_c audio2_wave_player_c::_sample_and_mix( sint32_c output_channel_count, sint32_c output_sample_rate, sint32_c output_sample_count, float32_c * output_sample_buffer, float32_c * output_channel_volumes )
	{
		assert( _is_playing && _wave_buffer_state != nullptr && _wave_buffer_state->_format != audio2_wave_buffer_format_e_none );

		// sample from the wave buffer and mix with output.
		if ( _wave_buffer_state->_format == audio2_wave_buffer_format_e_raw )
		{
			// sample from the raw buffer.
			float64_c output_duration = static_cast< float64_c >( output_sample_count ) / static_cast< float64_c >( output_sample_rate );
			sint32_c input_sample_count = static_cast< sint32_c >( output_duration * _speed * static_cast< float64_c >( _wave_buffer_state->_sample_rate ) ); // number of samples that we need to decode from the stream to satisfy the output.
			if ( _sample + input_sample_count >= _wave_buffer_state->_sample_count ) // detect if end is reached.
			{
				if ( _sample > _wave_buffer_state->_sample_count ) // not enough input to cover output.
				{
					input_sample_count = _wave_buffer_state->_sample_count - _sample; // cap input sample count to what is available.
					output_duration = static_cast< float64_c >( input_sample_count ) / static_cast< float64_c >( _wave_buffer_state->_sample_rate ); // scale output proportionally to shortened input.
					output_sample_count = static_cast< sint32_c >( output_duration * static_cast< float64_c >( output_sample_rate ) );
				}
				_is_playing = false; // finish play back.
				_sample = 0;
			}
			else
			{
				_sample += input_sample_count; // advance play back for next update.
			}

			// find input sample buffer offset from _current_sample.
			void_c const * input_sample_buffer = &_wave_buffer_state->_data_buffer[ _wave_buffer_state->_sample_size * _wave_buffer_state->_channel_count * _sample ];

			// do the mix.
			audio2_mix( _wave_buffer_state->_sample_format, _wave_buffer_state->_channel_count, input_sample_count, input_sample_buffer, output_channel_count, output_sample_count, output_sample_buffer, output_channel_volumes );
		}
		else if ( _wave_buffer_state->_format == audio2_wave_buffer_format_e_ogg )
		{
			// audio thread should not call us if we aren't ready.
			assert( _ogg_stb_vorbis != nullptr );

			// decode the ogg, sample from and advance the stream.
			float64_c output_duration = static_cast< float64_c >( output_sample_count ) / static_cast< float64_c >( output_sample_rate );
			sint32_c input_sample_count_needed = static_cast< sint32_c >( output_duration * _speed * static_cast< float64_c >( _wave_buffer_state->_sample_rate ) ); // number of samples that we need to decode from the stream to satisfy the output.
			float32_c input_sample_buffer[ 0xFFFF ];
			assert( input_sample_count_needed * _wave_buffer_state->_channel_count < 0xFFFF );
			sint32_c input_sample_count = stb_vorbis_get_samples_float_interleaved( _ogg_stb_vorbis, _wave_buffer_state->_channel_count, input_sample_buffer, input_sample_count_needed * _wave_buffer_state->_channel_count );
			_sample += input_sample_count; // update track of current play back so we can determine when we reach the end.
			if ( _sample >= _wave_buffer->_state->_sample_count ) // detect if end is reached.
			{
				if ( input_sample_count < input_sample_count_needed )
				{
					output_duration = static_cast< float64_c >( input_sample_count ) / static_cast< float64_c >( _wave_buffer_state->_sample_rate ); // scale output proportionally to shortened input.
					output_sample_count = static_cast< sint32_c >( output_duration * static_cast< float64_c >( output_sample_rate ) );
				}
				_is_playing = false; // finish play back.
				_sample = 0;
			}

			// do the mix.
			audio2_mix( audio2_sample_format_e_float32, _wave_buffer_state->_channel_count, input_sample_count_needed, input_sample_buffer, output_channel_count, output_sample_count, output_sample_buffer, output_channel_volumes );
		}

		return true;
	}

	core_linked_list_c< audio2_wave_player_c * > audio2_wave_player_c::_instance_list;

	audio2_wave_player_c::audio2_wave_player_c()
		: _instance_list_node( this )
		, _wave_player_list_node( this )
		, _reference_count( 0 )
		, _wave_buffer( nullptr )
		, _wave_buffer_state( nullptr )
		, _randomize_seek( false )
		, _is_queued( false )
		, _is_playing( false )
		, _layer( audio2_layer_e_effect )
		, _volume( 1.0f )
		, _speed( 1.0f )
		, _ogg_stb_vorbis( nullptr )
		, _sample( 0 )
		, _linked_audio_scene_source( nullptr )
		, _linked_audio_interface( nullptr )
	{
		_instance_list.insert_at_end( &_instance_list_node );
	}

	audio2_wave_player_c::~audio2_wave_player_c()
	{
		_instance_list.remove( &_instance_list_node );
		if ( _ogg_stb_vorbis != nullptr )
		{
			stb_vorbis_close( _ogg_stb_vorbis );
			_ogg_stb_vorbis = nullptr;
		}
		if ( _wave_buffer_state != nullptr )
		{
			_wave_buffer_state->remove_reference();
			_wave_buffer_state = nullptr;
		}
		if ( _wave_buffer != nullptr )
		{
			_wave_buffer->remove_reference();
			_wave_buffer = nullptr;
		}
	}

	audio2_wave_player_c * audio2_wave_player_c::make_new_instance()
	{
		return new audio2_wave_player_c();
	}

	void_c audio2_wave_player_c::add_reference()
	{
		_reference_count++;
	}

	void_c audio2_wave_player_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	audio2_wave_buffer_c * audio2_wave_player_c::get_wave_buffer() const
	{
		return _wave_buffer;
	}

	void_c audio2_wave_player_c::set_wave_buffer( audio2_wave_buffer_c * value )
	{
		if ( _wave_buffer != nullptr )
		{
			_wave_buffer->remove_reference();
			_wave_buffer = nullptr;
		}
		_wave_buffer = value;
		if ( _wave_buffer != nullptr )
		{
			_wave_buffer->add_reference();
		}
	}

	audio2_layer_e audio2_wave_player_c::get_layer() const
	{
		return _layer;
	}

	void_c audio2_wave_player_c::set_layer( audio2_layer_e value )
	{
		_layer = value;
	}

	float32_c audio2_wave_player_c::get_volume() const
	{
		return _volume;
	}

	void_c audio2_wave_player_c::set_volume( float32_c value )
	{
		_volume = ops::math_clamp( value, 0.0f, 1000.0f );
	}

	float32_c audio2_wave_player_c::get_speed() const
	{
		return _speed;
	}

	void_c audio2_wave_player_c::set_speed( float32_c value )
	{
		_speed = ops::math_clamp( value, 0.2f, 5.0f );
	}

	boolean_c audio2_wave_player_c::get_appears_to_be_playing() const
	{
		return _is_queued || _is_playing;
	}

	audio2_scene_source_c::audio2_scene_source_c()
		: _scene_source_list_node( this )
		, _reference_count( 0 )
		, _world_space_position()
		, _world_space_basis( matrix32x3x3_c::get_identity() )
		, _world_space_velocity()
		, _wave_player( nullptr )
		, _linked_audio_scene( nullptr )
	{
		_wave_player = new audio2_wave_player_c();
		_wave_player->_linked_audio_scene_source = this;
	}

	audio2_scene_source_c::~audio2_scene_source_c()
	{
		_wave_player->remove_reference();
	}

	audio2_scene_source_c * audio2_scene_source_c::make_new_instance()
	{
		return new audio2_scene_source_c();
	}

	void_c audio2_scene_source_c::add_reference()
	{
		_reference_count++;
	}

	void_c audio2_scene_source_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	void_c audio2_scene_source_c::set_world_space_transform( transform3d_c const & value )
	{
		_world_space_position = value.position;
		_world_space_basis = value.get_unscaled_basis();
	}

	vector64x3_c const & audio2_scene_source_c::get_world_space_position() const
	{
		return _world_space_position;
	}

	matrix32x3x3_c const & audio2_scene_source_c::get_world_space_basis() const
	{
		return _world_space_basis;
	}

	vector32x3_c const & audio2_scene_source_c::get_world_space_velocity() const
	{
		return _world_space_velocity;
	}

	audio2_wave_player_c * audio2_scene_source_c::get_wave_player() const
	{
		return _wave_player;
	}

	float32_c audio2_scene_source_c::get_effective_range_squared() const
	{
		return _wave_player->_volume / ( 4.0f * constants< float32_c >::pi() * cheonsa_audio_minimum_attenuation );
	}

	audio2_scene_listener_c::audio2_scene_listener_c()
		: _reference_count( 0 )
		, _world_space_position()
		, _world_space_basis( matrix32x3x3_c::get_identity() )
		, _world_space_velocity()
		, _damping( 0.0f )
		, _sensitivity( 1.0f )
		, _linked_audio_scene( nullptr )
	{
	}

	audio2_scene_listener_c * audio2_scene_listener_c::make_new_instance()
	{
		return new audio2_scene_listener_c();
	}

	void_c audio2_scene_listener_c::add_reference()
	{
		_reference_count++;
	}

	void_c audio2_scene_listener_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	void_c audio2_scene_listener_c::set_world_space_transform( transform3d_c const & value )
	{
		_world_space_position = value.position;
		_world_space_basis = value.get_unscaled_basis();
	}

	vector64x3_c const & audio2_scene_listener_c::get_world_space_position() const
	{
		return _world_space_position;
	}

	matrix32x3x3_c const & audio2_scene_listener_c::get_world_space_basis() const
	{
		return _world_space_basis;
	}

	vector32x3_c const & audio2_scene_listener_c::get_world_space_velocity() const
	{
		return _world_space_velocity;
	}

	float32_c audio2_scene_listener_c::get_damping() const
	{
		return _damping;
	}

	void_c audio2_scene_listener_c::set_damping( float32_c value )
	{
		_damping = value;
	}

	float32_c audio2_scene_listener_c::get_sensitivity() const
	{
		return _sensitivity;
	}

	void_c audio2_scene_listener_c::set_sensitivity( float32_c value )
	{
		_sensitivity = 1.0f;
	}

	audio2_scene_c::audio2_scene_c()
		: _reference_count( 0 )
		, _scene_listener( nullptr )
		, _scene_source_list()
		, _queued_scene_source_list()
		, _linked_audio_interface( nullptr )
	{
	}

	audio2_scene_c::~audio2_scene_c()
	{
		if ( _scene_listener != nullptr )
		{
			_scene_listener->remove_reference();
			_scene_listener = nullptr;
		}

		core_linked_list_c< audio2_scene_source_c * >::node_c const * sources_node = _scene_source_list.get_first();
		while ( sources_node != nullptr )
		{
			sources_node->get_value()->remove_reference();
			sources_node = sources_node->get_next();
		}
		_scene_source_list.remove_all_quick_and_dirty();

		for ( sint32_c i = 0; i < _queued_scene_source_list.get_length(); i++ )
		{
			_queued_scene_source_list[ i ].instance->remove_reference();
		}
		_queued_scene_source_list.remove_all();
	}

	audio2_scene_c * audio2_scene_c::make_new_instance()
	{
		return new audio2_scene_c();
	}

	void_c audio2_scene_c::add_reference()
	{
		_reference_count++;
	}

	void_c audio2_scene_c::remove_reference()
	{
		assert( _reference_count > 0 );
		_reference_count--;
		if ( _reference_count == 0 )
		{
			delete this;
		}
	}

	audio2_scene_listener_c * audio2_scene_c::get_scene_listener() const
	{
		return _scene_listener;
	}

	void_c audio2_scene_c::set_scene_listener( audio2_scene_listener_c * value )
	{
		_critical_section.enter();
		if ( _scene_listener )
		{
			_scene_listener->remove_reference();
		}
		_scene_listener = value;
		if ( _scene_listener )
		{
			_scene_listener->add_reference();
		}
		_critical_section.exit();
	}

	void_c audio2_scene_c::add_scene_source( audio2_scene_source_c * value, boolean_c randomize_seek )
	{
		assert( value != nullptr );
		if ( value->_wave_player->_is_queued == false )
		{
			_critical_section.enter();
			value->add_reference();
			value->_wave_player->_is_queued = true;
			queued_scene_source_c * queued_scene_source = _queued_scene_source_list.emplace( -1, 1 );
			queued_scene_source->operation = audio2_queue_operation_e_add;
			queued_scene_source->instance = value;
			queued_scene_source->instance->_wave_player->_randomize_seek = randomize_seek;
			_critical_section.exit();
		}
	}

	void_c audio2_scene_c::remove_scene_source( audio2_scene_source_c * value )
	{
		assert( value != nullptr );
		_critical_section.enter();
		value->add_reference();
		queued_scene_source_c * queued_scene_source = _queued_scene_source_list.emplace( -1, 1 );
		queued_scene_source->operation = audio2_queue_operation_e_remove;
		queued_scene_source->instance = value;
		_critical_section.exit();
	}

	void_c audio2_interface_c::_compressor_update( float32_c * sample_buffer, sint32_c sample_count, sint32_c channel_count, float32_c duration )
	{
		float32_c compressor_threshold = _compressor_threshold;
		float32_c compressor_fall_off_rate = ( 1.0f / _compressor_fall_off_duration ) * ( duration / static_cast< float32_c >( sample_count ) );
		float32_c compressor_peak = _compressor_peak;
		sint32_c sample_element_count = sample_count * channel_count;
		for ( sint32_c i = 0; i < sample_element_count; i += channel_count )
		{
			// find largest magnitude sample element in current sample.
			float32_c sample_magnitude = 0.0f;
			for ( sint32_c j = 0; j < channel_count; j++ )
			{
				float32_c sample_element = sample_buffer[ i + j ];
				if ( sample_element < 0.0f )
				{
					sample_element = -sample_element;
				}
				if ( sample_element > sample_magnitude )
				{
					sample_magnitude = sample_element;
				}
			}

			// enable compressor clip thing.
			if ( sample_magnitude > compressor_threshold )
			{
				compressor_peak = sample_magnitude;
			}

			// modulate.
			for ( sint32_c j = 0; j < channel_count; j++ )
			{
				sample_buffer[ i + j ] = sample_buffer[ i + j ] / compressor_peak * _compressor_threshold;
			}

			// update fall off.
			compressor_peak -= compressor_fall_off_rate;
			if ( compressor_peak < compressor_threshold )
			{
				compressor_peak = compressor_threshold;
			}
		}
		compressor_peak = compressor_peak;
	}

	#if defined( cheonsa_platform_windows )
	// this is only a class so that rather than befriending the function we can befriend the class that contains the function.
	// this is a work around to avoid including Win32 headers in the class header.
	// we don't want to be friends with Win32 because it's ugly.
	class wave_out_implementation_c
	{
	public:
		// how many wave out buffers to use.
		// each one of these represents a "frame" of audio.
		// windows plays back the contents of one buffer at a time, while our program concurrently "renders|draws" to the "back buffers".
		// it probably shouldn't be too long since that would introduce lag.
		// but if its too short then it could introduce skipping.
		static uint32_c const buffer_split_count = 2;
		HWAVEOUT wave_out_handle; // handle to wave out d3d11_device thingy.
		WAVEFORMATEX wave_out_format; // format of sound card output.
		WAVEHDR wave_out_headers[ buffer_split_count ];
		uint8_c * buffer; // raw output buffer, this is buffer_split_count number of buffers in one, each one aligned to start at a memory address offset of 16 bytes.
		boolean_c is_running;
		sint32_c is_running_count;

		// this function is exeuted on the audio mixing thread.
		// it contains a while loop so it will run perpetually, until its asked to stop.
		//static DWORD WINAPI WaveOutProcess( LPVOID instance )
		static void CALLBACK waveOutProcImpl(
		   HWAVEOUT  hwo,
		   UINT      uMsg,
		   DWORD_PTR dwInstance,
		   DWORD_PTR dwParam1,
		   DWORD_PTR dwParam2 )
		{
			// calculate base normals.
			// two front channels, two rear channels.
			// front speakers are 26 degrees off forward axis.
			// center speaker is 0 degrees off forward axis.
			// rear speakers are 100 degrees off forward axis.
			static float32_c const front_offset = 30.0f * constants< float32_c >::degrees_to_radians();
			static vector32x3_c const front_left = vector32x3_c( ops::math_sine( -front_offset ), ops::math_cosine( -front_offset ), 0.0f );
			static vector32x3_c const front_right = vector32x3_c( ops::math_sine( front_offset ), ops::math_cosine( front_offset ), 0.0f );
			static vector32x3_c const front_center = vector32x3_c( 0.0f, 1.0f, 0.0f );
			static float32_c const rear_offset = 110.0f * constants< float32_c >::degrees_to_radians();
			static vector32x3_c const rear_left = vector32x3_c( ops::math_sine( -rear_offset ), ops::math_cosine( -rear_offset ), 0.0f );
			static vector32x3_c const rear_right = vector32x3_c( ops::math_sine( rear_offset ), ops::math_cosine( rear_offset ), 0.0f );
			static vector32x3_c const mixing_channel_normals_base[ 6 ] = { front_left, front_right, front_center, vector32x3_c( 0.0f, 0.0f, 0.0f ), rear_left, rear_right }; // one normal for each speaker/channel, points from origin point towards speaker/ear.

			// thread scope stack variables.
			audio2_interface_c * audio_interface = reinterpret_cast< audio2_interface_c * >( dwInstance );
			wave_out_implementation_c * implementation = reinterpret_cast< wave_out_implementation_c * >( audio_interface->_implementation );

			if ( implementation->is_running == false )
			{
				audio_interface->_critical_section.enter();
				implementation->is_running_count--;
				audio_interface->_critical_section.exit();
				return;
			}

			float32_c time_step = 1.0f / static_cast< float32_c >( audio_interface->_output_frame_rate );
			sint32_c mix_sample_count = audio_interface->_output_sample_rate / audio_interface->_output_frame_rate;
			sint32_c mix_sample_element_count = mix_sample_count * audio_interface->_output_channel_count;
			assert( mix_sample_element_count < 0xFFFF );
			float32_c mix_buffer[ 0xFFFF ]; // buffer for mixing samples as floats before we convert and submit.
			float32_c mix_layer_volumes[ audio2_layer_e_count_ ]; // local thread copy of layer volumes.
			vector32x3_c mix_channel_normals[ 6 ]; // resolved normals for the current listener.
			float32_c mix_channel_volumes[ 6 ]; // resolved volumes for each source for each channel.

			if ( uMsg != WOM_DONE )
			{
				return;
			}

			// process audio interface queues.
			if ( audio_interface->_queued_scene_list.get_length() || audio_interface->_queued_wave_player_list.get_length() )
			{
				audio_interface->_critical_section.enter();
				for ( sint32_c i = 0; i < audio_interface->_queued_scene_list.get_length(); i++ )
				{
					audio2_interface_c::queued_scene_c & queued_scene = audio_interface->_queued_scene_list[ i ];
					if ( queued_scene.operation == audio2_queue_operation_e_add )
					{
						if ( queued_scene.instance->_linked_audio_interface == nullptr )
						{
							queued_scene.instance->add_reference(); // add reference from this mixing thread.
							queued_scene.instance->_linked_audio_interface = audio_interface;
							audio_interface->_scene_list.insert( -1, queued_scene.instance );
						}
					}
					else
					{
						if ( queued_scene.instance->_linked_audio_interface == audio_interface )
						{
							audio_interface->_scene_list.remove_value( queued_scene.instance );
							queued_scene.instance->_linked_audio_interface = nullptr;
							queued_scene.instance->remove_reference(); // remove reference that was added by this mixing thread.
						}
					}
					queued_scene.instance->remove_reference(); // remove reference that was added by main thread.
				}
				audio_interface->_queued_scene_list.remove_all();
				for ( sint32_c i = 0; i < audio_interface->_queued_wave_player_list.get_length(); i++ )
				{
					audio2_interface_c::queued_wave_player_c & queued_wave_player = audio_interface->_queued_wave_player_list[ i ];
					if ( queued_wave_player.operation == audio2_queue_operation_e_add )
					{
						if ( queued_wave_player.instance->_linked_audio_interface == nullptr )
						{
							if ( queued_wave_player.instance->_start() )
							{
								// link wave player with audio interface.
								queued_wave_player.instance->add_reference();
								queued_wave_player.instance->_linked_audio_interface = audio_interface;
								audio_interface->_wave_player_list.insert_at_end( &queued_wave_player.instance->_wave_player_list_node );
							}
						}
					}
					else
					{
						if ( queued_wave_player.instance->_linked_audio_interface == audio_interface )
						{
							// unlink wave player from audio interface.
							audio_interface->_wave_player_list.remove( &queued_wave_player.instance->_wave_player_list_node );
							queued_wave_player.instance->_linked_audio_interface = nullptr;
							queued_wave_player.instance->remove_reference(); // remove reference that was added by this mixing thread.
						}
					}
					queued_wave_player.instance->_is_queued = false;
					queued_wave_player.instance->remove_reference(); // remove reference that was added by main thread.
				}
				audio_interface->_queued_wave_player_list.remove_all();
				audio_interface->_critical_section.exit();
			}

			// if we are here then it's time to mix another frame of audio.
			// iterate through each scene.
			ops::memory_zero( mix_buffer, sizeof( float32_c ) * mix_sample_element_count );
			float32_c mix_master_volume = audio_interface->settings.master_volume;
			mix_layer_volumes[ audio2_layer_e_music ] = audio_interface->settings.music_volume * mix_master_volume;
			mix_layer_volumes[ audio2_layer_e_ambient ] = audio_interface->settings.ambient_volume * mix_master_volume;
			mix_layer_volumes[ audio2_layer_e_effect ] = audio_interface->settings.effect_volume * mix_master_volume;
			mix_layer_volumes[ audio2_layer_e_voice ] = audio_interface->settings.voice_volume * mix_master_volume;
			mix_layer_volumes[ audio2_layer_e_menu ] = audio_interface->settings.menu_volume * mix_master_volume;
			for ( sint32_c i = 0; i < audio_interface->_scene_list.get_length(); i++ )
			{
				audio2_scene_c * scene = audio_interface->_scene_list[ i ];

				// process scene queues.
				scene->_critical_section.enter();
				audio2_scene_listener_c * scene_listener = scene->_scene_listener;
				if ( scene_listener != nullptr )
				{
					scene_listener->add_reference();
				}
				for ( sint32_c j = 0; j < scene->_queued_scene_source_list.get_length(); j++ )
				{
					audio2_scene_c::queued_scene_source_c & queued_scene_source = scene->_queued_scene_source_list[ j ];
					if ( queued_scene_source.operation == audio2_queue_operation_e_add )
					{
						if ( queued_scene_source.instance->_linked_audio_scene == nullptr )
						{
							if ( queued_scene_source.instance->_wave_player->_start() )
							{
								queued_scene_source.instance->add_reference();
								queued_scene_source.instance->_linked_audio_scene = scene;
								scene->_scene_source_list.insert_at_end( &queued_scene_source.instance->_scene_source_list_node );
							}
						}
					}
					else
					{
						if ( queued_scene_source.instance->_linked_audio_scene == scene )
						{
							queued_scene_source.instance->_linked_audio_scene = nullptr;
							scene->_scene_source_list.remove( &queued_scene_source.instance->_scene_source_list_node );
							queued_scene_source.instance->remove_reference();
						}
					}
					queued_scene_source.instance->_wave_player->_is_queued = false;
					queued_scene_source.instance->remove_reference(); // remove reference that was added by main thread.
				}
				scene->_queued_scene_source_list.remove_all();
				scene->_critical_section.exit();

				// mix 3d scene sources.
				if ( scene_listener != nullptr )
				{
					// calculate listener normals.
					for ( sint32_c j = 0; j < 6; j++ )
					{
						mix_channel_normals[ j ] = ops::rotate_and_scale_vector32x3( mixing_channel_normals_base[ j ], scene_listener->_world_space_basis );
					}

					// mix scene sources.
					core_linked_list_c< audio2_scene_source_c * >::node_c const * scene_source_list_node = scene->_scene_source_list.get_first();
					while ( scene_source_list_node != nullptr )
					{
						audio2_scene_source_c * scene_source = scene_source_list_node->get_value();
						scene_source_list_node = scene_source_list_node->get_next();

						// calculate the channel volumes for the source relative to the listener.
						// this factors in distance attenuation (for all channels), and directional attenuation (for each channel).
						float32_c volume = mix_layer_volumes[ scene_source->_wave_player->_layer ];
						vector32x3_c listener_to_source = vector32x3_c( scene_source->_world_space_position - scene_listener->_world_space_position );
						float32_c listener_to_source_distance = ops::length_float32( listener_to_source );
						float32_c distance_attenuation = 0.0f;
						float32_c direction_attenuation_factor = 1.0f; // will be in range 0 to 1, when 1 it means that directional attenuation has full influence, when 0 it means that it has no influence.
						float32_c distance_plus_damping = listener_to_source_distance;
						if ( distance_plus_damping < cheonsa_audio_minimum_distance )
						{
							distance_plus_damping = cheonsa_audio_minimum_distance;
						}
						else
						{
							listener_to_source /= listener_to_source_distance; // normalize listener to source vector.
							direction_attenuation_factor = listener_to_source_distance - cheonsa_audio_minimum_distance;
							if ( direction_attenuation_factor < 0.0f )
							{
								direction_attenuation_factor = 0.0f;
							}
							direction_attenuation_factor /= cheonsa_audio_defactor_distance - cheonsa_audio_minimum_distance;
							if ( direction_attenuation_factor > 1.0f )
							{
								direction_attenuation_factor = 1.0f;
							}
						}
						distance_plus_damping += scene_listener->_damping; // add damping factor.
						distance_attenuation = ops::math_clamp( scene_source->_wave_player->_volume, 0.0f, 1000.0f ) / ( 4.0f * constants< float32_c >::pi() * ( distance_plus_damping * distance_plus_damping ) );
						distance_attenuation *= volume;
						if ( distance_attenuation >= cheonsa_audio_minimum_attenuation )
						{
							if ( direction_attenuation_factor > 0.0f )
							{
								listener_to_source /= listener_to_source_distance;
								for ( sint32_c j = 0; j < 6; j++ )
								{
									float32_c direction_attenuation = ops::dot_product_float32( mix_channel_normals[ j ], listener_to_source ); // will be in range -1.0 to 1.0.
									direction_attenuation = ( direction_attenuation * 0.75f ) + 0.25f; // refit to range 0.25 to 1.0.
									direction_attenuation = ops::interpolate_linear( 1.0f, direction_attenuation, direction_attenuation_factor ); // bias towards 1.0 based on how close listener is to source.
									mix_channel_volumes[ j ] = direction_attenuation * distance_attenuation;
								}
							}
							else
							{
								for ( sint32_c j = 0; j < 6; j++ )
								{
									mix_channel_volumes[ j ] = distance_attenuation;
								}
							}
						}
						else
						{
							distance_attenuation = 0.0f;
							for ( sint32_c j = 0; j < 6; j++ )
							{
								mix_channel_volumes[ j ] = 0.0f;
							}
						}

						// finally, sample and mix the source with the channel volumes we calculated earlier.
						// this can be optimized, if the volume is zero for all channels then we won't need to sample, only advance the stream.
						scene_source->_wave_player->_sample_and_mix( audio_interface->_output_channel_count, audio_interface->_output_sample_rate, audio_interface->_output_sample_rate / audio_interface->_output_frame_rate, mix_buffer, mix_channel_volumes );

						// if the scene source is done then release it.
						if ( scene_source->_wave_player->_is_playing == false )
						{
							scene->_scene_source_list.remove( &scene_source->_scene_source_list_node );
							scene_source->_linked_audio_scene = nullptr;
							scene_source->remove_reference();
						}
					}
					scene_listener->remove_reference();
					scene_listener = nullptr;
				}
				else
				{
					// no listener, just advance streams.
					core_linked_list_c< audio2_scene_source_c * >::node_c const * scene_source_list_node = scene->_scene_source_list.get_first();
					while ( scene_source_list_node != nullptr );
					{
						audio2_scene_source_c * scene_source = scene_source_list_node->get_value();
						scene_source_list_node = scene_source_list_node->get_next();
						scene_source->_wave_player->_advance( time_step );

						// if the scene source is done then release it.
						if ( scene_source->_wave_player->_is_playing == false )
						{
							scene->_scene_source_list.remove( &scene_source->_scene_source_list_node );
							scene_source->_linked_audio_scene = nullptr;
							scene_source->remove_reference();
						}
					}
				}

				// mix 2d wave players.
				core_linked_list_c< audio2_wave_player_c * >::node_c const * wave_player_list_node = audio_interface->_wave_player_list.get_first();
				while ( wave_player_list_node != nullptr )
				{
					audio2_wave_player_c * wave_player = wave_player_list_node->get_value();
					wave_player_list_node = wave_player_list_node->get_next();

					float32_c volume = mix_layer_volumes[ wave_player->_layer ] * ops::math_saturate( wave_player->_volume );
					mix_channel_volumes[ 0 ] = volume;
					mix_channel_volumes[ 1 ] = volume;
					mix_channel_volumes[ 2 ] = volume;
					mix_channel_volumes[ 3 ] = 0.0f;
					mix_channel_volumes[ 4 ] = volume;
					mix_channel_volumes[ 5 ] = volume;

					// sample and mix the wave player.
					wave_player->_sample_and_mix( audio_interface->_output_channel_count, audio_interface->_output_sample_rate, audio_interface->_output_sample_rate / audio_interface->_output_frame_rate, mix_buffer, mix_channel_volumes );

					// if the wave player is done then release it.
					if ( wave_player->_is_playing == false )
					{
						audio_interface->_wave_player_list.remove( &wave_player->_wave_player_list_node );
						wave_player->_linked_audio_interface = nullptr;
						wave_player->remove_reference();
					}
				}
			}

			// apply compressor filter to prevent clipping.
			audio_interface->_compressor_update( mix_buffer, audio_interface->_output_sample_rate / audio_interface->_output_frame_rate, audio_interface->_output_channel_count, 1.0f / static_cast< float32_c >( audio_interface->_output_frame_rate ) );

			// copy and convert the mixing buffer to the proper output buffer and format.
			LPWAVEHDR wave_header = reinterpret_cast< LPWAVEHDR >( dwParam1 );
			sint16_c * sample_out = reinterpret_cast< sint16_c * >( wave_header->lpData );
			for ( sint32_c i = 0; i < mix_sample_element_count; i++ )
			{
				float32_c sample_in = mix_buffer[ i ];
				if ( sample_in < -1.0f ) // clip the sample if needed.
				{
					sample_in = -1.0f;
				}
				else if ( sample_in > 1.0f )
				{
					sample_in = 1.0f;
				}
				*sample_out++ = static_cast< sint16_c >( sample_in * 32767.0f );
			}

			// submit the audio frame we just created to the operating system.
			MMRESULT result = waveOutWrite( implementation->wave_out_handle, wave_header, sizeof( WAVEHDR ) );
			if ( result != MMSYSERR_NOERROR )
			{
			}
		}
	};
	#endif

	void_c audio2_interface_c::_save_settings( data_scribe_ini_c & scribe )
	{
		// [audio]
		scribe.add_section( string8_c( core_list_mode_e_static, "audio" ) );
		// master_volume
		string8_c value2;
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.master_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( core_list_mode_e_static, "master_volume" ), value2 );
		// music_volume
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.music_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( core_list_mode_e_static, "music_volume" ), value2 );
		// ambient_volume
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.ambient_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( core_list_mode_e_static, "ambient_voume" ), value2 );
		// effect_volume
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.effect_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( core_list_mode_e_static, "effect_volume" ), value2 );
		// voice_volume
		ops::convert_sint32_to_string8( static_cast< sint32_c >( settings.voice_volume * 100.0f ), value2 );
		scribe.add_property( string8_c( core_list_mode_e_static, "voice_volume" ), value2 );
	}

	void_c audio2_interface_c::_load_settings( data_scribe_ini_c & scribe )
	{
		settings.reset_to_default();

		string8_c value;

		if ( scribe.find_property_value( string8_c( core_list_mode_e_static, "audio" ), string8_c( core_list_mode_e_static, "master_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.master_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( core_list_mode_e_static, "audio" ), string8_c( core_list_mode_e_static, "music_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.music_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( core_list_mode_e_static, "audio" ), string8_c( core_list_mode_e_static, "ambient_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.ambient_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( core_list_mode_e_static, "audio" ), string8_c( core_list_mode_e_static, "effect_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.effect_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( core_list_mode_e_static, "audio" ), string8_c( core_list_mode_e_static, "voice_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.voice_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}

		if ( scribe.find_property_value( string8_c( core_list_mode_e_static, "audio" ), string8_c( core_list_mode_e_static, "menu_volume" ), value ) )
		{
			sint32_c value2;
			if ( ops::convert_string8_to_sint32( value, value2 ) )
			{
				settings.menu_volume = ops::math_saturate( static_cast< float32_c >( value2 ) / 100.0f );
			}
		}
	}

	audio2_interface_c::settings_c::settings_c()
	{
		reset_to_default();
	}

	void_c audio2_interface_c::settings_c::reset_to_default()
	{
		master_volume = 1.0f;
		music_volume = 1.0f;
		ambient_volume = 1.0f;
		effect_volume = 1.0f;
		voice_volume = 1.0f;
		menu_volume = 1.0f;
	}

	audio2_interface_c::audio2_interface_c()
		: _scene_list()
		, _wave_player_list()
		, _queued_scene_list()
		, _queued_wave_player_list()
		, _implementation( nullptr )
		, _output_sample_rate( 0 )
		, _output_channel_count( 0 )
		, _output_frame_rate( 0 )
		, _compressor_threshold( 1.0f )
		, _compressor_fall_off_duration( 5.0f )
		, _compressor_peak( 1.0f )
	{
	}

	audio2_interface_c::~audio2_interface_c()
	{
#if defined( cheonsa_platform_windows )
		wave_out_implementation_c * implementation = reinterpret_cast< wave_out_implementation_c * >( _implementation );

		// stop wave out play back.
		implementation->is_running = false;
		while ( implementation->is_running_count > 0 )
		{
			platform_thread_c::sleep( 10 );
		}

		// release wave out.
		if ( implementation->wave_out_handle )
		{
			for ( sint32_c i = 0; i < wave_out_implementation_c::buffer_split_count; i++ )
			{
				if ( implementation->wave_out_headers[ i ].dwFlags & WHDR_PREPARED )
				{
					waveOutUnprepareHeader( implementation->wave_out_handle, &implementation->wave_out_headers[ i ], sizeof( WAVEHDR ) );
					ops::memory_zero( &implementation->wave_out_headers[ i ], sizeof( WAVEHDR ) );
				}
			}
			waveOutClose( implementation->wave_out_handle );
			implementation->wave_out_handle = 0;
		}
		if ( implementation->buffer )
		{
			ops::memory_aligned_deallocate( implementation->buffer );
			implementation->buffer = nullptr;
		}

		delete _implementation;
		_implementation = nullptr;
#else
#error not implemented.
#endif

		// reset.
		_output_channel_count = 0;
		_output_sample_rate = 0;
		_output_frame_rate = 0;

		for ( sint32_c i = 0; i < _scene_list.get_length(); i++ )
		{
			audio2_scene_c * scene = _scene_list[ i ];
			scene->_linked_audio_interface = nullptr;
			scene->remove_reference();
		}
		_scene_list.remove_all();

		core_linked_list_c< audio2_wave_player_c * >::node_c * wave_player_list_node = _wave_player_list.get_first();
		while ( wave_player_list_node != nullptr )
		{
			audio2_wave_player_c * wave_player = wave_player_list_node->get_value();
			wave_player_list_node = wave_player_list_node->get_next();
			wave_player->_linked_audio_interface = nullptr;
			wave_player->remove_reference();
		}

		for ( sint32_c i = 0; i < _queued_scene_list.get_length(); i++ )
		{
			_queued_scene_list[ i ].instance->remove_reference();
		}
		_queued_scene_list.remove_all();

		for ( sint32_c i = 0; i < _queued_wave_player_list.get_length(); i++ )
		{
			_queued_wave_player_list[ i ].instance->remove_reference();
		}
		_queued_wave_player_list.remove_all();
	}

	boolean_c audio2_interface_c::start( sint32_c sample_rate, sint32_c channel_count )
	{
		// initialize mixing settings
		_output_channel_count = 2;
		_output_frame_rate = 60;
		_output_sample_rate = 44100;
		//_output_samples_per_frame = _sample_rate / _frames_per_second;

#if defined( cheonsa_platform_windows )
		wave_out_implementation_c * implementation = new wave_out_implementation_c();
		_implementation = implementation;

		// start wave out.
		implementation->wave_out_format.nSamplesPerSec = _output_sample_rate;
		implementation->wave_out_format.wBitsPerSample = 16;
		implementation->wave_out_format.nChannels = static_cast< WORD >( _output_channel_count );
		implementation->wave_out_format.wFormatTag = WAVE_FORMAT_PCM;
		implementation->wave_out_format.nBlockAlign = implementation->wave_out_format.nChannels * implementation->wave_out_format.wBitsPerSample / 8;
		implementation->wave_out_format.nAvgBytesPerSec = implementation->wave_out_format.nBlockAlign * implementation->wave_out_format.nSamplesPerSec;
		implementation->wave_out_format.cbSize = 0;
		MMRESULT result = waveOutOpen( &implementation->wave_out_handle, 0, &implementation->wave_out_format, (DWORD_PTR)&wave_out_implementation_c::waveOutProcImpl, (DWORD_PTR)this, CALLBACK_FUNCTION );
		if ( result != MMSYSERR_NOERROR )
		{
			debug_annoy( L"error", L"waveOutOpen failed." );
			return false;
		}

		// begin wave out play back.
		implementation->is_running = true;
		implementation->is_running_count = wave_out_implementation_c::buffer_split_count;
		//implementation->wave_out_buffer_split_size = 
		sint32_c buffer_split_size = ( implementation->wave_out_format.wBitsPerSample / 8 ) * _output_sample_rate / _output_frame_rate * _output_channel_count;
		sint32_c buffer_split_size_algined = ops::math_next_nearest_multiple( buffer_split_size, 16 ); // buffer_split_size, maybe with some padding.
		sint32_c buffer_size = buffer_split_size_algined * wave_out_implementation_c::buffer_split_count;
		implementation->buffer = reinterpret_cast< uint8_c * >( ops::memory_aligned_allocate( buffer_size, 16 ) );
		ops::memory_zero( implementation->buffer, buffer_size );
		for ( uint32_c i = 0; i < wave_out_implementation_c::buffer_split_count; i++ )
		{
			ops::memory_zero( &implementation->wave_out_headers[ i ], sizeof( WAVEHDR ) );
			implementation->wave_out_headers[ i ].dwBufferLength = buffer_split_size;
			implementation->wave_out_headers[ i ].lpData = reinterpret_cast< LPSTR >( &implementation->buffer[ buffer_split_size_algined * i ] );
			result = waveOutPrepareHeader( implementation->wave_out_handle, &implementation->wave_out_headers[ i ], sizeof( WAVEHDR ) );
			if ( result != MMSYSERR_NOERROR )
			{
				debug_annoy( L"error", L"waveOutPrepareHeader failed." );
				return false;
			}
			result = waveOutWrite( implementation->wave_out_handle, &implementation->wave_out_headers[ i ], sizeof( WAVEHDR ) ); // begin callback loop.
			if ( result != MMSYSERR_NOERROR )
			{
				debug_annoy( L"error", L"waveOutWrite failed." );
				return false;
			}
		}
#else
#error not implemented.
#endif

		return true;
	}

	void_c audio2_interface_c::refresh()
	{
#if defined( cheonsa_platform_windows )
		wave_out_implementation_c * implementation = reinterpret_cast< wave_out_implementation_c * >( _implementation );
		waveOutPause( implementation->wave_out_handle );
#else
#error not implemented.
#endif

		// iterate over each wave buffer instance.
		_critical_section.enter();
		core_linked_list_c< audio2_wave_player_c * >::node_c const * wave_player_list_node = audio2_wave_player_c::_instance_list.get_first();
		while ( wave_player_list_node != nullptr )
		{
			audio2_wave_player_c * wave_player = wave_player_list_node->get_value();
			wave_player_list_node = wave_player_list_node->get_next();
			wave_player->_start();
		}
		_critical_section.exit();

		// iterate over each wave player and restart it if needed.

#if defined( cheonsa_platform_windows )
		waveOutRestart( implementation->wave_out_handle );
#endif
	}
	
	void_c audio2_interface_c::add_scene( audio2_scene_c * value )
	{
		_critical_section.enter();
		value->add_reference();
		queued_scene_c * queued_scene = _queued_scene_list.emplace( -1, 1 );
		queued_scene->operation = audio2_queue_operation_e_add;
		queued_scene->instance = value;
		_critical_section.exit();
	}

	void_c audio2_interface_c::remove_scene( audio2_scene_c * value )
	{
		_critical_section.enter();
		value->add_reference();
		queued_scene_c * queued_scene = _queued_scene_list.emplace( -1, 1 );
		queued_scene->operation = audio2_queue_operation_e_remove;
		queued_scene->instance = value;
		_critical_section.exit();
	}

	void_c audio2_interface_c::add_wave_player( audio2_wave_player_c * value )
	{
		_critical_section.enter();
		if ( value->_is_queued == false )
		{
			value->add_reference();
			value->_is_queued = true;
			queued_wave_player_c * queued_wave_player = _queued_wave_player_list.emplace( -1, 1 );
			queued_wave_player->operation = audio2_queue_operation_e_add;
			queued_wave_player->instance = value;
			queued_wave_player->instance->_randomize_seek = false;
			_critical_section.exit();
		}
	}

	void_c audio2_interface_c::remove_wave_player( audio2_wave_player_c * value )
	{
		_critical_section.enter();
		value->add_reference();
		queued_wave_player_c * queued_wave_player = _queued_wave_player_list.emplace( -1, 1 );
		queued_wave_player->operation = audio2_queue_operation_e_remove;
		queued_wave_player->instance = value;
		_critical_section.exit();
	}

}
