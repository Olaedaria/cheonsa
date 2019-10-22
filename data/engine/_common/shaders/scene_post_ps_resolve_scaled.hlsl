#include "base.hlsl"

void ps_main( in vertex_point_view_c input, out float4 output : SV_Target0 )
{
	float2 lookup = ( input.position.xy * float2( 0.5, -0.5 ) + float2( 0.5f, 0.5 ) );
	output = material_texture_0.Sample( sample_linear_clamp, lookup );
}
