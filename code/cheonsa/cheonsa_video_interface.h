#pragma once

#include "cheonsa_types.h"
#include "cheonsa_video_types.h"

namespace cheonsa
{

	// this simple video API is primarily designed to wrap around Direct3D 10.1, but with the hope that it remains forward compatible enough so that Direct3D 11, Direct3D 12, and Vulkan can be implemented with it later.
	// this will enable the game engine to use different underlying video APIs, so long as we can write an implementation behind this interface.
	class video_interface_c
	{
		friend class engine_c;

	public:
		virtual ~video_interface_c() = 0;

	private:
		virtual boolean_c start() = 0;

	public:
		virtual video_output_c * create_output( void_c * window_handle, video_texture_format_e texture_format ) = 0;
		virtual video_vertex_buffer_c * create_vertex_buffer( video_vertex_format_c const * vertex_format, sint32_c vertex_count, void_c * data, sint32_c data_size, boolean_c set_data_enable, boolean_c get_data_enable, boolean_c target_enable ) = 0; // creates a new vertex buffer.
		virtual video_index_buffer_c * create_index_buffer( video_index_format_e index_format, sint32_c index_count, void_c * data, sint32_c data_size, boolean_c set_data_enable, boolean_c get_data_enable ) = 0; // creates an index buffer.
		virtual video_constant_buffer_c * create_constant_buffer( sint32_c size ) = 0; // creates a new constant buffer.
		virtual video_texture_buffer_c * create_texture_buffer( sint32_c size ) = 0; // creates a new texture buffer (which like a constant buffer is used to store shader constants but it is bound to a texture slot instead, it's benefits are that it has better shader compile-time performance and better run-time performance for random access patterns).
		virtual video_depth_stencil_c * create_depth_stencil( video_depth_stencil_format_e depth_stencil_format, sint32_c width, sint32_c height, sint32_c msaa_count, sint32_c array_slice_count ) = 0; // creates a new depth stencil surface.
		virtual video_texture_c * create_texture( video_texture_format_e texture_format, sint32_c width, sint32_c height, sint32_c msaa_count, sint32_c array_slice_count, void_c const * top_mip_level_data, sint32_c top_mip_level_data_size, boolean_c set_data_enable, boolean_c get_data_enable, boolean_c target_enable, boolean_c mip_enable ) = 0; // creates a new texture surface. the rules are somewhat complicated since we use 1 texture class for all the different kinds of 2d texture types in the shader: Texture2D, Texture2DMS, Texture2DArray, Texture2DMSArray, TextureCube, TextureCubeArray.
		virtual video_vertex_shader_c * create_vertex_shader( char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines, video_vertex_layout_c const * input_vertex_layout ) = 0; // creates and compiles a new vertex shader.
		virtual video_vertex_shader_c * create_vertex_shader( char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size, video_vertex_layout_c const * input_vertex_layout ) = 0;
		virtual video_geometry_shader_c * create_geometry_shader( char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines ) = 0; // creates and compiles a new geometry shader.
		virtual video_geometry_shader_c * create_geometry_shader( char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size ) = 0;
		virtual video_pixel_shader_c * create_pixel_shader( char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines ) = 0; // creates and compiles a new pixel shader.
		virtual video_pixel_shader_c * create_pixel_shader( char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size ) = 0;

		virtual void_c copy_sub_resource( video_texture_c * destination_resource, uint32_c destination_sub_resource_index, video_texture_c * source_resource, uint32_c source_sub_resource_index ) = 0;
		virtual void_c copy_sub_resource( video_texture_c * destination_resource, uint32_c destination_sub_resource_index, video_depth_stencil_c * source_resource, uint32_c source_sub_resource_index ) = 0;

