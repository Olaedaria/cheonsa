#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_resource_object_texture.h"
#include "cheonsa_video_renderer_types.h"
#include "cheonsa_video_renderer_shader_index.h"

namespace cheonsa
{

	class scene_component_emitter_c
		: public scene_component_c
	{
	public:
		static inline char8_c const * get_type_static() { return "emitter"; }
		virtual inline char8_c const * get_type() const { return get_type_static(); }

	public:
		class settings_c
		{
		public:
			// emitter settings:
			float32_c emit_radius; // the radius around this object's origin in which new seeds will be spawned.
			float32_c emit_rate; // the rate at which new seeds will be spawned in count per second.
			float32_c emit_life_time_minimum; // minimum randomized life time that new particles will be given.
			float32_c emit_life_time_maximum; // maximum randomized life time that new particles will be given.
			float32_c emit_speed_minimum; // the minimum randomized speed that new particles will be emitted at (along the object's positive z axis).
			float32_c emit_speed_maximum; // the maximum randomized speed that new particles will be emitted at (along the object's positive z axis).
			float32_c emit_speed_wobble; // if wobble is <= 0.0f then a random speed between _emit_speed_minimum and _emit_speed_maximum will be used for each spawned particle. otherwise, wobble defines the frequency at which speeds will smoothly vary between _emit_speed_minimum and _emit_speed_maximum.
			float32_c emit_angle_maximum; //	
			float32_c emit_angle_wobble; // if wobble is <= 0.0f then a random angle between 0.0f and _emit_angle_maximum will be used for each spawned particle. otherwise, wobble defines the frequency at which the emission axis will fluctuate between 0.0f and _emit_angle_maximum.

			// generic particle settings:
			float32_c particle_gravity_scale; // effect that gravity has on motion of particle. a value of 1 means that the particle inherits fully the force from gravity.
			float32_c particle_wind_scale; // effect that wind and air resistance has on motion of particle. a value of one means that the particle inherits fully the force from wind and air resistance.

			// confetti settings:
			boolean confetti_enable; // if true then each spawned element will be visualized by a sprite.
			scene_sprite_type_e confetti_sprite_type; // how the sprite will behave.
			string16 confetti_texture_resource_path;
			resource_texture_reference_c confetti_texture_resource; // texture to draw the sprite with.
			string16 confetti_pixel_shader_resource_path;
			video_renderer_pixel_shader_reference_c confetti_pixel_shader_resource; // pixel shader to use to draw the sprite.
			video_blend_type_e confetti_blend_type; // how the sprite will make_matrix_blended with the canvas.
			float32 confetti_head_radius;
			vector32x4_c confetti_head_color;
			float32 confetti_tail_radius;
			vector32x4_c confetti_tail_color;

			// streamer settings:
			boolean streamer_enable; // if true then each spawned element will be visualized by a streamer.
			float32 streamer_head_radius; // width of streamer head.
			vector32x4_c streamer_head_color; // color at head of streamer.
			float32 streamer_tail_radius; // width of streamer tail.
			vector32x4_c streamer_tail_color; // color at tail of streamer.
			float32 streamer_tension; // elastic tension between adjascent streamer particles which applies forces to keep the streamer from stretching|compressing.
			float32 streamer_interval_length; // length in meters between intervals. this length is also used with _streamer_tension to calculate the simulated forces needed to keep the streamer from stretching|compressing.
			sint32 streamer_interval_count; // number of intervals (quads).
			string16 streamer_texture_resource_path;
			resource_texture_reference_c streamer_texture_resource; // texture to draw the streamer with. the v|y axis is the length wise axis. the u|x axis is the width wise axis. the top of the texture is mapped toward the head of the streamer and the bottom of the texture is mapped toward the tail.
			float32 streamer_texture_map_scale; // by default, a scale of 1 maps the texture over the entire length of the streamer. set to a value larger than one to map the texture multiple times over the length of the streamer. or set to a value less than one to map the texture not more than one time over the length of the streamer. a negative value flips the mapping in the head|tail direction.
			float32 streamer_texture_map_scroll_speed; // how fast the streamer texture map will be scrolled over the length of the streamer.
			string16 streamer_pixel_shader_resource_path;
			video_renderer_pixel_shader_reference_c streamer_pixel_shader_resource; // pixel shader to use to draw the streamer.
			video_blend_type_e streamer_blend_type; // how the streamer will make_matrix_blended with the canvas.

			settings_c();

			void validate(); // fixes certain values that are set incorrectly (namely simulation related ones) to prevent things from breaking later.

			settings_c & operator = ( settings_c const & other );

		};

		// confetti and|or streamer instance.
		class seed_c
		{
		public:
			float32 life; // how long the spawn has left to live in seconds.
			sint32 particles_start; // references into _particles.
			sint32 particles_end; // references into _particles.
			sint32 vertices_start; // references into _vertices.
			sint32 vertices_end; // references into _vertices.
		};

		// basic unit of simulation.
		// each confetti is made of one of these.
		// each ribbon is made of a string of these.
		class particle_c
		{
		public:
			vector32x3_c position; // current world space position of particle.
			vector32x3_c velocity; // current world space velocity of particle.
		};

		settings_c _settings;

		list_c< uint8_c > _buffer; // one fixed size buffer to hold all of the data: seeds, particles, and vertices.

		sint32 _seeds_count; // number of seeds allocated within _buffer.
		seed_c * _seeds; // pointer to the seeds region within _buffer.

		sint32 _particles_count; // number of particles allocated within _buffer. each seed always accesses the same sub range of these.
		particle_c * _particles; // pointer to the particles region within _buffer.

		sint32 _vertices_count; // number of vertices allocated within _buffer.
		video_renderer_vertex_mesh_c * _vertices; // pointer to the vertices region within _buffer. each seed always accesses the same sub range of these.

		float32 _current_spawn_accumulator; // keeps track of how many seeds need to be spawned across updates.
		sint32 _current_living_seed_index; // last spawned seed. this value is incremented by the number of seeds spawned in a given update. once this reaches _seeds_count, it wraps back to 0.
		sint32 _current_living_seed_count; // current number of living seeds before and including _current_living_seed_index. _current_living_seed_index and _current_living_seed_count together define a circular buffer.
		vector32x3_c _last_position; // used to spawn new particles at sub frame precision in between last and current position.

		void _handle_settings_changed(); // if changes are needed then the buffer will be resized and the particle system simulation state will be reset. if no changes are needed then the paticle system simulation can continue from its current state.

		virtual void _handle_after_added_to_scene() override;
		virtual void _handle_before_removed_from_scene() override;

	public:
		scene_component_emitter_c();
		virtual ~scene_component_emitter_c() override;

		void get_settings( settings_c & result ) const;
		void set_settings( settings_c const & value );

		virtual void update( float64 time_step ) override;

	};

}
