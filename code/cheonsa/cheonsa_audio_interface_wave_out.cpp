#include "cheonsa_audio_interface_wave_out.h"
#include "cheonsa_audio_buffer.h"
#include "cheonsa_audio_source.h"
#include "cheonsa_audio_listener.h"
#include "cheonsa_audio_player.h"
#include "cheonsa_ops.h"
#include "cheonsa_engine.h"

#if defined( cheonsa_platform_windows )

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <Mmsystem.h> // for audio output on windows.
#include <assert.h>

namespace cheonsa
{

	// this is only a class so that rather than befriending the function we can befriend the class that contains the function.
	// this is a work around to avoid including Win32 headers in the class header.
	// we don't want to be friends with Win32 because it's ugly.
	class wave_out_process_c
	{
	public:
		// how many wave out buffers to use.
		// each one of these represents a "frame" of audio.
		// windows plays back the contents of one buffer at a time, while our program concurrently "renders|draws" to the "back buffers".
		// it probably shouldn't be too long since that would introduce lag.
		// but if its too short then it could introduce skipping.
		static uint32_c const wave_out_buffer_split_count = 3;
		static HWAVEOUT wave_out_handle; // handle to wave out d3d11_device thingy.
		static WAVEFORMATEX wave_out_format; // format of sound card output.
		static WAVEHDR wave_out_headers[ wave_out_buffer_split_count ]; // array of wave out headers. each one is responsible for playing a "frame" of audio, that is why there's 4, it's like double buffering. maybe 2 would be better...
		static uint32_c wave_out_buffer_split_size; // this is equal to the byte rate times the sample rate divided by the frame rate.
		static uint8_c * wave_out_buffer; // raw output buffer, this is 4 buffers in 1.
		static boolean_c is_running;
		static sint32_c is_running_count;
		static platform_critical_section_c critical_section;

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

			if ( wave_out_process_c::is_running == false )
			{
				wave_out_process_c::critical_section.enter();
				is_running_count--;
				wave_out_process_c::critical_section.exit();
				return;
			}

			// thread scope stack variables.
			audio_interface_wave_out_c * audio_interface = reinterpret_cast< audio_interface_wave_out_c * >( dwInstance );
			float32_c time_step = 1.0f / static_cast< float32_c >( audio_interface->_frames_per_second );
			assert( audio_interface->_samples_per_frame * audio_interface->_channel_count < 0xFFFF );
			float32_c mixing_buffer[ 0xFFFF ]; // buffer for mixing samples as floats before we convert and submit.
			float32_c mixing_layer_volumes[ audio_layer_e_count_ ]; // local thread copy of layer volumes.

			vector32x3_c mixing_channel_normals[ 6 ]; // resolved normals for the current listener.
			float32_c mixing_channel_volumes[ 6 ]; // resolved volumes for each source for each channel.

			float32_c dynamic_range = 1.0f; // current dynamic range. 1 is base line, but it gets pushed up if loud sounds are mixed.
			float32_c dynamic_range_target = 1.0f; // target can be less than 1 for super-sensitivity to quiet sounds in quiet settings.
			float32_c dynamic_range_decay_rate = 0.1f; // how fast dynamic_range will return back to dynamic_range_target.

			if ( uMsg != WOM_DONE )
			{
				return;
			}

