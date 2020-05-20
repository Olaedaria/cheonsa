#include "cheonsa_video_renderer_canvas.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"
#include <cassert>

namespace cheonsa
{

	video_renderer_canvas_c::video_renderer_canvas_c( boolean_c primary_enable, boolean_c post_enable, void_c * window_handle )
		: _primary_enable( primary_enable )
		, _post_process_enable( post_enable )
		, _statistic_object_count( 0 )
		, _statistic_triangle_count( 0 )
		, _actual_width( 0 )
		, _actual_height( 0 )
		, _apparent_width( 0 )
		, _apparent_height( 0 )
		, _target_depth_stencil( nullptr )
		, _target_outline( nullptr )
		, _target_normal( nullptr )
		, _target_depth( nullptr )
		, _target_color( nullptr )
		, _target_color_copy( nullptr )
		, _target_color_half_blurred_x( nullptr )
		, _target_color_half_blurred_xy( nullptr )
		, _target_color_quarter_blurred_x( nullptr )
		, _target_color_quarter_blurred_xy( nullptr )
		, _target_color_eighth_blurred_x( nullptr )
		, _target_color_eighth_blurred_xy( nullptr )
		, _target_color_final( nullptr )
		, _target_color_final_readable( nullptr )
		, _output( nullptr )
		//, _reflections_canvas( nullptr )
	{
		assert( engine.get_video_interface() != nullptr );

		//if ( _primary_enable )
		//{
		//	_reflections_canvas = new video_renderer_canvas_c( false, false, false, nullptr );
		//}

		if ( window_handle )
		{
			_output = engine.get_video_interface()->create_output( window_handle, _format_color_final );
		}
	}

	video_renderer_canvas_c::~video_renderer_canvas_c()
	{
		if ( _target_depth_stencil )
		{
			delete _target_depth_stencil;
			_target_depth_stencil = nullptr;
		}

		if ( _target_outline )
		{
			delete _target_outline;
			_target_outline = nullptr;
		}

		if ( _target_normal )
		{
			delete _target_normal;
			_target_normal = nullptr;
		}
		
		if ( _target_depth )
		{
			delete _target_depth;
			_target_depth = nullptr;
		}

		if ( _target_color )
		{
			delete _target_color;
			_target_color = nullptr;
		}

		if ( _target_color_copy )
		{
			delete _target_color_copy;
			_target_color_copy = nullptr;
		}

		if ( _target_color_half_blurred_x )
		{
			delete _target_color_half_blurred_x;
			_target_color_half_blurred_x = nullptr;
		}

		if ( _target_color_half_blurred_xy )
		{
			delete _target_color_half_blurred_xy;
			_target_color_half_blurred_xy = nullptr;
		}

		if ( _target_color_eighth_blurred_x )
		{
			delete _target_color_eighth_blurred_x;
			_target_color_eighth_blurred_x = nullptr;
		}

		if ( _target_color_eighth_blurred_xy )
		{
			delete _target_color_eighth_blurred_xy;
			_target_color_eighth_blurred_xy = nullptr;
		}

		if ( _target_color_quarter_blurred_x )
		{
			delete _target_color_quarter_blurred_x;
			_target_color_quarter_blurred_x = nullptr;
		}

		if ( _target_color_quarter_blurred_xy )
		{
			delete _target_color_quarter_blurred_xy;
			_target_color_quarter_blurred_xy = nullptr;
		}

		if ( _target_color_final )
		{
			delete _target_color_final;
			_target_color_final = nullptr;
		}

		if ( _target_color_final_readable )
		{
			delete _target_color_final_readable;
			_target_color_final_readable = nullptr;
		}

		if ( _output )
		{
			delete _output;
			_output = nullptr;
		}

		//delete _reflections_canvas;
		//_reflections_canvas = nullptr;
	}

