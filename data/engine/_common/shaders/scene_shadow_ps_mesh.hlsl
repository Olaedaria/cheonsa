#include "base.hlsl"

#if defined( masked )
void ps_main( in float4 input_position_view : SV_Position, in float4 input_texture_a : TEXCOORD0, out float4 output : SV_Target0 )
{
	clip( material_texture_0.Sample( sample_linear_wrap, input_texture_a ).a - 0.5 );
	output = float4( input_position_view.z, 0.0, 0.0, 0.0 );
}
#else
void ps_main( in float4 input_position_view : SV_Position, out float4 output : SV_Target0 )
{
	output = float4( input_position_view.z, 0.0, 0.0, 0.0 );
}
#endif
