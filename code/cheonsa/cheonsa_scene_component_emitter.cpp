#include "cheonsa_scene_component_emitter.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene_scene.h"
#include "cheonsa_ops.h"

namespace cheonsa
{

	scene_component_emitter_c::settings_c::settings_c()
		: emit_radius( 0.0f )
		, emit_rate( 10.0f )
		, emit_life_time_minimum( 1.0f )
		, emit_life_time_maximum( 1.0f )
		, emit_speed_minimum( 0.0f )
		, emit_speed_maximum( 0.0f )
		, emit_speed_wobble( 0.0f )
		, emit_angle_maximum( 0.0f )
		, emit_angle_wobble( 0.0f )
		, particle_gravity_scale( 1.0f )
		, particle_wind_scale( 0.5f )
		, confetti_enable( false )
		, confetti_sprite_type( scene_sprite_type_e_face_camera_global_up )
		, confetti_texture_resource_path()
		, confetti_texture_resource()
		, confetti_pixel_shader_resource_path()
		, confetti_pixel_shader_resource()
		, confetti_blend_type( video_blend_type_e_mix )
		, confetti_head_radius( 1.0f )
		, confetti_head_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, confetti_tail_radius( 1.0f )
		, confetti_tail_color( 1.0f, 1.0f, 1.0f, 0.0f )
		, streamer_enable( false )
		, streamer_head_radius( 1.0f )
		, streamer_head_color( 1.0f, 1.0f, 1.0f, 1.0f )
		, streamer_tail_radius( 1.0f )
		, streamer_tail_color( 1.0f, 1.0f, 1.0f, 0.0f )
		, streamer_tension( 1.0f )
		, streamer_interval_length( 1.0f )
		, streamer_interval_count( 10 )
		, streamer_texture_resource_path()
		, streamer_texture_resource()
		, streamer_texture_map_scale()
		, streamer_texture_map_scroll_speed()
		, streamer_pixel_shader_resource_path()
		, streamer_pixel_shader_resource()
		, streamer_blend_type( video_blend_type_e_mix )
	{
	}

	void scene_component_emitter_c::settings_c::validate()
	{
		emit_radius = ops::clamp( emit_radius, 0.0f, 1000000.0f );
		emit_rate = ops::clamp( emit_rate, 0.01f, 100.0f );
		emit_life_time_minimum = ops::clamp( emit_life_time_minimum, 0.001f, 1000.0f );
		emit_life_time_maximum = ops::clamp( emit_life_time_maximum, emit_life_time_minimum, 1000.0f );
		emit_speed_minimum = ops::clamp( emit_speed_minimum, 0.0f, 1000.0f );
		emit_speed_maximum = ops::clamp( emit_speed_maximum, emit_speed_minimum, 1000.0f );
		emit_speed_wobble = ops::clamp( emit_speed_wobble, 0.0f, 100.0f );
		emit_angle_maximum = ops::clamp( emit_angle_maximum, 0.0f, 180.0f );
		emit_angle_wobble = ops::clamp( emit_angle_wobble, 0.0f, 100.0f );
		confetti_head_radius = ops::clamp( confetti_head_radius, 0.0f, 100.0f );
		confetti_tail_radius = ops::clamp( confetti_tail_radius, 0.0f, 100.0f );
		streamer_head_radius = ops::clamp( streamer_head_radius, 0.0f, 100.0f );
		streamer_tail_radius = ops::clamp( streamer_tail_radius, 0.0f, 100.0f );
		streamer_tension = ops::clamp( streamer_tension, 0.0f, 1000.0f );
		streamer_interval_length = ops::clamp( streamer_interval_length, 0.001f, 1000.0f );
		streamer_interval_count = ops::clamp( streamer_interval_count, 1, 1000 );
	}

	scene_component_emitter_c::settings_c & scene_component_emitter_c::settings_c::operator = ( settings_c const & other )
	{
		emit_radius = other.emit_radius;
		emit_rate = other.emit_rate;
		emit_life_time_minimum = other.emit_life_time_minimum;
		emit_life_time_maximum = other.emit_life_time_maximum;
		emit_speed_minimum = other.emit_speed_minimum;
		emit_speed_maximum = other.emit_speed_maximum;
		emit_speed_wobble = other.emit_speed_wobble;
		emit_angle_maximum = other.emit_angle_maximum;
		emit_angle_wobble = other.emit_angle_wobble;

		particle_gravity_scale = other.particle_gravity_scale;
		particle_wind_scale = other.particle_wind_scale;

		confetti_enable = other.confetti_enable;
		confetti_sprite_type = other.confetti_sprite_type;
		confetti_texture_resource_path = other.confetti_texture_resource_path;
		confetti_texture_resource = other.confetti_texture_resource;
		confetti_pixel_shader_resource_path = other.confetti_pixel_shader_resource_path;
		confetti_pixel_shader_resource = other.confetti_pixel_shader_resource;
		confetti_blend_type = other.confetti_blend_type;
		confetti_head_radius = other.confetti_head_radius;
		confetti_head_color = other.confetti_head_color;
		confetti_tail_radius = other.confetti_tail_radius;
		confetti_tail_color = other.confetti_tail_color;

		streamer_enable = other.streamer_enable;
		streamer_head_radius = other.streamer_head_radius;
		streamer_head_color = other.streamer_head_color;
		streamer_tail_radius = other.streamer_tail_radius;
		streamer_tail_color = other.streamer_tail_color;
		streamer_tension = other.streamer_tension;
		streamer_interval_length = other.streamer_interval_length;
		streamer_interval_count = other.streamer_interval_count;
		streamer_texture_resource_path = other.streamer_texture_resource_path;
		streamer_texture_resource = other.streamer_texture_resource;
		streamer_texture_map_scale = other.streamer_texture_map_scale;
		streamer_texture_map_scroll_speed = other.streamer_texture_map_scroll_speed;
		streamer_pixel_shader_resource_path = other.streamer_pixel_shader_resource_path;
		streamer_pixel_shader_resource = other.streamer_pixel_shader_resource;
		streamer_blend_type = other.streamer_blend_type;

		return *this;
	}