	boolean_c video_renderer_canvas_c::update()
	{
		_statistic_object_count = 0;
		_statistic_triangle_count = 0;

		// if we are associated with a window output, then acquire our desired size from the output.
		if ( _output != nullptr )
		{
			if ( _output->update() )
			{
				_apparent_width = _output->get_texture_resource()->get_width();
				_apparent_height = _output->get_texture_resource()->get_height();
			}
			else
			{
				return false;
			}
		}

		// scale to multiples of 4.
		// this is so that the targets play well with our down sampling shader.
		sint32_c desired_actual_width = _apparent_width + ( 4 - _apparent_width % 4 );
		sint32_c desired_actual_height = _apparent_height + ( 4 - _apparent_height % 4 );
		if ( desired_actual_width <= 0 || desired_actual_height <= 0 )
		{
			return false;
		}

		// return now if no resize is needed.
		if ( _actual_width == desired_actual_width && _actual_height == desired_actual_height )
		{
			return false;
		}

		// release existing targets.
		if ( _target_depth_stencil != nullptr )
		{
			delete _target_depth_stencil;
			_target_depth_stencil = nullptr;
			delete _target_outline;
			_target_outline = nullptr;
			delete _target_normal;
			_target_normal = nullptr;
			delete _target_depth;
			_target_depth = nullptr;
			delete _target_color;
			_target_color = nullptr;
			delete _target_color_copy;
			_target_color_copy = nullptr;
			delete _target_color_half_blurred_x;
			_target_color_half_blurred_x = nullptr;
			delete _target_color_half_blurred_xy;
			_target_color_half_blurred_xy = nullptr;
			delete _target_color_quarter_blurred_x;
			_target_color_quarter_blurred_x = nullptr;
			delete _target_color_quarter_blurred_xy;
			_target_color_quarter_blurred_xy = nullptr;
			delete _target_color_eighth_blurred_x;
			_target_color_eighth_blurred_x = nullptr;
			delete _target_color_eighth_blurred_xy;
			_target_color_eighth_blurred_xy = nullptr;
			delete _target_color_final;
			_target_color_final = nullptr;
			delete _target_color_final_readable;
			_target_color_final_readable = nullptr;
		}

		// create new targets.
		if ( _target_depth_stencil == nullptr )
		{
			_actual_width = desired_actual_width;
			_actual_height = desired_actual_height;
			_target_depth_stencil = engine.get_video_interface()->create_depth_stencil( _format_depth_stencil, _actual_width, _actual_height, 1, 1 );
			_target_outline = engine.get_video_interface()->create_texture( _format_outline, _actual_width, _actual_height, 1, 1, nullptr, 0, false, false, true, false );
			_target_normal = engine.get_video_interface()->create_texture( _format_normal, _actual_width, _actual_height, 1, 1, nullptr, 0, false, false, true, false );
			_target_depth = engine.get_video_interface()->create_texture( _format_depth, _actual_width, _actual_height, 1, 1, nullptr, 0, false, false, true, false );
			_target_color = engine.get_video_interface()->create_texture( _format_color, _actual_width, _actual_height, 1, 1, nullptr, 0, false, false, true, false );
			_target_color_copy = engine.get_video_interface()->create_texture( _format_color_final, _actual_width, _actual_height, 1, 1, nullptr, 0, false, false, true, false );
			if ( _post_process_enable )
			{
				_target_color_half_blurred_x = engine.get_video_interface()->create_texture( _format_color, _actual_width / 2, _actual_height, 1, 1, nullptr, 0, false, false, true, false );
				_target_color_half_blurred_xy = engine.get_video_interface()->create_texture( _format_color, _actual_width / 2, _actual_height / 2, 1, 1, nullptr, 0, false, false, true, false );
				_target_color_quarter_blurred_x = engine.get_video_interface()->create_texture( _format_color, _actual_width / 4, _actual_height / 2, 1, 1, nullptr, 0, false, false, true, false );
				_target_color_quarter_blurred_xy = engine.get_video_interface()->create_texture( _format_color, _actual_width / 4, _actual_height / 4, 1, 1, nullptr, 0, false, false, true, false );
				_target_color_eighth_blurred_x = engine.get_video_interface()->create_texture( _format_color, _actual_width / 8, _actual_height / 4, 1, 1, nullptr, 0, false, false, true, false );
				_target_color_eighth_blurred_xy = engine.get_video_interface()->create_texture( _format_color, _actual_width / 8, _actual_height / 8, 1, 1, nullptr, 0, false, false, true, false );
			}
			if ( _output == nullptr )
			{
				_target_color_final = engine.get_video_interface()->create_texture( _format_color_final, _actual_width, _actual_height, 1, 1, nullptr, 0, false, false, true, false );
			}
		}

		return true;
	}

