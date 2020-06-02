#include "cheonsa_audio_types.h"
#include "cheonsa_data_stream_file.h"
#include "cheonsa_data_scribe_binary.h"
#include "cheonsa__ops.h"

namespace cheonsa
{

	audio_wave_buffer_c::state_c::state_c( data_stream_c * stream )
		: _reference_counter( 0 )
		, _format( audio_wave_buffer_format_e_none )
		, _channel_count( 0 )
		, _sample_format( audio_sample_format_e_none )
		, _sample_rate( 0 )
		, _sample_count( 0 )
		, _data_buffer( nullptr )
		, _data_buffer_size( 0 )
	{
		assert( stream );
		assert( stream->get_position() == 0 );

		data_scribe_binary_c scribe;
		scribe.open( stream, byte_order_e_little );

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
				_sample_format = audio_sample_format_e_sint8;
				_sample_size = 1;
			}
			else if ( bits_per_sample == 16 )
			{
				_sample_format = audio_sample_format_e_sint16;
				_sample_size = 2;
			}
			else
			{
				return;
			}
			_sample_rate = sample_rate;
			_sample_count = _data_buffer_size / ( channel_count * bits_per_sample / 8 );

			// done.
			_format = audio_wave_buffer_format_e_raw;
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
			_sample_format = audio_sample_format_e_float32;
			_sample_size = 4;
			_sample_rate = temp_stb_vorbis_info.sample_rate;
			_sample_count = stb_vorbis_stream_length_in_samples( temp_stb_vorbis );
			stb_vorbis_close( temp_stb_vorbis );

