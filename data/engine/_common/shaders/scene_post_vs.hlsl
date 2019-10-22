#include "base.hlsl"

void vs_main( in vertex_point_c input, out vertex_point_view_c output )
{
	output.position_view = float4( input.position, 1.0 );
	output.position = input.position;
}
