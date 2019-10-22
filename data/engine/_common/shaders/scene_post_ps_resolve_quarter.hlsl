#include "base.hlsl"

void ps_main( in vertex_point_view_c input, out float4 output : SV_Target0 )
{
	float2 lookup = ( input.position.xy * float2( 0.5, -0.5 ) + float2( 0.5, 0.5 ) ) + ( canvas_actual_size_inverse * 0.5 ); // offset by half a pixel so that the sample point lies at the intersecting corners of 4 pixels so that we can get the average of 4 pixels with one sample.

	// use linear sampling to sample 4 neighborhoods of 4 pixels each to gather from a total of 16 pixels and produce 1 pixel as output.
	output  = material_texture_0.Sample( sample_linear_clamp, lookup + float2( -canvas_actual_size_inverse.x, -canvas_actual_size_inverse.y ) ); // top left neighborhood.
	output += material_texture_0.Sample( sample_linear_clamp, lookup + float2(  canvas_actual_size_inverse.x, -canvas_actual_size_inverse.y ) ); // top right neighborhood.
	output += material_texture_0.Sample( sample_linear_clamp, lookup + float2( -canvas_actual_size_inverse.x,  canvas_actual_size_inverse.y ) ); // bottom left neighborhood.
	output += material_texture_0.Sample( sample_linear_clamp, lookup + float2(  canvas_actual_size_inverse.x,  canvas_actual_size_inverse.y ) ); // bottom right neighborhood.
	output *= 0.25;
}