			// done.
			_format = audio_wave_buffer_format_e_ogg;
		}
	}

	audio_wave_buffer_c::state_c::~state_c()
	{
		if ( _data_buffer )
		{
			delete[] _data_buffer;
			_data_buffer = nullptr;
		}
	}

	audio_wave_buffer_c::state_c * audio_wave_buffer_c::state_c::make_new_instance( data_stream_c * stream )
	{
		return new state_c( stream );
	}

	sint32_c audio_wave_buffer_c::state_c::add_reference()
	{
		return _reference_counter.increment();
	}

	sint32_c audio_wave_buffer_c::state_c::remove_reference()
	{
		sint32_c result = _reference_counter.decrement();
		if ( result == 0 )
		{
			delete this;
		}
		return result;
	}

	core_linked_list_c< audio_wave_buffer_c * > audio_wave_buffer_c::_instance_list;

	audio_wave_buffer_c::audio_wave_buffer_c()
		: _instance_list_node( this )
		, _state( nullptr )
		, _reference_count( 0 )
	{
		_instance_list.insert_at_end( &_instance_list_node );
	}

	audio_wave_buffer_c::~audio_wave_buffer_c()
	{
		if ( _state )
		{
			_state->remove_reference();
			_state = nullptr;
		}
		_instance_list.remove( &_instance_list_node );
	}

	audio_wave_buffer_c * audio_wave_buffer_c::make_new_instance()
	{
		return new audio_wave_buffer_c();
	}

	sint32_c audio_wave_buffer_c::add_reference()
	{
		return _reference_count.increment();
	}

	sint32_c audio_wave_buffer_c::remove_reference()
	{
		sint32_c result = _reference_count.decrement();
		if ( result == 0 )
		{
			delete this;
		}
		return result;
	}

	void_c audio_wave_buffer_c::load_new_state( data_stream_c * stream )
	{
		state_c * old_state = _state;
		_state = state_c::make_new_instance( stream );
		_state->add_reference(); // it would never return 0 at this point.
		if ( old_state )
		{
			old_state->remove_reference();
		}
	}

	void_c audio_wave_buffer_c::release_state()
	{
		state_c * old_state = _state;
		_state = nullptr;
		if ( old_state )
		{
			old_state->remove_reference();
		}
	}

	boolean_c audio_wave_player_c::_start()
	{
		// default to not playing.
		_is_playing = false;
		_sample = 0;

		// check that wave player can be played.
		if ( ( _wave_buffer == nullptr ) || ( _wave_buffer->_state == nullptr ) || ( _wave_buffer->_state->_format == audio_wave_buffer_format_e_none ) )
		{
			return false;
		}

		// replace wave buffer state link if needed.
		if ( _wave_buffer_state == nullptr || _wave_buffer_state != _wave_buffer->_state )
		{
			if ( _wave_buffer->_state->add_reference() != 0 ) // try to get reference to new state.
			{
				// forget old state.
				if ( _wave_buffer_state )
				{
					if ( _ogg_stb_vorbis )
					{
						stb_vorbis_close( _ogg_stb_vorbis );
						_ogg_stb_vorbis = nullptr;
					}
					_wave_buffer_state->remove_reference();
					_wave_buffer_state = nullptr;
				}

				// hold new state. we already got a reference to it in the if condition.
				_wave_buffer_state = _wave_buffer->_state;

				// try to open ogg stream if needed.
				if ( _wave_buffer_state->_format == audio_wave_buffer_format_e_ogg )
				{
					assert( _ogg_stb_vorbis == nullptr );
					int error = 0;
					_ogg_stb_vorbis = stb_vorbis_open_memory( _wave_buffer_state->_data_buffer, _wave_buffer_state->_data_buffer_size, &error, nullptr );
					if ( _ogg_stb_vorbis == nullptr )
					{
						// could not open ogg stream. forget new state.
						_wave_buffer_state->remove_reference();
						_wave_buffer_state = nullptr;
						return false;
					}
				}
			}
		}

		// play.
		_is_playing = true;
		if ( _randomize_seek )
		{
			_sample = ops::random_sint32( 0, _wave_buffer_state->_sample_count );
		}
		if ( _wave_buffer_state->_format == audio_wave_buffer_format_e_ogg )
		{
			stb_vorbis_seek( _ogg_stb_vorbis, _sample );
		}

		// done.
		return true;
	}

	boolean_c audio_wave_player_c::_advance( float32_c time_step )
	{
		return true;
	}

	boolean_c audio_wave_player_c::_sample_and_mix( sint32_c output_channel_count, sint32_c output_sample_rate, sint32_c output_sample_count, float32_c * output_sample_buffer, float32_c * output_channel_volumes )
	{
		assert( _is_playing && _wave_buffer_state && _wave_buffer_state->_format != audio_wave_buffer_format_e_none );

		// sample from the wave buffer and mix with output.
		if ( _wave_buffer_state->_format == audio_wave_buffer_format_e_raw )
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
			audio_mix( _wave_buffer_state->_sample_format, _wave_buffer_state->_channel_count, input_sample_count, input_sample_buffer, output_channel_count, output_sample_count, output_sample_buffer, output_channel_volumes );
		}
		else if ( _wave_buffer_state->_format == audio_wave_buffer_format_e_ogg )
		{
			// audio thread should not call us if we aren't ready.
			assert( _ogg_stb_vorbis );

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
			audio_mix( audio_sample_format_e_float32, _wave_buffer_state->_channel_count, input_sample_count_needed, input_sample_buffer, output_channel_count, output_sample_count, output_sample_buffer, output_channel_volumes );
		}

		return true;
	}

	core_linked_list_c< audio_wave_player_c * > audio_wave_player_c::_instance_list;

	audio_wave_player_c::audio_wave_player_c()
		: _instance_list_node( this )
		, _wave_player_list_node( this )
		, _wave_buffer( nullptr )
		, _wave_buffer_state( nullptr )
		, _randomize_seek( false )
		, _is_queued( false )
		, _is_playing( false )
		, _layer( audio_layer_e_effect )
		, _volume( 1.0f )
		, _speed( 1.0f )
		, _ogg_stb_vorbis( nullptr )
		, _sample( 0 )
		, _reference_count( 0 )
	{
		_instance_list.insert_at_end( &_instance_list_node );
	}

	audio_wave_player_c::~audio_wave_player_c()
	{
		_instance_list.remove( &_instance_list_node );
		if ( _ogg_stb_vorbis )
		{
			stb_vorbis_close( _ogg_stb_vorbis );
			_ogg_stb_vorbis = nullptr;
		}
		if ( _wave_buffer_state )
		{
			_wave_buffer_state->remove_reference();
			_wave_buffer_state = nullptr;
		}
		if ( _wave_buffer )
		{
			_wave_buffer->remove_reference();
			_wave_buffer = nullptr;
		}
	}

	audio_wave_player_c * audio_wave_player_c::make_new_instance()
	{
		return new audio_wave_player_c();
	}

	sint32_c audio_wave_player_c::add_reference()
	{
		return _reference_count.increment();
	}

	sint32_c audio_wave_player_c::remove_reference()
	{
		sint32_c result = _reference_count.decrement();
		if ( result == 0 )
		{
			delete this;
		}
		return result;
	}

	audio_wave_buffer_c * audio_wave_player_c::get_wave_buffer() const
	{
		return _wave_buffer;
	}

	void_c audio_wave_player_c::set_wave_buffer( audio_wave_buffer_c * value )
	{
		if ( _wave_buffer )
		{
			_wave_buffer->remove_reference();
			_wave_buffer = nullptr;
		}
		if ( value->add_reference() != 0 )
		{
			_wave_buffer = value;
		}
	}

	audio_layer_e audio_wave_player_c::get_layer() const
	{
		return _layer;
	}

	void_c audio_wave_player_c::set_layer( audio_layer_e value )
	{
		_layer = value;
	}

	float32_c audio_wave_player_c::get_volume() const
	{
		return _volume;
	}

	void_c audio_wave_player_c::set_volume( float32_c value )
	{
		_volume = ops::math_clamp( value, 0.0f, 1000.0f );
	}

	float32_c audio_wave_player_c::get_speed() const
	{
		return _speed;
	}

	void_c audio_wave_player_c::set_speed( float32_c value )
	{
		_speed = ops::math_clamp( value, 0.2f, 5.0f );
	}

	boolean_c audio_wave_player_c::get_appears_to_be_playing() const
	{
		return ( _is_queued || _is_playing );
	}

	audio_scene_source_c::audio_scene_source_c()
		: _scene_source_list_node( this )
		, _world_space_position()
		, _world_space_basis( matrix32x3x3_c::get_identity() )
		, _world_space_velocity()
		, _wave_player( nullptr )
		, _reference_count( 0 )
	{
		_wave_player = new audio_wave_player_c();
	}

	audio_scene_source_c::~audio_scene_source_c()
	{
		_wave_player->remove_reference();
	}

	audio_scene_source_c * audio_scene_source_c::make_new_instance()
	{
		return new audio_scene_source_c();
	}

	sint32_c audio_scene_source_c::add_reference()
	{
		return _reference_count.increment();
	}

	sint32_c audio_scene_source_c::remove_reference()
	{
		sint32_c result = _reference_count.decrement();
		if ( result == 0 )
		{
			delete this;
		}
		return result;
	}

	void_c audio_scene_source_c::set_world_space_transform( transform3d_c const & value )
	{
		_world_space_position = value.position;
		_world_space_basis = value.get_unscaled_basis();
	}

	vector64x3_c const & audio_scene_source_c::get_world_space_position() const
	{
		return _world_space_position;
	}

	matrix32x3x3_c const & audio_scene_source_c::get_world_space_basis() const
	{
		return _world_space_basis;
	}

	vector32x3_c const & audio_scene_source_c::get_world_space_velocity() const
	{
		return _world_space_velocity;
	}

	audio_wave_player_c * audio_scene_source_c::get_wave_player() const
	{
		return _wave_player;
	}

	float32_c audio_scene_source_c::get_effective_range_squared() const
	{
		return _wave_player->_volume / ( 4.0f * constants< float32_c >::pi() * cheonsa_audio_minimum_attenuation );
	}

	audio_scene_listener_c::audio_scene_listener_c()
		: _world_space_position()
		, _world_space_basis( matrix32x3x3_c::get_identity() )
		, _world_space_velocity()
		, _damping( 0.0f )
		, _sensitivity( 1.0f )
		//, _reference_count( 0 )
	{
	}

	//audio_scene_listener_c * audio_scene_listener_c::make_new_instance()
	//{
	//	return new audio_scene_listener_c();
	//}

	//sint32_c audio_scene_listener_c::add_reference()
	//{
	//	return _reference_count.increment();
	//}

	//sint32_c audio_scene_listener_c::remove_reference()
	//{
	//	sint32_c result = _reference_count.decrement();
	//	if ( result == 0 )
	//	{
	//		delete this;
	//	}
	//	return result;
	//}

	void_c audio_scene_listener_c::set_world_space_transform( transform3d_c const & value )
	{
		_world_space_position = value.position;
		_world_space_basis = value.get_unscaled_basis();
	}

	vector64x3_c const & audio_scene_listener_c::get_world_space_position() const
	{
		return _world_space_position;
	}

	matrix32x3x3_c const & audio_scene_listener_c::get_world_space_basis() const
	{
		return _world_space_basis;
	}

	vector32x3_c const & audio_scene_listener_c::get_world_space_velocity() const
	{
		return _world_space_velocity;
	}

	float32_c audio_scene_listener_c::get_damping() const
	{
		return _damping;
	}

	void_c audio_scene_listener_c::set_damping( float32_c value )
	{
		_damping = value;
	}

	float32_c audio_scene_listener_c::get_sensitivity() const
	{
		return _sensitivity;
	}

	void_c audio_scene_listener_c::set_sensitivity( float32_c value )
	{
		_sensitivity = 1.0f;
	}

	audio_scene_c::audio_scene_c()
		: _scene_listener()
		, _queued_scene_source_list()
		, _scene_source_list()
		, _is_added( false )
		, _reference_count( 0 )
	{
	}

	audio_scene_c::~audio_scene_c()
	{
		_critical_section.enter();

		for ( sint32_c i = 0; i < _queued_scene_source_list.get_length(); i++ )
		{
			_queued_scene_source_list[ i ].instance->remove_reference();
		}
		_queued_scene_source_list.remove_all();

		core_linked_list_c< audio_scene_source_c * >::node_c const * sources_node = _scene_source_list.get_first();
		while ( sources_node )
		{
			sources_node->get_value()->remove_reference();
			sources_node = sources_node->get_next();
		}
		_scene_source_list.remove_all_quick_and_dirty();

		_critical_section.exit();
	}

	audio_scene_c * audio_scene_c::make_new_instance()
	{
		return new audio_scene_c();
	}

	sint32_c audio_scene_c::add_reference()
	{
		return _reference_count.increment();
	}

	sint32_c audio_scene_c::remove_reference()
	{
		sint32_c result = _reference_count.decrement();
		if ( result == 0 )
		{
			delete this;
		}
		return result;
	}

	audio_scene_listener_c const & audio_scene_c::get_scene_listener() const
	{
		return _scene_listener;
	}

	audio_scene_listener_c & audio_scene_c::get_scene_listener()
	{
		return _scene_listener;
	}

	//vector64x3_c audio_scene_c::get_scene_listener_world_space_position() const
	//{
	//	if ( _scene_listener )
	//	{
	//		return _scene_listener->get_world_space_position();
	//	}
	//	return vector64x3_c();
	//}

	//void_c audio_scene_c::set_scene_listener( audio_scene_listener_c * value )
	//{
	//	if ( value->add_reference() != 0 )
	//	{
	//		_critical_section.enter();
	//		if ( _scene_listener )
	//		{
	//			_scene_listener->remove_reference();
	//			_scene_listener = nullptr;
	//		}
	//		_scene_listener = value;
	//		_critical_section.exit();
	//	}
	//}

	boolean_c audio_scene_c::add_scene_source( audio_scene_source_c * value, boolean_c randomize_seek )
	{
		assert( value );
		if ( value->add_reference() != 0 )
		{
			if ( value->_wave_player->_is_queued == false )
			{
				_critical_section.enter();
				value->_wave_player->_is_queued = true;
				value->_wave_player->_randomize_seek = randomize_seek;
				_scene_source_list.insert_at_end( &value->_scene_source_list_node );
				_critical_section.exit();
				return true;
			}
			value->remove_reference();
		}
		return false;
	}

	boolean_c audio_scene_c::remove_scene_source( audio_scene_source_c * value )
	{
		assert( value );
		if ( value->add_reference() != 0 )
		{
			_critical_section.enter();
			value->_wave_player->_is_queued = false;
			_scene_source_list.remove( &value->_scene_source_list_node );
			_critical_section.exit();
			return true;
		}
		return false;
	}

}
