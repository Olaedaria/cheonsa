#include "cheonsa_audio_interface.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

#include <cassert>
#if defined( cheonsa_platform_windows )
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <Mmsystem.h> // for audio output on windows.
#endif

namespace cheonsa
{

	void_c audio_convert_sint16_to_sint8( sint32_c samples_count, sint16_c const * samples_in, sint8_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint8_c >( samples_in[ i ] * 0xFF / 0xFFFF );
		}
	}

	void_c audio_convert_float32_to_sint8( sint32_c samples_count, float32_c const * samples_in, sint8_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint8_c >( samples_in[ i ] * 127.0f );
		}
	}

	void_c audio_convert_sint8_to_sint16( sint32_c samples_count, sint8_c const * samples_in, sint16_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint16_c >( samples_in[i] * 0xFFFF / 0xFF );
		}
	}

	void_c audio_convert_float32_to_sint16( sint32_c samples_count, float32_c const * samples_in, sint16_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< sint16_c >( samples_in[ i ] * 32767.0f );
		}
	}

	void_c audio_convert_sint8_to_float32( sint32_c samples_count, sint8_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< float32_c >( samples_in[ i ] ) / 127.0f;
		}
	}

	void_c audio_convert_sint16_to_float32( sint32_c samples_count, sint16_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			samples_out[ i ] = static_cast< float32_c >( samples_in[ i ] ) / 32767.0f;
		}
	}

	void_c audio_convert_6_to_1( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 6;
			samples_out[ i ] = ( samples_in[ j ] + samples_in[ j + 1 ] + samples_in[ j + 2 ] + samples_in[ j + 3 ] + samples_in[ j + 4 ] + samples_in[ j + 5 ] ) / 6.0f;
		}
	}

	void_c audio_convert_2_to_1( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 2;
			samples_out[ i ] = ( samples_in[ j ] + samples_in[ j + 1 ] ) / 2.0f;
		}
	}

	void_c audio_convert_1_to_2( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 2;
			samples_out[ j + 1 ] = samples_out[ j ] = ( samples_in[ i ] );
		}
	}

	void_c audio_convert_6_to_2( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 6;
			sint32_c k = i * 2;
			samples_out[ k ] = ( samples_in[ j ] + samples_in[ j + 4 ] ) / 2.0f; // left = front left + back left.
			samples_out[ k + 1 ] = ( samples_in[ j + 1 ] + samples_in[ j + 5 ] ) / 2.0f; // right = front right + back right.
		}
	}

	void_c audio_convert_1_to_6( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
	{
		for ( sint32_c i = 0; i < samples_count; i++ )
		{
			sint32_c j = i * 6;
			samples_out[ j ] = samples_out[ j + 1 ] = samples_out[ j + 4 ] = samples_out[ j + 5 ] = samples_in[ i ];
			samples_out[ j + 2 ] = samples_out[ j + 3 ] = 0.0f;
		}
	}

	void_c audio_convert_2_to_6( sint32_c samples_count, float32_c const * samples_in, float32_c * samples_out )
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

	void_c audio_mix(
		audio_sample_format_e input_sample_format,
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
		assert( input_sample_format == audio_sample_format_e_sint8 || input_sample_format == audio_sample_format_e_sint16 || input_sample_format == audio_sample_format_e_float32 );
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
		if ( input_sample_format == audio_sample_format_e_sint8 )
		{
			audio_convert_sint8_to_float32( input_sample_count * input_channel_count, static_cast< sint8_c const * >( input_sample_buffer ), float32_temp_buffer_a );
			stage_0 = float32_temp_buffer_a;
		}
		else if ( input_sample_format == audio_sample_format_e_sint16 )
		{
			audio_convert_sint16_to_float32( input_sample_count * input_channel_count, static_cast< sint16_c const * >( input_sample_buffer ), float32_temp_buffer_a );
			stage_0 = float32_temp_buffer_a;
		}
		else if ( input_sample_format == audio_sample_format_e_float32 )
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
				audio_convert_2_to_1( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
			else if ( input_channel_count == 6 )
			{
				audio_convert_6_to_1( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
		}
		else if ( output_channel_count == 2 )
		{
			if ( input_channel_count == 1 )
			{
				audio_convert_1_to_2( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
			else if ( input_channel_count == 6 )
			{
				audio_convert_6_to_2( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
		}
		else if ( output_channel_count == 6 )
		{
			if ( input_channel_count == 1 )
			{
				audio_convert_1_to_6( input_sample_count, stage_0, float32_temp_buffer_b );
				stage_1 = float32_temp_buffer_b;
			}
			else if ( input_channel_count == 2 )
			{
				audio_convert_2_to_6( input_sample_count, stage_0, float32_temp_buffer_b );
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

	

	void_c audio_interface_c::_compressor_update( float32_c * sample_buffer, sint32_c sample_count, sint32_c channel_count, float32_c duration )
	{
		// do not trust, not tested yet.

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
			audio_interface_c * audio_interface = reinterpret_cast< audio_interface_c * >( dwInstance );
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
			float32_c mix_layer_volumes[ audio_layer_e_count_ ]; // local thread copy of layer volumes.
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

				// process 2d wave player queue.
				{
					for ( sint32_c i = 0; i < audio_interface->_queued_wave_player_list.get_length(); i++ )
					{
						audio_interface_c::queued_wave_player_c & queued_wave_player = audio_interface->_queued_wave_player_list[ i ];
						if ( queued_wave_player.command == audio_queue_command_e_add )
						{
							if ( queued_wave_player.instance->_is_queued == true )
							{
								queued_wave_player.instance->_is_queued = false;
								if ( queued_wave_player.instance->_start() )
								{
									queued_wave_player.instance->add_reference(); // add reference from this mixing thread.
									audio_interface->_wave_player_list.insert_at_end( &queued_wave_player.instance->_wave_player_list_node );
								}
							}
						}
						else if ( queued_wave_player.command == audio_queue_command_e_remove )
						{
							if ( queued_wave_player.instance->_is_playing == true )
							{
								audio_interface->_wave_player_list.remove( &queued_wave_player.instance->_wave_player_list_node );
								queued_wave_player.instance->_is_playing = false;
								queued_wave_player.instance->_is_queued = false; // reset to false for safety.
								queued_wave_player.instance->remove_reference(); // remove reference from this mixing thread.
							}
						}
						queued_wave_player.instance->remove_reference(); // remove reference from game thread.
					}
					audio_interface->_queued_wave_player_list.remove_all();
				}

				// process 3d scene queue.
				{
					for ( sint32_c i = 0; i < audio_interface->_queued_scene_list.get_length(); i++ )
					{
						audio_interface_c::queued_scene_c & queued_scene = audio_interface->_queued_scene_list[ i ];
						if ( queued_scene.command == audio_queue_command_e_add )
						{
							if ( queued_scene.instance->_is_added == false )
							{
								queued_scene.instance->add_reference(); // add reference from this mixing thread.
								queued_scene.instance->_is_added = true;
								audio_interface->_scene_list.insert( -1, queued_scene.instance );
							}
						}
						else if ( queued_scene.command == audio_queue_command_e_remove )
						{
							if ( queued_scene.instance->_is_added == true )
							{
								audio_interface->_scene_list.remove_value( queued_scene.instance );
								queued_scene.instance->_is_added = false;
								queued_scene.instance->remove_reference(); // remove reference from this mixing thread.
							}
						}
						queued_scene.instance->remove_reference(); // remove reference from game thread.
					}
					audio_interface->_queued_scene_list.remove_all();
				}

				audio_interface->_critical_section.exit();
			}

			// prepare to mix this frame.
			ops::memory_zero( mix_buffer, sizeof( float32_c ) * mix_sample_element_count );
			float32_c mix_master_volume = audio_interface->settings.master_volume;
			mix_layer_volumes[ audio_layer_e_music ] = audio_interface->settings.music_volume * mix_master_volume;
			mix_layer_volumes[ audio_layer_e_ambient ] = audio_interface->settings.ambient_volume * mix_master_volume;
			mix_layer_volumes[ audio_layer_e_effect ] = audio_interface->settings.effect_volume * mix_master_volume;
			mix_layer_volumes[ audio_layer_e_voice ] = audio_interface->settings.voice_volume * mix_master_volume;
			mix_layer_volumes[ audio_layer_e_menu ] = audio_interface->settings.menu_volume * mix_master_volume;

			// mix 2d wave players.
			core_linked_list_c< audio_wave_player_c * >::node_c * wave_player_list_node = audio_interface->_wave_player_list.get_first();
			while ( wave_player_list_node != nullptr )
			{
				audio_wave_player_c * wave_player = wave_player_list_node->get_value();
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
					audio_interface->_critical_section.enter();
					audio_interface->_wave_player_list.remove( &wave_player->_wave_player_list_node );
					wave_player->remove_reference(); // remove reference from this mixing thread.
					audio_interface->_critical_section.exit();
				}
			}

			// process scenes.
			for ( sint32_c i = 0; i < audio_interface->_scene_list.get_length(); i++)
			{
				audio_scene_c * scene = audio_interface->_scene_list[ i ];

				
				if ( scene->add_reference() != 0 )
				{
					// process 3d scene source queue.
					scene->_critical_section.enter();
					for ( sint32_c j = 0; j < scene->_queued_scene_source_list.get_length(); j++ )
					{
						audio_scene_c::queued_scene_source_c & queued_scene_source = scene->_queued_scene_source_list[ j ];
						if ( queued_scene_source.command == audio_queue_command_e_add )
						{
							if ( queued_scene_source.instance->_wave_player->_is_queued )
							{
								queued_scene_source.instance->_wave_player->_is_queued = false;
								if ( queued_scene_source.instance->_wave_player->_start() )
								{
									queued_scene_source.instance->add_reference(); // add reference from this mixing thread.
									scene->_scene_source_list.insert_at_end( &queued_scene_source.instance->_scene_source_list_node );
								}
							}
						}
						else if ( queued_scene_source.command == audio_queue_command_e_remove )
						{
							if ( queued_scene_source.instance->_wave_player->_is_playing == true )
							{
								scene->_scene_source_list.remove( &queued_scene_source.instance->_scene_source_list_node );
								queued_scene_source.instance->_wave_player->_is_playing = false;
								queued_scene_source.instance->_wave_player->_is_queued = false; // reset to false for safety.
								queued_scene_source.instance->remove_reference(); // remove reference from this mixing thread.
							}
						}
						queued_scene_source.instance->remove_reference(); // remove reference from game thread.
					}
					scene->_queued_scene_source_list.remove_all();
					scene->_critical_section.exit();

					// mix 3d scene sources.
					// calculate listener normals.
					for ( sint32_c j = 0; j < 6; j++ )
					{
						mix_channel_normals[ j ] = ops::rotate_and_scale_vector32x3( mixing_channel_normals_base[ j ], scene->get_scene_listener()._world_space_basis );
					}

					// mix scene sources.
					core_linked_list_c< audio_scene_source_c * >::node_c const * scene_source_list_node = scene->_scene_source_list.get_first();
					while ( scene_source_list_node != nullptr )
					{
						audio_scene_source_c * scene_source = scene_source_list_node->get_value();
						scene_source_list_node = scene_source_list_node->get_next();

						// calculate the channel volumes for the source relative to the listener.
						// this factors in distance attenuation (for all channels), and directional attenuation (for each channel).
						float32_c volume = mix_layer_volumes[ scene_source->_wave_player->_layer ];
						vector32x3_c listener_to_source = vector32x3_c( scene_source->_world_space_position - scene->get_scene_listener()._world_space_position );
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
						distance_plus_damping += scene->get_scene_listener()._damping; // add damping factor.
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
							scene->_critical_section.enter();
							scene->_scene_source_list.remove( &scene_source->_scene_source_list_node );
							scene_source->_wave_player->_is_queued = false; // reset to false for safety.
							scene_source->remove_reference(); // remove reference from this mixing thread.
							scene->_critical_section.exit();
						}
					}
				}

				scene->remove_reference();
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

	void_c audio_interface_c::_save_settings( data_scribe_ini_c & scribe )
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

	void_c audio_interface_c::_load_settings( data_scribe_ini_c & scribe )
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

	audio_interface_c::settings_c::settings_c()
	{
		reset_to_default();
	}

	void_c audio_interface_c::settings_c::reset_to_default()
	{
		master_volume = 1.0f;
		music_volume = 1.0f;
		ambient_volume = 1.0f;
		effect_volume = 1.0f;
		voice_volume = 1.0f;
		menu_volume = 1.0f;
	}

	audio_interface_c::audio_interface_c()
		: _queued_wave_player_list()
		, _wave_player_list()
		, _queued_scene_list()
		, _scene_list()
		, _implementation( nullptr )
		, _output_sample_rate( 0 )
		, _output_channel_count( 0 )
		, _output_frame_rate( 0 )
		, _compressor_threshold( 1.0f )
		, _compressor_fall_off_duration( 5.0f )
		, _compressor_peak( 1.0f )
	{
	}

	audio_interface_c::~audio_interface_c()
	{
#if defined( cheonsa_platform_windows )
		wave_out_implementation_c * implementation = reinterpret_cast< wave_out_implementation_c * >( _implementation );

		// stop wave out play back.
		implementation->is_running = false;
		while ( implementation->is_running_count > 0 )
		{
			thread_c::sleep( 10 );
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

		_critical_section.enter();

		core_linked_list_c< audio_wave_player_c * >::node_c * wave_player_list_node = _wave_player_list.get_first();
		while ( wave_player_list_node != nullptr )
		{
			audio_wave_player_c * wave_player = wave_player_list_node->get_value();
			wave_player_list_node = wave_player_list_node->get_next();
			wave_player->remove_reference(); // remove reference that was added by mixing thread.
		}

		for ( sint32_c i = 0; i < _scene_list.get_length(); i++ )
		{
			audio_scene_c * scene = _scene_list[ i ];
			scene->_is_added = false;
			scene->remove_reference(); // release reference that was added by mixing thread.
		}
		_scene_list.remove_all();

		for ( sint32_c i = 0; i < _queued_wave_player_list.get_length(); i++ )
		{
			_queued_wave_player_list[ i ].instance->remove_reference(); // remove reference that was added by game thread.
		}
		_queued_wave_player_list.remove_all();

		for ( sint32_c i = 0; i < _queued_scene_list.get_length(); i++ )
		{
			_queued_scene_list[ i ].instance->remove_reference(); // remove reference that was added by game thread.
		}
		_queued_scene_list.remove_all();

		_critical_section.exit();
	}

	boolean_c audio_interface_c::start( sint32_c sample_rate, sint32_c channel_count )
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

	void_c audio_interface_c::refresh()
	{
#if defined( cheonsa_platform_windows )
		wave_out_implementation_c * implementation = reinterpret_cast< wave_out_implementation_c * >( _implementation );
		waveOutPause( implementation->wave_out_handle );
#else
#error not implemented.
#endif

		// iterate over each wave buffer instance.
		_critical_section.enter();
		core_linked_list_c< audio_wave_player_c * >::node_c const * wave_player_list_node = audio_wave_player_c::_instance_list.get_first();
		while ( wave_player_list_node != nullptr )
		{
			audio_wave_player_c * wave_player = wave_player_list_node->get_value();
			wave_player_list_node = wave_player_list_node->get_next();
			wave_player->_start();
		}
		_critical_section.exit();

		// iterate over each wave player and restart it if needed.

#if defined( cheonsa_platform_windows )
		waveOutRestart( implementation->wave_out_handle );
#endif
	}
	
	boolean_c audio_interface_c::add_scene( audio_scene_c * value )
	{
		assert( value );
		if ( value->add_reference() != 0 )
		{
			_critical_section.enter();
			_scene_list.insert( -1, value );
			_critical_section.exit();
			return true;
		}
		return false;
	}

	boolean_c audio_interface_c::remove_scene( audio_scene_c * value )
	{
		assert( value );
		if ( value->add_reference() != 0 )
		{
			_critical_section.enter();
			_scene_list.remove_value( value );
			_critical_section.exit();
			return true;
		}
		return false;
	}

	boolean_c audio_interface_c::add_wave_player( audio_wave_player_c * value )
	{
		assert( value != nullptr );
		if ( value->add_reference() != 0 )
		{
			if ( value->_is_queued == false )
			{
				_critical_section.enter();
				value->_is_queued = true;
				value->_randomize_seek = false;
				_wave_player_list.insert_at_end( &value->_wave_player_list_node );
				_critical_section.exit();
				return true;
			}
			value->remove_reference();
		}
		return false;
	}

	boolean_c audio_interface_c::remove_wave_player( audio_wave_player_c * value )
	{
		assert( value != nullptr );
		if ( value->add_reference() != 0 )
		{
			_critical_section.enter();
			value->_is_queued = false;
			_wave_player_list.remove( &value->_wave_player_list_node );
			_critical_section.exit();
			return true;
		}
		return false;
	}

}
