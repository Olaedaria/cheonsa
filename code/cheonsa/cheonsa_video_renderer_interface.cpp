#include "cheonsa_video_renderer_interface.h"
#include "cheonsa_scene.h"
#include "cheonsa_scene_object.h"
#include "cheonsa_scene_component.h"
#include "cheonsa_scene_component_light.h"
#include "cheonsa_scene_component_light_probe.h"
#include "cheonsa_scene_component_model.h"
#include "cheonsa_scene_component_sprite.h"
#include "cheonsa_scene_component_sound.h"
#include "cheonsa_scene_component_menu_control.h"
#include "cheonsa__ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	video_blend_type_e debug_line_blend_type = video_blend_type_e_mix;

	static video_vertex_buffer_c * vertex_buffers_to_bind[ 4 ] = {};
	static video_texture_c * textures_to_bind[ 8 ] = {};
	static video_texture_type_e textures_to_bind_types[ 8 ] = {};
	static sint32_c textures_slices_to_bind[ 8 ] = {};

	// initialize static constants.

	// video_renderer_interface_c::_cube_map_view_transforms
	video_renderer_interface_c::cube_face_view_c const video_renderer_interface_c::cube_face_view_list_base[ 6 ] = {
		cube_face_view_c{ vector32x3_c( -1.0f,  0.0f,  0.0f ), vector32x3_c(  0.0f,  0.0f,  1.0f ), ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( 0.0f, 0.0f, 0.0f ), vector32x3_c( -1.0f,  0.0f,  0.0f ), vector32x3_c(  0.0f,  0.0f,  1.0f ) ) }, // -x.
		cube_face_view_c{ vector32x3_c(  1.0f,  0.0f,  0.0f ), vector32x3_c(  0.0f,  0.0f,  1.0f ), ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( 0.0f, 0.0f, 0.0f ), vector32x3_c(  1.0f,  0.0f,  0.0f ), vector32x3_c(  0.0f,  0.0f,  1.0f ) ) }, // +x.
		cube_face_view_c{ vector32x3_c(  0.0f, -1.0f,  0.0f ), vector32x3_c(  0.0f,  0.0f,  1.0f ), ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( 0.0f, 0.0f, 0.0f ), vector32x3_c(  0.0f, -1.0f,  0.0f ), vector32x3_c(  0.0f,  0.0f,  1.0f ) ) }, // -y.
		cube_face_view_c{ vector32x3_c(  0.0f,  1.0f,  0.0f ), vector32x3_c(  0.0f,  0.0f,  1.0f ), ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( 0.0f, 0.0f, 0.0f ), vector32x3_c(  0.0f,  1.0f,  0.0f ), vector32x3_c(  0.0f,  0.0f,  1.0f ) ) }, // +y.
		cube_face_view_c{ vector32x3_c(  0.0f,  0.0f, -1.0f ), vector32x3_c(  0.0f,  1.0f,  0.0f ), ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( 0.0f, 0.0f, 0.0f ), vector32x3_c(  0.0f,  0.0f, -1.0f ), vector32x3_c(  0.0f,  1.0f,  0.0f ) ) }, // -z.
		cube_face_view_c{ vector32x3_c(  0.0f,  0.0f,  1.0f ), vector32x3_c(  0.0f,  1.0f,  0.0f ), ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( 0.0f, 0.0f, 0.0f ), vector32x3_c(  0.0f,  0.0f,  1.0f ), vector32x3_c(  0.0f,  1.0f,  0.0f ) ) } }; // +z.

	matrix32x4x4_c video_renderer_interface_c::get_cube_map_view_transform( sint32_c face_index, vector32x3_c const & position )
	{
		assert( face_index >= 0 && face_index < 6 );
		matrix32x4x4_c result = cube_face_view_list_base[ face_index ].view_transform;
		result.d.a = -ops::make_float32_dot_product( vector32x3_c( result.a.a, result.b.a, result.c.a ), position );
		result.d.b = -ops::make_float32_dot_product( vector32x3_c( result.a.b, result.b.b, result.c.b ), position );
		result.d.c = -ops::make_float32_dot_product( vector32x3_c( result.a.c, result.c.b, result.c.c ), position );
		return result;
	}

	// video_renderer_interface_c::vertex_format_mesh_base
	video_vertex_format_c::element_c const vertex_format_mesh_base_elements[ 5 ] = {
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 3, video_vertex_format_c::element_c::name_e_position, 0 ),
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 3, video_vertex_format_c::element_c::name_e_normal, 0 ),
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 3, video_vertex_format_c::element_c::name_e_normal_u, 0 ),
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 3, video_vertex_format_c::element_c::name_e_normal_v, 0 ),
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 4, video_vertex_format_c::element_c::name_e_texture, 0 ) };
	video_vertex_format_c const video_renderer_interface_c::vertex_format_mesh_base = video_vertex_format_c( "vertex_format_mesh_base", 5, vertex_format_mesh_base_elements, sizeof( video_renderer_vertex_mesh_base_c ) );

	// video_renderer_interface_c::vertex_format_mesh_bone_weight
	video_vertex_format_c::element_c const vertex_format_mesh_bone_weight_elements[ 2 ] = {
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_unorm16, 4, video_vertex_format_c::element_c::name_e_bone_weights, 0 ),
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_uint16, 4, video_vertex_format_c::element_c::name_e_bone_indices, 0 ) };
	video_vertex_format_c const video_renderer_interface_c::vertex_format_mesh_bone_weight = video_vertex_format_c( "vertex_format_mesh_bone_weight", 2, vertex_format_mesh_bone_weight_elements, sizeof( video_renderer_vertex_mesh_bone_weight_c ) );

	// video_renderer_interface_c::vertex_format_menu
	video_vertex_format_c::element_c const vertex_format_menu_elements[ 4 ] = {
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 3, video_vertex_format_c::element_c::name_e_position, 0 ),
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 3, video_vertex_format_c::element_c::name_e_texture, 0 ),
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 4, video_vertex_format_c::element_c::name_e_color, 0 ),
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 4, video_vertex_format_c::element_c::name_e_color, 1 ) };
	video_vertex_format_c const video_renderer_interface_c::vertex_format_menu = video_vertex_format_c( "vertex_format_menu", 4, vertex_format_menu_elements, sizeof( video_renderer_vertex_menu_c ) );

	// video_renderer_interface_c::vertex_format_debug
	video_vertex_format_c::element_c const vertex_format_debug_elements[ 2 ] = {
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 3, video_vertex_format_c::element_c::name_e_position, 0 ),
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 4, video_vertex_format_c::element_c::name_e_color, 0 ) };
	video_vertex_format_c const video_renderer_interface_c::vertex_format_debug = video_vertex_format_c( "vertex_format_debug", 2, vertex_format_debug_elements, sizeof( video_renderer_vertex_debug_c ) );

	// video_renderer_interface_c::vertex_format_point
	video_vertex_format_c::element_c const vertex_format_point_elements[ 1 ] {
		video_vertex_format_c::element_c( video_vertex_format_c::element_c::type_e_float32, 3, video_vertex_format_c::element_c::name_e_position, 0 ) };
	video_vertex_format_c const video_renderer_interface_c::vertex_format_point = video_vertex_format_c( "vertex_format_point", 1, vertex_format_point_elements, sizeof( video_renderer_vertex_point_c ) );
	
	// video_renderer_interface_c::vertex_layout_mesh_base
	video_vertex_format_c const * vertex_layout_mesh_base_elements[ 1 ] = {
		&video_renderer_interface_c::vertex_format_mesh_base };
	video_vertex_layout_c const video_renderer_interface_c::vertex_layout_mesh_base = video_vertex_layout_c( "vertex_layout_mesh_base", 1, vertex_layout_mesh_base_elements );

	// video_renderer_interface_c::vertex_layout_mesh_base_and_bone_weight
	video_vertex_format_c const * vertex_layout_mesh_base_and_bone_weight_elements[ 2 ] = {
		&video_renderer_interface_c::vertex_format_mesh_base,
		&video_renderer_interface_c::vertex_format_mesh_bone_weight };
	video_vertex_layout_c const video_renderer_interface_c::vertex_layout_mesh_base_and_bone_weight = video_vertex_layout_c( "vertex_layout_mesh_base_and_bone_weight", 2, vertex_layout_mesh_base_and_bone_weight_elements );

	// video_renderer_interface_c::vertex_layout_menu
	video_vertex_format_c const * vertex_layout_menu_elements[ 1 ] = {
		&video_renderer_interface_c::vertex_format_menu };
	video_vertex_layout_c const video_renderer_interface_c::vertex_layout_menu = video_vertex_layout_c( "vertex_layout_menu", 1, vertex_layout_menu_elements );

	// video_renderer_interface_c::vertex_layout_debug
	video_vertex_format_c const * vertex_layout_debug_elements[ 1 ] = {
		&video_renderer_interface_c::vertex_format_debug };
	video_vertex_layout_c const video_renderer_interface_c::vertex_layout_debug = video_vertex_layout_c( "vertex_layout_debug", 1, vertex_layout_debug_elements );

	// video_renderer_interface_c::vertex_layout_point
	video_vertex_format_c const * vertex_layout_point_elements[ 1 ] = {
		&video_renderer_interface_c::vertex_format_point };
	video_vertex_layout_c const video_renderer_interface_c::vertex_layout_point = video_vertex_layout_c( "vertex_layout_point", 1, vertex_layout_point_elements );

	//// video_renderer_interface_c::shader_define
	//video_shader_defines_c::element_c shader_define_elements[ 1 ] = {
	//	video_shader_defines_c::element_c( "lights_per_model_count", get_lights_per_model_count_string( video_renderer_interface_c::_lights_per_model_count ) ) };
	//video_shader_defines_c const video_renderer_interface_c::shader_define = video_shader_defines_c( 1, shader_define_elements );

	// video_renderer_interface_c::shader_define_masked
	video_shader_defines_c::element_c shader_define_masked_elements[ 1 ] = {
		video_shader_defines_c::element_c( "masked", "" ) };
	video_shader_defines_c const video_renderer_interface_c::shader_define_masked = video_shader_defines_c( 1, shader_define_masked_elements );

	// video_renderer_interface_c::shader_define_clipped
	video_shader_defines_c::element_c shader_define_clipped_elements[ 1 ] = {
		video_shader_defines_c::element_c( "clipped", "" ) };
	video_shader_defines_c const video_renderer_interface_c::shader_define_clipped = video_shader_defines_c( 1, shader_define_clipped_elements );

	// video_renderer_interface_c::shader_define_waved
	video_shader_defines_c::element_c shader_define_waved_elements[ 1 ] = {
		video_shader_defines_c::element_c( "waved", "" ) };
	video_shader_defines_c const video_renderer_interface_c::shader_define_waved = video_shader_defines_c( 1, shader_define_waved_elements );

	// video_renderer_interface_c::shader_define_waved_clipped
	video_shader_defines_c::element_c shader_define_waved_clipped_elements[ 2 ] = {
		video_shader_defines_c::element_c( "waved", "" ),
		video_shader_defines_c::element_c( "clipped", "" ) };
	video_shader_defines_c const video_renderer_interface_c::shader_define_waved_clipped = video_shader_defines_c( 2, shader_define_waved_clipped_elements );

	boolean_c video_renderer_interface_c::settings_c::get_window_mode_string( window_mode_e value, string8_c & result )
	{
		if ( value == window_mode_e_window )
		{
			result = string8_c( mode_e_static, "window" );
			return true;
		}
		else if ( value == window_mode_e_window_full_screen )
		{
			result = string8_c( mode_e_static, "window_full_screen" );
			return true;
		}
		else if ( value == window_mode_e_full_screen )
		{
			result = string8_c( mode_e_static, "full_screen" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_window_mode_value( string8_c const & string, window_mode_e & result )
	{
		if ( string == "window" )
		{
			result = window_mode_e_window;
			return true;
		}
		else if ( string == "window_full_screen" )
		{
			result = window_mode_e_window_full_screen;
			return true;
		}
		else if ( string == "full_screen" )
		{
			result = window_mode_e_full_screen;
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_presentation_mode_string( presentation_mode_e value, string8_c & result )
	{
		if ( value == presentation_mode_e_immediate )
		{
			result = string8_c( mode_e_static, "immediate" );
			return true;
		}
		else if ( value == presentation_mode_e_v_sync )
		{
			result = string8_c( mode_e_static, "v_sync" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_presentation_mode_value( string8_c const & string, presentation_mode_e & result )
	{
		if ( string == "immediate" )
		{
			result = presentation_mode_e_immediate;
			return true;
		}
		else if ( string == "v_sync" )
		{
			result = presentation_mode_e_v_sync;
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_post_process_quality_string( post_process_quality_e value, string8_c & result )
	{
		if ( value == post_process_quality_e_off )
		{
			result = string8_c( mode_e_static, "off" );
			return true;
		}
		else if ( value == post_process_quality_e_high )
		{
			result = string8_c( mode_e_static, "high" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_post_process_quality_value( string8_c const & string, post_process_quality_e & result )
	{
		if ( string == "off" )
		{
			result = post_process_quality_e_off;
			return true;
		}
		else if ( string == "high" )
		{
			result = post_process_quality_e_high;
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_texture_resolution_string( texture_resolution_e value, string8_c & result )
	{
		if ( value == texture_resolution_e_quarter )	
		{
			result = string8_c( mode_e_static, "quarter" );
			return true;
		}
		else if ( value == texture_resolution_e_half )
		{
			result = string8_c( mode_e_static, "half" );
			return true;
		}
		else if ( value == texture_resolution_e_whole )
		{
			result = string8_c( mode_e_static, "whole" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_texture_resolution_value( string8_c const & string, texture_resolution_e & result )
	{
		if ( string == "quarter" )
		{
			result = texture_resolution_e_quarter;
			return true;
		}
		else if ( string == "half" )
		{
			result = texture_resolution_e_half;
			return true;
		}
		else if ( string == "whole" )
		{
			result = texture_resolution_e_whole;
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_texture_sample_quality_string( texture_sample_quality_e value, string8_c & result )
	{
		if ( value == texture_sample_quality_e_bilinear )
		{
			result = string8_c( mode_e_static, "bilinear" );
			return true;
		}
		else if ( value == texture_sample_quality_e_trilinear )
		{
			result = string8_c( mode_e_static, "trilinear" );
			return true;
		}
		else if ( value == texture_sample_quality_e_anisotropic_2x )
		{
			result = string8_c( mode_e_static, "anisotropic_2x" );
			return true;
		}
		else if ( value == texture_sample_quality_e_anisotropic_4x )
		{
			result = string8_c( mode_e_static, "anisotropic_4x" );
			return true;
		}
		else if ( value == texture_sample_quality_e_anisotropic_8x )
		{
			result = string8_c( mode_e_static, "anisotropic_8x" );
			return true;
		}
		else if ( value == texture_sample_quality_e_anisotropic_16x )
		{
			result = string8_c( mode_e_static, "anisotropic_16x" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_texture_sample_quality_value( string8_c const & string, texture_sample_quality_e & result )
	{
		if ( string == "bilinear" )
		{
			result = texture_sample_quality_e_bilinear;
			return true;
		}
		else if ( string == "trilinear" )
		{
			result = texture_sample_quality_e_trilinear;
			return true;
		}
		else if ( string == "anisotropic_2x" )
		{
			result = texture_sample_quality_e_anisotropic_2x;
			return true;
		}
		else if ( string == "anisotropic_4x" )
		{
			result = texture_sample_quality_e_anisotropic_4x;
			return true;
		}
		else if ( string == "anisotropic_8x" )
		{
			result = texture_sample_quality_e_anisotropic_8x;
			return true;
		}
		else if ( string == "anisotropic_16x" )
		{
			result = texture_sample_quality_e_anisotropic_16x;
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_light_count_string( light_count_e value, string8_c & result )
	{
		if ( value == light_count_e_16 )
		{
			result = string8_c( mode_e_static, "16" );
			return true;
		}
		else if ( value == light_count_e_32 )
		{
			result = string8_c( mode_e_static, "32" );
			return true;
		}
		else if ( value == light_count_e_64 )
		{
			result = string8_c( mode_e_static, "64" );
			return true;
		}
		else if ( value == light_count_e_128 )
		{
			result = string8_c( mode_e_static, "128" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_light_count_value( string8_c const & string, light_count_e & result )
	{
		sint32_c value;
		if ( ops::convert_string8_to_sint32( string, value ) )
		{
			if ( value >= 128 )
			{
				result = light_count_e_128;
				return true;
			}
			else if ( value >= 64 )
			{
				result = light_count_e_64;
				return true;
			}
			else if ( value >= 32 )
			{
				result = light_count_e_32;
				return true;
			}
			else if ( value >= 16 )
			{
				result = light_count_e_16;
				return true;
			}
		}
		return false;
	}

	sint32_c video_renderer_interface_c::settings_c::get_light_count( light_count_e value )
	{
		assert( value >= 0 && value < light_count_e_count_ );
		static sint32_c const light_count_map[ light_count_e_count_ ] = { 16, 32, 64, 128 };
		return light_count_map[ value ];
	}

	boolean_c video_renderer_interface_c::settings_c::get_shadow_view_count_string( shadow_view_count_e value, string8_c & result )
	{
		if ( value == shadow_view_count_e_8 )
		{
			result = string8_c( mode_e_static, "8" );
			return true;
		}
		else if ( value == shadow_view_count_e_16 )
		{
			result = string8_c( mode_e_static, "16" );
			return true;
		}
		else if ( value == shadow_view_count_e_32 )
		{
			result = string8_c( mode_e_static, "32" );
			return true;
		}
		else if ( value == shadow_view_count_e_64 )
		{
			result = string8_c( mode_e_static, "64" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_shadow_view_count_value( string8_c const & string, shadow_view_count_e & result )
	{
		sint32_c value;
		if ( ops::convert_string8_to_sint32( string, value ) )
		{
			if ( value >= 64 )
			{
				result = shadow_view_count_e_64;
				return true;
			}
			else if ( value >= 32 )
			{
				result = shadow_view_count_e_32;
				return true;
			}
			else if ( value >= 16 )
			{
				result = shadow_view_count_e_16;
				return true;
			}
			else if ( value >= 8 )
			{
				result = shadow_view_count_e_8;
				return true;
			}
		}
		return false;
	}

	sint32_c video_renderer_interface_c::settings_c::get_shadow_view_count( shadow_view_count_e value )
	{
		assert( value >= 0 && value < shadow_view_count_e_count_ );
		static sint32_c const shadow_view_count_map[ shadow_view_count_e_count_ ] = { 8, 16, 32, 64 };
		return shadow_view_count_map[ value ];
	}

	boolean_c video_renderer_interface_c::settings_c::get_shadow_resolution_string( shadow_resolution_e value, string8_c & result )
	{
		if ( value == shadow_resolution_e_512 )
		{
			result = string8_c( mode_e_static, "512" );
			return true;
		}
		else if ( value == shadow_resolution_e_1024 )
		{
			result = string8_c( mode_e_static, "1024" );
			return true;
		}
		else if ( value == shadow_resolution_e_2048 )
		{
			result = string8_c( mode_e_static, "2048" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_shadow_resolution_value( string8_c const & string, shadow_resolution_e & result )
	{
		sint32_c value;
		if ( ops::convert_string8_to_sint32( string, value ) )
		{
			if ( value >= 2048 )
			{
				result = shadow_resolution_e_2048;
				return true;
			}
			else if ( value >= 1024 )
			{
				result = shadow_resolution_e_1024;
				return true;
			}
			else if ( value >= 512 )
			{
				result = shadow_resolution_e_512;
				return true;
			}
		}
		return false;
	}

	sint32_c video_renderer_interface_c::settings_c::get_shadow_resolution( shadow_resolution_e value )
	{
		assert( value >= 0 && value < shadow_resolution_e_count_ );
		static sint32_c const shadow_resolution_map[ shadow_resolution_e_count_ ] = { 512, 1024, 2048 };
		return shadow_resolution_map[ value ];
	}

	boolean_c video_renderer_interface_c::settings_c::get_shadow_sample_quality_string( shadow_sample_quality_e value, string8_c & result )
	{
		if ( value == shadow_sample_quality_e_1x )
		{
			result = string8_c( mode_e_static, "1x" );
			return true;
		}
		else if ( value == shadow_sample_quality_e_5x )
		{
			result = string8_c( mode_e_static, "5x" );
			return true;
		}
		else if ( value == shadow_sample_quality_e_9x )
		{
			result = string8_c( mode_e_static, "9x" );
			return true;
		}
		else if ( value == shadow_sample_quality_e_16x )
		{
			result = string8_c( mode_e_static, "16x" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_shadow_sample_quality_value( string8_c const & string, shadow_sample_quality_e & result )
	{
		sint32_c value;
		if ( ops::convert_string8_to_sint32( string, value ) )
		{
			if ( value >= 16 )
			{
				result = shadow_sample_quality_e_16x;
				return true;
			}
			else if ( value >= 9 )
			{
				result = shadow_sample_quality_e_9x;
				return true;
			}
			else if ( value >= 5 )
			{
				result = shadow_sample_quality_e_5x;
				return true;
			}
			else if ( value >= 1 )
			{
				result = shadow_sample_quality_e_1x;
				return true;
			}
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_light_probe_resolution_string( light_probe_resolution_e value, string8_c & result )
	{
		if ( value == light_probe_resolution_e_16 )
		{
			result = string8_c( mode_e_static, "16" );
			return true;
		}
		else if ( value == light_probe_resolution_e_32 )
		{
			result = string8_c( mode_e_static, "32" );
			return true;
		}
		else if ( value == light_probe_resolution_e_64 )
		{
			result = string8_c( mode_e_static, "64" );
			return true;
		}
		return false;
	}

	boolean_c video_renderer_interface_c::settings_c::get_light_probe_resolution_value( string8_c const & string, light_probe_resolution_e & result )
	{
		sint32_c value;
		if ( ops::convert_string8_to_sint32( string, value ) )
		{
			if ( value >= 64 )
			{
				result = light_probe_resolution_e_64;
				return true;
			}
			else if ( value >= 32 )
			{
				result = light_probe_resolution_e_32;
				return true;
			}
			else if ( value >= 16 )
			{
				result = light_probe_resolution_e_16;
				return true;
			}
		}
		return false;
	}

	sint32_c video_renderer_interface_c::settings_c::get_light_probe_resolution( light_probe_resolution_e value )
	{
		static sint32_c const light_probe_resolution_map[ light_probe_resolution_e_count_ ] = { 16, 32, 64 };
		assert( value >= 0 && value < light_probe_resolution_e_count_ );
		return light_probe_resolution_map[ value ];
	}

	video_renderer_interface_c::settings_c::settings_c()
	{
		reset_to_default();
	}

	video_renderer_interface_c::settings_c & video_renderer_interface_c::settings_c::operator = ( settings_c const & other )
	{
		full_screen_display = other.full_screen_display;
		window_mode = other.window_mode;
		presentation_mode = other.presentation_mode;
		post_process_quality = other.post_process_quality;
		texture_resolution = other.texture_resolution;
		texture_sample_quality = other.texture_sample_quality;
		light_count = other.light_count;
		light_probe_resolution = other.light_probe_resolution;
		shadow_view_count = other.shadow_view_count;
		shadow_resolution = other.shadow_resolution;
		shadow_sample_quality = other.shadow_sample_quality;
		return *this;
	}

	void_c video_renderer_interface_c::settings_c::reset_to_default()
	{
		full_screen_display = 0;
		window_mode = window_mode_e_window;
		presentation_mode = presentation_mode_e_immediate;
		post_process_quality = post_process_quality_e_high;
		texture_resolution = texture_resolution_e_whole;
		texture_sample_quality = texture_sample_quality_e_anisotropic_4x;
		light_count = light_count_e_64;
		light_probe_resolution = light_probe_resolution_e_32;
		shadow_view_count = shadow_view_count_e_32;
		shadow_resolution = shadow_resolution_e_1024;
		shadow_sample_quality = shadow_sample_quality_e_9x;
	}

	video_renderer_interface_c::view_c::view_c()
		: canvas( nullptr )
		, camera( nullptr )
		, world_space_position()
		, world_space_basis()
		, world_space_frustum()
		, camera_view_projection_transform()
		//, camera_clip_near( 0.0f )
		//, camera_clip_far( 0.0f )
		, clip_plane_enable( false )
		, clip_plane()
		, view_port_rectangle{ 0, 0, 0, 0 }
		, scissor_rectangle{ 0, 0, 0, 0 }
		, mirror_model( nullptr )
	{
	}

	void_c video_renderer_interface_c::view_c::initialize_for_camera( scene_camera_c const * camera, vector64x3_c primary_view_world_space_position )
	{
		world_space_position = camera->_world_space_transform.position;
		world_space_basis = camera->_world_space_transform.get_unscaled_basis();
		if ( camera->_projection_type == scene_projection_type_e_perspective )
		{
			float32_c aspect_ratio = static_cast< float32_c >( canvas->get_apparent_width() ) / static_cast< float32_c >( canvas->get_apparent_height() );
			float32_c fov_a = camera->get_perspective_field_of_view();
			float32_c fov_b = ops::make_float32_fov_b( fov_a, aspect_ratio );
			world_space_frustum = ops::make_frustum64_perspective3( 
				camera->_world_space_transform.position,
				vector64x3_c( camera->_world_space_transform.get_unscaled_basis_b() ),
				vector64x3_c( camera->_world_space_transform.get_unscaled_basis_c() ),
				fov_a,
				fov_b,
				camera->get_perspective_clip_near(),
				camera->get_perspective_clip_far() );
		}
		else
		{
			float64_c aspect_ratio = static_cast< float32_c >( canvas->get_apparent_width() ) / canvas->get_apparent_height();
			float64_c fov_b = camera->_orthographic_field_of_view;
			float64_c fov_a = fov_b / aspect_ratio;
			world_space_frustum = ops::make_frustum64_orthographic3(
				camera->_world_space_transform.position,
				vector64x3_c( camera->_world_space_transform.get_unscaled_basis_b() ),
				vector64x3_c( camera->_world_space_transform.get_unscaled_basis_c() ),
				fov_b,
				fov_a,
				camera->_orthographic_clip_near,
				camera->_orthographic_clip_far );
		}

		matrix32x4x4_c camera_view_transform = camera->make_view_transform_relative_to_origin( primary_view_world_space_position );
		matrix32x4x4_c camera_projection_transform = camera->make_projection_transform( canvas->get_apparent_width(), canvas->get_apparent_height() );
		camera_view_projection_transform = camera_view_transform * camera_projection_transform;
	}

	video_renderer_interface_c::shadow_view_c::shadow_view_c()
		: world_space_position()
		, world_space_frustum()
		, virtual_space_position()
		, virtual_space_view_projection_transform()
	{
	}

	video_renderer_interface_c::video_renderer_interface_c()
		: _menu_draw_debug( false )
		, _menu_root_control_group_list()
		, _menu_draw_list_list()
		, _menu_vertex_list()
		, _menu_index_list()
		, _menu_vertex_buffer( nullptr )
		, _menu_index_buffer( nullptr )
		, _menu_debug_vertex_buffer( nullptr )
		, _scene( nullptr )
		, texture_white_pixel()
		, texture_white_pixel_wrapper( true )
		, texture_green_pixel()
		, texture_green_pixel_wrapper( true )
		, _scene_default_material()
		, _scene_light_probe_object( nullptr )
		, _scene_light_probe_model()
		, _constant_buffers()
		, _texture_buffers()
		, _quad_vertex_buffer( nullptr )
		, _light_probe_cube_energy_default( nullptr )
		, _views()
		, _shadow_views()
		, _shadow_map_texture2darray( nullptr )
		, _gathered_lights()
		, _lights()
		, _gathered_components()
		, _models_to_render()
		, _models_to_render_for_outline()
		, _models_to_skin()
		, _sprites_to_render()
		, _sprites_to_render_for_outline()
		, _sprite_vertex_list()
		, _sprite_vertex_buffer( nullptr )
		, _menu_controls_to_render()
		, _debug_line_list_64()
		, _debug_line_list_xray_64()
		, _debug_line_list()
		, _debug_line_list_vertex_buffer()
	{
	}

	video_renderer_interface_c::~video_renderer_interface_c()
	{
		texture_white_pixel_wrapper.set_video_texture( nullptr );
		delete texture_white_pixel;
		texture_white_pixel = nullptr;

		texture_green_pixel_wrapper.set_video_texture( nullptr );
		delete texture_green_pixel;
		texture_green_pixel = nullptr;

		delete _scene_light_probe_object;
		_scene_light_probe_object = nullptr;

		delete _light_probe_cube_energy_default;
		_light_probe_cube_energy_default = nullptr;

		ops::memory_aligned_deallocate( _constant_buffers.shadow_camera_block );
		_constant_buffers.shadow_camera_block = nullptr;
		delete _constant_buffers.shadow_camera_block_constant_buffer;
		_constant_buffers.shadow_camera_block_constant_buffer = nullptr;

		ops::memory_aligned_deallocate( _constant_buffers.shadow_object_block );
		_constant_buffers.shadow_object_block = nullptr;
		delete _constant_buffers.shadow_object_block_constant_buffer;
		_constant_buffers.shadow_object_block_constant_buffer = nullptr;

		ops::memory_aligned_deallocate( _constant_buffers.scene_block );
		_constant_buffers.scene_block = nullptr;
		delete _constant_buffers.scene_block_constant_buffer;
		_constant_buffers.scene_block_constant_buffer = nullptr;

		ops::memory_aligned_deallocate( _constant_buffers.camera_block );
		_constant_buffers.camera_block = nullptr;
		delete _constant_buffers.camera_block_constant_buffer;
		_constant_buffers.camera_block_constant_buffer = nullptr;

		ops::memory_aligned_deallocate( _constant_buffers.object_block );
		_constant_buffers.object_block = nullptr;
		delete _constant_buffers.object_block_constant_buffer;
		_constant_buffers.object_block_constant_buffer = nullptr;

		ops::memory_aligned_deallocate( _constant_buffers.material_block );
		_constant_buffers.material_block = nullptr;
		delete _constant_buffers.material_block_constant_buffer;
		_constant_buffers.material_block_constant_buffer = nullptr;

		ops::memory_aligned_deallocate( _constant_buffers.menu_block );
		_constant_buffers.menu_block = nullptr;
		delete _constant_buffers.menu_block_constant_buffer;
		_constant_buffers.menu_block_constant_buffer = nullptr;

		ops::memory_aligned_deallocate( _constant_buffers.menu_batch_block );
		_constant_buffers.menu_batch_block = nullptr;
		delete _constant_buffers.menu_batch_block_constant_buffer;
		_constant_buffers.menu_batch_block_constant_buffer = nullptr;

		delete _texture_buffers.bones_texture_buffer;
		_texture_buffers.bones_texture_buffer = nullptr;

		delete _texture_buffers.lights_texture_buffer;
		_texture_buffers.lights_texture_buffer = nullptr;

		delete _texture_buffers.shadow_views_texture_buffer;
		_texture_buffers.shadow_views_texture_buffer = nullptr;

		delete _quad_vertex_buffer;
		_quad_vertex_buffer = nullptr;

		delete _sprite_vertex_buffer;
		_sprite_vertex_buffer = nullptr;

		delete _shadow_map_texture2darray;
		_shadow_map_texture2darray = nullptr;

		delete _debug_line_list_vertex_buffer;
		_debug_line_list_vertex_buffer = nullptr;

		delete _menu_vertex_buffer;
		_menu_vertex_buffer = nullptr;

		delete _menu_index_buffer;
		_menu_index_buffer = nullptr;

		delete _menu_debug_vertex_buffer;
		_menu_debug_vertex_buffer = nullptr;
	}

	boolean_c video_renderer_interface_c::start()
	{
		assert( engine_c::get_instance()->get_video_interface() != nullptr );

		_views.construct_mode_dynamic( 0, 4 );

		// initialize _point_vertex_quad_buffer.
		video_renderer_vertex_point_c point_vertex_quad_array[ 4 ];
		point_vertex_quad_array[ 0 ].position = vector32x3_c( -1.0f, -1.0f, 0.0f ); // top left.
		point_vertex_quad_array[ 1 ].position = vector32x3_c( 1.0f, -1.0f, 0.0f ); // top right.
		point_vertex_quad_array[ 2 ].position = vector32x3_c( -1.0f, 1.0f, 0.0f ); // bottom left.
		point_vertex_quad_array[ 3 ].position = vector32x3_c( 1.0f, 1.0f, 0.0f ); // bottom right.
		_quad_vertex_buffer = engine_c::get_instance()->get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_point, 4, point_vertex_quad_array, sizeof( point_vertex_quad_array ), false, false, false );

		// create constant buffers.
		// allocate blocks aligned to multiples of 16 bytes to optimize memcpy or map or something.
		// "constant buffers without constant pain"
		// https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0
		_constant_buffers.shadow_camera_block = reinterpret_cast< shadow_camera_block_c * >( ops::memory_aligned_allocate( sizeof( shadow_camera_block_c ), 16 ) ); ops::memory_zero( _constant_buffers.shadow_camera_block, sizeof( shadow_camera_block_c ) );
		_constant_buffers.shadow_camera_block_constant_buffer = engine_c::get_instance()->get_video_interface()->create_constant_buffer( sizeof( shadow_camera_block_c ) );
		_constant_buffers.shadow_object_block = reinterpret_cast< shadow_object_block_c * >( ops::memory_aligned_allocate( sizeof( shadow_object_block_c ), 16 ) ); ops::memory_zero( _constant_buffers.shadow_object_block, sizeof( shadow_object_block_c ) );
		_constant_buffers.shadow_object_block_constant_buffer = engine_c::get_instance()->get_video_interface()->create_constant_buffer( sizeof( shadow_object_block_c ) );
		_constant_buffers.scene_block = reinterpret_cast< scene_block_c * >( ops::memory_aligned_allocate( sizeof( scene_block_c ), 16 ) ); ops::memory_zero( _constant_buffers.scene_block, sizeof( scene_block_c ) );
		_constant_buffers.scene_block_constant_buffer = engine_c::get_instance()->get_video_interface()->create_constant_buffer( sizeof( scene_block_c ) );
		_constant_buffers.camera_block = reinterpret_cast< camera_block_c * >( ops::memory_aligned_allocate( sizeof( camera_block_c ), 16 ) ); ops::memory_zero( _constant_buffers.camera_block, sizeof( camera_block_c ) );
		_constant_buffers.camera_block_constant_buffer = engine_c::get_instance()->get_video_interface()->create_constant_buffer( sizeof( camera_block_c ) );
		_constant_buffers.object_block = reinterpret_cast< object_block_c * >( ops::memory_aligned_allocate( sizeof( object_block_c ), 16 ) ); ops::memory_zero( _constant_buffers.object_block, sizeof( object_block_c ) );
		_constant_buffers.object_block_constant_buffer = engine_c::get_instance()->get_video_interface()->create_constant_buffer( sizeof( object_block_c ) );
		_constant_buffers.material_block = reinterpret_cast< material_block_c * >( ops::memory_aligned_allocate( sizeof( material_block_c ), 16 ) ); ops::memory_zero( _constant_buffers.material_block, sizeof( material_block_c ) );
		_constant_buffers.material_block_constant_buffer = engine_c::get_instance()->get_video_interface()->create_constant_buffer( sizeof( material_block_c ) );
		_constant_buffers.menu_block = reinterpret_cast< menu_block_c * >( ops::memory_aligned_allocate( sizeof( menu_block_c ), 16 ) ); ops::memory_zero( _constant_buffers.menu_block, sizeof( menu_block_c ) );
		_constant_buffers.menu_block_constant_buffer = engine_c::get_instance()->get_video_interface()->create_constant_buffer( sizeof( menu_block_c ) );
		_constant_buffers.menu_batch_block = reinterpret_cast< menu_batch_block_c * >( ops::memory_aligned_allocate( sizeof( menu_batch_block_c ), 16 ) ); ops::memory_zero( _constant_buffers.menu_batch_block, sizeof( menu_batch_block_c ) );
		_constant_buffers.menu_batch_block_constant_buffer = engine_c::get_instance()->get_video_interface()->create_constant_buffer( sizeof( menu_batch_block_c ) );

		// create texture buffers.
		_texture_buffers.bones_texture_buffer = engine_c::get_instance()->get_video_interface()->create_texture_buffer( sizeof( matrix32x4x4_c ) * bones_limit );
		_texture_buffers.lights_texture_buffer = engine_c::get_instance()->get_video_interface()->create_texture_buffer( sizeof( video_renderer_light_c ) * _settings.light_count );
		_texture_buffers.shadow_views_texture_buffer = engine_c::get_instance()->get_video_interface()->create_texture_buffer( sizeof( video_renderer_shadow_view_c ) * _settings.shadow_view_count );

		// load default texture that is used to draw materials with textures that coulnd't be loaded.
		//_scene_default_texture = global_engine_instance.interfaces.resource_manager->load_texture( string16_c( mode_e_static, L"[e]internal/default.png" ) );

		vector32x4_c _light_probe_cube_energy_default_data[ 6 ] = {
			vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0 ),
			vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0 ),
			vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0 ),
			vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0 ),
			vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0 ),
			vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0 ) };
		_light_probe_cube_energy_default = engine_c::get_instance()->get_video_interface()->create_texture( video_texture_format_e_r32g32b32a32_float, 1, 1, 1, 6, _light_probe_cube_energy_default_data, 4 * 4 * 6, false, false, false, false );

		// light probe model used to display light probes when light probe visualization is enabled ( global_engine_instance.debug.draw_object_light_probes == true ).
		//_scene_light_probe_model = global_engine_instance.interfaces.resource_manager->load_model( string16_c( mode_e_static, L"[e]internal/sphere.model" ) );

		// bind constant buffers once, they will stay bound for the life of the program.
		video_constant_buffer_c * constant_buffers_to_bind[ 8 ];
		// bind vs constant buffers..
		constant_buffers_to_bind[ 0 ] = _constant_buffers.shadow_camera_block_constant_buffer;
		constant_buffers_to_bind[ 1 ] = _constant_buffers.shadow_object_block_constant_buffer;
		constant_buffers_to_bind[ 2 ] = _constant_buffers.scene_block_constant_buffer;
		constant_buffers_to_bind[ 3 ] = _constant_buffers.camera_block_constant_buffer;
		constant_buffers_to_bind[ 4 ] = _constant_buffers.object_block_constant_buffer;
		constant_buffers_to_bind[ 5 ] = _constant_buffers.material_block_constant_buffer;
		constant_buffers_to_bind[ 6 ] = _constant_buffers.menu_block_constant_buffer;
		constant_buffers_to_bind[ 7 ] = _constant_buffers.menu_batch_block_constant_buffer;
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader_constant_buffers( 0, 8, constant_buffers_to_bind );
		// pind ps constant buffers, a few are ommitted.
		constant_buffers_to_bind[ 0 ] = nullptr;
		constant_buffers_to_bind[ 1 ] = nullptr;
		constant_buffers_to_bind[ 2 ] = _constant_buffers.scene_block_constant_buffer;
		constant_buffers_to_bind[ 3 ] = _constant_buffers.camera_block_constant_buffer;
		constant_buffers_to_bind[ 4 ] = _constant_buffers.object_block_constant_buffer;
		constant_buffers_to_bind[ 5 ] = _constant_buffers.material_block_constant_buffer;
		constant_buffers_to_bind[ 6 ] = nullptr;
		constant_buffers_to_bind[ 7 ] = _constant_buffers.menu_batch_block_constant_buffer;
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_constant_buffers( 0, 8, constant_buffers_to_bind );

		// bind texture buffers once and forget about them.
		// these should remain bound for the life time of the program.
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader_texture_buffers( _texture_bind_index_for_bones_texture_buffer, 1, &_texture_buffers.bones_texture_buffer );
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_texture_buffers( _texture_bind_index_for_bones_texture_buffer, 1, &_texture_buffers.lights_texture_buffer );
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_texture_buffers( _texture_bind_index_for_bones_texture_buffer, 1, &_texture_buffers.shadow_views_texture_buffer );

		uint8_c white[ 4 ] = { 255, 255, 255, 255 };
		texture_white_pixel = engine_c::get_instance()->get_video_interface()->create_texture( video_texture_format_e_r8g8b8a8_unorm, 1, 1, 1, 1, white, 4, false, false, false, false );
		texture_white_pixel_wrapper.set_video_texture( texture_white_pixel );

		uint8_c green[ 4 ] = { 0, 255, 0, 255 };
		texture_green_pixel = engine_c::get_instance()->get_video_interface()->create_texture( video_texture_format_e_r8g8b8a8_unorm, 1, 1, 1, 1, green, 4, false, false, false, false );
		texture_green_pixel_wrapper.set_video_texture( texture_green_pixel );

		return true;
	}

	plane64_c video_renderer_quick_sort_plane; // used to sort draw lists based on distance to camera near plane.
	float64_c video_renderer_quick_sort_models( scene_component_model_c * const & a )
	{
		return -ops::distance_between_point_and_plane( a->get_scene_object()->get_world_space_transform().position, video_renderer_quick_sort_plane );
	}
	float64_c video_renderer_quick_sort_sprites( scene_component_sprite_c * const & a )
	{
		return -ops::distance_between_point_and_plane( a->get_scene_object()->get_world_space_transform().position, video_renderer_quick_sort_plane );
	}
	float64_c video_renderer_quick_sort_menu_controls( scene_component_menu_control_c * const & a )
	{
		return -ops::distance_between_point_and_plane( a->get_scene_object()->get_world_space_transform().position, video_renderer_quick_sort_plane );
	}

	void_c video_renderer_interface_c::render_scene_light_probes( scene_c * scene, sint32_c count )
	{
		sint32_c const light_probe_resolution = settings_c::get_light_probe_resolution( _settings.light_probe_resolution );
		float32_c const cube_face_field_of_view = constants< float32_c >::pi() / 2.0f;
		matrix32x4x4_c const cube_face_projection_transform = ops::make_matrix32x4x4_projection_perspective( cube_face_field_of_view, 1.0f, _scene->_light_probe_clip_near, _scene->_light_probe_clip_far );

		// update scene constant buffer.
		_constant_buffers.scene_block->scene_time_counter = 0.0f;
		_constant_buffers.scene_block->scene_wind_direction = vector32x3_c( 0.0f, 0.0f, 0.0f );
		_constant_buffers.scene_block->scene_wind_speed = 0.0f;
		_constant_buffers.scene_block->scene_wind_counter = 0.0f;
		_constant_buffers.scene_block->scene_wind_period_inverse = 0.0f;
		_constant_buffers.scene_block->scene_wind_period_weight_minimum = 0.0f;
		_constant_buffers.scene_block->scene_wind_period_weight_range = 0.0f;
		_constant_buffers.scene_block->scene_directional_light_count = 0;
		_constant_buffers.scene_block->scene_light_probe_mip_count = 0;
		_constant_buffers.scene_block_constant_buffer->set_data( _constant_buffers.scene_block, sizeof( scene_block_c ) );

		// iterate through light probes.
		sint32_c baked_count = 0;
		core_linked_list_c< scene_component_light_probe_c * >::node_c const * light_probe_list_node = _scene->_last_baked_light_probe != nullptr ? &_scene->_last_baked_light_probe->_scene_light_probe_list_node : _scene->_light_probe_list.get_first(); // continue from where we last left off at, or start over.
		while ( ( light_probe_list_node != nullptr ) && ( baked_count < count || count < 0 ) )
		{
			scene_component_light_probe_c * light_probe = light_probe_list_node->get_value();
			if ( light_probe->_is_up_to_date == true )
			{
				continue;
			}

			// initialize targets, which are stored with the light probe.
			// at some point, we should prioritize light probes nearest to the primary view, and have a way to release probes when the primary view moves far away.
			if ( light_probe->_cube_color == nullptr || light_probe->_cube_color->get_width() != light_probe_resolution )
			{
				if ( light_probe->_cube_color != nullptr )
				{
					delete light_probe->_cube_color;
					light_probe->_cube_color = nullptr;
					delete light_probe->_cube_depth_stencil;
					light_probe->_cube_depth_stencil = nullptr;
				}
				light_probe->_cube_color = engine_c::get_instance()->get_video_interface()->create_texture( video_texture_format_e_r16g16b16a16_float, light_probe_resolution, light_probe_resolution, 1, 6, nullptr, 0, false, false, true, true );
				light_probe->_cube_depth_stencil = engine_c::get_instance()->get_video_interface()->create_depth_stencil( video_depth_stencil_format_e_d32, light_probe_resolution, light_probe_resolution, 1, 6 );
			}

			// clear render targets (clears all 6 array slices).
			engine_c::get_instance()->get_video_interface()->clear_depth_stencil( light_probe->_cube_depth_stencil, 1.0f, 0 );
			engine_c::get_instance()->get_video_interface()->clear_texture( light_probe->_cube_color, 0.0f, 0.0f, 0.0f, 0.0f );

			// render each cube face view.
			for ( sint32_c cube_face_view_index = 0; cube_face_view_index < 6; cube_face_view_index++ )
			{
				view_c view; // we only need to set a couple properties on this in order to do what we need.
				view.world_space_position = light_probe->get_scene_object()->get_world_space_transform().position;
				view.world_space_frustum = ops::make_frustum64_perspective3( light_probe->get_scene_object()->get_world_space_transform().position, vector64x3_c( video_renderer_interface_c::cube_face_view_list_base[ cube_face_view_index ].forward ), vector64x3_c( video_renderer_interface_c::cube_face_view_list_base[ cube_face_view_index ].up ), cube_face_field_of_view, cube_face_field_of_view, static_cast< float64_c >( _scene->_light_probe_clip_near ), static_cast< float64_c >( _scene->_light_probe_clip_far ) );

				// gather and prepare data for render.
				_just_do_everything_for_me( view );

				// sort object draw lists from near to far.
				video_renderer_quick_sort_plane = view.world_space_frustum.planes[ frustum_plane_index_e_near ];
				_models_to_render.quick_sort( &video_renderer_quick_sort_models, false );

				// bind shadow map texture.
				textures_to_bind[ 0 ] = _shadow_map_texture2darray;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2darray;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_shadow_map_texture, 1, textures_to_bind, textures_to_bind_types );

				// update camera constants.
				_constant_buffers.camera_block->camera_view_projection_transform = cube_face_view_list_base[ cube_face_view_index ].view_transform * cube_face_projection_transform;
				_constant_buffers.camera_block->camera_clip_plane = vector32x4_c( 0.0f, 0.0f, 0.0f, 0.0f );
				_constant_buffers.camera_block->canvas_actual_size.a = static_cast< float32_c >( light_probe_resolution );
				_constant_buffers.camera_block->canvas_actual_size.b = static_cast< float32_c >( light_probe_resolution );
				_constant_buffers.camera_block->canvas_actual_size_inverse.a = 1.0f / static_cast< float32_c >( light_probe_resolution );
				_constant_buffers.camera_block->canvas_actual_size_inverse.b = 1.0f / static_cast< float32_c >( light_probe_resolution );
				_constant_buffers.camera_block->canvas_apparent_to_actual_ratio.a = 1.0f;
				_constant_buffers.camera_block->canvas_apparent_to_actual_ratio.b = 1.0f;
				_constant_buffers.camera_block->camera_clip_near = _scene->_light_probe_clip_near;
				_constant_buffers.camera_block->camera_clip_far = _scene->_light_probe_clip_far;
				_constant_buffers.camera_block_constant_buffer->set_data( _constant_buffers.camera_block, sizeof( camera_block_c ) );

				// bind render targets (bind 1 array slice for the current cube face).
				textures_to_bind[ 0 ] = light_probe->_cube_color;
				textures_slices_to_bind[ 0 ] = cube_face_view_index;
				engine_c::get_instance()->get_video_interface()->bind_target_texture2darrays_slices( 1, textures_to_bind, textures_slices_to_bind, light_probe->_cube_depth_stencil, cube_face_view_index );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );

				// render sky models for color.
				for ( sint32_c j = 0; j < _scene->sky_models.get_length(); j++ )
				{
					scene_component_model_c * model = _scene->sky_models[ j ];
					if ( model->_render_enable && model->get_model_resource_is_bound() )
					{
						_draw_model_for_sky( model );
					}
				}

				// change depth stencil behavior.
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_less_equal, true );

				// render opaque models for color.
				for ( sint32_c j = 0; j < _models_to_render.get_length(); j++ )
				{
					scene_component_model_c * model = _models_to_render[ j ];
					_draw_model_for_color( model, view.world_space_position, false, true, false, false, true, false );
				}

				// unbind shadow map texture.
				textures_to_bind[ 0 ] = nullptr;
				textures_to_bind_types[ 0 ] = video_texture_type_e_none; // value is irrelevant and not used, but setting anyway.
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_shadow_map_texture, 1, textures_to_bind, textures_to_bind_types );
			}
			light_probe->_cube_color->generate_mips();
			light_probe->_is_up_to_date = true;
			_scene->_last_baked_light_probe = light_probe;
			light_probe_list_node = light_probe_list_node->get_next();
			baked_count++;
		}

		// un-bind resources.
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( 0, 18, nullptr, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none );
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 0, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_index_buffer( nullptr );
	}

	void_c video_renderer_interface_c::render_scene( scene_c * scene, scene_camera_c const * camera, video_renderer_canvas_c * canvas )
	{
		assert( scene != nullptr );
		assert( camera != nullptr );
		assert( canvas != nullptr );

		static sint32_c const pick_limit = 128; // maximum number of things that can be sampled for pixel perfect pick at once. if the pick ray intersects with more than this number, the pick will still function but it may not give the correct or desired result.
		static core_list_c< scene_component_model_c * > pixel_perfect_pick_models;
		static core_list_c< scene_component_sprite_c * > pixel_perfect_pick_sprites;
		pixel_perfect_pick_models.remove_all();
		pixel_perfect_pick_sprites.remove_all();

		// increment scene render frame, which is used to determine when models need to be skin, and so that those models can be rendered more than once per frame without being skinned more than once per frame.
		_scene = scene;
		_scene->_render_frame += 2; // add 2, because it starts count at 1, and because 0 is reserved for when models are added to the scene (and 0 is even while 1 is odd, so this forces us to skin newly added models).

		// initialize additional off-screen textures for the scene if needed.
		if ( _scene->_target_pick_depth == nullptr )
		{
			_scene->_target_pick_depth = engine_c::get_instance()->get_video_interface()->create_depth_stencil( video_depth_stencil_format_e_d32, pick_limit, 1, 1, 1 );
			_scene->_target_pick_depth_readable = engine_c::get_instance()->get_video_interface()->create_texture( video_texture_format_e_r32_float, pick_limit, 1, 1, 1, nullptr, 0, false, true, false, false );
		}

		// add the primary view for the primary camera.
		_views.remove_all();
		view_c * view = _views.emplace_at_end();
		view->canvas = canvas;
		view->camera = camera;
		view->initialize_for_camera( camera, camera->_world_space_transform.position );
		view->clip_plane_enable = false;
		view->clip_plane = plane32_c();
		view->view_port_rectangle[ 0 ] = 0;
		view->view_port_rectangle[ 1 ] = 0;
		view->view_port_rectangle[ 2 ] = view->canvas->get_actual_width();
		view->view_port_rectangle[ 3 ] = view->canvas->get_actual_height();
		view->scissor_rectangle[ 0 ] = 0;
		view->scissor_rectangle[ 1 ] = 0;
		view->scissor_rectangle[ 2 ] = view->canvas->get_actual_width();
		view->scissor_rectangle[ 3 ] = view->canvas->get_actual_height();
		view->mirror_model = nullptr;

		// add additional views for mirrors or planar reflectors.
		//core_list_c< scene_component_c * > gathered_component_list;
		//_current_scene->_octo_tree.gather_items_in_frustum( gathered_component_list, primary_view->real_frustum );
		//for ( sint32_c i = 0; i < gathered_component_list.get_length(); i++ )
		//{
		//	scene_component_c * gathered_component = gathered_component_list[ i ];
		//	if ( gathered_component->get_type_code() == scene_component_model_c::get_type_code_static() )
		//	{
		//		scene_component_model_c * gathered_model = dynamic_cast< scene_component_model_c * >( gathered_component );
		//		if ( views_count < _views_per_frame_count && gathered_model->_meshes_with_mirrors.get_length() )
		//		{
		//			views_count++;
		//			vector64x3_c mirror_normal = ops::make_normalized( gathered_model->_scene_object->_world_transform.c );
		//			vector64x3_c mirror_origin = gathered_model->_scene_object->_world_transform.d;
		//			plane64_c mirror_plane = ops::make_plane_from_normal_and_point( mirror_normal, mirror_origin );
		//			// if the primary view can see the front side of the mirror...
		//			if ( ops::distance_between_point_and_plane( primary_view->real_world_transform.d, mirror_plane ) > 0.0f )
		//			{
		//				view_c * mirror_view = * _views.emplace_at_end();
		//				initialize_mirror_perspective( mirror_plane, gathered_model->_local_bounding_box, gathered_model->_scene_object->_world_transform, mirror_view->scissor_rectangle, mirror_view->real_world_transform, mirror_view->real_view_transform, mirror_view->real_projection_transform, mirror_view->real_frustum );
		//				mirror_view->view_transform = matrix32x4x4_c( mirror_view->view_transform_64 );
		//				mirror_view->view_transform.d.a = 0.0f;
		//				mirror_view->view_transform.d.b = 0.0f;
		//				mirror_view->view_transform.d.c = 0.0f;
		//				mirror_view->projection_transform = matrix32x4x4_c( mirror_view->view_projection_transform );
		//				mirror_view->view_projection_transform = mirror_view->view_transform * mirror_view->projection_transform;
		//				mirror_view->clip_near = camera_near_clip;
		//				mirror_view->clip_far = camera_far_clip;
		//				mirror_view->clip_plane_enable = true;
		//				mirror_view->clip_plane = mirror_plane;
		//				mirror_view->mirror_model = gathered_model;
		//				mirror_view->component_list.remove_all_but_keep_allocated();
		//				mirror_view->component_model_list.remove_all_but_keep_allocated();
		//				mirror_view->component_sprite_list.remove_all_but_keep_allocated();
		//				mirror_view->component_menu_list.remove_all_but_keep_allocated();
		//				mirror_view->outline_component_model_list.remove_all_but_keep_allocated();
		//				mirror_view->outline_component_sprite_list.remove_all_but_keep_allocated();
		//			}
		//		}
		//	}
		//}

		// update scene constant buffer.
		_constant_buffers.scene_block->scene_time_counter = static_cast< float32_c >( _scene->_time_counter );
		_constant_buffers.scene_block->scene_wind_direction = vector32x3_c( 0.0f, 0.0f, 0.0f );
		_constant_buffers.scene_block->scene_wind_speed = 0.0f;
		_constant_buffers.scene_block->scene_wind_counter = 0.0f;
		_constant_buffers.scene_block->scene_wind_period_inverse = 0.0f;
		_constant_buffers.scene_block->scene_wind_period_weight_minimum = 0.0f;
		_constant_buffers.scene_block->scene_wind_period_weight_range = 0.0f;
		_constant_buffers.scene_block->scene_directional_light_count = _scene->_global_lights_list.get_length();
		_constant_buffers.scene_block->scene_light_probe_mip_count = 1.0f;
		for ( sint32_c i = 0; i < scene_colors_count; i++ )
		{
			_constant_buffers.scene_block->scene_colors[ i ] = _scene->colors[ i ];
		}
		_constant_buffers.scene_block_constant_buffer->set_data( _constant_buffers.scene_block, sizeof( scene_block_c ) );

		// render views.
		for ( sint32_c view_index = _views.get_length() - 1; view_index >= 0; view_index-- )
		{
			view_c const & view = _views[ view_index ];

			// gather and prepare data for render.
			_just_do_everything_for_me( view );

			// bind shadow map texture.
			textures_to_bind[ 0 ] = _shadow_map_texture2darray;
			textures_to_bind_types[ 0 ] = video_texture_type_e_texture2darray;
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_shadow_map_texture, 1, textures_to_bind, textures_to_bind_types );

			// sort object draw lists from near to far, which is optimal for rendering opaque geometry with a z-buffer and depth test.
			video_renderer_quick_sort_plane = view.world_space_frustum.planes[ frustum_plane_index_e_near ];
			_models_to_render.quick_sort( &video_renderer_quick_sort_models, false );
			_models_to_render_for_outline.quick_sort( &video_renderer_quick_sort_models, false );
			_sprites_to_render.quick_sort( &video_renderer_quick_sort_sprites, false );
			_sprites_to_render_for_outline.quick_sort( &video_renderer_quick_sort_sprites, false );
			_menu_controls_to_render.quick_sort( &video_renderer_quick_sort_menu_controls, false );

			//// compile menu render procedure for 3d controls.
			//menu_render_procedure.reset();
			//for ( sint32_c i = 0; i < _menu_controls_to_render.get_length(); i++ )
			//{
			//	scene_component_menu_control_c * menu_control = _menu_controls_to_render[ i ];
			//	if ( menu_control->_control != nullptr )
			//	{
			//		menu_render_procedure.add_control( menu_control->_control );
			//	}
			//}

			// do stuff for the primary view.
			if ( view_index == 0 )
			{
				// add debug lines.
				if ( engine_c::get_instance()->get_debug_manager()->get_draw_world_axes() )
				{
					add_debug_axes( true, space_transform_c(), 1.0f, 1.0f );
				}
				if ( engine_c::get_instance()->get_debug_manager()->get_draw_world_grid() )
				{
					vector32x4_c grid_color = engine_c::get_instance()->get_debug_manager()->get_grid_color();
					float32_c grid_dimension = 100.0f;
					float32_c grid_dimension_half = grid_dimension / 2.0f;
					float32_c grid_interval = 5.0f;
					float32_c anchor_a = 0.0f;
					float32_c anchor_b = 0.0f;
					for ( float32_c i = -grid_dimension_half; i <= grid_dimension_half; i += grid_interval )
					{
						add_debug_line( false, vector64x3_c( anchor_a - grid_dimension_half, anchor_b + i, 0.0f ), vector64x3_c( anchor_a + grid_dimension_half, anchor_b + i, 0.0f ), grid_color );
						add_debug_line( false, vector64x3_c( anchor_a + i, anchor_b - grid_dimension_half, 0.0f ), vector64x3_c( anchor_a + i, anchor_b + grid_dimension_half, 0.0f ), grid_color );
					}
				}
				if ( engine_c::get_instance()->get_debug_manager()->get_draw_object_bones() )
				{
					for ( sint32_c model_index = 0; model_index < _models_to_render.get_length(); model_index++ )
					{
						scene_component_model_c * model = _models_to_render[ model_index ];
						for ( sint32_c bone_index = 0; bone_index < model->_bone_list.get_length(); bone_index++ )
						{
							scene_component_model_c::bone_c * bone = &model->_bone_list[ bone_index ];
							vector64x3_c world_space_bone_head = ops::make_vector64x3_transformed_point( vector64x3_c( ops::make_vector32x3_transformed_point( vector32x3_c( bone->_source_bone->head_position ), bone->_object_space_skin_matrix ) ), model->get_scene_object()->get_world_space_transform() );
							vector64x3_c world_space_bone_tail = ops::make_vector64x3_transformed_point( vector64x3_c( ops::make_vector32x3_transformed_point( vector32x3_c( bone->_source_bone->tail_position ), bone->_object_space_skin_matrix ) ), model->get_scene_object()->get_world_space_transform() );
							space_transform_c world_space_bone_transform = bone->_object_space_transform * model->get_scene_object()->get_world_space_transform();
							add_debug_line( true, world_space_bone_head, world_space_bone_tail, engine_c::get_instance()->get_debug_manager()->get_bone_color() );
							add_debug_axes( true, world_space_bone_transform, 0.1f, 0.5f );
						}
					}
				}
				if ( engine_c::get_instance()->get_debug_manager()->get_draw_object_physics() )
				{
					_scene->_physics_scene->render(); // bullet will call our add_debug_line functions.
				}
				//if ( engine_instance.settings.draw_object_bounds || engine_instance.settings.draw_object_axes )
				//{
				//	for ( sint32_c i = 0; i < view->object_list.get_count(); i++ )
				//	{
				//		scene_object_c * object = view->object_list[ i ];
				//		if ( intersect_frustum_vs_box( view->frustum, box64x3_c( object->_bounding_box ), object->_world_transform ) )
				//		{
				//			if ( engine_instance.settings.draw_object_bounds )
				//			{
				//				add_debug_box( false, object->_bounding_box, object->_world_transform, vector32x4_c( 0.5f, 0.5f, 0.5f, 0.5f ) );
				//			}
				//			if ( engine_instance.settings.draw_object_axes )
				//			{
				//				add_debug_axes( false, object->_world_transform, 0.5f );
				//			}
				//		}
				//	}
				//}

				// render geoemtry for pixel perfect picking.
				if ( _scene->_pick_on_next_render )
				{
					assert( view.camera && view.canvas );
					_scene->_pick_on_next_render = false;
					segment64_c pick_segment = view.camera->build_segment_through_canvas( _scene->_pick_canvas_coordinates, view.canvas->get_actual_width(), view.canvas->get_actual_height() );
					vector32x3_c pick_ray_direction = vector32x3_c( ops::make_vector64x3_normalized( pick_segment.point_b - pick_segment.point_a ) );
					_scene->_gather_pickables_along_segment( pick_segment, pixel_perfect_pick_models, pixel_perfect_pick_sprites ); // this will populate _current_scene->_pick_list_original.
					if ( _scene->_pick_list_original.get_length() )
					{
						engine_c::get_instance()->get_video_interface()->clear_depth_stencil( _scene->_target_pick_depth, 1.0f, 0 );

						matrix32x4x4_c pick_view_transform = ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( 0.0f, 0.0f, 0.0f ), pick_ray_direction, view.world_space_basis.c );
						matrix32x4x4_c pick_projection_transform = ops::make_matrix32x4x4_projection_orthographic( 0.001f, 0.001f, view.camera_clip_near, view.camera_clip_far ); // a 1mm x 1mm ray.
						matrix32x4x4_c pick_view_projection_transform = pick_view_transform * pick_projection_transform;

						_constant_buffers.camera_block->camera_view_projection_transform = pick_view_projection_transform;
						_constant_buffers.camera_block_constant_buffer->set_data( _constant_buffers.camera_block, sizeof( camera_block_c ) );

						engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, _scene->_target_pick_depth, video_texture_type_e_texture2d );
						engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
						engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_less_equal, true );
						engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( video_blend_type_e_set );
						engine_c::get_instance()->get_video_interface()->bind_pixel_shader( nullptr );

						for ( sint32_c i = 0; i < _scene->_pick_list_original.get_length(); i++ )
						{
							scene_pick_c * pick = &_scene->_pick_list_original[ i ];
							engine_c::get_instance()->get_video_interface()->bind_rasterizer_view_port_rectangle( i, 0, 1, 1, 0.0f, 1.0f );
							if ( pick->component->get_type_code() == scene_component_model_c::get_type_code_static() )
							{
								_draw_model_with_no_material( dynamic_cast< scene_component_model_c * >( pick->component ), view.world_space_position );
							}
							else if ( pick->component->get_type_code() == scene_component_sprite_c::get_type_code_static() )
							{
								scene_component_sprite_c * sprite = dynamic_cast< scene_component_sprite_c * >( pick->component );
								//textures_to_bind[ 0 ] = sprite->_material.textures[ 0 ]->_video_texture; // we could do alpha-masking but eh.
								//textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
								//engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( 0, 1, textures_to_bind, textures_to_bind_types );
								engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
								vertex_buffers_to_bind[ 0 ] = _sprite_vertex_buffer;
								engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
								engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_strip );
								engine_c::get_instance()->get_video_interface()->bind_vertex_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_color_vs_mesh() );
								engine_c::get_instance()->get_video_interface()->draw( sprite->_renderer_vertex_start, 4 );
							}
						}

						engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none );

						// read back results and build final pick list.
						// direct3d documentation recommends waiting for several frames before attempting to map a buffer (read from a texture), otherwise the pipeline might stall.
						// for now, to remain straight forward, we'll just retreive the results now.
						engine_c::get_instance()->get_video_interface()->copy_sub_resource( _scene->_target_pick_depth_readable, 0, _scene->_target_pick_depth, 0 );
						float32_c pick_depth_array[ pick_limit ];
						_scene->_target_pick_depth_readable->get_data( pick_depth_array, pick_limit * sizeof( float32_c ) );
						for ( sint32_c i = 0; i < _scene->_pick_list_original.get_length(); i++ )
						{
							scene_pick_c * pick = &_scene->_pick_list_original[ i ];
							pick->depth = pick_depth_array[ i ];
							if ( pick->depth < 1.0f )
							{
								_scene->_pick_list_final.insert_at_end( *pick );
							}
						}
						_scene->_pick_list_final.quick_sort( &scene_pick_c::quick_sort_function, false );
					}
				}
			}

			// update camera constants.
			_constant_buffers.camera_block->camera_view_projection_transform = view.camera_view_projection_transform;
			_constant_buffers.camera_block->camera_clip_plane = view.clip_plane.as_vector32x4();
			_constant_buffers.camera_block->canvas_actual_size.a = static_cast< float32_c >( view.canvas->get_actual_width() );
			_constant_buffers.camera_block->canvas_actual_size.b = static_cast< float32_c >( view.canvas->get_actual_height() );
			_constant_buffers.camera_block->canvas_actual_size_inverse.a = 1.0f / static_cast< float32_c >( view.canvas->get_actual_width() );
			_constant_buffers.camera_block->canvas_actual_size_inverse.b = 1.0f / static_cast< float32_c >( view.canvas->get_actual_height() );
			_constant_buffers.camera_block->canvas_apparent_to_actual_ratio.a = static_cast< float32_c >( view.canvas->get_apparent_width() ) / static_cast< float32_c >( view.canvas->get_actual_width() );
			_constant_buffers.camera_block->canvas_apparent_to_actual_ratio.b = static_cast< float32_c >( view.canvas->get_apparent_height() ) / static_cast< float32_c >( view.canvas->get_actual_height() );
			_constant_buffers.camera_block->camera_clip_near = view.camera_clip_near;
			_constant_buffers.camera_block->camera_clip_far = view.camera_clip_far;
			_constant_buffers.camera_block_constant_buffer->set_data( _constant_buffers.camera_block, sizeof( camera_block_c ) );

			
			//
			//
			//
			//
			//
			//
			////
			/////
			///////
			//////////
			//////////////
			/////////////////// normal, depth, and early z pre-pass of opaque models and sprites.
			textures_to_bind[ 0 ] = view.canvas->_target_normal;
			textures_to_bind[ 1 ] = view.canvas->_target_depth;
			engine_c::get_instance()->get_video_interface()->bind_target_textures( 2, textures_to_bind, view.canvas->_target_depth_stencil, video_texture_type_e_texture2d );
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_less_equal, true );
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( video_blend_type_e_set );
			for ( sint32_c model_index = 0; model_index < _models_to_render.get_length(); model_index++ )
			{
				scene_component_model_c * model = _models_to_render[ model_index ];
				_draw_model_for_normal_and_depth( model, view.world_space_position, view.clip_plane_enable );
			}
			if ( _sprites_to_render.get_length() > 0 )
			{
				vertex_buffers_to_bind[ 0 ] = _sprite_vertex_buffer;
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_strip );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
				for ( sint32_c sprite_index = 0; sprite_index < _sprites_to_render.get_length(); sprite_index++ )
				{
					scene_component_sprite_c * sprite = _sprites_to_render[ sprite_index ];
					if ( sprite->_material.blend_type == video_blend_type_e_set )
					{
						_bind_mesh_vs_for_normal_and_depth( sprite->_material.is_waved, view.clip_plane_enable );
						if ( sprite->_material.is_masked )
						{
							textures_to_bind[ 0 ] = sprite->_material.textures[ 0 ].is_reference_set_and_loaded() ? sprite->_material.textures[ 0 ]->get_video_texture() : nullptr;
							textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
							engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
							engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_ps_mesh_masked() );
						}
						else
						{
							engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_ps_mesh_masked() );
						}
						engine_c::get_instance()->get_video_interface()->draw( sprite->_renderer_vertex_start, 4 );
					}
				}
			}
			///////////////////
			//////////////
			//////////
			///////
			/////
			////
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			////
			/////
			///////
			//////////
			//////////////
			/////////////////// outline pass of models and sprites.
			if ( _models_to_render_for_outline.get_length() > 0 || _sprites_to_render_for_outline.get_length() > 0 )
			{
				textures_to_bind[ 0 ] = view.canvas->_target_outline;
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, view.canvas->_target_depth_stencil, video_texture_type_e_texture2d );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_equal, false );
				for ( sint32_c model_index = 0; model_index < _models_to_render_for_outline.get_length(); model_index++ )
				{
					scene_component_model_c * model = _models_to_render_for_outline[ model_index ];
					_draw_model_for_outline( model, view.world_space_position, view.clip_plane_enable );
					view.canvas->_statistic_object_count += 1;
				}
				if ( _sprites_to_render_for_outline.get_length() > 0 )
				{
					vertex_buffers_to_bind[ 0 ] = _sprite_vertex_buffer;
					engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
					engine_c::get_instance()->get_video_interface()->bind_index_buffer( nullptr );
					engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_strip );
					engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
					for ( sint32_c sprite_index = 0; sprite_index < _sprites_to_render_for_outline.get_length(); sprite_index++ )
					{
						scene_component_sprite_c * sprite = _sprites_to_render_for_outline[ sprite_index ];
						if ( sprite->_material.blend_type == video_blend_type_e_set )
						{
							_bind_mesh_vs_for_color( sprite->_material.is_waved, view.clip_plane_enable );
							if ( !sprite->_material.is_masked )
							{
								engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_outline_ps_mesh() );
							}
							else
							{
								textures_to_bind[ 0 ] = sprite->_material.textures[ 0 ].is_reference_set_and_loaded() ? sprite->_material.textures[ 0 ]->get_video_texture() : texture_green_pixel;
								textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
								engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( 0, 1, textures_to_bind, textures_to_bind_types );
								engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_outline_ps_mesh_masked() );
							}
							engine_c::get_instance()->get_video_interface()->draw( sprite->_renderer_vertex_start, 4 );
							view.canvas->_statistic_object_count += 1;
							view.canvas->_statistic_triangle_count += 2;
						}
					}
				}
			}
			///////////////////
			//////////////
			//////////
			///////
			/////
			////
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			////
			/////
			///////
			//////////
			//////////////
			/////////////////// color pass of sky models.
			if ( _scene->sky_models.get_length() )
			{
				textures_to_bind[ 0 ] = view.canvas->_target_color;
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_less_equal, false );
				for ( sint32_c i = 0; i < _scene->sky_models.get_length(); i++ )
				{
					scene_component_model_c * model = _scene->sky_models[ i ];
					if ( model->_render_enable && model->get_model_resource_is_bound() )
					{
						_draw_model_for_sky( model );
						view.canvas->_statistic_object_count += 1;
					}
				}
			}
			///////////////////
			//////////////
			//////////
			///////
			/////
			////
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			////
			/////
			///////
			//////////
			//////////////
			/////////////////// color pass of opaque models and sprites.
			textures_to_bind[ 0 ] = view.canvas->_target_color;
			engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, view.canvas->_target_depth_stencil, video_texture_type_e_texture2d );
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_equal, false );
			for ( sint32_c model_index = 0; model_index < _models_to_render.get_length(); model_index++ )
			{
				scene_component_model_c * model = _models_to_render[ model_index ];
				_draw_model_for_color( model, view.world_space_position, view.clip_plane_enable, true, false, false, true, true );
				view.canvas->_statistic_object_count += 1;
			}
			if ( _sprites_to_render.get_length() > 0 )
			{
				vertex_buffers_to_bind[ 0 ] = _sprite_vertex_buffer;
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
				engine_c::get_instance()->get_video_interface()->bind_index_buffer( nullptr );
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_strip );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
				for ( sint32_c sprite_index = 0; sprite_index < _sprites_to_render.get_length(); sprite_index++ )
				{
					scene_component_sprite_c * sprite = _sprites_to_render[ sprite_index ];
					if ( sprite->_material.blend_type == video_blend_type_e_set )
					{
						_bind_mesh_vs_for_color( sprite->_material.is_waved, view.clip_plane_enable );
						_bind_material_for_color( &sprite->_material );
						engine_c::get_instance()->get_video_interface()->draw( sprite->_renderer_vertex_start, 4 );
						view.canvas->_statistic_object_count += 1;
						view.canvas->_statistic_triangle_count += 2;
					}
				}
			}
			///////////////////
			//////////////
			//////////
			///////
			/////
			////
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			////
			/////
			///////
			//////////
			//////////////
			/////////////////// color pass of transparent models and sprites.
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_less_equal, false );
			for ( sint32_c model_index = 0; model_index < _models_to_render.get_length(); model_index++ )
			{
				scene_component_model_c * model = _models_to_render[ model_index ];
				_draw_model_for_color( model, view.world_space_position, view.clip_plane_enable, false, true, false, true, true );
				view.canvas->_statistic_object_count += 1;
			}
			if ( _sprites_to_render.get_length() > 0 )
			{
				vertex_buffers_to_bind[ 0 ] = _sprite_vertex_buffer;
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
				engine_c::get_instance()->get_video_interface()->bind_index_buffer( nullptr );
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_strip );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
				for ( sint32_c sprite_index = 0; sprite_index < _sprites_to_render.get_length(); sprite_index++ )
				{
					scene_component_sprite_c * sprite = _sprites_to_render[ sprite_index ];
					if ( sprite->_material.blend_type != video_blend_type_e_set )
					{
						_bind_mesh_vs_for_color( sprite->_material.is_waved, view.clip_plane_enable );
						_bind_material_for_color( &sprite->_material );
						engine_c::get_instance()->get_video_interface()->draw( sprite->_renderer_vertex_start, 4 );
						view.canvas->_statistic_object_count += 1;
						view.canvas->_statistic_triangle_count += 2;
					}
				}
			}
			if ( view_index == 0 )
			{
				// render 3d menu.
			}
			///////////////////
			//////////////
			//////////
			///////
			/////
			////
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			////
			/////
			///////
			//////////
			//////////////
			/////////////////// color overlay pass of models and sprites. (overlay pass allows a copy of the color target to be used as input, which enables effects like refraction to be done in the pixel shader).
			_resolve_texture_full( view.canvas->_target_color, view.canvas->_target_color_copy );
			textures_to_bind[ 0 ] = view.canvas->_target_outline;
			textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
			textures_to_bind[ 1 ] = view.canvas->_target_normal;
			textures_to_bind_types[ 1 ] = video_texture_type_e_texture2d;
			textures_to_bind[ 2 ] = view.canvas->_target_depth;
			textures_to_bind_types[ 2 ] = video_texture_type_e_texture2d;
			textures_to_bind[ 3 ] = view.canvas->_target_color_copy;
			textures_to_bind_types[ 3 ] = video_texture_type_e_texture2d;
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_target_textures, 4, textures_to_bind, textures_to_bind_types );
			textures_to_bind[ 0 ] = view.canvas->_target_color;
			engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, view.canvas->_target_depth_stencil, video_texture_type_e_texture2d );
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_less_equal, false );
			for ( sint32_c model_index = 0; model_index < _models_to_render.get_length(); model_index++ )
			{
				scene_component_model_c * model = _models_to_render[ model_index ];
				_draw_model_for_color( model, view.world_space_position, view.clip_plane_enable, false, false, true, true, true );
				view.canvas->_statistic_object_count += 1;
			}
			///////////////////
			//////////////
			//////////
			///////
			/////
			////
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			//
			////
			/////
			///////
			//////////
			//////////////
			/////////////////// post process (only for the primary view).
			if ( view_index == 0 )
			{
				// render debug line list.
				// we only do this for the primary view.
				// copy and translate debug vertices from real world space to relative to the primary camera view.

				assert( sizeof( video_renderer_vertex_debug_c ) == 28 );

				vector64x3_c camera_position = view.world_space_position;
				sint32_c debug_vertex_count = _debug_line_list_64.get_length();
				sint32_c debug_vertex_xray_count = _debug_line_list_xray_64.get_length();
				_debug_line_list.construct_mode_dynamic( debug_vertex_count + debug_vertex_xray_count );
				for ( sint32_c i = 0; i < _debug_line_list_64.get_length(); i++ )
				{
					video_renderer_vertex_debug_64_c & in = _debug_line_list_64[ i ];
					video_renderer_vertex_debug_c & out = _debug_line_list[ i ];
					out.position = vector32x3_c( in.position - camera_position );
					out.color = in.color;
				}
				for ( sint32_c i = 0; i < _debug_line_list_xray_64.get_length(); i++ )
				{
					video_renderer_vertex_debug_64_c & in = _debug_line_list_xray_64[ i ];
					video_renderer_vertex_debug_c & out = _debug_line_list[ debug_vertex_count + i ];
					out.position = vector32x3_c( in.position - camera_position );
					out.color = in.color;
				}
				_debug_line_list_64.remove_all();
				_debug_line_list_xray_64.remove_all();
				if ( _debug_line_list.get_length() )
				{
					// upload the line list to the GPU.
					if ( !_debug_line_list_vertex_buffer || _debug_line_list.get_length() > _debug_line_list_vertex_buffer->get_vertex_count() )
					{
						if ( _debug_line_list_vertex_buffer )
						{
							delete _debug_line_list_vertex_buffer;
							_debug_line_list_vertex_buffer = nullptr;
						}
						_debug_line_list_vertex_buffer = engine_c::get_instance()->get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_debug, _debug_line_list.get_length_allocated(), 0, 0, true, false, false );
					}
					_debug_line_list_vertex_buffer->set_data( _debug_line_list.get_internal_array(), _debug_line_list.get_internal_array_size_used() );
					_debug_line_list.remove_all();

					// draw the line list.
					engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
					engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( debug_line_blend_type );
					engine_c::get_instance()->get_video_interface()->bind_vertex_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_color_vs_debug() );
					engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_color_ps_debug() );
					vertex_buffers_to_bind[ 0 ] = _debug_line_list_vertex_buffer;
					engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
					engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_line_list );
					engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_less_equal, false );
					engine_c::get_instance()->get_video_interface()->draw( 0, debug_vertex_count );
					engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );
					engine_c::get_instance()->get_video_interface()->draw( debug_vertex_count, debug_vertex_xray_count );
				}

				// something to think about ...
				// blurs can be accumulated over more than one frame, which allows for more extreme blur without much more cost.

				// down scale/sample the _target_color to _target_color_half.
				// this also has the side effect of binding the state needed for doing post process.
				//_resolve_texture_quarter( view.canvas->_target_color, view.canvas->_target_color_quarter );

				// half blur.
				// prepare render states.
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( video_blend_type_e_set );
				engine_c::get_instance()->get_video_interface()->bind_vertex_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_vs() );
				vertex_buffers_to_bind[ 0 ] = _quad_vertex_buffer;
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_strip );
				textures_to_bind[ 0 ] = view.canvas->_target_color;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				// prepare constants.
				_constant_buffers.camera_block->canvas_actual_size.a = static_cast< float32_c >( view.canvas->get_actual_width() / 2 );
				_constant_buffers.camera_block->canvas_actual_size.b = static_cast< float32_c >( view.canvas->get_actual_height() / 2 );
				_constant_buffers.camera_block->canvas_actual_size_inverse.a = 1.0f / _constant_buffers.camera_block->canvas_actual_size.a;
				_constant_buffers.camera_block->canvas_actual_size_inverse.b = 1.0f / _constant_buffers.camera_block->canvas_actual_size.b;
				_constant_buffers.camera_block_constant_buffer->set_data( _constant_buffers.camera_block, sizeof( camera_block_c ) );
				// render blur x.
				textures_to_bind[ 0 ] = view.canvas->_target_color_half_blurred_x;
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_blur_x() );
				textures_to_bind[ 0 ] = view.canvas->_target_color;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				engine_c::get_instance()->get_video_interface()->draw( 0, 4 );
				// render blur xy.
				textures_to_bind[ 0 ] = view.canvas->_target_color_half_blurred_xy;
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_blur_y() );
				textures_to_bind[ 0 ] = view.canvas->_target_color_half_blurred_x;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				engine_c::get_instance()->get_video_interface()->draw( 0, 4 );

				// quarter blur.
				// prepare constants.
				_constant_buffers.camera_block->canvas_actual_size.a = static_cast< float32_c >( view.canvas->get_actual_width() / 4 );
				_constant_buffers.camera_block->canvas_actual_size.b = static_cast< float32_c >( view.canvas->get_actual_height() / 4 );
				_constant_buffers.camera_block->canvas_actual_size_inverse.a = 1.0f / _constant_buffers.camera_block->canvas_actual_size.a;
				_constant_buffers.camera_block->canvas_actual_size_inverse.b = 1.0f / _constant_buffers.camera_block->canvas_actual_size.b;
				_constant_buffers.camera_block_constant_buffer->set_data( _constant_buffers.camera_block, sizeof( camera_block_c ) );
				// render blur x.
				textures_to_bind[ 0 ] = view.canvas->_target_color_quarter_blurred_x;
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_blur_x() );
				textures_to_bind[ 0 ] = view.canvas->_target_color_half_blurred_xy;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				engine_c::get_instance()->get_video_interface()->draw( 0, 4 );
				// render blur xy.
				textures_to_bind[ 0 ] = view.canvas->_target_color_quarter_blurred_xy;
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_blur_y() );
				textures_to_bind[ 0 ] = view.canvas->_target_color_quarter_blurred_x;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				engine_c::get_instance()->get_video_interface()->draw( 0, 4 );

				// eighth blur.
				// prepare constants.
				_constant_buffers.camera_block->canvas_actual_size.a = static_cast< float32_c >( view.canvas->get_actual_width() / 8 );
				_constant_buffers.camera_block->canvas_actual_size.b = static_cast< float32_c >( view.canvas->get_actual_height() / 8 );
				_constant_buffers.camera_block->canvas_actual_size_inverse.a = 1.0f / _constant_buffers.camera_block->canvas_actual_size.a;
				_constant_buffers.camera_block->canvas_actual_size_inverse.b = 1.0f / _constant_buffers.camera_block->canvas_actual_size.b;
				_constant_buffers.camera_block_constant_buffer->set_data( _constant_buffers.camera_block, sizeof( camera_block_c ) );
				// render blur x.
				textures_to_bind[ 0 ] = view.canvas->_target_color_eighth_blurred_x;
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_blur_x() );
				textures_to_bind[ 0 ] = view.canvas->_target_color_quarter_blurred_xy;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				engine_c::get_instance()->get_video_interface()->draw( 0, 4 );
				// render blur xy.
				textures_to_bind[ 0 ] = view.canvas->_target_color_eighth_blurred_xy;
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_blur_y() );
				textures_to_bind[ 0 ] = view.canvas->_target_color_eighth_blurred_x;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				engine_c::get_instance()->get_video_interface()->draw( 0, 4 );

				// unbind.
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, nullptr, nullptr );
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none );

				// render post process to final output target.
				video_texture_c * output_target = view.canvas->_output != nullptr ? view.canvas->_output->get_texture_resource() : view.canvas->_target_color_final;
				assert( output_target );
				textures_to_bind[ 0 ] = view.canvas->_target_outline;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				textures_to_bind[ 1 ] = view.canvas->_target_normal;
				textures_to_bind_types[ 1 ] = video_texture_type_e_texture2d;
				textures_to_bind[ 2 ] = view.canvas->_target_depth;
				textures_to_bind_types[ 2 ] = video_texture_type_e_texture2d;
				textures_to_bind[ 3 ] = view.canvas->_target_color;
				textures_to_bind_types[ 3 ] = video_texture_type_e_texture2d;
				textures_to_bind[ 4 ] = view.canvas->_target_color_half_blurred_xy;
				textures_to_bind_types[ 4 ] = video_texture_type_e_texture2d;
				textures_to_bind[ 5 ] = view.canvas->_target_color_quarter_blurred_xy;
				textures_to_bind_types[ 5 ] = video_texture_type_e_texture2d;
				textures_to_bind[ 6 ] = view.canvas->_target_color_eighth_blurred_xy;
				textures_to_bind_types[ 6 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_target_textures, 7, textures_to_bind, textures_to_bind_types );
				textures_to_bind[ 0 ] = output_target;
				engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_process() );
				engine_c::get_instance()->get_video_interface()->draw( 0, 4 );
			}
			///////////////////
			//////////////
			//////////
			///////
			/////
			////
			//
			//
		}

		// clear bindings (could be optimized).
		engine_c::get_instance()->get_video_interface()->bind_index_buffer( nullptr );
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 0, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( 0, 18, nullptr, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none );
		_scene = nullptr;
	}

	/*
	void_c video_renderer_interface_c::render_menu( menu_render_procedure_c * menu_render_procedure, video_renderer_canvas_c * canvas )
	{
		assert( menu_render_procedure != nullptr );
		assert( canvas != nullptr );
		assert( ( menu_render_procedure->_is_3d && _views.get_length() > 0 ) || ( !menu_render_procedure->_is_3d && _views.get_length() == 0 ) );

		// if _view_list.get_length() == 0 then it means that we are being called by the game to render the 2d menus, so we need to bind targets.
		// if _view_list.get_length() > 0 then it means that we were called by render_scene() and we are rendering a 3d menu component, so we don't need to bind targets.

		// update menu constant buffer.
		view_c * primary_view = nullptr;
		if ( !menu_render_procedure->_is_3d )
		{
			_constant_buffers.menu_block->menu_view_projection_transform = ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( static_cast< float32_c >( canvas->get_apparent_width() ) * 0.5f, static_cast< float32_c >( canvas->get_apparent_height() ) * 0.5f, -1.0f ), vector32x3_c( 0.0f, 0.0f, 1.0f ), vector32x3_c( 0.0f, -1.0f, 0.0f ) ) * ops::make_matrix32x4x4_projection_orthographic( static_cast< float32_c >( canvas->get_apparent_width() ), static_cast< float32_c >( canvas->get_apparent_height() ), 0.1f, 10.0f );
		}
		else
		{
			_constant_buffers.menu_block->menu_view_projection_transform = _constant_buffers.camera_block->camera_view_projection_transform;
			primary_view = &_views[ 0 ];
		}
		_constant_buffers.menu_block_constant_buffer->set_data( _constant_buffers.menu_block, sizeof( menu_block_c ) );

		// bind output targets if needed.
		if ( !menu_render_procedure->_is_3d )
		{
			video_texture_c * output_target = canvas->_output != nullptr ? canvas->_output->get_texture_resource() : canvas->_target_color_final;
			assert( output_target );

			textures_to_bind[ 0 ] = output_target;
			engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );

			textures_to_bind[ 0 ] = canvas->_target_outline;
			textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
			textures_to_bind[ 1 ] = canvas->_target_normal;
			textures_to_bind_types[ 1 ] = video_texture_type_e_texture2d;
			textures_to_bind[ 2 ] = canvas->_target_depth;
			textures_to_bind_types[ 2 ] = video_texture_type_e_texture2d;
			textures_to_bind[ 3 ] = canvas->_target_color;
			textures_to_bind_types[ 3 ] = video_texture_type_e_texture2d;
			textures_to_bind[ 4 ] = canvas->_target_color_quarter;
			textures_to_bind_types[ 4 ] = video_texture_type_e_texture2d;
			textures_to_bind[ 5 ] = canvas->_target_color_quarter_blurred_xy;
			textures_to_bind_types[ 5 ] = video_texture_type_e_texture2d;
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_target_textures, 6, textures_to_bind, textures_to_bind_types );
		}

		for ( sint32_c layer_index = 0; layer_index < menu_layer_e_count_; layer_index++ )
		{
			menu_render_procedure_c::layer_c * layer = &menu_render_procedure->layers[ layer_index ];
			if ( layer->_vertex_list.get_length() && layer->_index_list.get_length() )
			{
				// upload vertices to vertex buffer.
				if ( _menu_vertex_buffer == nullptr || layer->_vertex_list.get_length() > _menu_vertex_buffer->get_vertex_count() )
				{
					if ( _menu_vertex_buffer != nullptr )
					{
						delete _menu_vertex_buffer;
						_menu_vertex_buffer = nullptr;
					}
					_menu_vertex_buffer = engine_c::get_instance()->get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_menu, layer->_vertex_list.get_internal_array_length_allocated(), nullptr, 0, true, false, false );
				}
				_menu_vertex_buffer->set_data( layer->_vertex_list.get_internal_array(), layer->_vertex_list.get_internal_array_size() );

				// upload indices to index buffer.
				if ( _menu_index_buffer == nullptr || layer->_index_list.get_length() > _menu_index_buffer->get_index_count() )
				{
					if ( _menu_index_buffer != nullptr )
					{
						delete _menu_index_buffer;
						_menu_index_buffer = nullptr;
					}
					_menu_index_buffer = engine_c::get_instance()->get_video_interface()->create_index_buffer( video_index_format_e_uint16, layer->_index_list.get_internal_array_length_allocated(), nullptr, 0, true, false );
				}
				_menu_index_buffer->set_data( layer->_index_list.get_internal_array(), layer->_index_list.get_internal_array_size() );

				// bind glyph map texture.
				textures_to_bind[ 0 ] = global_engine_instance.interfaces.glyph_manager->_glyph_atlas_texture;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2darray;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_glyph_atlas_texture, 1, textures_to_bind, textures_to_bind_types );

				// render batches.
				// if _view_list.get_length() != 0 then we are being rendered into a 3d scene.
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( video_blend_type_e_mix );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
				engine_c::get_instance()->get_video_interface()->bind_vertex_shader( _views.get_length() == 0 ? global_engine_instance.interfaces.video_renderer_shader_manager->menu2_vs : global_engine_instance.interfaces.video_renderer_shader_manager->menu3_vs );
				vertex_buffers_to_bind[ 0 ] = _menu_vertex_buffer;
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
				engine_c::get_instance()->get_video_interface()->bind_index_buffer( _menu_index_buffer );
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_list );
				for ( sint32_c i = 0; i < layer->_draw_batch_list.get_length(); i++ )
				{
					menu_render_procedure_c::layer_c::menu_draw_batch_c * draw_batch = &layer->_draw_batch_list[ i ];
					if ( _views.get_length() != 0 )
					{
						_constant_buffers.menu_batch_block->menu_world_transform = ops::make_matrix32x4x4_from_space_transform( draw_batch->world_space_transform, primary_view->world_space_position );
					}
					_constant_buffers.menu_batch_block->menu_basis.a = draw_batch->basis.a.a;
					_constant_buffers.menu_batch_block->menu_basis.b = draw_batch->basis.a.b;
					_constant_buffers.menu_batch_block->menu_basis.c = draw_batch->basis.b.a;
					_constant_buffers.menu_batch_block->menu_basis.d = draw_batch->basis.b.b;
					_constant_buffers.menu_batch_block->menu_origin = draw_batch->origin;
					_constant_buffers.menu_batch_block->menu_saturation = draw_batch->saturation;
					_constant_buffers.menu_batch_block->menu_color = draw_batch->color;
					_constant_buffers.menu_batch_block->menu_clip_plane_stack_length = draw_batch->clip_plane_stack_length;
					for ( sint32_c i = 0; i < video_renderer_interface_c::menu_clip_plane_stack_count; i++ )
					{
						_constant_buffers.menu_batch_block->menu_clip_plane_stack[ i ] = draw_batch->clip_plane_stack[ i ];
					}
					_constant_buffers.menu_batch_block_constant_buffer->set_data( _constant_buffers.menu_batch_block, sizeof( menu_batch_block_c ) );

					uint32_c draw_end = draw_batch->draw_start + draw_batch->draw_count;
					for ( uint32_c j = draw_batch->draw_start; j < draw_end; j++ )
					{
						menu_render_procedure_c::layer_c::menu_draw_c * draw = &layer->_draw_list[ j ];
						engine_c::get_instance()->get_video_interface()->bind_pixel_shader( draw->pixel_shader );
						textures_to_bind[ 0 ] = draw->texture != nullptr ? draw->texture->_video_texture : nullptr;
						textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
						engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( 0, 1, textures_to_bind, textures_to_bind_types );
						engine_c::get_instance()->get_video_interface()->draw_indexed( draw->index_start, draw->index_count );
					}
				}
			}

			// render debug line list.
			if ( layer->_debug_draw_list.get_length() )
			{
				// upload line list.
				if ( _menu_debug_vertex_buffer == nullptr || layer->_debug_vertex_list.get_length() > _menu_debug_vertex_buffer->get_vertex_count() )
				{
					if ( _menu_debug_vertex_buffer != nullptr )
					{
						delete _menu_debug_vertex_buffer;
						_menu_debug_vertex_buffer = nullptr;
					}
					_menu_debug_vertex_buffer = engine_c::get_instance()->get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_debug, layer->_debug_vertex_list.get_internal_array_length_allocated(), nullptr, 0, true, false, false );
				}
				_menu_debug_vertex_buffer->set_data( layer->_debug_vertex_list.get_internal_array(), layer->_debug_vertex_list.get_internal_array_size() );

				// draw primitives
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
				engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( debug_line_blend_type );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( global_engine_instance.interfaces.video_renderer_shader_manager->menu_ps_debug );
				for ( sint32_c i = 0; i < layer->_debug_draw_list.get_length(); i++ )
				{
					menu_render_procedure_c::layer_c::menu_debug_draw_c * debug_draw = &layer->_debug_draw_list[ i ];
					if ( debug_draw->is_3d )
					{
						_constant_buffers.menu_batch_block->menu_world_transform = ops::make_matrix32x4x4_from_space_transform( debug_draw->world_space_transform, primary_view->world_space_position );
						_constant_buffers.menu_batch_block_constant_buffer->set_data( _constant_buffers.menu_batch_block, sizeof( menu_batch_block_c ) );
						engine_c::get_instance()->get_video_interface()->bind_vertex_shader( global_engine_instance.interfaces.video_renderer_shader_manager->menu3_vs_debug );
					}
					else
					{
						engine_c::get_instance()->get_video_interface()->bind_vertex_shader( global_engine_instance.interfaces.video_renderer_shader_manager->menu2_vs_debug );
					}
					vertex_buffers_to_bind[ 0 ] = _menu_debug_vertex_buffer;
					engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
					for ( sint32_c i = 0; i < layer->_debug_draw_list.get_length(); i++ )
					{
						menu_render_procedure_c::layer_c::menu_debug_draw_c const * debug_draw = &layer->_debug_draw_list[ i ];
						engine_c::get_instance()->get_video_interface()->bind_primitive_type( debug_draw->primitive_type );
						engine_c::get_instance()->get_video_interface()->draw( debug_draw->vertex_start, debug_draw->vertex_count );
					}
				}
			}
		}

		// un-bind resources.
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_glyph_atlas_texture, 1, nullptr, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( 0, 18, nullptr, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 0, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_index_buffer( nullptr );
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader( nullptr );
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader( nullptr );
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none );
	}
	*/

	void_c video_renderer_interface_c::_add_shadow_views( scene_light_c * const light, view_c const & camera_view )
	{
		static core_list_c< scene_component_c * > gathered_components;

		assert( light->_shadow_cast_enable == true );

		vector64x3_c view_position = camera_view.world_space_position;
		sint32_c shadow_view_start = _shadow_views.get_length();
		sint32_c shadow_view_end = 0;
		sint32_c shadow_view_limit = settings_c::get_shadow_view_count( _settings.shadow_view_count );

		if ( light->get_type() == scene_light_type_e_direction )
		{
			if ( _shadow_views.get_length() + 4 > shadow_view_limit )
			{
				// no more room for more shadow views.
				return;
			}

			shadow_view_end = shadow_view_start + 4;
			light->_shadow_view_start = _shadow_views.get_length();
			light->_shadow_view_count = 4;

			vector32x3_c light_forward = light->get_world_space_transform().get_unscaled_basis_b();
			vector32x3_c light_up = -light->get_world_space_transform().get_unscaled_basis_c();

			// figure out the size in meters of each shadow slice.
			float32_c first_cascade_size = 5.0f;

			// figure out the placement of each slice in the scene.
			for ( sint32_c i = 0; i < 4; i++ )
			{
				float32_c cascade_scale = ops::math_power( i + 1.0f, 1.5f );
				float32_c slice_width_and_height = first_cascade_size * cascade_scale;
				float32_c slice_depth = 1000.0f * cascade_scale;
				vector64x3_c slice_origin = view_position;

				// build initial view matrix that isn't clamped, but will help us to figure out how to build one that is clamped
				vector32x3_c view_up = vector32x3_c( 0.0f, 0.0f, 1.0f );
				if ( ops::math_absolute_value( ops::make_float32_dot_product( light_forward, view_up ) ) > 0.9f )
				{
					view_up = vector32x3_c( 0.0f, 1.0f, 0.0f ); // choose a different up since shadow caster is pointing up or down
				}

				// clamp cascade to world texel increments so shadow doesn't flicker as the primary camera moves.
				sint32_c shadow_resolution = settings_c::get_shadow_resolution( _settings.shadow_resolution );
				float64_c world_space_texel_size = slice_width_and_height / ( static_cast< float32_c >( shadow_resolution ) / 4.0f ); // we don't know exactly what fraction of the shadow map we will be partitioned to so we don't know what resolution we'll end up with, but taking the shadow map atlas texture resolution and dividing it by 4 is a safe bet (we'll be compatible with partitions that take up a fractioal size of 1/1, 1/2, and 1/4).
				matrix32x4x4_c virtual_space_view_transform = ops::make_matrix32x4x4_view_from_look_at( vector32x3_c( 0.0f, 0.0f, 0.0f ), light_forward, view_up );
				vector64x3_c slice_origin_quantized = ops::make_vector64x3_transformed_point( slice_origin, virtual_space_view_transform ); // transform from world space to view space so we can clamp coordinates.
				slice_origin_quantized.a = ops::math_remainder( slice_origin_quantized.a, world_space_texel_size );
				slice_origin_quantized.b = ops::math_remainder( slice_origin_quantized.b, world_space_texel_size );
				slice_origin_quantized.c = ops::math_remainder( slice_origin_quantized.c, world_space_texel_size );
				slice_origin_quantized = ops::make_vector64x3_transformed_point( slice_origin_quantized, ops::make_matrix32x4x4_inverted_fast( virtual_space_view_transform ) ); // transform from view space back to world space.
				matrix32x4x4_c projection_transform = ops::make_matrix32x4x4_projection_orthographic( slice_width_and_height, slice_width_and_height, 1.0f, slice_depth );

				// add shadow view.
				shadow_view_c * shadow_view = _shadow_views.emplace_at_end();
				shadow_view->world_space_position = light->get_world_space_transform().position + slice_origin_quantized;
				shadow_view->world_space_frustum = ops::make_frustum64_orthographic3( shadow_view->world_space_position, vector64x3_c( light_forward ), vector64x3_c( light_up ), slice_width_and_height, slice_width_and_height, 1.0f, slice_depth );
				shadow_view->virtual_space_position = vector32x3_c( 0.0f, 0.0f, 0.0f ); // only used by point light shadows.
				shadow_view->virtual_space_view_projection_transform = virtual_space_view_transform * projection_transform;
				shadow_view->virtual_space_view_projection_transform.d.a = static_cast< float32_c >( slice_origin_quantized.a );
				shadow_view->virtual_space_view_projection_transform.d.b = static_cast< float32_c >( slice_origin_quantized.b );
				shadow_view->virtual_space_view_projection_transform.d.c = static_cast< float32_c >( slice_origin_quantized.c );
			}
		}
		else if ( light->get_type() == scene_light_type_e_point )
		{
			if ( _shadow_views.get_length() + 6 > shadow_view_limit )
			{
				return;
			}

			shadow_view_end = shadow_view_start + 6;
			light->_shadow_view_start = _shadow_views.get_length();
			light->_shadow_view_count = 6;

			float32_c clip_near = 0.05f * light->_range;
			float32_c cube_face_field_of_view = constants< float32_c >::pi() / 2.0f;
			matrix32x4x4_c cube_face_projection_transform = ops::make_matrix32x4x4_projection_perspective( cube_face_field_of_view, 1.0f, clip_near, light->_range );

			for ( sint32_c i = 0; i < 6; i++ )
			{
				// build virtual space view projection transform.
				vector32x3_c virtual_space_position = vector32x3_c( light->get_world_space_transform().position - view_position );
				matrix32x4x4_c virtual_space_view_transform = get_cube_map_view_transform( i, virtual_space_position );

				// add shadow view.
				shadow_view_c * shadow_view = _shadow_views.emplace_at_end();
				shadow_view->world_space_position = light->get_world_space_transform().position;
				shadow_view->world_space_frustum = ops::make_frustum64_perspective3( shadow_view->world_space_position, vector64x3_c( cube_face_view_list_base[ i ].forward ), vector64x3_c( cube_face_view_list_base[ i ].up ), cube_face_field_of_view, cube_face_field_of_view, clip_near, light->get_range() );
				shadow_view->virtual_space_position = virtual_space_position;
				shadow_view->virtual_space_view_projection_transform = virtual_space_view_transform * cube_face_projection_transform;
			}
		}
		else if ( light->get_type() == scene_light_type_e_cone )
		{
			if ( _shadow_views.get_length() + 1 > shadow_view_limit )
			{
				return;
			}

			shadow_view_end = shadow_view_start + 1;
			light->_shadow_view_start = _shadow_views.get_length();
			light->_shadow_view_count = 1;

			vector32x3_c light_forward = light->get_world_space_transform().get_unscaled_basis_b();
			vector32x3_c light_up = light->get_world_space_transform().get_unscaled_basis_c();

			// build virtual space view projection transform.
			vector32x3_c virtual_space_position = vector32x3_c( light->get_world_space_transform().position - view_position );
			matrix32x4x4_c virtual_space_view_transform = ops::make_matrix32x4x4_view_from_look_at( virtual_space_position, light_forward, light_up );

			// add shadow view.
			shadow_view_c * shadow_view = _shadow_views.emplace_at_end();
			shadow_view->world_space_position = light->get_world_space_transform().position;
			shadow_view->world_space_frustum = ops::make_frustum64_perspective3( light->get_world_space_transform().position, vector64x3_c( light_forward ), vector64x3_c( light_up ), light->_cone_angle, light->_cone_angle, 0.001, light->_range );
			shadow_view->virtual_space_position	= virtual_space_position;
			shadow_view->virtual_space_view_projection_transform = virtual_space_view_transform * ops::make_matrix32x4x4_projection_perspective( light->get_cone_angle(), 1.0f, 0.05f, light->get_range() );
		}

		// do additional processing for the shadow views that were just added.
		for ( sint32_c shadow_view_index = shadow_view_start; shadow_view_index < shadow_view_end; shadow_view_index++ )
		{
			shadow_view_c * shadow_view = &_shadow_views[ shadow_view_index ];

			// gather renderables.
			_scene->_scene_component_tree.gather_items_in_frustum( gathered_components, shadow_view->world_space_frustum );
			for ( sint32_c component_index = 0; component_index < gathered_components.get_length(); component_index++ )
			{
				scene_component_c * component = gathered_components[ component_index ];
				if ( component->get_type_code() == scene_component_model_c::get_type_code_static() )
				{
					scene_component_model_c * model = dynamic_cast< scene_component_model_c * >( component );
					if ( model->_shadow_cast_enable && model->_opacity == 1.0f )
					{
						shadow_view->model_list.insert_at_end( model );
						if ( model->_render_frame_last != _scene->_render_frame )
						{
							model->_render_frame_last = _scene->_render_frame;
							if ( model->_bone_skin_matrix_list.get_length() > 0 )
							{
								_models_to_skin.insert_at_end( model );
							}
						}
					}
				}
			}
			gathered_components.remove_all();

			// add gpu shadow view.
			video_renderer_shadow_view_c * gpu_shadow_view = _texture_buffers.shadow_views.emplace_at_end();
			gpu_shadow_view->view_projection_transform = shadow_view->virtual_space_view_projection_transform;
			gpu_shadow_view->position = shadow_view->virtual_space_position;
			gpu_shadow_view->unused = 0.0f;
		}
	}

	void_c video_renderer_interface_c::_just_do_everything_for_me( view_c const & view )
	{
		// clear all the lists.
		_shadow_views.remove_all();
		_texture_buffers.shadow_views.remove_all();
		_gathered_lights.remove_all();
		_lights.remove_all();
		_texture_buffers.lights.remove_all();
		_gathered_components.remove_all();
		_models_to_render.remove_all();
		_models_to_render_for_outline.remove_all();
		_models_to_skin.remove_all();
		_sprites_to_render.remove_all();
		_sprites_to_render_for_outline.remove_all();
		_sprite_vertex_list.remove_all();

		// gather lights.
		for ( sint32_c global_light_index = 0; global_light_index < _scene->_global_lights_list.get_length(); global_light_index++ )
		{
			_gathered_lights.insert_at_end( _scene->_global_lights_list[ global_light_index ] );
		}
		_scene->_local_lights_tree.gather_items_in_frustum( _gathered_lights, view.world_space_frustum );

		// score|prioritize lights in view based on distance from viewer.
		// directional lights have highest score|prioirity and get inserted at front of list.
		static float64_c const score_cut_off = 0.0001; // skip lights that score lower than this.
		sint32_c light_limit = settings_c::get_light_count( _settings.light_count );
		assert( _lights.get_length() == 0 );
		assert( _texture_buffers.lights.get_length() == 0 );
		for ( sint32_c i = 0; i < _gathered_lights.get_length(); i++ )
		{
			scene_light_c * light = _gathered_lights[ i ];

			// skip this light if it's basically turned off.
			if ( !light->_render_enable || light->_color == vector32x3_c( 0.0f, 0.0f, 0.0f ) || light->_brightness == 0.0f )
			{
				continue;
			}

			// score this light.
			light->_render_score = constants< float64_c >::maximum(); // default to maximum.
			if ( light->_type != scene_light_type_e_direction )
			{
				float64_c distance_squared = ops::make_float64_length_squared( light->get_world_space_transform().position - view.world_space_position );
				if ( distance_squared > constants< float64_c >::division_near_zero() )
				{
					light->_render_score = ( 1.0 / distance_squared ) * light->get_brightness();
					if ( light->_render_score < score_cut_off )
					{
						continue;
					}
				}
			}
			insert_scored_light( _lights, light );
		}

		// process scored lights.
		if ( _lights.get_length() > light_limit )
		{
			_lights.set_length( light_limit );
		}
		for ( sint32_c i = 0; i < _lights.get_length(); i++ )
		{
			// we will use this light.
			scene_light_c * light = _lights[ i ];
			light->_render_index = i;

			// add shadow views for the light.
			light->_shadow_view_start = 0;
			light->_shadow_view_count = 0;
			if ( light->_shadow_cast_enable )
			{
				_add_shadow_views( light, view );
			}
			
			// translate to gpu compatible data structure.
			vector32x3_c light_position = vector32x3_c( light->get_world_space_transform().position - view.world_space_position );
			vector32x3_c light_direction = light->get_world_space_transform().get_unscaled_basis_b();
			video_renderer_light_c * gpu_light = _texture_buffers.lights.emplace_at_end();
			gpu_light->light_type = static_cast< float32_c >( light->get_type() );
			gpu_light->shadow_view_start = static_cast< float32_c >( light->_shadow_view_start );
			gpu_light->shadow_view_count = static_cast< float32_c >( light->_shadow_view_count );
			gpu_light->unused = 0.0f;
			gpu_light->position_range.a = light_position.a;
			gpu_light->position_range.b = light_position.b;
			gpu_light->position_range.c = light_position.c;
			gpu_light->position_range.d = light->get_range();
			gpu_light->direction_angle.a = light_direction.a;
			gpu_light->direction_angle.b = light_direction.b;
			gpu_light->direction_angle.c = light_direction.c;
			gpu_light->direction_angle.d = light->get_cone_angle();
			gpu_light->energy_unused.a = light->get_color().a * light->get_brightness();
			gpu_light->energy_unused.b = light->get_color().b * light->get_brightness();
			gpu_light->energy_unused.c = light->get_color().c * light->get_brightness();
			gpu_light->energy_unused.d = 0.0f;
		}

		// gather renderables (models, sprites, menu controls).
		_scene->_scene_component_tree.gather_items_in_frustum( _gathered_components, view.world_space_frustum );
		for ( sint32_c component_index = 0; component_index < _gathered_components.get_length(); component_index++ )
		{
			scene_component_c * component = _gathered_components[ component_index ];
			if ( component->get_type_code() == scene_component_model_c::get_type_code_static() )
			{
				scene_component_model_c * model = dynamic_cast< scene_component_model_c * >( component );
				_models_to_render.insert_at_end( model );
				if ( model->get_scene_object()->get_outline_color_index() != 0 )
				{
					_models_to_render_for_outline.insert_at_end( model );
				}
				if ( model->_render_frame_last != _scene->_render_frame )
				{
					model->_render_frame_last = _scene->_render_frame;
					if ( model->_bone_skin_matrix_list.get_length() > 0 )
					{
						_models_to_skin.insert_at_end( model );
					}
				}
			}
			else if ( component->get_type_code() == scene_component_sprite_c::get_type_code_static() )
			{
				scene_component_sprite_c * sprite = dynamic_cast< scene_component_sprite_c * >( component );
				_sprites_to_render.insert_at_end( sprite );
				if ( sprite->get_scene_object()->get_outline_color_index() != 0 )
				{
					_sprites_to_render_for_outline.insert_at_end( sprite );
				}
				if ( sprite->_last_render_frame != _scene->_render_frame )
				{
					sprite->_last_render_frame = _scene->_render_frame;

					sprite->_renderer_vertex_start = _sprite_vertex_list.get_length();

					vector32x3_c sprite_axis_a; // right. breaking convention...
					vector32x3_c sprite_axis_b; // up.
					vector32x3_c sprite_axis_c; // forward.

					if ( sprite->get_sprite_type() == scene_sprite_type_e_face_camera_global_up )
					{
						sprite_axis_b = vector32x3_c( _scene->_physics_scene->sample_up( sprite->get_scene_object()->get_world_space_transform().position ) );
						sprite_axis_a = ops::make_vector32x3_normalized( ops::make_vector32x3_cross_product( vector32x3_c( sprite->get_scene_object()->get_world_space_transform().position - view.world_space_position ), sprite_axis_b ) );
						sprite_axis_c = ops::make_vector32x3_cross_product( sprite_axis_a, sprite_axis_b );
					}
					else if ( sprite->get_sprite_type() == scene_sprite_type_e_face_camera_view_up )
					{
						sprite_axis_a = vector32x3_c( view.world_space_basis.a );
						sprite_axis_b = vector32x3_c( view.world_space_basis.b );
						sprite_axis_c = ops::make_vector32x3_cross_product( sprite_axis_a, sprite_axis_b );
					}
					else if ( sprite->get_sprite_type() == scene_sprite_type_e_real )
					{
						sprite_axis_a = vector32x3_c( sprite->get_scene_object()->get_world_space_transform().get_unscaled_basis_a() );
						sprite_axis_b = vector32x3_c( sprite->get_scene_object()->get_world_space_transform().get_unscaled_basis_b() );
						sprite_axis_c = ops::make_vector32x3_cross_product( sprite_axis_a, sprite_axis_b );
					}

					vector32x3_c sprite_position = vector32x3_c( sprite->get_scene_object()->get_world_space_transform().position - view.world_space_position ); // relative to camera origin.

					// top left vertex.
					video_renderer_vertex_mesh_base_c * vertex = _sprite_vertex_list.emplace_at_end();
					vertex->position = sprite_position + ( -sprite_axis_a + sprite_axis_b ) * sprite->_radius;
					vertex->normal = sprite_axis_c;
					vertex->normal_u = sprite_axis_a;
					vertex->normal_v = sprite_axis_b;
					vertex->texture = vector32x4_c( 0.0f, 0.0f, 0.0f, 0.0f );

					// bottom left vertex.
					vertex = _sprite_vertex_list.emplace_at_end();
					vertex->position = sprite_position + ( -sprite_axis_a - sprite_axis_b ) * sprite->_radius;
					vertex->normal = sprite_axis_c;
					vertex->normal_u = sprite_axis_a;
					vertex->normal_v = sprite_axis_b;
					vertex->texture = vector32x4_c( 0.0f, 1.0f, 0.0f, 0.0f );

					// top right vertex.
					vertex = _sprite_vertex_list.emplace_at_end();
					vertex->position = sprite_position + ( sprite_axis_a + sprite_axis_b ) * sprite->_radius;
					vertex->normal = sprite_axis_c;
					vertex->normal_u = sprite_axis_a;
					vertex->normal_v = sprite_axis_b;
					vertex->texture = vector32x4_c( 1.0f, 0.0f, 0.0f, 0.0f );

					// bottom right vertex.
					vertex = _sprite_vertex_list.emplace_at_end();
					vertex->position = sprite_position + ( sprite_axis_a - sprite_axis_b ) * sprite->_radius;
					vertex->normal = sprite_axis_c;
					vertex->normal_u = sprite_axis_a;
					vertex->normal_v = sprite_axis_b;
					vertex->texture = vector32x4_c( 1.0f, 1.0f, 0.0f, 0.0f );
				}
			}
			else if ( component->get_type_code() == scene_component_menu_control_c::get_type_code_static() )
			{
				scene_component_menu_control_c * menu_control = dynamic_cast< scene_component_menu_control_c * >( component );
				_menu_controls_to_render.insert_at_end( menu_control );
			}
		}

		// associate renderables with lights.
		// prioritize lights based on a factor of their energy and distance.
		static core_list_c< scene_light_c * > scored_lights;
		scored_lights.remove_all();
		for ( sint32_c model_index = 0; model_index < _models_to_render.get_length(); model_index++ )
		{
			scene_component_model_c * model = _models_to_render[ model_index ];
			model->_render_light_indices_count = 0;

			for ( sint32_c light_index = 0; light_index < _lights.get_length(); light_index++ )
			{
				scene_light_c * light = _lights[ light_index ];
				light->_render_score = constants< float64_c >::maximum(); // default to maximum.
				if ( light->_type != scene_light_type_e_direction )
				{
					float64_c distance_squared = ops::make_float64_length_squared( light->get_world_space_transform().position - ops::nearest_point_on_box( light->get_world_space_transform().position, model->_world_space_aabb ) );
					if ( distance_squared > constants< float64_c >::division_near_zero() )
					{
						light->_render_score = ( 1.0 / distance_squared ) * light->get_brightness();
						if ( light->_render_score < score_cut_off )
						{
							continue;
						}
					}
				}
				insert_scored_light( scored_lights, light );
			}

			sint32_c scored_light_index_minimum = scored_lights.get_length() - object_lights_count;
			if ( scored_light_index_minimum < 0 )
			{
				scored_light_index_minimum = 0;
			}

			model->_render_light_indices_count = ops::math_minimum( scored_lights.get_length(), object_lights_count );
			for ( sint32_c scored_light_index = 0; scored_light_index < model->_render_light_indices_count; scored_light_index++ )
			{
				model->_render_light_indices[ scored_light_index ] = scored_lights[ scored_lights.get_length() - scored_light_index ]->_render_index;
			}
		}

		// skin models.
		if ( _models_to_skin.get_length() > 0 )
		{
			// set render states needed to skin with gpu.
			engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_point_list );
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader( nullptr );
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );
			// skin models with gpu and cpu.
			for ( sint32_c i = 0; i < _models_to_skin.get_length(); i++ )
			{
				scene_component_model_c * model = _models_to_skin[ i ];
				if ( model->_vertex_skin_mode == scene_component_model_c::vertex_skin_mode_e_cpu )
				{
					_skin_model_vertices_with_cpu( model );
				}
				else if ( model->_vertex_skin_mode == scene_component_model_c::vertex_skin_mode_e_gpu )
				{
					_skin_model_vertices_with_gpu( model );
				}
			}
			// revert render states that were needed to skin with gpu.
			engine_c::get_instance()->get_video_interface()->bind_target_vertex_buffer( nullptr );
			engine_c::get_instance()->get_video_interface()->bind_vertex_shader( nullptr );
		}

		// render shadow views to shadow maps.
		if ( _shadow_views.get_length() > 0 )
		{
			engine_c::get_instance()->get_video_interface()->clear_texture( _shadow_map_texture2darray, 1.0f, 1.0f, 1.0f, 1.0f );
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_back, video_fill_type_e_face );
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );
			engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( video_blend_type_e_min );
			for ( sint32_c shadow_view_index = 0; shadow_view_index < _shadow_views.get_length(); shadow_view_index++ )
			{
				shadow_view_c * shadow_view = &_shadow_views[ shadow_view_index ];
				textures_to_bind[ 0 ] = _shadow_map_texture2darray;
				textures_slices_to_bind[ 0 ] = shadow_view_index;
				engine_c::get_instance()->get_video_interface()->bind_target_texture2darrays_slices( 1, textures_to_bind, textures_slices_to_bind, nullptr, 0 );
				_constant_buffers.shadow_camera_block->shadow_camera_view_projection_transform = shadow_view->virtual_space_view_projection_transform;
				_constant_buffers.shadow_camera_block_constant_buffer->set_data( _constant_buffers.shadow_camera_block, sizeof( shadow_camera_block_c ) );
				for ( sint32_c model_index = 0; model_index < shadow_view->model_list.get_length(); model_index++ )
				{
					scene_component_model_c * model = shadow_view->model_list[ model_index ];
					_draw_model_for_shadow( model, view.world_space_position );
				}
				shadow_view->model_list.remove_all();
			}
			engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none ); // un-bind the shadow map render target, so that it can be later re-bound as a texture input instead.
		}

		// upload lights.
		if ( _texture_buffers.lights.get_length() > 0 )
		{
			_texture_buffers.lights_texture_buffer->set_data( _texture_buffers.lights.get_internal_array(), _texture_buffers.lights.get_internal_array_size_used() );
			_texture_buffers.lights.remove_all();
			_lights.remove_all();
		}
		_gathered_lights.remove_all();

		// upload shadow views.
		if ( _texture_buffers.shadow_views.get_length() > 0 )
		{
			_texture_buffers.shadow_views_texture_buffer->set_data( _texture_buffers.shadow_views.get_internal_array(), _texture_buffers.shadow_views.get_internal_array_size_used() );
			_texture_buffers.shadow_views.remove_all();
			_shadow_views.remove_all();
		}

		// upload sprite vertices.
		if ( _sprite_vertex_list.get_length() > 0 )
		{
			if ( _sprite_vertex_buffer == nullptr || _sprite_vertex_list.get_length() > _sprite_vertex_buffer->get_vertex_count() )
			{
				if ( _sprite_vertex_buffer )
				{
					delete _sprite_vertex_buffer;
					_sprite_vertex_buffer = nullptr;
				}
				_sprite_vertex_buffer = engine_c::get_instance()->get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_mesh_base, _sprite_vertex_list.get_length_allocated(), nullptr, 0, true, false, false );
			}
			_sprite_vertex_buffer->set_data( _sprite_vertex_list.get_internal_array(), _sprite_vertex_list.get_internal_array_size_used() );
			_sprite_vertex_list.remove_all();
		}
	}

	// inserts light into scored_lights, highest scores are stored at front of list.
	void_c insert_scored_light( core_list_c< scene_light_c * > & scored_lights, scene_light_c * light )
	{
		if ( scored_lights.get_length() != 0 )
		{
			sint32_c i_low = 0;
			sint32_c i_high = scored_lights.get_length();
			sint32_c i_median = ( i_low + i_high ) / 2;
			while ( i_low != i_median )
			{
				if ( light->_render_score < scored_lights[ i_median ]->_render_score )
				{
					i_low = i_median;
				}
				else if ( light->_render_score > scored_lights[ i_median ]->_render_score )
				{
					i_high = i_median;
				}
				else
				{
					scored_lights.insert_at_index( i_median, light );
					return;
				}
				i_median = ( i_low + i_high ) / 2;
			}
			if ( light->_render_score < scored_lights[ i_median ]->_render_score )
			{
				i_median++;
			}
			scored_lights.insert_at_index( i_median, light );
		}
		else
		{
			scored_lights.insert_at_end( light );
		}
	}

	void_c video_renderer_interface_c::_skin_model_vertices_with_cpu( scene_component_model_c * model )
	{
		assert( model->_vertex_skin_mode == scene_component_model_c::vertex_skin_mode_e_cpu );
		assert( model->_bone_skin_matrix_list.get_length() > 0 );

		if ( model->get_model_resource_is_bound() == false )
		{
			return;
		}

		if ( model->_mesh_vertex_buffer_skinned == nullptr )
		{
			model->_mesh_vertex_buffer_skinned = engine_c::get_instance()->get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_mesh_base, model->_model_resource->_data.mesh_vertex_list_base.get_length(), nullptr, 0, true, false, false );
		}

		// if we can parallelize this it would be better.
		// be respectful, don't soak up all of the CPU, leave resources for other programs, user likely has other programs besides ours.
		// pick and choose to skin only the vertices that are a part of meshes that are visible.
		for ( sint32_c i = 0; i < model->_mesh_list.get_length(); i++ )
		{
			scene_component_model_c::mesh_c & mesh = model->_mesh_list[ i ];
			if ( mesh._render_enable == true )
			{
				for ( uint32_c j = mesh._source_mesh->draw_start; j < mesh._source_mesh->draw_end; j++ )
				{
					resource_file_model_c::mesh_draw_c const & source_mesh_draw = model->_model_resource->_data.mesh_draw_list[ j ];
					for ( uint32_c k = source_mesh_draw.vertex_start; k < source_mesh_draw.vertex_end; k++ )
					{
						resource_file_model_c::mesh_vertex_base_c source_vertex_base = model->_model_resource->_data.mesh_vertex_list_base[ k ];
						resource_file_model_c::mesh_vertex_bone_weight_c source_vertex_bone_weight = model->_model_resource->_data.mesh_vertex_list_bone_weight[ k ];
						video_renderer_vertex_mesh_base_c & skinned_vertex = model->_mesh_vertex_list_cpu_skinned[ k ];
						matrix32x4x4_c bone_skin_matrix_blended = matrix32x4x4_c();
						if ( source_vertex_bone_weight.bone_weights[ 0 ] > 0 )
						{
							sint32_c l = 0;
							while ( l < 4 && source_vertex_bone_weight.bone_weights[ l ] > 0 ) // bone influences are sorted by weight, so if we encounter a 0 weight then we can break the loop early.
							{
								float32_c bone_weight = static_cast< float32_c >( source_vertex_bone_weight.bone_weights[ l ] ) / 65535.0f;
								matrix32x4x4_c bone_skin_matrix = *model->_bone_skin_matrix_list[ source_vertex_bone_weight.bone_indices[ l ] ];
								bone_skin_matrix_blended.a.a += bone_skin_matrix.a.a * bone_weight;
								bone_skin_matrix_blended.a.b += bone_skin_matrix.a.b * bone_weight;
								bone_skin_matrix_blended.a.c += bone_skin_matrix.a.c * bone_weight;
								//bone_skin_matrix_blended.a.d += bone_skin_matrix.a.d * bone_weight;
								bone_skin_matrix_blended.b.a += bone_skin_matrix.b.a * bone_weight;
								bone_skin_matrix_blended.b.b += bone_skin_matrix.b.b * bone_weight;
								bone_skin_matrix_blended.b.c += bone_skin_matrix.b.c * bone_weight;
								//bone_skin_matrix_blended.b.d += bone_skin_matrix.b.d * bone_weight;
								bone_skin_matrix_blended.c.a += bone_skin_matrix.c.a * bone_weight;
								bone_skin_matrix_blended.c.b += bone_skin_matrix.c.b * bone_weight;
								bone_skin_matrix_blended.c.c += bone_skin_matrix.c.c * bone_weight;
								//bone_skin_matrix_blended.c.d += bone_skin_matrix.c.d * bone_weight;
								bone_skin_matrix_blended.d.a += bone_skin_matrix.d.a * bone_weight;
								bone_skin_matrix_blended.d.b += bone_skin_matrix.d.b * bone_weight;
								bone_skin_matrix_blended.d.c += bone_skin_matrix.d.c * bone_weight;
								//bone_skin_matrix_blended.d.d += bone_skin_matrix.d.d * bone_weight;
								l++;
							}
							skinned_vertex.position = ops::make_vector32x3_transformed_point( *reinterpret_cast< vector32x3_c * >( source_vertex_base.position ), bone_skin_matrix_blended );
							skinned_vertex.normal = ops::make_vector32x3_normalized( ops::make_vector32x3_transformed_vector( *reinterpret_cast< vector32x3_c * >( source_vertex_base.normal ), bone_skin_matrix_blended ) );
							skinned_vertex.normal_u = ops::make_vector32x3_normalized( ops::make_vector32x3_transformed_vector( *reinterpret_cast< vector32x3_c * >( source_vertex_base.normal_u ), bone_skin_matrix_blended ) );
							skinned_vertex.normal_v = ops::make_vector32x3_normalized( ops::make_vector32x3_transformed_vector( *reinterpret_cast< vector32x3_c * >( source_vertex_base.normal_v ), bone_skin_matrix_blended ) );
							skinned_vertex.texture = vector32x4_c( source_vertex_base.texture );
						}
						else
						{
							skinned_vertex.position = vector32x3_c( source_vertex_base.position );
							skinned_vertex.normal = vector32x3_c( source_vertex_base.normal );
							skinned_vertex.normal_u = vector32x3_c( source_vertex_base.normal_u );
							skinned_vertex.normal_u = vector32x3_c( source_vertex_base.normal_v );
							skinned_vertex.texture = vector32x4_c( source_vertex_base.texture );
						}
					}
				}
			}
		}
		model->_mesh_vertex_buffer_skinned->set_data( model->_mesh_vertex_list_cpu_skinned.get_internal_array(), model->_mesh_vertex_list_cpu_skinned.get_internal_array_size_used() );
	}

	void_c video_renderer_interface_c::_skin_model_vertices_with_gpu( scene_component_model_c * model )
	{
		assert( model->_vertex_skin_mode == scene_component_model_c::vertex_skin_mode_e_gpu );
		assert( model->_bone_skin_matrix_list.get_length() > 0 );

		if ( model->get_model_resource_is_bound() == false )
		{
			return;
		}

		if ( model->_mesh_vertex_buffer_skinned == nullptr )
		{
			model->_mesh_vertex_buffer_skinned = engine_c::get_instance()->get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_mesh_base, model->_model_resource->_data.mesh_vertex_list_base.get_length(), nullptr, 0, false, false, true );
		}

		// skin vertices.
		matrix32x4x4_c bones[ bones_limit ];
		assert( model->_bone_skin_matrix_list.get_length() < bones_limit );
		for ( sint32_c bone_index = 0; bone_index < model->_bone_skin_matrix_list.get_length(); bone_index++ )
		{
			bones[ bone_index ] = matrix32x4x4_c( * model->_bone_skin_matrix_list[ bone_index ] ); // we do not transpose these, because each matrix row is treated as a float4 component in the shader and is loaded as such. any transposition that might happen is done by the shader or hardware or whatever when it loads the vectors into a matrix.
		}
		_texture_buffers.bones_texture_buffer->set_data( bones, sizeof( matrix32x4x4_c ) * model->_bone_skin_matrix_list.get_length() );
		engine_c::get_instance()->get_video_interface()->bind_target_vertex_buffer( model->_mesh_vertex_buffer_skinned );
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_skin_mesh() );
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 0, nullptr );
		engine_c::get_instance()->get_video_interface()->draw( 0, model->_model_resource->_data.mesh_list.get_length());
	}

	void_c video_renderer_interface_c::_bind_model_properties( scene_component_model_c * model, vector64x3_c const & view_position, boolean_c with_lights, boolean_c with_ambience, boolean_c with_outline )
	{
		if ( model->_mother_model != nullptr )
		{
			return; // model properties would already be bound.
		}

		_constant_buffers.object_block->object_world_transform = ops::make_matrix32x4x4_from_space_transform( model->get_scene_object()->get_world_space_transform(), view_position );
		if ( with_lights )
		{
			for ( sint32_c i = 0; i < 16; i++ )
			{
				_constant_buffers.object_block->object_light_indices[ i ] = model->_render_light_indices[ i ];
			}
		}
		else
		{
			_constant_buffers.object_block->object_light_indices[ 0 ] = -1;
		}
		if ( with_outline )
		{
			ops::memory_zero( _constant_buffers.object_block->object_colors, sizeof( _constant_buffers.object_block->object_colors ) );
			_constant_buffers.object_block->object_colors[ 0 ].a = static_cast< float32_c >( model->get_scene_object()->get_outline_color_index() );
		}
		else if ( model->object_colors_enable )
		{
			for ( sint32_c i = 0; i < object_colors_count; i++ )
			{
				_constant_buffers.object_block->object_colors[ i ] = model->object_colors[ i ];
			}
		}
		_constant_buffers.object_block_constant_buffer->set_data( &_constant_buffers.object_block, sizeof( object_block_c ) );

		if ( model->object_textures_enable )
		{
			for ( sint32_c i = 0; i < object_textures_count; i++ )
			{
				textures_to_bind[ i ] = model->object_textures[ i ].is_reference_set_and_loaded() ? model->object_textures[ i ]->get_video_texture() : nullptr;
				textures_to_bind_types[ i ] = video_texture_type_e_texture2d;
			}
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_model_textures, object_textures_count, textures_to_bind, textures_to_bind_types );
		}

		textures_to_bind[ 0 ] = _light_probe_cube_energy_default;
		if ( with_ambience )
		{
			scene_component_light_probe_c * light_probe = _scene->find_light_probe_with_point( model->get_scene_object()->_world_space_transform.position );
			textures_to_bind[ 0 ] = light_probe != nullptr ? light_probe->_cube_color : nullptr;
			textures_to_bind_types[ 0 ] = video_texture_type_e_texturecube;
		}
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( video_renderer_interface_c::_texture_bind_index_for_light_probe_texture, 1, textures_to_bind, textures_to_bind_types );
	}

	void_c video_renderer_interface_c::_bind_mesh_vs_for_normal_and_depth( boolean_c is_waved, boolean_c is_clipped )
	{
		video_vertex_shader_c * vertex_shader = nullptr;
		if ( !is_waved )
		{
			if ( !is_clipped )
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_vs_mesh();
			}
			else
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_vs_mesh_clipped();
			}
		}
		else
		{
			if ( !is_clipped )
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_vs_mesh_waved();
			}
			else
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_vs_mesh_waved_clipped();
			}
		}
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader( vertex_shader );
	}

	void_c video_renderer_interface_c::_bind_mesh_vs_for_color( boolean_c is_waved, boolean_c is_clipped )
	{
		video_vertex_shader_c * vertex_shader = nullptr;
		if ( !is_waved )
		{
			if ( !is_clipped )
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_color_vs_mesh();
			}
			else
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_color_vs_mesh_clipped();
			}
		}
		else
		{
			if ( !is_clipped )
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_color_vs_mesh_waved();
			}
			else
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_color_vs_mesh_waved_clipped();
			}
		}
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader( vertex_shader );
	}

	void_c video_renderer_interface_c::_bind_material_for_color( scene_material_c * material )
	{
		assert( material );

		if ( !material->is_masked )
		{
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader( material->pixel_shader.get_pixel_shader() );
		}
		else
		{
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader( material->pixel_shader.get_pixel_shader_masked() );
		}

		for ( uint32_c i = 0; i < material_textures_count; i++ )
		{
			textures_to_bind[ i ] = material->textures[ i ].is_reference_set_and_loaded() ? material->textures[ i ]->get_video_texture() : texture_green_pixel;
			textures_to_bind_types[ i ] = video_texture_type_e_texture2d;
		}
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, material_textures_count, textures_to_bind, textures_to_bind_types );

		for ( uint32_c i = 0; i < material_colors_count; i++ )
		{
			_constant_buffers.material_block->material_colors[ i ] = material->colors[ i ];
		}
		_constant_buffers.material_block_constant_buffer->set_data( _constant_buffers.material_block, sizeof( material_block_c ) );

		engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( material->cull_mode, video_fill_type_e_face );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( material->blend_type );
	}

	void_c video_renderer_interface_c::_draw_model_with_no_material( scene_component_model_c * model, vector64x3_c const & view_position )
	{
		if ( model->get_model_resource_is_bound() == false )
		{
			return;
		}

		boolean_c is_model_bound = false;
		for ( sint32_c mesh_index = 0; mesh_index < model->_mesh_list.get_length(); mesh_index++ )
		{
			scene_component_model_c::mesh_c * mesh = &model->_mesh_list[ mesh_index ];

			if ( mesh->_render_enable == false )
			{
				continue;
			}

			scene_material_c * material = mesh->_material_assignment != nullptr ? &mesh->_material_assignment->_material : &_scene_default_material;

			if ( !is_model_bound )
			{
				is_model_bound = true;
				_constant_buffers.object_block->object_world_transform = ops::make_matrix32x4x4_from_space_transform( model->get_scene_object()->get_world_space_transform(), view_position );
				_constant_buffers.object_block_constant_buffer->set_data( _constant_buffers.object_block, sizeof( object_block_c ) );
				vertex_buffers_to_bind[ 0 ] = model->_get_vertex_buffer_for_rendering();
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
				engine_c::get_instance()->get_video_interface()->bind_index_buffer( model->_model_resource->_data.mesh_index_buffer );
				_bind_mesh_vs_for_color( material->is_waved, false );
			}

			for ( uint32_c draw_index = mesh->_source_mesh->draw_start; draw_index < mesh->_source_mesh->draw_end; draw_index++ )
			{
				resource_file_model_c::mesh_draw_c const * draw = &model->_model_resource->_data.mesh_draw_list[ draw_index ];
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( draw->get_video_primitive_type() );
				engine_c::get_instance()->get_video_interface()->draw_indexed( draw->index_start, draw->index_end - draw->index_start, 0 );
			}
		}

		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_model_list_node = model->_daughter_model_list.get_first();
		while ( daughter_model_list_node != nullptr )
		{
			_draw_model_with_no_material( daughter_model_list_node->get_value(), view_position );
			daughter_model_list_node = daughter_model_list_node->get_next();
		}
	}

	void_c video_renderer_interface_c::_draw_model_for_sky( scene_component_model_c * model )
	{
		if ( model->get_model_resource_is_bound() == false )
		{
			return;
		}

		boolean_c is_model_bound = false;
		for ( sint32_c mesh_index = 0; mesh_index < model->_mesh_list.get_length(); mesh_index++ )
		{
			scene_component_model_c::mesh_c * mesh = &model->_mesh_list[ mesh_index ];

			if ( mesh->_render_enable == false )
			{
				continue;
			}

			scene_material_c * mesh_material = mesh->_material_assignment != nullptr ? &mesh->_material_assignment->_material : &_scene_default_material;

			boolean_c is_opaque = ( mesh_material->blend_type == video_blend_type_e_set ) && ( model->get_opacity() == 1.0f );

			if ( mesh_material->is_overlay )
			{
				continue;
			}

			if ( !is_model_bound )
			{
				is_model_bound = true;
				_bind_model_properties( model, vector64x3_c( 0.0f, 0.0f, 0.0f ), false, false, false );
				vertex_buffers_to_bind[ 0 ] = model->_get_vertex_buffer_for_rendering();
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
				engine_c::get_instance()->get_video_interface()->bind_index_buffer( model->_model_resource->_data.mesh_index_buffer );
				engine_c::get_instance()->get_video_interface()->bind_vertex_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_color_vs_mesh() );
			}

			_bind_material_for_color( mesh_material );

			for ( uint32_c draw_index = mesh->_source_mesh->draw_start; draw_index < mesh->_source_mesh->draw_end; draw_index++ )
			{
				resource_file_model_c::mesh_draw_c * draw = &model->_model_resource->_data.mesh_draw_list[ draw_index ];
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( draw->get_video_primitive_type() );
				engine_c::get_instance()->get_video_interface()->draw_indexed( draw->index_start, draw->index_end - draw->index_start, 0 );
			}
		}

		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_model_list_node = model->_daughter_model_list.get_first();
		while ( daughter_model_list_node != nullptr )
		{
			_draw_model_for_sky( daughter_model_list_node->get_value() );
			daughter_model_list_node = daughter_model_list_node->get_next();
		}
	}

	void_c video_renderer_interface_c::_draw_model_for_outline( scene_component_model_c * model, vector64x3_c const & view_position, boolean_c is_clipped )
	{
		if ( model->get_model_resource_is_bound() == false )
		{
			return;
		}

		boolean_c is_model_bound = false;
		for ( sint32_c mesh_index = 0; mesh_index < model->_mesh_list.get_length(); mesh_index++ )
		{
			scene_component_model_c::mesh_c * mesh = &model->_mesh_list[ mesh_index ];

			if ( mesh->_render_enable == false )
			{
				continue;
			}

			scene_material_c * material = mesh->_material_assignment != nullptr ? &mesh->_material_assignment->_material : &_scene_default_material;

			boolean_c is_opaque = ( material->blend_type == video_blend_type_e_set ) && ( model->get_opacity() == 1.0f );

			if ( material->is_overlay || !is_opaque )
			{
				continue;
			}

			if ( !is_model_bound )
			{
				is_model_bound = true;
				_bind_model_properties( model, view_position, false, false, true );
				vertex_buffers_to_bind[ 0 ] = model->_get_vertex_buffer_for_rendering();
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
				_bind_mesh_vs_for_color( material->is_waved, is_clipped );
				engine_c::get_instance()->get_video_interface()->bind_index_buffer( model->_model_resource->_data.mesh_index_buffer );
			}

			if ( !material->is_masked )
			{
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_outline_ps_mesh() );
			}
			else
			{
				textures_to_bind[ 0 ] = material->textures[ 0 ].is_reference_set_and_loaded() ? material->textures[ 0 ]->get_video_texture() : texture_green_pixel;
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_outline_ps_mesh_masked() );
			}

			for ( uint32_c draw_index = mesh->_source_mesh->draw_start; draw_index < mesh->_source_mesh->draw_end; draw_index++ )
			{
				resource_file_model_c::mesh_draw_c const * draw = &model->_model_resource->_data.mesh_draw_list[ draw_index ];
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( draw->get_video_primitive_type() );
				engine_c::get_instance()->get_video_interface()->draw_indexed( draw->index_start, draw->index_end - draw->index_start, 0 );
			}
		}

		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_model_list_node = model->_daughter_model_list.get_first();
		while ( daughter_model_list_node != nullptr )
		{
			_draw_model_for_outline( daughter_model_list_node->get_value(), view_position, is_clipped );
			daughter_model_list_node = daughter_model_list_node->get_next();
		}
	}

	void_c video_renderer_interface_c::_draw_model_for_color( scene_component_model_c * model, vector64x3_c const & view_position, boolean_c is_clipped, boolean_c do_opaque, boolean_c do_transparent, boolean_c do_overlay, boolean_c with_lights, boolean_c with_ambience )
	{
		if ( model->get_model_resource_is_bound() == false )
		{
			return;
		}

		boolean_c is_model_bound = false;
		for ( sint32_c mesh_index = 0; mesh_index < model->_mesh_list.get_length(); mesh_index++ )
		{
			scene_component_model_c::mesh_c * mesh = &model->_mesh_list[ mesh_index ];

			if ( mesh->_render_enable == false )
			{
				continue;
			}

			scene_material_c * material = mesh->_material_assignment != nullptr ? &mesh->_material_assignment->_material : &_scene_default_material;

			boolean_c is_opaque = ( material->blend_type == video_blend_type_e_set ) && ( model->get_opacity() == 1.0f );

			if ( material->is_overlay )
			{
				if ( !do_overlay )
				{
					continue;
				}
			}
			else if ( !do_opaque && is_opaque )
			{
				continue;
			}
			else if ( !do_transparent && !is_opaque )
			{
				continue;
			}

			if ( !is_model_bound )
			{
				is_model_bound = true;
				_bind_model_properties( model, view_position, with_lights, with_ambience, false );
				vertex_buffers_to_bind[ 0 ] = model->_get_vertex_buffer_for_rendering();
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
				_bind_mesh_vs_for_color( material->is_waved, is_clipped );
				engine_c::get_instance()->get_video_interface()->bind_index_buffer( model->_model_resource->_data.mesh_index_buffer );
			}

			_bind_material_for_color( material );

			for ( uint32_c draw_index = mesh->_source_mesh->draw_start; draw_index < mesh->_source_mesh->draw_end; draw_index++ )
			{
				resource_file_model_c::mesh_draw_c const * draw = &model->_model_resource->_data.mesh_draw_list[ draw_index ];
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( draw->get_video_primitive_type() );
				engine_c::get_instance()->get_video_interface()->draw_indexed( draw->index_start, draw->index_end - draw->index_start, 0 );
			}
		}

		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_model_list_node = model->_daughter_model_list.get_first();
		while ( daughter_model_list_node != nullptr )
		{
			_draw_model_for_color( daughter_model_list_node->get_value(), view_position, is_clipped, do_opaque, do_transparent, do_overlay, with_lights, with_ambience );
			daughter_model_list_node = daughter_model_list_node->get_next();
		}
	}

	/*
	void_c video_renderer_interface_c::_draw_model_for_energy( scene_component_model_c * model, vector64x3_c const & view_position )
	{
		if ( model->get_model_resource_is_bound() == false )
		{
			return;
		}

		boolean_c is_model_bound = false;
		for ( sint32_c mesh_index = 0; mesh_index < model->_mesh_list.get_length(); mesh_index++ )
		{
			scene_component_model_c::mesh_c * mesh = &model->_mesh_list[ mesh_index ];

			if ( mesh->render_enable == false )
			{
				continue;
			}

			video_renderer_material_c * material = mesh->material_assignment ? &mesh->material_assignment->material : &_scene_default_material;

			if ( material->is_overlay )
			{
				continue;
			}

			if ( !is_model_bound )
			{
				is_model_bound = true;
				_bind_model_properties( model, view_position, true, false );
				engine_c::get_instance()->get_video_interface()->bind_vertex_buffer( model->_mesh_vertex_buffer_cpu_skinned, 0 );
				_bind_mesh_vs_for_color( material->is_waved, false );
				engine_c::get_instance()->get_video_interface()->bind_index_buffer( model->_model_resource->_mesh_index_buffer );
			}

			_bind_material_for_color( material );

			for ( uint32_c draw_index = mesh->source_mesh->draw_start; draw_index < mesh->source_mesh->draw_end; draw_index++ )
			{
				resource_file_model_c::mesh_draw_c * draw = &model->_model_resource->_header_extras.mesh_draw_list[ draw_index ];
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( static_cast< video_primitive_type_e >( draw->primitive_type ) );
				engine_c::get_instance()->get_video_interface()->draw_indexed( draw->index_start, draw->index_end - draw->index_start );
			}
		}

		core_linked_list_c< scene_component_model_c * >::node_c * daughter_model = model->_daughter_model_list.get_first();
		while ( daughter_model )
		{
			_draw_model_for_energy( daughter_model->value, view_position );
			daughter_model = daughter_model->next;
		}
	}
	*/

	void_c video_renderer_interface_c::_draw_model_for_shadow( scene_component_model_c * model, vector64x3_c const & view_position )
	{
		if ( !model->get_model_resource_is_bound() )
		{
			return;
		}

		_constant_buffers.shadow_object_block->shadow_object_world_transform = ops::make_matrix32x4x4_from_space_transform( model->get_scene_object()->get_world_space_transform(), view_position );
		_constant_buffers.shadow_object_block_constant_buffer->set_data( _constant_buffers.shadow_object_block, sizeof( shadow_object_block_c ) );

		vertex_buffers_to_bind[ 0 ] = model->_get_vertex_buffer_for_rendering();
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
		engine_c::get_instance()->get_video_interface()->bind_index_buffer( model->_model_resource->_data.mesh_index_buffer );

		for ( sint32_c mesh_index = 0; mesh_index < model->_mesh_list.get_length(); mesh_index++ )
		{
			scene_component_model_c::mesh_c * mesh = &model->_mesh_list[mesh_index];

			if ( mesh->_render_enable == false )
			{
				continue;
			}

			scene_material_c * material = mesh->_material_assignment ? &mesh->_material_assignment->_material : &_scene_default_material;

			if ( material->blend_type != video_blend_type_e_set && material->blend_type != video_blend_type_e_mix )
			{
				continue;
			}

			video_vertex_shader_c * vertex_shader = nullptr;
			if ( !material->is_waved )
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_shadow_vs_mesh();
			}
			else
			{
				vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_shadow_vs_mesh_waved();
			}
			engine_c::get_instance()->get_video_interface()->bind_vertex_shader( vertex_shader );

			if ( material->is_masked && material->textures[ 0 ].is_reference_set_and_loaded() )
			{
				textures_to_bind[ 0 ] = material->textures[ 0 ]->get_video_texture();
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_shadow_ps_mesh_masked() );
			}
			else
			{
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_shadow_ps_mesh() );
			}

			engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( material->cull_mode, material->fill_mode );

			for ( uint32_c draw_index = mesh->_source_mesh->draw_start; draw_index < mesh->_source_mesh->draw_end; draw_index++ )
			{
				resource_file_model_c::mesh_draw_c * draw = &model->_model_resource->_data.mesh_draw_list[ draw_index ];
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( draw->get_video_primitive_type() );
				engine_c::get_instance()->get_video_interface()->draw_indexed( draw->index_start, draw->index_end - draw->index_start, 0 );
			}
		}

		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_model_list_node = model->_daughter_model_list.get_first();
		while ( daughter_model_list_node != nullptr )
		{
			_draw_model_for_shadow( daughter_model_list_node->get_value(), view_position );
			daughter_model_list_node = daughter_model_list_node->get_next();
		}
	}

	void_c video_renderer_interface_c::_draw_model_for_normal_and_depth( scene_component_model_c * model, vector64x3_c const & view_position, boolean_c is_clipped )
	{
		_constant_buffers.object_block->object_world_transform = ops::make_matrix32x4x4_from_space_transform( model->get_scene_object()->get_world_space_transform(), view_position );
		_constant_buffers.object_block_constant_buffer->set_data( _constant_buffers.object_block, sizeof( object_block_c ) );

		vertex_buffers_to_bind[ 0 ] = model->_get_vertex_buffer_for_rendering();
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
		engine_c::get_instance()->get_video_interface()->bind_index_buffer( model->_model_resource->_data.mesh_index_buffer );

		boolean_c is_model_bound = false;
		for ( sint32_c mesh_index = 0; mesh_index < model->_mesh_list.get_length(); mesh_index++ )
		{
			scene_component_model_c::mesh_c * mesh = &model->_mesh_list[mesh_index];

			if ( mesh->_render_enable == false )
			{
				continue;
			}

			scene_material_c * material = mesh->_material_assignment != nullptr ? &mesh->_material_assignment->_material : &_scene_default_material;

			if ( material->is_overlay || material->blend_type != video_blend_type_e_set || model->get_opacity() != 1.0f )
			{
				continue;
			}

			video_vertex_shader_c * vertex_shader = nullptr;
			if ( !is_clipped )
			{
				if ( !material->is_waved )
				{
					vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_vs_mesh();
				}
				else
				{
					vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_vs_mesh_waved();
				}
			}
			else
			{
				if ( !material->is_masked )
				{
					vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_vs_mesh_clipped();
				}
				else
				{
					vertex_shader = engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_vs_mesh_waved_clipped();
				}
			}
			engine_c::get_instance()->get_video_interface()->bind_vertex_shader( vertex_shader );

			if ( material->is_masked && material->textures[ 0 ].is_reference_set_and_loaded() )
			{
				textures_to_bind[ 0 ] = material->textures[ 0 ]->get_video_texture();
				textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_ps_mesh_masked() );
			}
			else
			{
				engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_camera_normal_and_depth_ps_mesh() );
			}

			engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( material->cull_mode, video_fill_type_e_face );

			for ( uint32_c draw_index = mesh->_source_mesh->draw_start; draw_index < mesh->_source_mesh->draw_end; draw_index++ )
			{
				resource_file_model_c::mesh_draw_c * draw = &model->_model_resource->_data.mesh_draw_list[ draw_index ];
				engine_c::get_instance()->get_video_interface()->bind_primitive_type( draw->get_video_primitive_type() );
				engine_c::get_instance()->get_video_interface()->draw_indexed( draw->index_start, draw->index_end - draw->index_start, 0 );
			}
		}

		core_linked_list_c< scene_component_model_c * >::node_c const * daughter_model_list_node = model->_daughter_model_list.get_first();
		while ( daughter_model_list_node != nullptr )
		{
			_draw_model_for_shadow( daughter_model_list_node->get_value(), view_position );
			daughter_model_list_node = daughter_model_list_node->get_next();
		}
	}

	/*
	void_c video_renderer_interface_c::_resolve_msaa_canvas( video_renderer_canvas_c * canvas )
	{
		assert( canvas->_msaa_count > 1 );

		video_texture_c * textures[8] = {};

		// resolve targets if needed
		_constant_buffers.camera.view_port_size.a = static_cast<float32_c>( canvas->_target_color_resolved->get_width() );
		_constant_buffers.camera.view_port_size.b = static_cast<float32_c>( canvas->_target_color_resolved->get_height() );
		_constant_buffers.camera.view_port_size_inverse.a = 1.0f / _constant_buffers.camera.view_port_size.a;
		_constant_buffers.camera.view_port_size_inverse.b = 1.0f / _constant_buffers.camera.view_port_size.b;
		_constant_buffers.camera_constant_buffer->set_data( &_constant_buffers.camera, sizeof( _constant_buffers.camera ) );

		//if ( canvas->_msaa_count == 1 )
		//{
		//	engine_instance.video_interface->bind_pixel_shader( shader_cache.scene_post_ps_resolve_msaa_count_1 );
		//}
		if ( canvas->_msaa_count == 2 )
		{
			engine_instance.video_interface->bind_pixel_shader( shader_cache.scene_post_ps_resolve_msaa_count_2 );
		}
		else if ( canvas->_msaa_count == 4 )
		{
			engine_instance.video_interface->bind_pixel_shader( shader_cache.scene_post_ps_resolve_msaa_count_4 );
		}
		else if ( canvas->_msaa_count == 8 )
		{
			engine_instance.video_interface->bind_pixel_shader( shader_cache.scene_post_ps_resolve_msaa_count_8 );
		}
		else
		{
			assert( false ); // something unanticipated.
		}

		// something to think about ...
		// maybe we could resolve both textures in one draw call by using multiple render targets ...
		// will need to write another pixel shader to do this though ...
		//textures[0] = canvas->_target_color_resolved;
		//textures[1] = canvas->_target_normal_and_depth_resolved;
		//video_interface->bind_target( 2, textures, nullptr );
		//textures[0] = canvas->_target_color;
		//textures[1] = canvas->_target_normal_and_depth;
		//video_interface->bind_pixel_shader_textures( _TextureIndex_MultiSampleCountColor, textures, 2 );
		//video_interface->draw( 0, 4 );

		// bind two inputs.
		textures[0] = canvas->_target_normal_and_depth;
		textures[1] = canvas->_target_color;
		engine_instance.video_interface->bind_pixel_shader_textures( texture_bind_index_for_view_textures, 2, textures );

		// bind two targets.
		textures[0] = canvas->_target_normal_and_depth_resolved;
		textures[1] = canvas->_target_color_resolved;
		engine_instance.video_interface->bind_target( 2, textures, nullptr );

		// resolve both textures with a single draw.
		engine_instance.video_interface->draw( 0, 4 );

		//engine_instance.video_interface->bind_target( 1, &canvas->_target_normal_and_depth_resolved, nullptr );
		//engine_instance.video_interface->bind_pixel_shader_textures( view_normal_depth_msaa_index, 1, &canvas->_target_normal_and_depth );
		//engine_instance.video_interface->draw( 0, 4 );

		//engine_instance.video_interface->bind_target( 1, &canvas->_target_color_resolved, nullptr );
		//engine_instance.video_interface->bind_pixel_shader_textures( view_normal_depth_msaa_index, 1, &canvas->_target_color );
		//engine_instance.video_interface->draw( 0, 4 );
	}
	*/

	void_c video_renderer_interface_c::_resolve_texture_full( video_texture_c * input, video_texture_c * output )
	{
		assert( input );
		assert( output );
		assert( input->get_multi_sample_count() == 1 ); // msaa not supported right now :( ... it used to be but i took it out. it creates a lot of special cases, it's probably better to just supersample instead.
		assert( output->get_multi_sample_count() == 1 );

		textures_to_bind[ 0 ] = output;
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( video_blend_type_e_set );
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_vs() );
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, &_quad_vertex_buffer );
		if ( input->get_width() == output->get_width() && input->get_height() == output->get_height() )
		{
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_resolve_native() );
		}
		else
		{
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_resolve_scaled() );
		}
		textures_to_bind[ 0 ] = input;
		textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
		engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_strip );
		engine_c::get_instance()->get_video_interface()->draw( 0, 4 );
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, nullptr, nullptr);
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none );
	}

	/*
	void_c video_renderer_interface_c::_resolve_texture_quarter( video_texture_c * input, video_texture_c * output )
	{
		assert( input );
		assert( output );
		assert( input->get_multi_sample_count() == 1 );
		assert( output->get_multi_sample_count() == 1 );

		_constant_buffers.camera_block->canvas_actual_size = vector32x2_c( static_cast< float32_c >( output->get_width() ), static_cast< float32_c >( output->get_height() ) );
		_constant_buffers.camera_block->canvas_actual_size_inverse = vector32x2_c( 1.0f / static_cast< float32_c >( output->get_width() ), 1.0f / static_cast< float32_c >( output->get_height() ) );
		_constant_buffers.camera_block_constant_buffer->set_data( _constant_buffers.camera_block, sizeof( camera_block_c ) );

		textures_to_bind[ 0 ] = output;
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( video_blend_type_e_set );
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_vs() );
		vertex_buffers_to_bind[ 0 ] = _quad_vertex_buffer;
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_scene_post_ps_resolve_quarter() );
		textures_to_bind[ 0 ] = input;
		textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, textures_to_bind, textures_to_bind_types );
		engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_strip );
		engine_c::get_instance()->get_video_interface()->draw( 0, 4 );

		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_material_textures, 1, nullptr, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none );
	}
	*/

	void_c video_renderer_interface_c::add_debug_garbage( sint32_c count )
	{
		for ( sint32_c i = 0; i < count; i++ )
		{
			add_debug_line( true, vector64x3_c( ops::random_float32( -1.0f, 1.0f ), ops::random_float32( -1.0f, 1.0f ), ops::random_float32( -1.0f, 1.0f ) ), vector64x3_c( ops::random_float32( -1.0f, 1.0f ), ops::random_float32( -1.0f, 1.0f ), ops::random_float32( -1.0f, 1.0f ) ), vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
		}
	}

	void_c video_renderer_interface_c::add_debug_line( boolean_c xray, vector64x3_c const & a_position, vector64x3_c const & b_position, vector32x4_c const & a_and_b_color )
	{
		video_renderer_vertex_debug_64_c * vertices;
		if ( xray )
		{
			vertices = _debug_line_list_xray_64.emplace_range_at_end( 2 );
		}
		else
		{
			vertices = _debug_line_list_64.emplace_range_at_end( 2 );
		}
		vertices[ 0 ].position = a_position;
		vertices[ 0 ].color = a_and_b_color;
		vertices[ 1 ].position = b_position;
		vertices[ 1 ].color = a_and_b_color;
	}

	void_c video_renderer_interface_c::add_debug_line( boolean_c xray, vector64x3_c const & a_position, vector64x3_c const & b_position, vector32x4_c const & a_color, vector32x4_c const & b_color )
	{
		video_renderer_vertex_debug_64_c * vertices;
		if ( xray )
		{
			vertices = _debug_line_list_xray_64.emplace_range_at_end( 2 );
		}
		else
		{
			vertices = _debug_line_list_64.emplace_range_at_end( 2 );
		}
		vertices[ 0 ].position = a_position;
		vertices[ 0 ].color = a_color;
		vertices[ 1 ].position = b_position;
		vertices[ 1 ].color = b_color;
	}

	void_c video_renderer_interface_c::add_debug_box( boolean_c xray, box64x3_c const & box, vector32x4_c const & color )
	{
		vector64x3_c minimum_minimum_minimum = box.minimum;
		vector64x3_c minimum_minimum_maximum = vector64x3_c( box.minimum.a, box.minimum.b, box.maximum.c );
		vector64x3_c minimum_maximum_minimum = vector64x3_c( box.minimum.a, box.maximum.b, box.minimum.c );
		vector64x3_c minimum_maximum_maximum = vector64x3_c( box.minimum.a, box.maximum.b, box.maximum.c );
		vector64x3_c maximum_minimum_minimum = vector64x3_c( box.maximum.a, box.minimum.b, box.minimum.c );
		vector64x3_c maximum_minimum_maximum = vector64x3_c( box.maximum.a, box.minimum.b, box.maximum.c );
		vector64x3_c maximum_maximum_minimum = vector64x3_c( box.maximum.a, box.maximum.b, box.minimum.c );
		vector64x3_c maximum_maximum_maximum = box.maximum;

		add_debug_line( xray, minimum_minimum_minimum, minimum_minimum_maximum, color );
		add_debug_line( xray, minimum_minimum_maximum, maximum_minimum_maximum, color );
		add_debug_line( xray, maximum_minimum_maximum, maximum_minimum_minimum, color );
		add_debug_line( xray, maximum_minimum_minimum, minimum_minimum_minimum, color );

		add_debug_line( xray, minimum_maximum_minimum, minimum_maximum_maximum, color );
		add_debug_line( xray, minimum_maximum_maximum, maximum_maximum_maximum, color );
		add_debug_line( xray, maximum_maximum_maximum, maximum_maximum_minimum, color );
		add_debug_line( xray, maximum_maximum_minimum, minimum_maximum_minimum, color );

		add_debug_line( xray, minimum_minimum_minimum, minimum_maximum_minimum, color );
		add_debug_line( xray, minimum_minimum_maximum, minimum_maximum_maximum, color );
		add_debug_line( xray, maximum_minimum_maximum, maximum_maximum_maximum, color );
		add_debug_line( xray, maximum_minimum_minimum, maximum_maximum_minimum, color );
	}

	void_c video_renderer_interface_c::add_debug_box( boolean_c xray, box64x3_c const & box, space_transform_c & box_transform, vector32x4_c const & color )
	{
		
		vector64x3_c minimum_minimum_minimum = ops::make_vector64x3_transformed_point( box.minimum, box_transform );;
		vector64x3_c minimum_minimum_maximum = ops::make_vector64x3_transformed_point( vector64x3_c( box.minimum.a, box.minimum.b, box.maximum.c ), box_transform );
		vector64x3_c minimum_maximum_minimum = ops::make_vector64x3_transformed_point( vector64x3_c( box.minimum.a, box.maximum.b, box.minimum.c ), box_transform );
		vector64x3_c minimum_maximum_maximum = ops::make_vector64x3_transformed_point( vector64x3_c( box.minimum.a, box.maximum.b, box.maximum.c ), box_transform );
		vector64x3_c maximum_minimum_minimum = ops::make_vector64x3_transformed_point( vector64x3_c( box.maximum.a, box.minimum.b, box.minimum.c ), box_transform );
		vector64x3_c maximum_minimum_maximum = ops::make_vector64x3_transformed_point( vector64x3_c( box.maximum.a, box.minimum.b, box.maximum.c ), box_transform );
		vector64x3_c maximum_maximum_minimum = ops::make_vector64x3_transformed_point( vector64x3_c( box.maximum.a, box.maximum.b, box.minimum.c ), box_transform );
		vector64x3_c maximum_maximum_maximum = ops::make_vector64x3_transformed_point( box.maximum, box_transform );

		add_debug_line( xray, minimum_minimum_minimum, minimum_minimum_maximum, color );
		add_debug_line( xray, minimum_minimum_maximum, maximum_minimum_maximum, color );
		add_debug_line( xray, maximum_minimum_maximum, maximum_minimum_minimum, color );
		add_debug_line( xray, maximum_minimum_minimum, minimum_minimum_minimum, color );

		add_debug_line( xray, minimum_maximum_minimum, minimum_maximum_maximum, color );
		add_debug_line( xray, minimum_maximum_maximum, maximum_maximum_maximum, color );
		add_debug_line( xray, maximum_maximum_maximum, maximum_maximum_minimum, color );
		add_debug_line( xray, maximum_maximum_minimum, minimum_maximum_minimum, color );

		add_debug_line( xray, minimum_minimum_minimum, minimum_maximum_minimum, color );
		add_debug_line( xray, minimum_minimum_maximum, minimum_maximum_maximum, color );
		add_debug_line( xray, maximum_minimum_maximum, maximum_maximum_maximum, color );
		add_debug_line( xray, maximum_minimum_minimum, maximum_maximum_minimum, color );
	}

	void_c video_renderer_interface_c::add_debug_axes( boolean_c xray, scene_matrix_c & transform, float32_c scale, float32_c alpha )
	{
		add_debug_line( xray, transform.position, transform.position + vector64x3_c( transform.basis.a ) * scale, vector32x4_c( 1.000f, 0.279f, 0.000f, alpha ) );
		add_debug_line( xray, transform.position, transform.position + vector64x3_c( transform.basis.b ) * scale, vector32x4_c( 0.671f, 1.000f, 0.000f, alpha ) );
		add_debug_line( xray, transform.position, transform.position + vector64x3_c( transform.basis.c ) * scale, vector32x4_c( 0.000f, 0.893f, 1.000f, alpha ) );
	}

	void_c video_renderer_interface_c::add_debug_axes( boolean_c xray, space_transform_c & transform, float32_c scale, float32_c alpha )
	{
		vector64x3_c position = transform.position;
		matrix32x3x3_c basis = transform.get_scaled_basis();
		add_debug_line( xray, position, position + vector64x3_c( basis.a ) * scale, vector32x4_c( 1.000f, 0.279f, 0.000f, alpha ) );
		add_debug_line( xray, position, position + vector64x3_c( basis.b ) * scale, vector32x4_c( 0.671f, 1.000f, 0.000f, alpha ) );
		add_debug_line( xray, position, position + vector64x3_c( basis.c ) * scale, vector32x4_c( 0.000f, 0.893f, 1.000f, alpha ) );
	}

	//sint32_c video_renderer_interface_c::get_shadow_view_count()
	//{
	//	return _shadow_view_count;
	//}

	//void_c video_renderer_interface_c::set_shadow_view_count( sint32_c value )
	//{
	//	_shadow_view_count = ops::clamp( value, 1, _shadows_per_frame_count );
	//}

	//float32_c video_renderer_interface_c::get_voxel_tree_size()
	//{
	//	return _voxel_tree_size;
	//}

	//void_c video_renderer_interface_c::set_voxel_tree_size( float32_c value )
	//{
	//	_voxel_tree_size = clamp( value, 1.0f, float32_maximum );
	//}

	//sint32_c video_renderer_interface_c::get_voxel_tree_depth()
	//{
	//	return _voxel_tree_depth;
	//}

	//void_c video_renderer_interface_c::set_voxel_tree_depth( sint32_c value )
	//{
	//	_voxel_tree_depth = clamp( value, 1, 12 );
	//	_voxel_tree_resolution = power( 2, _voxel_tree_depth );
	//}

	boolean_c video_renderer_interface_c::get_menu_draw_debug()
	{
		return _menu_draw_debug;
	}

	void_c video_renderer_interface_c::set_menu_draw_debug( boolean_c value )
	{
		_menu_draw_debug = value;
	}

	void_c video_renderer_interface_c::pre_render_menus( user_interface_c * user_interface )
	{
		_menu_root_control_group_list.remove_all();
		_menu_draw_list_list.remove_all();
		_menu_vertex_list.remove_all();
		_menu_index_list.remove_all();

		// the controls that count as root controls of control groups have ( _mother_control == nullptr || _local_color.d < 1.0f ).
		// the controls that will render are ones that have ( _is_showing_weight > 0.0f && _local_color.d > 0.0f ).
		// the elements that will render are ones that have ( element->_is_showing && element->_is_showing_from_style ).

		// gather control groups.
		for ( sint32_c i = 0; i < user_interface->_control_list.get_length(); i++ )
		{
			user_interface->_control_list[ i ]->_compile_control_groups( _menu_root_control_group_list, _menu_draw_list_list );
		}

		// compile big lists and base offsets.
		for ( sint32_c i = 0; i < _menu_draw_list_list.get_length(); i++ )
		{
			menu_draw_list_c * draw_list = _menu_draw_list_list[ i ];
			draw_list->vertex_base = _menu_vertex_list.get_length();
			_menu_vertex_list.insert_at_end( draw_list->vertex_list.get_internal_array(), draw_list->vertex_list.get_length() );
			draw_list->index_base = _menu_index_list.get_length();
			_menu_index_list.insert_at_end( draw_list->index_list.get_internal_array(), draw_list->index_list.get_length() );
		}

		// if there's nothing going on then we can return now.
		if ( _menu_vertex_list.get_length() == 0 )
		{
			return;
		}

		// upload vertices to vertex buffer.
		if ( _menu_vertex_buffer == nullptr || _menu_vertex_list.get_length() > _menu_vertex_buffer->get_vertex_count() )
		{
			if ( _menu_vertex_buffer != nullptr )
			{
				delete _menu_vertex_buffer;
				_menu_vertex_buffer = nullptr;
			}
			_menu_vertex_buffer = engine_c::get_instance()->get_video_interface()->create_vertex_buffer( &video_renderer_interface_c::vertex_format_menu, _menu_vertex_list.get_length_allocated(), nullptr, 0, true, false, false );
		}
		_menu_vertex_buffer->set_data( _menu_vertex_list.get_internal_array(), _menu_vertex_list.get_internal_array_size_used() );

		// upload indices to index buffer.
		if ( _menu_index_buffer == nullptr || _menu_index_list.get_length() > _menu_index_buffer->get_index_count() )
		{
			if ( _menu_index_buffer != nullptr )
			{
				delete _menu_index_buffer;
				_menu_index_buffer = nullptr;
			}
			_menu_index_buffer = engine_c::get_instance()->get_video_interface()->create_index_buffer( video_index_format_e_uint16, _menu_index_list.get_length_allocated(), nullptr, 0, true, false );
		}
		_menu_index_buffer->set_data( _menu_index_list.get_internal_array(), _menu_index_list.get_internal_array_size_used() );

		// update and bind glyph map texture.
		engine_c::get_instance()->get_glyph_manager()->update_glyph_map_texture();
		textures_to_bind[ 0 ] = engine_c::get_instance()->get_glyph_manager()->_glyph_atlas_texture;
		textures_to_bind_types[ 0 ] = video_texture_type_e_texture2darray;
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( _texture_bind_index_for_glyph_atlas_texture, 1, textures_to_bind, textures_to_bind_types );

		// bind render states.
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( video_blend_type_e_mix );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_menu2_vs() );
		vertex_buffers_to_bind[ 0 ] = _menu_vertex_buffer;
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
		engine_c::get_instance()->get_video_interface()->bind_index_buffer( _menu_index_buffer );
		engine_c::get_instance()->get_video_interface()->bind_primitive_type( video_primitive_type_e_triangle_list );

		// render control groups, in reverse order, so that it goes depth first.
		// so that deeper level control groups finish rendering first.
		// so that they can copy the results of their renders to the control group render targets of higher level control groups.
		for ( sint32_c i = _menu_root_control_group_list.get_length() - 1; i >= 0; i-- )
		{
			menu_control_c * control_group = _menu_root_control_group_list[ i ];
			_render_control_for_control_group( control_group );
		}

		// unbind any residual states.
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( 0, 1, nullptr, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none );
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 0, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_index_buffer( nullptr );
	}

	void_c video_renderer_interface_c::_render_control_for_control_group( menu_control_c * control )
	{
		assert( control && control->_is_showing_weight >= 0.0f && control->_local_color.d >= 0.0f );

		static core_list_c< vector32x4_c > clip_planes;

		// clear and bind control group texture as target if this control is a root of a control group.
		// as this function recurses, this render state will be preserved.
		if ( control->_control_group_is_root )
		{
			assert( control->_control_group_texture != nullptr );
			engine_c::get_instance()->get_video_interface()->clear_texture( control->_control_group_texture, 1.0f, 1.0f, 1.0f, 0.0f );
			textures_to_bind[ 0 ] = control->_control_group_texture;
			engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
			_constant_buffers.menu_block->menu_target_size_inverse = vector32x2_c( 1.0f / static_cast< float32_c >( control->_control_group_texture->get_width() ), 1.0f / static_cast< float32_c >( control->_control_group_texture->get_height() ) );
			_constant_buffers.menu_block->menu_target_offset = -control->_local_box.minimum;
			_constant_buffers.menu_block_constant_buffer->set_data( _constant_buffers.menu_block, sizeof( menu_block_c ) );
		}

		// bind constants.
		clip_planes.remove_all();
		control->get_control_group_clip_planes( clip_planes );
		matrix32x2x2_c control_group_basis = control->get_control_group_basis();
		vector32x2_c control_group_origin = control->get_control_group_origin();
		_constant_buffers.menu_batch_block->menu_basis = control_group_basis.as_vector32x4();
		_constant_buffers.menu_batch_block->menu_origin = control_group_origin;
		_constant_buffers.menu_batch_block->menu_color = control->_control_group_color;
		_constant_buffers.menu_batch_block->menu_saturation = 1.0f;
		_constant_buffers.menu_batch_block->menu_clip_plane_stack_length = ops::math_minimum( clip_planes.get_length(), menu_clip_plane_stack_count );
		for ( sint32_c i = 0; i < _constant_buffers.menu_batch_block->menu_clip_plane_stack_length; i++ )
		{
			_constant_buffers.menu_batch_block->menu_clip_plane_stack[ i ] = clip_planes[ i ];
		}
		_constant_buffers.menu_batch_block_constant_buffer->set_data( _constant_buffers.menu_batch_block, sizeof( menu_batch_block_c ) );

		// render elements that are part of this control group.
		for ( sint32_c i = 0; i < control->_element_list.get_length(); i++ )
		{
			menu_element_c const * element = control->_element_list[ i ];
			if ( element->_is_showing && element->_is_showing_from_style )
			{
				menu_draw_list_c const & draw_list = element->get_draw_list();
				_render_menu_draw_list( draw_list );
			}
		}

		// render daughter controls that are part of this control group.
		for ( sint32_c i = 0; i < control->_control_list.get_length(); i++ )
		{
			menu_control_c * daughter_control = control->_control_list[ i ];
			if ( daughter_control->_is_showing_weight > 0.0f && daughter_control->_local_color.d > 0.0f )
			{
				if ( !daughter_control->_control_group_is_root )
				{
					_render_control_for_control_group( daughter_control );
				}
				else
				{
					_constant_buffers.menu_batch_block->menu_basis = ( control_group_basis * daughter_control->_local_basis ).as_vector32x4();
					_constant_buffers.menu_batch_block->menu_origin = control_group_origin + daughter_control->_local_origin;
					_constant_buffers.menu_batch_block->menu_color = control->_control_group_color * daughter_control->_local_color;
					_constant_buffers.menu_batch_block->menu_color.d *= daughter_control->_is_showing_weight;
					_constant_buffers.menu_batch_block->menu_clip_plane_stack_length = 0;
					_constant_buffers.menu_batch_block_constant_buffer->set_data( _constant_buffers.menu_batch_block, sizeof( menu_batch_block_c ) );
					_render_menu_draw_list( daughter_control->_control_group_draw_list );
				}
			}
		}

		// unbind control group target texture, so that it can be bound as input at later stage.
		if ( control->_control_group_is_root )
		{
			engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_texture2d );
		}
	}

	void_c video_renderer_interface_c::_render_menu_draw_list( menu_draw_list_c const & draw_list )
	{
		for ( sint32_c i = 0; i < draw_list.draw_list.get_length(); i++ )
		{
			menu_draw_list_c::draw_c const & draw = draw_list.draw_list[ i ];
			assert( draw.pixel_shader );
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader( draw.pixel_shader );
			textures_to_bind[ 0 ] = draw.texture ? draw.texture->get_video_texture() : texture_green_pixel;
			textures_to_bind_types[ 0 ] = video_texture_type_e_texture2d;
			engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( 0, 1, textures_to_bind, textures_to_bind_types );
			engine_c::get_instance()->get_video_interface()->draw_indexed( draw_list.index_base + draw.index_start, draw.index_count, draw_list.vertex_base );
		}
	}

	void_c video_renderer_interface_c::render_2d_menus( user_interface_c * user_interface )
	{
		if ( _menu_vertex_list.get_length() == 0 )
		{
			return;
		}

		video_renderer_canvas_c const * canvas = user_interface->_canvas_and_output;
		video_texture_c * output_target = canvas->_output != nullptr ? canvas->_output->get_texture_resource() : canvas->_target_color_final;
		assert( output_target );

		_constant_buffers.menu_block->menu_target_size_inverse = vector32x2_c( 1.0f / static_cast< float32_c >( output_target->get_width() ), 1.0f / static_cast< float32_c >( output_target->get_height() ) );
		_constant_buffers.menu_block->menu_target_offset = vector32x2_c( 0.0f, 0.0f );
		_constant_buffers.menu_block_constant_buffer->set_data( _constant_buffers.menu_block, sizeof( menu_block_c ) );

		textures_to_bind[ 0 ] = output_target;
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 1, textures_to_bind, nullptr, video_texture_type_e_texture2d );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_blend_state( video_blend_type_e_mix );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_cull_fill_state( video_cull_type_e_none, video_fill_type_e_face );
		engine_c::get_instance()->get_video_interface()->bind_rasterizer_depth_stencil_state( video_compare_function_e_disable, false );
		engine_c::get_instance()->get_video_interface()->bind_vertex_shader( engine_c::get_instance()->get_video_renderer_shader_manager()->get_menu2_vs() );
		vertex_buffers_to_bind[ 0 ] = _menu_vertex_buffer;
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 1, vertex_buffers_to_bind );
		engine_c::get_instance()->get_video_interface()->bind_index_buffer( _menu_index_buffer );
		for ( sint32_c i = 0; i < user_interface->_control_list.get_length(); i++ )
		{
			menu_control_c * control = user_interface->_control_list[ i ];
			if ( control->_scene_component == nullptr && control->_is_showing_weight > 0.0f && control->_local_color.d > 0.0f )
			{
				assert( control->_control_group_draw_list.draw_list.get_length() == 1 );
				_constant_buffers.menu_batch_block->menu_basis = control->_global_basis.as_vector32x4();
				_constant_buffers.menu_batch_block->menu_origin = control->_global_origin;
				_constant_buffers.menu_batch_block->menu_color = control->_local_color;
				_constant_buffers.menu_batch_block->menu_color.d *= control->_is_showing_weight;
				_constant_buffers.menu_batch_block->menu_saturation = 1.0f;
				_constant_buffers.menu_batch_block->menu_clip_plane_stack_length = 0;
				_constant_buffers.menu_batch_block_constant_buffer->set_data( _constant_buffers.menu_batch_block, sizeof( menu_batch_block_c ) );
				_render_menu_draw_list( control->_control_group_draw_list );
			}
		}

		// unbind.
		engine_c::get_instance()->get_video_interface()->bind_pixel_shader_textures( 0, 1, nullptr, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_target_textures( 0, nullptr, nullptr, video_texture_type_e_none );
		engine_c::get_instance()->get_video_interface()->bind_vertex_buffers( 0, nullptr );
		engine_c::get_instance()->get_video_interface()->bind_index_buffer( nullptr );
	}

	void_c video_renderer_interface_c::set_settings( settings_c const & desired_settings )
	{
		// validate desired settings.
		// if anything is out of range, then bring it in range.
		// if anything is incorrect, then correct it.
		// if anything is undefined, then define it.
		settings_c settings = desired_settings;
		settings.full_screen_display = 0;
		if ( settings.presentation_mode <= 0 || settings.presentation_mode > settings_c::presentation_mode_e_count_ )
		{
			settings.presentation_mode = settings_c::presentation_mode_e_immediate;
		}
		if ( settings.post_process_quality <= 0 || settings.post_process_quality > settings_c::post_process_quality_e_count_ )
		{
			settings.post_process_quality = settings_c::post_process_quality_e_high;
		}
		if ( settings.texture_resolution <= 0 || settings.texture_resolution > settings_c::texture_resolution_e_count_ )
		{
			settings.texture_resolution = settings_c::texture_resolution_e_whole;
		}
		if ( settings.texture_sample_quality <= 0 || settings.texture_sample_quality > settings_c::texture_sample_quality_e_count_ )
		{
			settings.texture_sample_quality = settings_c::texture_sample_quality_e_trilinear;
		}
		if ( settings.light_count <= 0 || settings.light_count > settings_c::light_count_e_count_ )
		{
			settings.light_count = settings_c::light_count_e_64;
		}
		if ( settings.light_probe_resolution <= 0 || settings.light_probe_resolution > settings_c::light_probe_resolution_e_count_ )
		{
			settings.light_probe_resolution = settings_c::light_probe_resolution_e_count_;
		}
		if ( settings.shadow_view_count <= 0 || settings.shadow_view_count > settings_c::shadow_view_count_e_count_ )
		{
			settings.shadow_view_count = settings_c::shadow_view_count_e_16;
		}
		if ( settings.shadow_resolution <= 0 || settings.shadow_resolution > settings_c::shadow_resolution_e_count_ )
		{
			settings.shadow_resolution = settings_c::shadow_resolution_e_1024;
		}
		if ( settings.shadow_sample_quality <= 0 || settings.shadow_sample_quality > settings_c::shadow_sample_quality_e_count_ )
		{
			settings.shadow_sample_quality = settings_c::shadow_sample_quality_e_9x;
		}

		// make the changes.
		if ( _settings.light_count != settings.light_count )
		{
			sint32_c light_count = settings_c::get_light_count( settings.light_count );
			if ( _texture_buffers.lights_texture_buffer )
			{
				delete _texture_buffers.lights_texture_buffer;
				_texture_buffers.lights_texture_buffer = nullptr;
			}
			_texture_buffers.lights_texture_buffer = engine_c::get_instance()->get_video_interface()->create_texture_buffer( sizeof( video_renderer_light_c ) * light_count );
			assert( _texture_buffers.lights_texture_buffer );
			_settings.light_count = settings.light_count;
		}

		if ( _settings.light_probe_resolution != settings.light_probe_resolution )
		{
			_settings.light_probe_resolution = settings.light_probe_resolution;
		}

		boolean_c reallocate_shadow_map_texture = ( _settings.shadow_resolution != settings.shadow_resolution );

		if ( _settings.shadow_view_count != settings.shadow_view_count )
		{
			reallocate_shadow_map_texture = true;

			sint32_c shadow_view_count = settings_c::get_shadow_view_count( settings.shadow_view_count );
			if ( _texture_buffers.shadow_views_texture_buffer )
			{
				delete _texture_buffers.shadow_views_texture_buffer;
				_texture_buffers.shadow_views_texture_buffer = nullptr;
			}
			_texture_buffers.shadow_views_texture_buffer = engine_c::get_instance()->get_video_interface()->create_texture_buffer( sizeof( shadow_view_c ) * shadow_view_count );
			assert( _texture_buffers.shadow_views_texture_buffer );
			_settings.shadow_view_count = settings.shadow_view_count;
		}

		if ( reallocate_shadow_map_texture )
		{
			sint32_c shadow_resolution = settings_c::get_shadow_resolution( settings.shadow_resolution );
			if ( _shadow_map_texture2darray )
			{
				delete _shadow_map_texture2darray;
				_shadow_map_texture2darray = nullptr;
			}
			sint32_c shadow_view_limit = settings_c::get_shadow_view_count( settings.shadow_view_count );
			_shadow_map_texture2darray = engine_c::get_instance()->get_video_interface()->create_texture( video_texture_format_e_r32_float, shadow_resolution, shadow_resolution, 1, shadow_view_limit, 0, 0, false, false, true, false );
			assert( _shadow_map_texture2darray );
			_settings.shadow_resolution = settings.shadow_resolution;
		}
	}

	void_c video_renderer_interface_c::_save_settings( data_scribe_ini_c & scribe )
	{
		string8_c value;

		_settings.reset_to_default();

		scribe.add_section( string8_c( mode_e_static, "video" ) );

		if ( settings_c::get_window_mode_string( _settings.window_mode, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "window_mode" ), value );
		}

		if ( settings_c::get_presentation_mode_string( _settings.presentation_mode, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "presentation_mode" ), value );
		}

		if ( settings_c::get_post_process_quality_string( _settings.post_process_quality, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "post_process_quality" ), value );
		}

		if ( settings_c::get_texture_resolution_string( _settings.texture_resolution, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "texture_resolution" ), value );
		}

		if ( settings_c::get_texture_sample_quality_string( _settings.texture_sample_quality, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "texture_sample_quality" ), value );
		}

		if ( settings_c::get_light_count_string( _settings.light_count, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "light_count" ), value );
		}

		if ( settings_c::get_light_probe_resolution_string( _settings.light_probe_resolution, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "light_probe_resolution" ), value );
		}

		if ( settings_c::get_shadow_view_count_string( _settings.shadow_view_count, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "shadow_view_count" ), value );
		}

		if ( settings_c::get_shadow_resolution_string( _settings.shadow_resolution, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "shadow_resolution" ), value );
		}

		if ( settings_c::get_shadow_sample_quality_string( _settings.shadow_sample_quality, value ) )
		{
			scribe.add_property( string8_c( mode_e_static, "shadow_sample_quality" ), value );
		}
	}

	void_c video_renderer_interface_c::_load_settings( data_scribe_ini_c & scribe )
	{
		settings_c new_settings;

		string8_c value;

		string8_c section( mode_e_static, "video" );

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "window_mode" ), value ) )
		{
			settings_c::get_window_mode_value( value, new_settings.window_mode );
		}

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "presentation_mode" ), value ) )
		{
			settings_c::get_presentation_mode_value( value, new_settings.presentation_mode );
		}

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "post_process_quality" ), value ) )
		{
			settings_c::get_post_process_quality_value( value, new_settings.post_process_quality );
		}

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "texture_resolution" ), value ) )
		{
			settings_c::get_texture_resolution_value( value, new_settings.texture_resolution );
		}

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "texture_sample_quality" ), value ) )
		{
			settings_c::get_texture_sample_quality_value( value, new_settings.texture_sample_quality );
		}

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "light_count" ), value ) )
		{
			settings_c::get_light_count_value( value, new_settings.light_count );
		}

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "light_probe_resolution" ), value ) )
		{
			settings_c::get_light_probe_resolution_value( value, new_settings.light_probe_resolution );
		}

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "shadow_view_count" ), value ) )
		{
			settings_c::get_shadow_view_count_value( value, new_settings.shadow_view_count );
		}

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "shadow_resolution" ), value ) )
		{
			settings_c::get_shadow_resolution_value( value, new_settings.shadow_resolution );
		}

		if ( scribe.find_property_value( section, string8_c( mode_e_static, "shadow_sample_quality" ), value ) )
		{
			settings_c::get_shadow_sample_quality_value( value, new_settings.shadow_sample_quality );
		}

		set_settings( new_settings );
	}

	/*
	void_c video_renderer_interface_c::initialize_mirror_perspective(
		plane64_c const & world_space_mirror_plane, // world-space reflection plane, that is facing the primary view.
		box64x3_c const & world_space_mirror_aabb, // world-space axis aligned bounding box of the mirror model, the plane itself is infinite, this box confines it.
		space_transform_c & at_transform, // the scene transform of the bounding box.
		view_c * result )
	{
		// how this works:
		// we build world-space corner points of world_space_mirror_aabb.
		// we build reflected view matrix, but position it at the world origin, but it's still right-handed (so it's not reflected in that respect).
		// we build reflected projection matrix using same parameters as the primary view's camera, but it's still right-handed (so it's not reflected in that respect).
		// we transform world-space corner points of world_space_mirror_aabb by the reflected view-projection matrix to get screen-space points.
		// we build minimum bounding rectangle with those screen space-points.
		// we build off-center reflected projection matrix, to fit around the minimum bounding rectangle.
		// we build a world-space frustum from the off-center projection matrix, and then translate it by the world-space reflected position.
		// we set the view-port to be the same as the minimum bounding rectangle.
		// we set depth test to equal (result from early-z-pre-pass already is already in the depth buffer).
		// we set stencil operation to increment.
		// we render the mirror geometry only (without it's material defined pixel shader), this gives us a stencil mask.
		// we set the stencil test to equal, set the stencil reference value to 1, this lets us render the scene geometry to only the pixels defined by the stencil mask.
		// we gather and render the models that are visible from the reflected point of view.

		assert( _view_list.get_length() > 0 );

		view_c * primary_view = &_view_list[ 0 ];

		vector64x3_c at_points[ 8 ];
		at_points[ 0 ] = at_transform.make_transformed_point( vector64x3_c( mirror_plane_world_space_aabb.minimum.a, mirror_plane_world_space_aabb.minimum.b, mirror_plane_world_space_aabb.minimum.c ) );
		at_points[ 1 ] = at_transform.make_transformed_point( vector64x3_c( mirror_plane_world_space_aabb.maximum.a, mirror_plane_world_space_aabb.minimum.b, mirror_plane_world_space_aabb.minimum.c ) );
		at_points[ 2 ] = at_transform.make_transformed_point( vector64x3_c( mirror_plane_world_space_aabb.minimum.a, mirror_plane_world_space_aabb.maximum.b, mirror_plane_world_space_aabb.minimum.c ) );
		at_points[ 3 ] = at_transform.make_transformed_point( vector64x3_c( mirror_plane_world_space_aabb.maximum.a, mirror_plane_world_space_aabb.maximum.b, mirror_plane_world_space_aabb.minimum.c ) );
		at_points[ 4 ] = at_transform.make_transformed_point( vector64x3_c( mirror_plane_world_space_aabb.minimum.a, mirror_plane_world_space_aabb.minimum.b, mirror_plane_world_space_aabb.maximum.c ) );
		at_points[ 5 ] = at_transform.make_transformed_point( vector64x3_c( mirror_plane_world_space_aabb.maximum.a, mirror_plane_world_space_aabb.minimum.b, mirror_plane_world_space_aabb.maximum.c ) );
		at_points[ 6 ] = at_transform.make_transformed_point( vector64x3_c( mirror_plane_world_space_aabb.minimum.a, mirror_plane_world_space_aabb.maximum.b, mirror_plane_world_space_aabb.maximum.c ) );
		at_points[ 7 ] = at_transform.make_transformed_point( vector64x3_c( mirror_plane_world_space_aabb.maximum.a, mirror_plane_world_space_aabb.maximum.b, mirror_plane_world_space_aabb.maximum.c ) );

		// get view projection transform for primary camera.
		//matrix64x4x4_c primary_camera_view_transform = primary_camera->get_view_transform();
		//matrix64x4x4_c primary_camera_projection_transform = primary_camera->get_projection_transform( primary_canvas_size );
		//matrix64x4x4_c primary_camera_view_projection_transform = primary_camera_view_transform * primary_camera_projection_transform;

		// convert 3d corner points of box to 2d points in normalized view space.
		vector64x4_c at_points_view[ 8 ];
		for ( int i = 0; i < 8; i++ )
		{
			vector64x3_c & at_point = at_points[ i ];
			at_points_view[ i ] = ops::make_vector32x4_transformed_point( vector64x4_c( at_point.a, at_point.b, at_point.c, 1.0 ), _current_view_primary->real_view_projection_transform );
			at_points_view[ i ].a /= at_points_view[ i ].d;
			at_points_view[ i ].b /= at_points_view[ i ].d;
			at_points_view[ i ].c /= at_points_view[ i ].d;
		}

		// figure out 2d bounding rectangle around 2d points.
		// this scissor rectangle is in a unit space, it's coordinates can range from -1 to 1.
		// (-1, -1) is the top left of the canvas, (1, 1) is the bottom right of the canvas.
		box64x2_c scissor_rectangle;
		scissor_rectangle.minimum.a = at_points_view[ 0 ].a;
		scissor_rectangle.minimum.b = at_points_view[ 0 ].b;
		scissor_rectangle.maximum.a = at_points_view[ 0 ].a;
		scissor_rectangle.maximum.b = at_points_view[ 0 ].b;
		for ( int i = 1; i < 8; i++ )
		{
			if ( at_points_view[ i ].a < scissor_rectangle.minimum.a )
			{
				scissor_rectangle.minimum.a = at_points_view[ i ].a;
			}
			else if ( at_points_view[ i ].a > scissor_rectangle.maximum.a )
			{
				scissor_rectangle.maximum.a = at_points_view[ i ].a;
			}
			if ( at_points_view[ i ].b < scissor_rectangle.minimum.b )
			{
				scissor_rectangle.minimum.b = at_points_view[ i ].b;
			}
			else if ( at_points_view[ i ].b > scissor_rectangle.maximum.b )
			{
				scissor_rectangle.maximum.b = at_points_view[ i ].b;
			}
		}

		// clamp to range (-1 ... 1), be cause any thing out side of this range is out side of the view port and is going to be clipped any way.
		scissor_rectangle.minimum.a = ops::clamp( scissor_rectangle.minimum.a, -1.0, 1.0 );
		scissor_rectangle.minimum.b = ops::clamp( scissor_rectangle.minimum.b, -1.0, 1.0 );
		scissor_rectangle.maximum.a = ops::clamp( scissor_rectangle.maximum.a, -1.0, 1.0 );
		scissor_rectangle.maximum.b = ops::clamp( scissor_rectangle.maximum.b, -1.0, 1.0 );

		// make_reflected every basis vector and position to the other side of the mirror plane.
		result->real_world_transform.a = ops::make_reflected_vector( ops::make_normalized( _current_view_primary->real_world_transform.a ), mirror_plane.get_normal() ); // right.
		result->real_world_transform.b = ops::make_reflected_vector( ops::make_normalized( _current_view_primary->real_world_transform.b ), mirror_plane.get_normal() ); // up.
		result->real_world_transform.c = ops::make_reflected_vector( ops::make_normalized( _current_view_primary->real_world_transform.c ), mirror_plane.get_normal() ); // forward.
		result->real_world_transform.d = ops::make_reflected_vector( _current_view_primary->real_world_transform.d, mirror_plane );

		// make the view matrix as normal, but negate its x axis so that it is left handed instead of right handed.
		result->real_view_transform = ops::make_view( result->real_world_transform.d, result->real_world_transform.c, result->real_world_transform.b );
		result->real_view_transform.a.a = -result->real_view_transform.a.a; // is this right? we negate the column?
		result->real_view_transform.b.a = -result->real_view_transform.b.a;
		result->real_view_transform.c.a = -result->real_view_transform.c.a;

		//ops_make_matrix_view( result_view_transform, vector32x3_c( 0.0f, 0.0f, 0.0f ), vector32x3_c( result_scene_transform.c ), vector32x3_c( result_scene_transform.b ) );
		//result_view_transform.a.a = -result_view_transform.a.a;
		//result_view_transform.b.a = -result_view_transform.b.a;
		//result_view_transform.c.a = -result_view_transform.c.a;

		// make the offset projection matrix, using result_view_port as the area.
		matrix64x4x4_c real_projection_transform;
		float32_c aspect_ratio = static_cast< float32_c >( _current_view_primary->canvas->get_actual_width() ) / static_cast< float32_c >( _current_view_primary->canvas->get_actual_height() );
		if ( _current_view_primary->camera->get_projection_type() == scene_projection_type_e_perspective )
		{
			//ops_make_matrix_projection_perspective_off_center(
			//	result_projection_transform,
			//	result_view_port.minimum.a * 0.5f * primary_camera->_perspective_near_clip * aspect_ratio,
			//	result_view_port.maximum.a * 0.5f * primary_camera->_perspective_near_clip * aspect_ratio,
			//	result_view_port.minimum.b * 0.5f * primary_camera->_perspective_near_clip,
			//	result_view_port.maximum.b * 0.5f * primary_camera->_perspective_near_clip,
			//	primary_camera->_perspective_near_clip,
			//	primary_camera->_perspective_far_clip );
			real_projection_transform = ops::make_projection_perspective(
				_current_view_primary->camera->_perspective_field_of_view,
				static_cast< float32_c >( _current_view_primary->canvas->get_actual_width() ) / static_cast< float32_c >( _current_view_primary->canvas->get_actual_height() ),
				_current_view_primary->camera->get_perspective_clip_near(),
				_current_view_primary->camera->get_perspective_clip_far() );
		}
		else if ( _current_view_primary->camera->get_projection_type() == scene_projection_type_e_orthographic )
		{
			//ops_make_matrix_projection_orthographic_off_center(
			//	result_projection_transform,
			//	result_view_port.minimum.a * 0.5f * primary_camera->_orthographic_field_of_view * aspect_ratio,
			//	result_view_port.maximum.a * 0.5f * primary_camera->_orthographic_field_of_view * aspect_ratio,
			//	result_view_port.minimum.b * 0.5f * primary_camera->_orthographic_field_of_view,
			//	result_view_port.maximum.b * 0.5f * primary_camera->_orthographic_field_of_view,
			//	primary_camera->_orthographic_near_clip,
			//	primary_camera->_orthographic_far_clip );
			real_projection_transform = ops::make_projection_orthographic(
				_current_view_primary->camera->_orthographic_field_of_view,
				static_cast< float32_c >( _current_view_primary->canvas->get_actual_width() ) / static_cast< float32_c >( _current_view_primary->canvas->get_actual_height() ),
				_current_view_primary->camera->get_orthographic_clip_near(),
				_current_view_primary->camera->get_orthographic_clip_far() );
		}

		// make scene space frustum, which can be used to gather objects later.
		result->real_view_projection_transform = result->real_view_transform * real_projection_transform;
		result->real_frustum = ops::make_frustum_from_view_projection( result->real_view_projection_transform );

		// finally, scale the result view port from view space (-1 ... 1) with y coordinates ascending upward to uv space (0 ... 1) with origin at top left.
		result->scissor_rectangle[ 0 ] = static_cast< sint32_c >( ( scissor_rectangle.minimum.a *  0.5f + 0.5f ) * static_cast< float32_c >( _current_view_primary->canvas->get_actual_width() ) );
		result->scissor_rectangle[ 1 ] = static_cast< sint32_c >( ( scissor_rectangle.minimum.a * -0.5f + 0.5f ) * static_cast< float32_c >( _current_view_primary->canvas->get_actual_height() ) );
		result->scissor_rectangle[ 2 ] = static_cast< sint32_c >( ( scissor_rectangle.maximum.a *  0.5f + 0.5f ) * static_cast< float32_c >( _current_view_primary->canvas->get_actual_width() ) );
		result->scissor_rectangle[ 3 ] = static_cast< sint32_c >( ( scissor_rectangle.minimum.a * -0.5f + 0.5f ) * static_cast< float32_c >( _current_view_primary->canvas->get_actual_height() ) );
	}
	*/

}