			// if we are here then it's time to mix another frame of audio.
			// lock data, make copies to our thread, add references, then unlock.
			ops::memory_zero( mixing_buffer, sizeof( mixing_buffer ) );
			dynamic_range = ops::interpolate_linear( dynamic_range, dynamic_range_target, dynamic_range_decay_rate * time_step );
			audio_interface->_critical_section.enter();
			mixing_layer_volumes[ audio_layer_e_music ] = audio_interface->settings.music_volume * audio_interface->settings.master_volume;
			mixing_layer_volumes[ audio_layer_e_ambient ] = audio_interface->settings.ambient_volume * audio_interface->settings.master_volume;
			mixing_layer_volumes[ audio_layer_e_effect ] = audio_interface->settings.effect_volume * audio_interface->settings.master_volume;
			mixing_layer_volumes[ audio_layer_e_voice ] = audio_interface->settings.voice_volume * audio_interface->settings.master_volume;
			mixing_layer_volumes[ audio_layer_e_menu ] = audio_interface->settings.menu_volume * audio_interface->settings.master_volume;
			for ( sint32_c i = 0; i < audio_interface->_audio_scenes.get_length(); i++ )
			{
				audio_scene_c * audio_scene = audio_interface->_audio_scenes[ i ];

				if ( audio_scene->_audio_listener._world_space_transform == nullptr )
				{
					continue;
				}

				// calculate listener.
				matrix32x3x3_c mixing_audio_listener_basis = audio_scene->_audio_listener._world_space_transform->get_unscaled_basis();
				for ( sint32_c j = 0; j < 6; j++ )
				{
					mixing_channel_normals[ j ] = ops::make_vector32x3_transformed_point( mixing_channel_normals_base[ i ], mixing_audio_listener_basis );
				}

				// process audio sources.
				audio_scene->_critical_section.enter();
				for ( sint32_c j = 0; j < audio_scene->_audio_sources.get_length(); j++ )
				{
					audio_source_c * audio_source = audio_scene->_audio_sources[ j ];

					assert( audio_source->_audio_player->_is_playing == true );

					if ( audio_source->_world_space_transform == nullptr )
					{
						continue;
					}

					// even if the source volume is 0 we will still advance the stream.
					// at the moment we are still mixing, but i should optimize this later.
					// calculate mix volume for this source.
					float32_c const max_volume = 10.0f;
					float32_c const distance_bias = 1.0f / max_volume; // we add a small bias to the distance attenuation which means that we don't have to check for divide by zero and we don't have to worry about volume levels approaching infinity.
					float32_c const score_cut_off = 0.001f; // sources with distance attenuations less than this value we will not mix.
					if ( audio_source->_is_3d )
					{
						// 3d audio source.
						// integrate volume as a function of distance and direction.
						// volume fall off as factor of the inverse of the distance squared (or rather to the power of 2 plus damping factor).
						vector32x3_c listener_to_source = vector32x3_c( audio_source->_world_space_transform->position - audio_scene->_audio_listener._world_space_transform->position );
						float32_c distance_squared = ops::make_float32_length_squared( listener_to_source );
						float32_c distance_attenuation = 1.0f / ( distance_squared + distance_bias );
						if ( distance_attenuation < score_cut_off )
						{
							continue;
						}
						//float32_c distance = ops::math_square_root( distance_squared );
						// integrate volume as function of direction.
						// we can only do this if there is enough distance between listener and source to normalize the vector.
						if ( distance_squared > constants< float32_c >::division_near_zero() )
						{
							listener_to_source = listener_to_source * ( 1.0f / ops::math_square_root( distance_squared ) ); // normalize.
							for ( sint32_c k = 0; k < 6; k++ )
							{
								float32_c direction_attenuation = ops::make_float32_dot_product( mixing_channel_normals[ k ], listener_to_source ); // will be in range -1.0 to 1.0.
								direction_attenuation = ( direction_attenuation * 0.75f ) + 0.25f; // refit to range 0.25 to 1.0.
								mixing_channel_volumes[ k ] = direction_attenuation * distance_attenuation;
							}
						}
						else
						{
							for ( sint32_c k = 0; k < 6; k++ )
							{
								mixing_channel_volumes[ k ] = distance_attenuation;
							}
						}
					}
					else
					{
						// 2d audio source.
						float32_c mixed_volume = audio_source->_volume * mixing_layer_volumes[ audio_source->_layer ];
						mixing_channel_volumes[ 0 ] = mixed_volume;
						mixing_channel_volumes[ 1 ] = mixed_volume;
						mixing_channel_volumes[ 2 ] = mixed_volume;
						mixing_channel_volumes[ 3 ] = 0.0f;
						mixing_channel_volumes[ 4 ] = mixed_volume;
						mixing_channel_volumes[ 5 ] = mixed_volume;
					}

					// finally, sample and mix the source.
					audio_source->_audio_player->_sample_and_mix( audio_source->_speed, audio_interface->_channel_count, audio_interface->_sample_rate, audio_interface->_samples_per_frame, mixing_buffer, mixing_channel_volumes );

					// if the audio source is done playing then we will release it.
					if ( audio_source->_audio_player->_is_playing == false )
					{
						audio_scene->_audio_sources.remove_at_index( j );
						j--;
					}
				}
				audio_scene->_critical_section.exit();
			}
			for ( sint32_c i = 0; i < audio_interface->_audio_sources.get_length(); i++ )
			{
				audio_source_c * audio_source = audio_interface->_audio_sources[ i ];
				float32_c mixed_volume = audio_source->_volume * mixing_layer_volumes[ audio_source->_layer ];
				mixing_channel_volumes[ 0 ] = mixed_volume;
				mixing_channel_volumes[ 1 ] = mixed_volume;
				mixing_channel_volumes[ 2 ] = mixed_volume;
				mixing_channel_volumes[ 3 ] = 0.0f;
				mixing_channel_volumes[ 4 ] = mixed_volume;
				mixing_channel_volumes[ 5 ] = mixed_volume;

				// sample and mix the source.
				audio_source->_audio_player->_sample_and_mix( audio_source->_speed, audio_interface->_channel_count, audio_interface->_sample_rate, audio_interface->_samples_per_frame, mixing_buffer, mixing_channel_volumes );

				// if the audio source is done playing then we will release it.
				if ( audio_source->_audio_player->_is_playing == false )
				{
					audio_interface->_audio_sources.remove_at_index( i );
					i--;
				}
			}
			audio_interface->_critical_section.exit();

