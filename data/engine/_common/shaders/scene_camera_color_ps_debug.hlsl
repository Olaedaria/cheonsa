#include "base.hlsl"

void ps_main( in vertex_debug_view_c input, out float4 output : SV_Target0 )
{
	output = input.color;
}