	void_c video_renderer_canvas_c::clear( vector32x4_c const & clear_color )
	{
		// clear render targets.
		engine.get_video_interface()->clear_depth_stencil( _target_depth_stencil, 1.0f, 0 );
		engine.get_video_interface()->clear_texture( _target_normal, 0.0f, 0.0f, 0.0f, 0.0f );
		engine.get_video_interface()->clear_texture( _target_depth, constants< float32_c >::maximum(), 0.0f, 0.0f, 0.0f ); // float32_maximum or view->clip_far?
		engine.get_video_interface()->clear_texture( _target_color, clear_color.a, clear_color.b, clear_color.c, clear_color.d );
		if ( _output != nullptr )
		{
			engine.get_video_interface()->clear_texture( _output, clear_color.a, clear_color.b, clear_color.c, clear_color.d );
		}
	}

	void_c video_renderer_canvas_c::present()
	{
		//uint64_c time_count = ops::time_get_high_resolution_timer_count();
		//float32_c time_step = static_cast< float32_c >( static_cast< float64_c >( time_count - _time_count_last ) / static_cast< float64_c >( ops::time_get_high_resolution_timer_frequency() ) );
		//float64_c frame_rate_sample = 1.0 / time_step;
		//_time_count_last = time_count;
		//_frame_rate = ops::interpolate_linear( _frame_rate, frame_rate_sample, time_step * 2.0 );
		//_scene_object_count = 0;
		//_scene_triangle_count = 0;
		//_scene_skinned_vertex_count = 0;

		if ( _output != nullptr )
		{
			_output->present();
		}
	}

	sint32_c video_renderer_canvas_c::get_actual_width() const
	{
		return _actual_width;
	}

	sint32_c video_renderer_canvas_c::get_actual_height() const
	{
		return _actual_height;
	}

	sint32_c video_renderer_canvas_c::get_apparent_width() const
	{
		return _apparent_width;
	}

	sint32_c video_renderer_canvas_c::get_apparent_height() const
	{
		return _apparent_height;
	}

	void_c video_renderer_canvas_c::set_apparent_size( sint32_c width, sint32_c height )
	{
		assert( _output == nullptr );
		assert( width > 0 );
		assert( height > 0 );
		_apparent_width = width;
		_apparent_height = height;
	}

	video_texture_c * video_renderer_canvas_c::get_readable_copy()
	{
		assert( _target_color_final );
		if ( _target_color_final_readable == nullptr )
		{
			_target_color_final_readable = engine.get_video_interface()->create_texture( _format_color_final, _actual_width, _actual_height, 1, 1, nullptr, 0, false, true, false, false );
		}
		assert( _target_color_final_readable != nullptr );
		engine.get_video_interface()->copy_sub_resource( _target_color_final_readable, 0, _target_color_final, 0 );
		return _target_color_final_readable;
	}

	sint32_c video_renderer_canvas_c::get_statistic_object_count()
	{
		return _statistic_object_count;
	}

	sint32_c video_renderer_canvas_c::get_statistic_triangle_count()
	{
		return _statistic_triangle_count;
	}

	video_texture_c * video_renderer_canvas_c::get_target_color_final() const
	{
		return _target_color_final;
	}

}
