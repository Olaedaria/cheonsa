#include "base.hlsl"

void vs_main( in float3 input_position : POSITION0, in float4 input_texture_a : TEXCOORD0, out float4 output_position_view : SV_Position, out float4 output_texture_a : TEXCOORD0 )
{
	float4 position = mul( float4( input_position, 1.0 ), object_world_transform );
	#if defined( waved )
	position.xyz += get_wave_displacement( position.xyz, input_texture_a.z, input_texture_a.w );
	#endif
	output_position_view = mul( position, camera_view_projection_transform );
	output_texture_a = input_texture_a;
}
