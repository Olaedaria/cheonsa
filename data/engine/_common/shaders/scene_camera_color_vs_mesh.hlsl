#include "base.hlsl"

void vs_main( in vertex_mesh_c input, out vertex_mesh_view_c output )
{
	float4 position = mul( float4( input.position, 1.0 ), object_world_transform );
	#if defined( waved )
	position.xyz += get_wave_displacement( position.xyz, input.texture_a.z, input.texture_a.w );
	#endif
	#if defined( clipped )
	output.clip_distance = dot( position.xyz, camera_clip_plane.xyz ) + camera_clip_plane.w;
	#else
	output.clip_distance = 1.0;
	#endif
	output.position_view = mul( position, camera_view_projection_transform );
	output.position = position.xyz;
	output.normal = mul( input.normal, (float3x3)object_world_transform );
	output.normal_u = mul( input.normal_u, (float3x3)object_world_transform );
	output.normal_v = mul( input.normal_v, (float3x3)object_world_transform );
	output.texture_a = input.texture_a;
}
