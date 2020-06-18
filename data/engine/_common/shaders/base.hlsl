/*
things that can be defined by the engine during compile time:
#define masked // will be defined if pixel shader is supposed to be alpha masked.
#define clipped // will be defined if the vertex shader is supposed to write clip plane distance.
#define waved // will be defined if the vertex shader is supposed to be animated by wind.
*/

// should match static const values declared and defined in "cheonsa::video_renderer_interface_c" in "cheonsa_video_renderer_interface.h".
// at some point, maybe these definitions will be passed in at compile time, so that we can update them from the exe.
#define menu_clip_plane_stack_count 32 // should match "cheonsa::video_renderer_interface_c::menu_clip_plane_stack_count" in "cheonsa_video_renderer_interface.h"
#define scene_colors_count 16 // should match "cheonsa::video_renderer_interface_c::scene_colors_count" in "cheonsa_video_renderer_interface.h"
#define object_lights_count 8 // should match "cheonsa::video_renderer_interface_c::object_lights_count" in "cheonsa_video_renderer_interface.h"
#define object_colors_count 16 // should match "cheonsa::video_renderer_interface_c::object_colors_count" in "cheonsa_video_renderer_interface.h"
#define material_colors_count 16 // should match "cheonsa::video_renderer_interface_c::material_colors_count" in "cheonsa_video_renderer_interface.h"
#define light_type_e_direction 0 // should match "cheonsa::video_light_type_e" in "cheonsa_video_interface.h"
#define light_type_e_point 1 // should match "cheonsa::video_light_type_e" in "cheonsa_video_interface.h"
#define light_type_e_cone 2 // should match "cheonsa::video_light_type_e" in "cheonsa_video_interface.h"

// sampler states are bound by "cheonsa::video_interface_d3d10_c::video_interface_d3d10_c()".
SamplerState sample_point_wrap : register( s0 );
SamplerState sample_linear_wrap : register( s1 );
SamplerState sample_anisotropic_wrap : register( s2 );
SamplerState sample_point_clamp : register( s3 );
SamplerState sample_linear_clamp : register( s4 );
SamplerState sample_anisotropic_clamp: register( s5 );
SamplerComparisonState sample_shadow : register( s6 );

// all the following data (textures, texture buffers, constant buffers) are managed by "cheonsa::video_renderer_interface_c"
Texture2D< float4 > material_texture_0 : register( ps, t0 );
Texture2D< float4 > material_texture_1 : register( ps, t1 );
Texture2D< float4 > material_texture_2 : register( ps, t2 );
Texture2D< float4 > material_texture_3 : register( ps, t3 );
Texture2D< float4 > object_texture_0 : register( ps, t4 );
Texture2D< float4 > object_texture_1 : register( ps, t5 );
Texture2D< float4 > object_texture_2 : register( ps, t6 );
Texture2D< float4 > object_texture_3 : register( ps, t7 );
TextureCube< float4 > light_probe_texture : register( ps, t8 ); // light probe cube map which contains ambient light for the current object being rendered.
Texture2DArray< float > shadow_map_texture : register( ps, t9 ); // texture array of shadow map slices.
Texture2DArray< float > glyph_atlas_texture : register( ps, t10 ); // texture array of glyph atlas slices.
Texture2D< uint > target_outline: register( ps, t11 );
Texture2D< float2 > target_normal : register( ps, t12 ); // overlay/post texture, contains the normal and depth of the scene from the opaque passes.
Texture2D< float > target_depth : register( ps, t13 );
Texture2D< float4 > target_color : register( ps, t14 ); // overlay/post texture, contains the color of the scene from the opaque and transparent passes.
Texture2D< float4 > target_color_half_blurred : register( ps, t15 ); // half size blurred version of target_color.
Texture2D< float4 > target_color_quarter_blurred : register( ps, t16 ); // quarter size even more blurred version of target_color.
Texture2D< float4 > target_color_eighth_blurred : register( ps, t17 ); // quarter size even more blurred version of target_color.
Texture2D< float4 > target_color_reflections : register( ps, t18 ); // planar reflections in target space.
Buffer< float4 > bones_texture_buffer : register( vs, t30 ); // an array of all the bones needed to apply skinning to the current draw call. the transformed mesh will be stored in another vertex buffer using the stream output feature.
Buffer< float4 > lights_texture_buffer : register( ps, t31 ); // an array of all of the lights that affect the current camera view. model pixel shaders use  to reference specific lights in this list that affect the model being rendered.
Buffer< float4 > shadow_views_texture_buffer : register( ps, t32 ); // formatted as "cheonsa::video_renderer_shadow_view_c" in "cheonsa_video_renderer_types.h", but we don't define an equivalent struct here in the shader since it's so simple that it's probably better if we don't.

