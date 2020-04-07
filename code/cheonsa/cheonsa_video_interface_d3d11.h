#pragma once

#include "cheonsa_video_interface.h"

namespace cheonsa
{

	class video_interface_d3d11_c final
		: public video_interface_c
	{
	public:
		video_interface_d3d11_c();
		virtual ~video_interface_d3d11_c() override; // releases all of the interfaces and the Direct3D 10 device that were created in a prior call to start().

		virtual boolean_c start() override;

	public:
		virtual video_output_c * create_output( void_c * window_handle, video_texture_format_e texture_format ) override;
		virtual video_vertex_buffer_c * create_vertex_buffer( video_vertex_format_c const * vertex_format, sint32_c vertex_count, void_c * data, sint32_c data_size, boolean_c set_data_enable, boolean_c get_data_enable, boolean_c target_enable ) override;
		virtual video_index_buffer_c * create_index_buffer( video_index_format_e index_format, sint32_c index_count, void_c * data, sint32_c data_size, boolean_c set_data_enable, boolean_c get_data_enable ) override;
		virtual video_constant_buffer_c * create_constant_buffer( sint32_c size ) override;
		virtual video_texture_buffer_c * create_texture_buffer( sint32_c size ) override;
		virtual video_depth_stencil_c * create_depth_stencil( video_depth_stencil_format_e depth_stencil_format, sint32_c width, sint32_c height, sint32_c msaa_count, sint32_c array_slice_count ) override;
		virtual video_texture_c * create_texture( video_texture_format_e texture_format, sint32_c width, sint32_c height, sint32_c msaa_count, sint32_c array_slice_count, void_c const * top_mip_level_data, sint32_c top_mip_level_data_size, boolean_c set_data_enable, boolean_c get_data_enable, boolean_c target_enable, boolean_c mip_enable ) override;
		virtual video_vertex_shader_c * create_vertex_shader( char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines, video_vertex_layout_c const * input_vertex_layout ) override;
		virtual video_vertex_shader_c * create_vertex_shader( char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size, video_vertex_layout_c const * input_vertex_layout ) override;
		virtual video_geometry_shader_c * create_geometry_shader( char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines ) override;
		virtual video_geometry_shader_c * create_geometry_shader( char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size ) override;
		virtual video_pixel_shader_c * create_pixel_shader( char16_c const * name, char8_c const * source_code, sint32_c source_code_size, char8_c const * source_code_file_path, video_shader_includer_c * shader_includer, video_shader_defines_c const * shader_defines ) override;
		virtual video_pixel_shader_c * create_pixel_shader( char16_c const * name, char8_c const * compiled_code, sint32_c compiled_code_size ) override;

		virtual void_c copy_sub_resource( video_texture_c * destination_resource, uint32_c destination_sub_resource_index, video_texture_c * source_resource, uint32_c source_sub_resource_index ) override;
		virtual void_c copy_sub_resource( video_texture_c * destination_resource, uint32_c destination_sub_resource_index, video_depth_stencil_c * source_resource, uint32_c source_sub_resource_index ) override;

	public:
		virtual void_c clear_texture( video_output_c * texture, float32_c red, float32_c green, float32_c blue, float32_c alpha ) override;
		virtual void_c clear_texture( video_texture_c * texture, float32_c red, float32_c green, float32_c blue, float32_c alpha ) override;
		virtual void_c clear_depth_stencil( video_depth_stencil_c * depth_stencil, float32_c depth, sint32_c stencil ) override;
		virtual void_c bind_target_vertex_buffer( video_vertex_buffer_c * vertex_buffer ) override;
		virtual void_c bind_target_textures( sint32_c textures_count, video_texture_c * * textures, video_depth_stencil_c * depth_stencil, video_texture_type_e texture_type ) override;
		virtual void_c bind_target_texture2darrays_slices( sint32_c textures_count, video_texture_c * * textures, sint32_c * textures_slices, video_depth_stencil_c * depth_stencil, sint32_c depth_stencil_slice_index ) override;
		virtual void_c bind_rasterizer_view_port_rectangle( sint32_c left, sint32_c top, sint32_c right, sint32_c bottom, float32_c depth_minimum, float32_c depth_maximum ) override;
		virtual void_c bind_rasterizer_cull_fill_state( video_cull_type_e video_cull_type_e, video_fill_type_e video_fill_mode ) override;
		virtual void_c bind_rasterizer_depth_stencil_state( video_compare_function_e depth_compare_mode, boolean_c depth_write_enable ) override;
		virtual void_c bind_rasterizer_blend_state( video_blend_type_e blend_type ) override;
		virtual void_c bind_vertex_buffers( sint32_c vertex_buffers_count, video_vertex_buffer_c * * vertex_buffers ) override;
		virtual void_c bind_index_buffer( video_index_buffer_c * index_buffer ) override;
		virtual void_c bind_primitive_type( video_primitive_type_e primitive_type ) override;
		virtual void_c bind_vertex_shader( video_vertex_shader_c * vertex_shader ) override;
		virtual void_c bind_vertex_shader_constant_buffers( sint32_c bind_index, sint32_c constant_buffers_count, video_constant_buffer_c * * constant_buffers ) override;
		virtual void_c bind_vertex_shader_texture_buffers( sint32_c bind_index, sint32_c texture_buffers_count, video_texture_buffer_c * * texture_buffers ) override;
		virtual void_c bind_geometry_shader( video_geometry_shader_c * geometry_shader ) override;
		virtual void_c bind_geometry_shader_constant_buffers( sint32_c bind_index, sint32_c constant_buffers_count, video_constant_buffer_c * * constant_buffers ) override;
		virtual void_c bind_geometry_shader_texture_buffers( sint32_c bind_index, sint32_c texture_buffers_count, video_texture_buffer_c * * texture_buffers ) override;
		virtual void_c bind_pixel_shader( video_pixel_shader_c * pixel_shader ) override;
		virtual void_c bind_pixel_shader_constant_buffers( sint32_c bind_index, sint32_c constant_buffers_count, video_constant_buffer_c * * constant_buffers ) override;
		virtual void_c bind_pixel_shader_texture_buffers( sint32_c bind_index, sint32_c texture_buffers_count, video_texture_buffer_c * * texture_buffers ) override;
		virtual void_c bind_pixel_shader_textures( sint32_c bind_index, sint32_c textures_count, video_texture_c * * textures, video_texture_type_e * textures_types ) override;
		virtual void_c draw( sint32_c vertex_start, sint32_c vertex_count ) override;
		virtual void_c draw_indexed( sint32_c index_start, sint32_c index_count, sint32_c vertex_base ) override;

	protected:
		friend class video_interface_members_d3d11_c;
		video_interface_members_d3d11_c * _members;
		void_c _validate_vertex_bindings();

	};

}