			// copy and convert the mixing buffer to the proper output buffer and format.
			LPWAVEHDR wave_header = reinterpret_cast< LPWAVEHDR >( dwParam1 );
			sint16_c * sample_out = reinterpret_cast< sint16_c * >( wave_header->lpData );
			uint32_c sample_entry_count = audio_interface->_samples_per_frame * audio_interface->_channel_count;
			for ( uint32_c i = 0; i < sample_entry_count; i++ )
			{
				float32_c sample_in = mixing_buffer[ i ];
				if ( sample_in < -1.0f ) // clip the sample if needed, but later on maybe we could implement a compressor.
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
			MMRESULT result = waveOutWrite( wave_out_process_c::wave_out_handle, wave_header, sizeof( WAVEHDR ) );
			if ( result != MMSYSERR_NOERROR )
			{
			}
		}
	};

	HWAVEOUT wave_out_process_c::wave_out_handle; // handle to wave out d3d11_device thingy.
	WAVEFORMATEX wave_out_process_c::wave_out_format; // format of sound card output.
	WAVEHDR wave_out_process_c::wave_out_headers[ wave_out_buffer_split_count ]; // array of wave out headers. each one is responsible for playing a "frame" of audio, that is why there's 4, it's like double buffering. maybe 2 would be better...
	uint32_c wave_out_process_c::wave_out_buffer_split_size; // this is equal to the byte rate times the sample rate divided by the frame rate.
	uint8_c * wave_out_process_c::wave_out_buffer; // raw output buffer, this is 4 buffers in 1.
	boolean_c wave_out_process_c::is_running = false;
	sint32_c wave_out_process_c::is_running_count;
	platform_critical_section_c wave_out_process_c::critical_section;

	audio_interface_wave_out_c::audio_interface_wave_out_c()
		: audio_interface_c()
	{
	}

	audio_interface_wave_out_c::~audio_interface_wave_out_c()
	{
		// stop wave out play back.
		wave_out_process_c::is_running = false;
		while ( wave_out_process_c::is_running_count > 0 )
		{
			platform_thread_c::sleep( 10 );
		}

		// release wave out.
		if ( wave_out_process_c::wave_out_handle )
		{
			for ( sint32_c i = 0; i < wave_out_process_c::wave_out_buffer_split_count; i++ )
			{
				if ( wave_out_process_c::wave_out_headers[ i ].dwFlags & WHDR_PREPARED )
				{
					waveOutUnprepareHeader( wave_out_process_c::wave_out_handle, &wave_out_process_c::wave_out_headers[ i ], sizeof( WAVEHDR ) );
					ops::memory_zero( &wave_out_process_c::wave_out_headers[ i ], sizeof( WAVEHDR ) );
				}
			}
			waveOutClose( wave_out_process_c::wave_out_handle );
			wave_out_process_c::wave_out_handle = 0;
		}
		if ( wave_out_process_c::wave_out_buffer )
		{
			ops::memory_aligned_deallocate( wave_out_process_c::wave_out_buffer );
			wave_out_process_c::wave_out_buffer = nullptr;
		}

		// reset.
		_channel_count = 0;
		_sample_rate = 0;
		_frames_per_second = 0;
		_samples_per_frame = 0;
	}