cbuffer shadow_camera_block : register( b0 )
{
	float4x4 shadow_camera_view_projection_transform;
};

cbuffer shadow_object_block : register( b1 )
{
	float4x4 shadow_object_world_transform;
};

cbuffer scene_block : register( b2 )
{
	float scene_time_counter;
	float3 scene_wind_direction;
	float scene_wind_speed;
	float scene_wind_counter;
	float scene_wind_period_inverse;
	float scene_wind_period_weight_minimum;
	float scene_wind_period_weight_range;
	int scene_directional_light_count; // the first of this many lights in the light list are directional lights, used by the sky dome shader to color the sky and light the atmosphere.
	float scene_light_probe_mip_count;
	float scene_unused_0;
	float4 scene_colors[ scene_colors_count ];
};

cbuffer camera_block : register( b3 )
{
	float4x4 camera_view_projection_transform; // the camera's view and projection matricies multiplied together. also used to render shadow maps.
	float4 camera_clip_plane; // used to clip pixels that lie behind the planar reflector so that they don't show up in the reflection itself.
	float2 canvas_actual_size; // width and height of target in pixels.
	float2 canvas_actual_size_inverse; // inverse of canvas_actual_size, which also represents the size of each pixel in texture space.
	float2 canvas_apparent_to_actual_ratio; // used to scale texture coordinates from apparent canvas size (constrained by view port) to actual canvas size.
	float camera_clip_near; // distance in meters to camera's near clip plane.
	float camera_clip_far; // distance in meters to camera's far clip plane.
	int4 camera_light_indices[ object_lights_count / 4 ]; // lights that affect the camera, used to do certain post process effects like water on lens, soft bloom, etc.
};

cbuffer object_block : register( b4 )
{
	float4x4 object_world_transform; // world space relative to camera.
	int4 object_light_indices[ object_lights_count / 4 ];
	float4 object_colors[ object_colors_count ];
};

cbuffer material_block : register( b5 )
{
	float4 material_counters; // 4 different time counters.
	float4 material_colors[ material_colors_count ]; // material colors.
};

cbuffer menu_block : register( b6 )
{
	float2 menu_actual_target_size_inverse;
	float2 menu_apparent_target_size_inverse;
	float2 menu_target_offset;
	float2 menu_unused_0;
};

cbuffer menu_batch_block : register( b7 )
{
	float4x4 menu_world_transform; // for placing 2d menus in 3d space.
	float4 menu_basis; // 2D basis (rotation and scale) matrix.
	float2 menu_origin; // 2D position translation offset.
	float menu_saturation;
	float menu_unused_1;
	float4 menu_color;
	int menu_clip_plane_stack_length; // number of clip planes to use, generally come in multiples of 4 because menu system uses clip rectangles.
	int3 menu_unused_2;
	float4 menu_clip_plane_stack[ menu_clip_plane_stack_count ]; // these work as long as each set of planes defines an enclosed convex shape, then we can have shapes within shapes.
};

cbuffer menu_draw_block : register( b8 )
{
	float4 menu_draw_color;
	float4 menu_draw_shared_colors[ 3 ];
	float4 menu_draw_clip_planes[ 4 ]; // per-element clip planes, in addition to the nested ones defined by menu_clip_plane_stack_length and menu_clip_plane_stack.
};

struct vertex_mesh_c // should match "cheonsa::video_renderer_vertex_mesh_base_c" in "cheonsa_video_renderer_types.h"
{
	float3 position : POSITION0;
	float3 normal : NORMAL0;
	float3 normal_u : TANGENT0;
	float3 normal_v : BINORMAL0;
	float4 texture_a : TEXCOORD0;
};

struct vertex_mesh_skin_c // should match "cheonsa::video_renderer_vertex_mesh_base_c" and "cheonsa::video_renderer_vertex_mesh_bone_weight_c" in "cheonsa_video_renderer_types.h"
{
	float3 position : POSITION0;
	float3 normal : NORMAL0;
	float3 normal_u : TANGENT0;
	float3 normal_v : BINORMAL0;
	float4 texture_a : TEXCOORD0;
	float4 bone_weights : BLENDWEIGHT0;
	uint4  bone_indices : BLENDINDICES0;
};

struct vertex_normal_and_depth_view_c
{
	float4 position_view : SV_Position;
	float3 position : POSITION0;
	float3 normal : NORMAL0;
	float2 texture_a : TEXCOORD0;
	float  clip_distance : SV_ClipDistance;
};

