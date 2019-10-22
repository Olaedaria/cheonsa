#pragma once

#include "cheonsa_math_types.h"

namespace cheonsa
{

	class audio_interface_c;
	class audio_buffer_c;
	class audio_player_c;
	class audio_listener_c;
	class audio_scene_c;
	class audio_source_c;

	enum audio_layer_e
	{
		audio_layer_e_music,
		audio_layer_e_ambient,
		audio_layer_e_effect,
		audio_layer_e_voice,
		audio_layer_e_menu,
		audio_layer_e_count_
	};

	//enum audio_channel_format_e
	//{
	//	audio_channel_format_e_none = 0,
	//	audio_channel_format_e_1 = 1, // mono. 0: center;
	//	audio_channel_format_e_2 = 2, // stero. 0: front left; 1: front right;
	//	audio_channel_format_e_6 = 6, // 5.1 surround. 0: front left; 1: front right; 2: front center; 3: low frequency; 4: rear left; 5: rear right;
	//};

	enum audio_sample_format_e
	{
		audio_sample_format_e_none = 0,
		audio_sample_format_e_sint8,
		audio_sample_format_e_sint16,
		audio_sample_format_e_float32,
	};

	// mixes audio from input to output.
	// if input and output have different sample buffer lengths, then the mixer will stretch or squish the number of samples from the input fits in to the number of samples in the output.
	// input_sample_format is the data type of each sample element.
	// input_channel_count is the numbers of channels in the input buffer.
	// input_sample_count is number of samples in input_sample_buffer. each sample has input_channel_count number of elements formatted as input_sample_format.
	// input_sample_buffer is the buffer that has sound samples to mix with output.
	// output_channel_count is the number of channels in the output buffer.
	// output_sample_countis number of samples in output_sample_buffer. each sample has output_channel_count number of elements formatted as float32_c.
	// output_sample_buffer is the in and out buffer to mix to.
	// output_channel_volumes defines volumes that mixer should apply to each channel.
	void_c audio_mix(
		audio_sample_format_e input_sample_format,
		sint32_c input_channel_count,
		sint32_c input_sample_count,
		void_c const * input_sample_buffer,
		sint32_c output_channel_count,
		sint32_c output_sample_count,
		float32_c * output_sample_buffer,
		float32_c * output_channel_volumes );

}