	boolean_c audio_interface_wave_out_c::start()
	{
		// initialize mixing settings
		_channel_count = 2;
		_frames_per_second = 60;
		_sample_rate = 44100;
		_samples_per_frame = _sample_rate / _frames_per_second;

		// start wave out.
		wave_out_process_c::wave_out_format.nSamplesPerSec = _sample_rate;
		wave_out_process_c::wave_out_format.wBitsPerSample = 16;
		wave_out_process_c::wave_out_format.nChannels = static_cast< WORD >( _channel_count );
		wave_out_process_c::wave_out_format.wFormatTag = WAVE_FORMAT_PCM;
		wave_out_process_c::wave_out_format.nBlockAlign = wave_out_process_c::wave_out_format.nChannels * wave_out_process_c::wave_out_format.wBitsPerSample / 8;
		wave_out_process_c::wave_out_format.nAvgBytesPerSec = wave_out_process_c::wave_out_format.nBlockAlign * wave_out_process_c::wave_out_format.nSamplesPerSec;
		wave_out_process_c::wave_out_format.cbSize = 0;
		MMRESULT result = waveOutOpen( &wave_out_process_c::wave_out_handle, 0, &wave_out_process_c::wave_out_format, (DWORD_PTR)&wave_out_process_c::waveOutProcImpl, (DWORD_PTR)this, CALLBACK_FUNCTION );
		if ( result != MMSYSERR_NOERROR )
		{
			cheonsa_annoy( L"error", L"waveOutOpen failed." );
			return false;
		}

		// begin wave out play back.
		wave_out_process_c::is_running = true;
		wave_out_process_c::is_running_count = wave_out_process_c::wave_out_buffer_split_count;
		wave_out_process_c::wave_out_buffer_split_size = ( wave_out_process_c::wave_out_format.wBitsPerSample / 8 ) * _channel_count * _sample_rate;
		wave_out_process_c::wave_out_buffer = reinterpret_cast< uint8_c * >( ops::memory_aligned_allocate( wave_out_process_c::wave_out_buffer_split_size * wave_out_process_c::wave_out_buffer_split_count, 16 ) );
		ops::memory_zero( wave_out_process_c::wave_out_buffer, wave_out_process_c::wave_out_buffer_split_size * wave_out_process_c::wave_out_buffer_split_count );
		for ( uint32_c i = 0; i < wave_out_process_c::wave_out_buffer_split_count; i++ )
		{
			ops::memory_zero( &wave_out_process_c::wave_out_headers[ i ], sizeof( WAVEHDR ) );
			wave_out_process_c::wave_out_headers[ i ].dwBufferLength = wave_out_process_c::wave_out_buffer_split_size;
			wave_out_process_c::wave_out_headers[ i ].lpData = reinterpret_cast< LPSTR >( &wave_out_process_c::wave_out_buffer[ wave_out_process_c::wave_out_buffer_split_size * i ] );
			result = waveOutPrepareHeader( wave_out_process_c::wave_out_handle, &wave_out_process_c::wave_out_headers[ i ], sizeof( WAVEHDR ) );
			if ( result != MMSYSERR_NOERROR )
			{
				cheonsa_annoy( L"error", L"waveOutPrepareHeader failed." );
				return false;
			}
			result = waveOutWrite( wave_out_process_c::wave_out_handle, &wave_out_process_c::wave_out_headers[ i ], sizeof( WAVEHDR ) ); // begin callback loop.
			if ( result != MMSYSERR_NOERROR )
			{
				cheonsa_annoy( L"error", L"waveOutWrite failed." );
				return false;
			}
		}

		return true;
	}

}

#endif