struct vertex_mesh_view_c
{
	float4 position_view : SV_Position;
	float3 position : POSITION0;
	float3 normal : NORMAL0;
	float3 normal_u : TANGENT0;
	float3 normal_v : BINORMAL0;
	float4 texture_a : TEXCOORD0;
	float  clip_distance : SV_ClipDistance;
};

struct vertex_menu_c // should match "cheosna::video_renderer_vertex_menu_c" in "cheonsa_video_renderer_types.h"
{
	float3 position : POSITION0;
	float3 texture_a : TEXCOORD0;
	float4 color : COLOR0;
	float4 parameters : COLOR1; // skew, weight, softness, atlas index.
};

struct vertex_menu_view_c
{
	float4 position_view : SV_Position;
	float3 position : POSITION0;
	float3 texture_a : TEXCOORD0;
	float4 color : COLOR0;
	float4 parameters : COLOR1; // skew, weight, softness, atlas index.
};

struct vertex_point_c // should match "cheonsa::video_renderer_vertex_point_c" in "cheonsa_video_renderer_types.h"
{
	float3 position : POSITION0;
};

struct vertex_point_view_c
{
	float4 position_view : SV_Position;
	float3 position : POSITION0;
};

struct vertex_debug_c // should match "cheonsa::video_renderer_vertex_debug_c" in "cheonsa_video_renderer_types.h"
{
	float3 position : POSITION0;
	float4 color : COLOR0;
};

struct vertex_debug_view_c
{
	float4 position_view : SV_Position;
	float3 position : POSITION0;
	float4 color : COLOR0;
};

struct light_c // should match "cheonsa::video_renderer_light_c" in "cheonsa_video_renderer_types.h"
{
	float4 parameters; // packed into a float4 so that it can be loaded in one instruction. x is light_type, y is shadow_view_start, z is shadow_view_count, w is unused.
	float4 position_range; // xyz: position of light in world space, and w: range of light for point lights and spot lights.
	float4 direction_angle; // xyz: direction of light for directional lights and spot lights, and w: cone angle for spot lights.
	float4 energy_unused; // rgb: light energy which is color * brightness, w: and unused.
};

inline float4x4 load_bone( uint bone_index ) // loads and returns a blend matrix at BoneIndex.
{
	float4x4 result;
	result[ 0 ] = bones_texture_buffer.Load( bone_index * 4 );
	result[ 1 ] = bones_texture_buffer.Load( bone_index * 4 + 1 );
	result[ 2 ] = bones_texture_buffer.Load( bone_index * 4 + 2 );
	result[ 3 ] = bones_texture_buffer.Load( bone_index * 4 + 3 );
	return result;
}

inline float4x4 load_bone_transform( in float4 bone_weights, in uint4 bone_indices ) // loads and returns a blended bone matrix, which is the result of mixing together 4 different blend matrices at BoneIndices by a factor of BoneWeights.
{
	float4x4 result = bone_weights.x * load_bone( bone_indices.x );
	if ( bone_weights.y > 0.0 )
	{
		result += bone_weights.y * load_bone( bone_indices.y );
		if ( bone_weights.z > 0.0 )
		{
			result += bone_weights.z * load_bone( bone_indices.z );
			if ( bone_weights.w > 0.0 )
			{
				result += bone_weights.w * load_bone( bone_indices.w );
			}
		}
	}
	return result;
}

inline light_c load_light( in int light_index )
{
	int base = light_index * 4;
	light_c result;
	result.parameters = lights_texture_buffer.Load( base );
	result.position_range = lights_texture_buffer.Load( base + 1 );
	result.direction_angle = lights_texture_buffer.Load( base + 2 );
	result.energy_unused = lights_texture_buffer.Load( base + 3 );
	return result;
}

inline float4x4 load_shadow_view_projection_transform( in int shadow_view_index )
{
	int base = shadow_view_index * 5;
	float4x4 result;
	result[ 0 ] = shadow_views_texture_buffer.Load( base );
	result[ 1 ] = shadow_views_texture_buffer.Load( base + 1 );
	result[ 2 ] = shadow_views_texture_buffer.Load( base + 2 );
	result[ 3 ] = shadow_views_texture_buffer.Load( base + 3 );
	return result;
}

inline float3 load_shadow_view_position( in int shadow_view_index )
{
	int base = shadow_view_index * 5;
	return shadow_views_texture_buffer.Load( shadow_view_index + 4 ).xyz;
}

