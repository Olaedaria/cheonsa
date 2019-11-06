#pragma once

#include "cheonsa__types.h"
#include "cheonsa_video_interface.h"

namespace cheonsa
{

	class video_renderer_interface_c;

	// a canvas is a collection of render targets used to render scenes and menus to.
	// right now there are quite a lot of render targets and it is kind of expensive, but our design is complex in order to enable fancy features.
	class video_renderer_canvas_c
	{
	private:
		friend class video_renderer_interface_c;

		boolean_c _primary_enable; // if true then this canvas will manage another canvas used for rendering reflections to.
		boolean_c _post_process_enable; // determines if _target_color_half, _target_color_quarter, _target_color_quarter_blurred_x, and _target_color_quarter_blurred_xy are used.

		sint32_c _statistic_object_count; // number of scene objects that were rendered in the last frame.
		sint32_c _statistic_triangle_count; // number of triangles that were drawn in the last frame, includes those from scene and menu.

		sint32_c _actual_width; // the width of the original size target, which is rounded up to the nearest value to _view_port_width that is evenly divisible by 4, which is done to ensure that downsampling/blurring behaves well.
		sint32_c _actual_height; // the height of the original size target, which is rounded up to the nearest value to _view_port_height that is evenly divisible by 4, which is done to ensure that downsampling/blurring behaves well.

		sint32_c _apparent_width; // the apparent width of the canvas, the actual width of the render targets might be a little bit larger (by up to 3 pixels).
		sint32_c _apparent_height; // the apparent height of the canvas, the actual height of the render targets might be a little bit larger (by up to 3 pixels).

		vector32x4_c _outline_color_palette[ 16 ]; // index 0 is unused, but the rest are used. these define the outline colors. we can potentially define 256 entries since the outline buffer format is r8_uint.

		//float32_c _current_render_scale; // can be set between 0.25 and 1.0f. determines how much area of the render targets will be used when rendering the scene.
		//sint32_c _current_width; // _available_width * ( _current_render_scale / _available_render_scale ), defines the width of the current view port.
		//sint32_c _current_height; // _available_height * ( _current_render_scale / _available_render_scale ), defines the height of the current view port.

		// so... IDK if it's more performant to sample from a 1 channel texture (no filtering) or a 4 channel texture (no filtering).
		// this matters in the case that a shader only needs depth but not normal, or only needs normal but not depth.
		// it can read one without reading the other.
		// a compact way to store view space surface normals is to use only 2 channels, for x and y, then z can be reconstructed, and world space normal can be reconstructed from that.

		static video_depth_stencil_format_e const _format_depth_stencil = video_depth_stencil_format_e_d32;
		static video_texture_format_e const _format_outline = video_texture_format_e_r8_uint;
		static video_texture_format_e const _format_normal = video_texture_format_e_r16g16_float;
		static video_texture_format_e const _format_depth = video_texture_format_e_r32_float;
		//static video_texture_format_e const _format_normal_and_depth = video_texture_format_e_r32g32b32a32_float;
		static video_texture_format_e const _format_color = video_texture_format_e_r16g16b16a16_float;
		static video_texture_format_e const _format_color_final = video_texture_format_e_r8g8b8a8_unorm;

		video_depth_stencil_c * _target_depth_stencil; // full size. typical depth stencil buffer.
		video_texture_c * _target_outline; // full size. each pixel represents an index into _outline_color_palette, except for index 0 which means no outline.
		video_texture_c * _target_normal; // full size. view space normal, z coordinate can be reconstructed from x and y coordinates.
		video_texture_c * _target_depth; // full size. high precision depth for certain effects, is redundant with the depth stencil buffer because shaders can't sample from the depth stencil buffer.
		video_texture_c * _target_color; // full size.
		video_texture_c * _target_color_copy; // full size. used as input for overlay material passes (to do things like refractions) so that _target_color can "read"/"sample" itself while it is being rendered to.

		video_texture_c * _target_color_quarter; // quarter size (half width, half height), used as input for horizontal blur pass.
		video_texture_c * _target_color_quarter_blurred_x; // three fourths size (half width, full height), used by scene renderer, holds horizontal blur result, used as input for vertical blur pass.
		video_texture_c * _target_color_quarter_blurred_xy; // quarter size (half width, half height), used by scene renderer, holds horizontal blur + vertical blur result.

		video_texture_c * _target_color_final; // full size. only initialized and used if _output is not set. this is the target for the scene post process or resolve, so it holds the final image after post processing is applied. is also the target for 2d menu rendering.
		video_texture_c * _target_color_final_readable; // full size. only used if _output is not set. only used if _read_enable is true. this is a staging texture of which the contents of _target_color_final can be copied to, and of which can then be read by the cpu, and usually be saved as a screen shot.

		video_output_c * _output; // if set, then this canvas is associated with an operating system window, it will be resized as the window is resized, and it will output to the window.

		//video_renderer_canvas_c * _reflections_canvas; // full size. used as target to render planar reflectors|screens. not anti aliased, even if _msaa_count > 1.

	public:
		video_renderer_canvas_c( boolean_c primary_enable, boolean_c post_enable, void_c * window_handle );
		~video_renderer_canvas_c();

		// if this canvas is associated with a window, and if the window size changed or msaa_count changed, then this will recreate the textures at the new size.
		// if this canvas is not associated with a window, and if the apparent size changed enough to change the actual size, then this will recreate the textures at a new size.
		// returns true if canvas was resized, false if not.
		boolean_c update();

		// clears all the render targets in this canvas.
		void_c clear();

		// if this canvas is associated with an output window, then this function will present the backbuffer to the window.
		void_c present();

		sint32_c get_actual_width() const; // the actual internal width of the canvas in pixels.
		sint32_c get_actual_height() const; // the actual internal height of the canvas in pixels.

		sint32_c get_apparent_width() const; // the apparent width of the canvas in pixels. the actual canvas may be larger than this, in order to accommodate down sampling (blur) effects.
		sint32_c get_apparent_height() const; // the apparent height of the canvas in pixels. the actual canvas may be larger than this, in order to accommodate down sampling (blur) effects.
		void_c set_apparent_size( sint32_c width, sint32_c height ); // render target textures will be resized if needed on next call to update().

		video_texture_c * get_readable_copy(); // returns a copy of the final color render target that can be read by the CPU (which basically means that get_data can be called on the returned texture to get a copy of the the pixels of the canvas).

		//float64_c get_frame_rate();
		sint32_c get_statistic_object_count();
		sint32_c get_statistic_triangle_count();

		video_texture_c * get_target_color_final() const; // gets the video texture that contains the final render result, so that it can be used as input.

	};

}