	public:
		virtual void_c clear_texture( video_output_c * texture, float32_c red, float32_c green, float32_c blue, float32_c alpha ) = 0; // clears an output texture.
		virtual void_c clear_texture( video_texture_c * texture, float32_c red, float32_c green, float32_c blue, float32_c alpha ) = 0; // clears all of the array slices in a render target texture.
		virtual void_c clear_depth_stencil( video_depth_stencil_c * depth_stencil, float32_c depth, sint32_c stencil ) = 0; // clears all of the array slices in a render target depth stencil texture.
		virtual void_c clear_depth_stencil_slice( video_depth_stencil_c * depth_stencil, float32_c depth, sint32_c stencil, sint32_c slice_index ) = 0;
		virtual void_c bind_target_vertex_buffer( video_vertex_buffer_c * vertex_buffer ) = 0; // if a target vertex buffer is set, then the output of the vertex shader (or geometry shader) will be written to this buffer (a feature known as "stream output" in Direct3D and "transform feedback" in openGL).
		virtual void_c bind_target_textures( sint32_c textures_count, video_texture_c * * textures, video_depth_stencil_c * depth_stencil, video_texture_type_e texture_type ) = 0; // all render targets must have the same width, height, and multi sample count. essentially binds just the first slice of each render target.
		virtual void_c bind_target_textures_slices( sint32_c textures_count, video_texture_c * * textures, sint32_c * textures_slices, video_depth_stencil_c * depth_stencil, sint32_c depth_stencil_slice ) = 0; // all render targets must have the same width, height, and multi sample count. binds just a single array slice of each render target so that it can be rendered to as though it is a regular texture2d and not a texture2darray.
		virtual void_c bind_rasterizer_view_port_rectangle( sint32_c x, sint32_c y, sint32_c width, sint32_c height, float32_c depth_minimum, float32_c depth_maximum ) = 0; // sets the view port rectangle, which defines a rectangular sub area of the target that will be rendered to. the output is scaled to fit in this rectangle, so aspect ratio of the projection matrix and this should at least match.
		virtual void_c bind_rasterizer_cull_fill_state( video_cull_type_e video_cull_type_e, video_fill_type_e video_fill_mode ) = 0; // sets the cull mode and fill mode for the rasterizer.
		virtual void_c bind_rasterizer_depth_stencil_state( video_compare_function_e depth_compare_mode, boolean_c depth_write_enable ) = 0; // sets the depth stencil mode for the rasterizer.
		virtual void_c bind_rasterizer_blend_state( video_blend_type_e blend_type ) = 0; // sets the blend mode for the rasterizer.
		virtual void_c bind_vertex_buffers( sint32_c vertex_buffers_count, video_vertex_buffer_c * * vertex_buffers ) = 0; // binds count vertex_buffers starting at input layout slot 0. all subsequent slots after count will be unbound.
		virtual void_c bind_index_buffer( video_index_buffer_c * index_buffer ) = 0; // set the index buffer for geometry input.
		virtual void_c bind_primitive_type( video_primitive_type_e primitive_type ) = 0; // set the primitive type.
		virtual void_c bind_vertex_shader( video_vertex_shader_c * vertex_shader ) = 0; // sets the vertex shader program that will process the input geometry.
		virtual void_c bind_vertex_shader_constant_buffers( sint32_c bind_index, sint32_c constant_buffers_count, video_constant_buffer_c * * constant_buffers ) = 0; // sets input paramters for the vertex shader program.
		virtual void_c bind_vertex_shader_texture_buffers( sint32_c bind_index, sint32_c texture_buffers_count, video_texture_buffer_c * * texture_buffers ) = 0;
		virtual void_c bind_geometry_shader( video_geometry_shader_c * geometry_shader ) = 0; // sets the geometry shader program that will process the vertex shader output.
		virtual void_c bind_geometry_shader_constant_buffers( sint32_c bind_index, sint32_c constant_buffers_count, video_constant_buffer_c * * constant_buffers ) = 0; // sets input paramters for the geometry shader program.
		virtual void_c bind_geometry_shader_texture_buffers( sint32_c bind_index, sint32_c texture_buffers_count, video_texture_buffer_c * * texture_buffers ) = 0;
		virtual void_c bind_pixel_shader( video_pixel_shader_c * pixel_shader ) = 0; // sets the pixel shader program that will process the vertex shader or geometry shader output.
		virtual void_c bind_pixel_shader_constant_buffers( sint32_c bind_index, sint32_c constant_buffers_count, video_constant_buffer_c * * constant_buffers ) = 0; // sets input paramters for the pixel shader program.
		virtual void_c bind_pixel_shader_texture_buffers( sint32_c bind_index, sint32_c texture_buffers_count, video_texture_buffer_c * * texture_buffers ) = 0;
		//virtual void_c bind_pixel_shader_texture( sint32_c bind_index, video_texture_c * texture, video_texture_type_e texture_type ) = 0;
		virtual void_c bind_pixel_shader_textures( sint32_c bind_index, sint32_c textures_count, video_texture_c * * textures, video_texture_type_e * textures_types ) = 0;
		virtual void_c draw( sint32_c vertex_start, sint32_c vertex_count ) = 0; // draws geometry from the currently bound vertex buffer.
		virtual void_c draw_indexed( sint32_c index_start, sint32_c index_count, sint32_c vertex_base ) = 0; // draws geometry from the currently bound vertex buffer and index buffer.

	};

}