float3 sample_ambient_light( float3 position, float3 normal, float roughness )
{
	return light_probe_texture.SampleLevel( sample_linear_clamp, normal, roughness * scene_light_probe_mip_count ).rgb;
}

static const float2 shadow_sample_vectors[ 9 ] =
{
	float2( 0.0, 0.0 ),
	float2( 1.0, 0.0 ),
	float2( 0.0, 1.0 ),
	float2( -1.0, 0.0 ),
	float2( 0.0, -1.0 ),
	float2( 1.0, 1.0 ),
	float2( 1.0, -1.0 ),
	float2( -1.0, -1.0 ),
	float2( 1.0, 1.0 )
};

float sample_shadow_slice( in float3 lookup, float reference, in int sample_count, in float2 sample_size )
{
	float result = 0.0;
	for ( int i = 0; i < sample_count; i++ )
	{
		result += shadow_map_texture.SampleCmpLevelZero( sample_shadow, float3( lookup.xy + shadow_sample_vectors[i].xy * sample_size.xy, lookup.z ), reference ).r;
	}
	return result / (float)sample_count;
}

float sample_shadow_for_direction_light( in int shadow_view_start, in float3 sample_point, in float sample_bias, in int sample_count, in float sample_size )
{
	// find the cascade slice that point lies in.
	float4 lookup_0 = float4( mul( float4( sample_point, 1.0 ), load_shadow_view_projection_transform( shadow_view_start ) ).xyz, 0 );
	float4 lookup_1 = float4( mul( float4( sample_point, 1.0 ), load_shadow_view_projection_transform( shadow_view_start + 1 ) ).xyz, 1 );
	float4 lookup_2 = float4( mul( float4( sample_point, 1.0 ), load_shadow_view_projection_transform( shadow_view_start + 2 ) ).xyz, 2 );
	float4 lookup_3 = float4( mul( float4( sample_point, 1.0 ), load_shadow_view_projection_transform( shadow_view_start + 3 ) ).xyz, 3 );

	// build shadow map look up coordinates.
	float4 lookup;
	lookup = all( abs( lookup_3.xyz ) < 0.95 ) ? lookup_3 : float4( 0.0, 0.0, 0.0, -1.0 );
	lookup = all( abs( lookup_2.xyz ) < 0.95 ) ? lookup_2 : lookup;
	lookup = all( abs( lookup_1.xyz ) < 0.95 ) ? lookup_1 : lookup;
	lookup = all( abs( lookup_0.xyz ) < 0.95 ) ? lookup_0 : lookup;
	if ( lookup.w == -1.0 )
	{
		// look up is out of bounds of all shadow views.
		return 1.0;
	}
	lookup.xy = lookup.xy * float2( 0.5, -0.5 ) + float2( 0.5, 0.5 );

	// sample the shadow map.
	return sample_shadow_slice( float3( lookup.xy, shadow_view_start + lookup.w ), lookup.z - ( sample_bias / ( lookup.w + 1 ) ), sample_count, sample_size / ( lookup.w + 1 ) );
}

float sample_shadow_for_point_light( in int shadow_view_start, in float3 sample_point, in float sample_bias, in int sample_count, in float sample_size )
{
	// find the cube face slice that point lies in.
	float3 light_to_pixel = sample_point - load_shadow_view_position( shadow_view_start );
	float3 light_to_pixel_abs = abs( light_to_pixel );
	int shadow_view_index = shadow_view_start;
	if ( light_to_pixel_abs.x > light_to_pixel_abs.y && light_to_pixel_abs.x > light_to_pixel_abs.z )
	{
		shadow_view_index += ( light_to_pixel.x > 0 );
	}
	else if ( light_to_pixel_abs.y > light_to_pixel_abs.x && light_to_pixel_abs.y > light_to_pixel_abs.z )
	{
		shadow_view_index += 2 + ( light_to_pixel.y > 0 );
	}
	else if ( light_to_pixel_abs.z > light_to_pixel_abs.x && light_to_pixel_abs.z > light_to_pixel_abs.y )
	{
		shadow_view_index += 4 + ( light_to_pixel.z > 0 );
	}

	// build shadow map look up coordinates.
	float4 lookup = mul( float4( sample_point, 1.0f ), load_shadow_view_projection_transform( shadow_view_index ) );
	lookup.xyz /= lookup.w; // needed to map perspective projection to uv coordinates
	if ( any( abs( lookup.xyz ) > 1.0 ) )
	{
		// look up is out of bounds of all shadow views. this shouldn't be possible?
		return 1.0;
	}
	lookup.xy = lookup.xy * float2( 0.5, -0.5 ) + float2( 0.5, 0.5 );

	// sample the shadow map.
	return sample_shadow_slice( float3( lookup.xy, shadow_view_start ), lookup.z - sample_bias * 2.0, sample_count, sample_size );
}

