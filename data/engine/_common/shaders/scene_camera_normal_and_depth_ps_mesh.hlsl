#include "base.hlsl"

void ps_main( in vertex_normal_and_depth_view_c input, out float4 output0 : SV_Target0, out float4 output1 : SV_Target1 )
{
#if defined( masked )
	clip( material_texture_0.Sample( sample_linear_wrap, input.texture_a.xy, 0 ).a - 0.5 );
#endif
	output0.rgb = input.normal;
	output1.r = length( input.position );
}