	void scene_component_emitter_c::_handle_settings_changed()
	{
		sint32 required_seeds = 0;
		sint32 required_particles = 0;
		sint32 required_vertices = 0;

		required_seeds = static_cast<sint32>( _settings.emit_rate * _settings.emit_life_time_maximum + 1 );
		if ( _settings.confetti_enable )
		{
			required_vertices += required_seeds * 4;
		}
		if ( _settings.streamer_enable )
		{
			required_particles += _settings.streamer_interval_count + 1;
			required_vertices += _settings.streamer_interval_count * 2 + 2;
		}
		if ( _settings.confetti_enable && !_settings.streamer_enable )
		{
			required_particles += required_seeds;
		}

		if ( required_seeds != _seeds_count || required_particles != _particles_count || required_vertices != _vertices_count )
		{
			_seeds_count = required_seeds;
			_particles_count = required_particles;
			_vertices_count = required_vertices;

			delete[] _buffer;
			_buffer = nullptr;

			_buffer_size = sizeof( seed_c ) * _seeds_count + sizeof( particle_c ) * _particles_count + sizeof( video_renderer_vertex_mesh_c ) * _vertices_count;
			uint8 * buffer = new uint8[_buffer_size];
			_buffer = buffer;

			uint32 offset = 0;
			_seeds = reinterpret_cast<seed_c *>( &buffer[offset] );
			offset += sizeof( seed_c ) * _seeds_count;
			_particles = reinterpret_cast<particle_c *>( &buffer[offset] );
			offset += sizeof( particle_c ) * _particles_count;
			_vertices = reinterpret_cast<video_renderer_vertex_mesh_c *>( &buffer[offset] );

			_current_spawn_accumulator = 0.0f;
			_current_living_seed_index = 0;
			_current_living_seed_count = 0;
		}
	}

	void scene_component_emitter_c::_handle_after_added_to_scene()
	{
		_scene_object->_scene->_emitter_list.insert_at_end( this );
	}

	void scene_component_emitter_c::_handle_before_removed_from_scene()
	{
		_scene_object->_scene->_emitter_list.remove( this );
	}

	scene_component_emitter_c::scene_component_emitter_c()
		: scene_component_c()
		, _settings()
		, _seeds_count( 0 )
		, _seeds( nullptr )
		, _particles_count( 0 )
		, _particles( nullptr )
		, _vertices_count( 0 )
		, _vertices( nullptr )
		, _buffer_size( 0 )
		, _buffer( nullptr )
		, _current_spawn_accumulator( 0.0f )
		, _current_living_seed_index( 0 )
		, _current_living_seed_count( 0 )
	{
	}

	scene_component_emitter_c::~scene_component_emitter_c()
	{
	}

	void scene_component_emitter_c::get_settings( settings_c & result ) const
	{
		result = _settings;
	}

	void scene_component_emitter_c::set_settings( settings_c const & value )
	{
		_settings = value;
		_settings.validate();
		_handle_settings_changed();
	}

	void scene_component_emitter_c::update( float64 time_step )
	{
		_current_spawn_accumulator += _settings.emit_rate * time_step;
		if ( _current_living_seed_count < _seeds_count )
		{
			_current_living_seed_count += static_cast<sint32>( _current_spawn_accumulator );
			if ( _current_living_seed_count > _seeds_count )
			{
				_current_living_seed_count = _seeds_count;
			}
		}

		float32 sub_frame_fraction = 1.0f / ( _current_spawn_accumulator + 1.0f );
		while ( _current_spawn_accumulator >= 1.0f )
		{
			if ( _current_living_seed_index >= _seeds_count )
			{
				_current_living_seed_index = 0;
			}
			else
			{
				_current_living_seed_index++;
			}

			particle_c & particle = _particles[0];
			particle.position = ops::interpolate_linear( _last_position, _scene_object->_position, sub_frame_fraction );
			seed_c & seed = _seeds[_current_living_seed_index];

			sub_frame_fraction += sub_frame_fraction;

			_current_spawn_accumulator -= 1.0f;
		}

		_last_position = _scene_object->_position;
	}

}