float sample_shadow_for_spot_light( in int shadow_view_start, in float3 sample_point, in float sample_bias, in int sample_count, in float sample_size )
{
	// build shadow map look up coordinates.
	float4 lookup = mul( float4( sample_point, 1.0 ), load_shadow_view_projection_transform( shadow_view_start ) );
	lookup.xyz /= lookup.w;
	if ( any( abs( lookup.xyz ) > 1.0 ) )
	{
		// look up is out of bounds of shadow view.
		return 1.0;
	}
	lookup.xy = lookup.xy * float2( 0.5, -0.5 ) + float2( 0.5, 0.5 );

	// sample the shadow map.
	return sample_shadow_slice( float3( lookup.xy, shadow_view_start ), lookup.z - sample_bias, sample_count, sample_size );
}

float sample_shadow_for_light( in light_c light, in float3 sample_point, in float sample_bias, in int sample_count, in float sample_size )
{
	if ( light.parameters.y > 0 )
	{
		if ( light.parameters.x == 0 )
		{
			sample_shadow_for_direction_light( light.parameters.y, sample_point, sample_bias, sample_count, sample_size );
		}
		else if ( light.parameters.x == 1 )
		{
			sample_shadow_for_point_light( light.parameters.y, sample_point, sample_bias, sample_count, sample_size );
		}
		else
		{
			sample_shadow_for_spot_light( light.parameters.y, sample_point, sample_bias, sample_count, sample_size );
		}
	}
	return 1.0;
}

float get_menu_clip( float2 position )
{
	for ( int i = 0; i < menu_clip_plane_stack_length; i++ )
	{
		float4 clip_plane = menu_clip_plane_stack[ i ];
		if ( dot( clip_plane.xy, position.xy ) + clip_plane.z < 0.0 )
		{
			return 0.0;
		}
	}
	if ( menu_draw_clip_planes[ 0 ].w == 0.0f )
	{
		for ( int i = 0; i < 4; i++ )
		{
			float4 clip_plane = menu_draw_clip_planes[ i ];
			if ( dot( clip_plane.xy, position.xy ) + clip_plane.z < 0.0 )
			{
				return 0.0;
			}
		}
	}
	return 1.0;
}

float get_luminance( float3 color )
{
	return dot( color, float3( 0.299, 0.587, 0.114 ) );
}

float3 adjust_color_saturation( float3 color, float amount )
{
	float p = sqrt( dot( color, float3( 0.299, 0.587, 0.114 ) ) );
	return ( ( color.rgb - p.rrr ) * amount.rrr ) + p.rrr;
}

//float3 GetFogColor( in float3 EyeToSurfaceDirection )
//{
//	float Rise = dot( EyeToSurfaceDirection, float3( 0.0, 0.0, 1.0 ) );
//	return lerp( SceneSkyHorizonColor, SceneSkyZenithColor, 1.0 - pow( 1.0 - Rise, 3.0 ) );
//}

//float GetFogWeight( in float3 SurfacePosition )
//{
//	return saturate( distance( CameraPosition, SurfacePosition ) / SceneFogRange );
//}

// can be used to create a low frequency churn for the surface of an ocean, or can be used to create high frequency ripples for leaves and grass.
// this same function is used by the game's physics engine to determine the z height of the water's surface at a given xy point.
// and it can also be used by the game's physics engine to apply impulses to bouyant objects so that they churn with the water.
float3 get_wave_displacement( in float3 sample_point, in float wave_density, in float wave_amplitude )
{
	float rolling_distance = dot( scene_wind_direction, sample_point ) + scene_wind_counter; // planar distance equation
	float rolling_weight = ( ( ( sin( rolling_distance * scene_wind_period_inverse ) * 0.5 + 0.5 ) * ( scene_wind_period_weight_range ) ) + scene_wind_period_weight_minimum ) * saturate( scene_wind_speed ) * wave_amplitude; // scene_wind_period_inverse was already multiplied by Pi*2 before it was uploaded.
	float d = rolling_distance * wave_density; // wind speed is already integrated in scene_wind_counter, and that was used to calculate rolling_distance
	float3 result = 0;
	result += float3( sin( d ), cos( d ), sin( d * 0.25 ) ) * rolling_weight; // wiggle.
	result += scene_wind_direction * rolling_weight; // push.
	return result;
}
