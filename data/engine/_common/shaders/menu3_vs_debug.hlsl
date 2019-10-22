#include "base.hlsl"

void vs_main( in vertex_debug_c input, out vertex_debug_view_c output )
{
	output.position_view = mul( float4( input.position, 1.0 ), camera_view_projection_transform );
	output.position = input.position;
	output.color = input.color;
}
