#pragma once

#include "cheonsa__types.h"
#include "cheonsa_video_interface.h"

namespace cheonsa
{

	class video_renderer_interface_c;

#pragma pack( push, 1 )

	// gpu shader compatible light structure.
	// 64 bytes.
	class video_renderer_light_c
	{
	public:
		float32_c light_type;
		float32_c shadow_view_start;
		float32_c shadow_view_count;
		float32_c unused;
		vector32x4_c position_range; // position of light in world space, and range of light for point lights and spot lights.
		vector32x4_c direction_angle; // direction of light for directional lights and spot lights, and cone angle for spot lights.
		vector32x4_c energy_unused; // red green blue energy, and unused.
	};
	static_assert( sizeof( video_renderer_light_c ) == 64, "size of video_renderer_light_c is wrong." );

	// gpu shader compatible shadow view structure.
	// 68 bytes.
	struct video_renderer_shadow_view_c
	{
		matrix32x4x4_c view_projection_transform;
		vector32x3_c position;
		float32_c unused;
	};
	static_assert( sizeof( video_renderer_shadow_view_c ) == 80, "size of video_renderer_shadow_view_c is wrong." );

	// gpu shader compatible vertex structure.
	// used by itself for static meshes.
	// used in tandem with video_renderer_vertex_weight_c for gpu skinned meshes.
	class video_renderer_vertex_mesh_base_c
	{
	public:
		vector32x3_c position;
		vector32x3_c normal;
		vector32x3_c normal_u;
		vector32x3_c normal_v;
		vector32x4_c texture;
	};
	static_assert( sizeof( video_renderer_vertex_mesh_base_c ) == 64, "size of video_renderer_vertex_mesh_base_c is wrong." );

	// gpu shader compatible vertex structure.
	// used for gpu skinned meshes.
	class video_renderer_vertex_mesh_bone_weight_c
	{
		uint16_c bone_weights[ 4 ];
		uint16_c bone_indices[ 4 ];
	};
	static_assert( sizeof( video_renderer_vertex_mesh_bone_weight_c ) == 16, "size of video_renderer_vertex_mesh_bone_weight_c is wrong." );

	// gpu shader compatible vertex structure.
	class video_renderer_vertex_menu_c
	{
	public:
		vector32x3_c position;
		vector32x3_c texture; // ab defines texture coordinates, c defines texture slice index to sample from in the case of rendering text.
		vector32x4_c color;
		vector32x4_c parameters; // defines glyph skew, weight, blur, and font texture array index.
	};
	static_assert( sizeof( video_renderer_vertex_menu_c ) == 56, "size of video_renderer_vertex_menu_c is wrong." );

	// debug vertices are initially created in scene space so they need to have 64-bit precision.
	// they are then translated to be relative to the camera and converted to 32-bit when it comes time to render them.
	class video_renderer_vertex_debug_64_c
	{
	public:
		vector64x3_c position;
		vector32x4_c color;
	};
	static_assert( sizeof( video_renderer_vertex_debug_64_c ) == 40, "size of video_renderer_vertex_debug_64_c is wrong." );

	// gpu shader compatible vertex structure.
	class video_renderer_vertex_debug_c
	{
	public:
		vector32x3_c position;
		vector32x4_c color;
	};
	static_assert( sizeof( video_renderer_vertex_debug_c ) == 28, "size of video_renderer_vertex_debug_c is wrong." );

	// gpu shader compatible vertex structure.
	class video_renderer_vertex_point_c
	{
	public:
		vector32x3_c position;
	};
	static_assert( sizeof( video_renderer_vertex_point_c ) == 12, "size of video_renderer_vertex_point_c is wrong." );

	//// maybe we'll use this later, for a more general and unified approach to rendering any thing in a scene.
	//class renderer_procedure_c
	//{
	//public:
	//	video_vertex_buffer_c * vertex_buffer; // vertex buffer to draw.
	//	video_index_buffer_c * index_buffer; // index buffer to draw, optional.
	//	video_vertex_shader_c * vertex_shader; // vertex shader to use.
	//	video_pixel_shader_c * pixel_shader; // pixel shader to use.
	//	video_texture_c * textures[ 4 ]; // textures to use.
	//	vector32x4_c colors[ 4 ]; // colors to use.
	//	matrix32x4x4_c world_transform; // world transform to use.
	//	video_primitive_type_e primitive_type; // primitive type to draw.
	//	video_cull_type_e primitive_cull_mode; // which primitives to draw and not draw.
	//	sint32_c vertex_count; // number of vertices to draw.
	//};

#pragma pack( pop )

}
