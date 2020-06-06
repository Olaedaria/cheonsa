#include "base.hlsl"

void vs_main( in vertex_debug_c input, out vertex_debug_view_c output )
{
	output.position_view.xy = ( ( ( input.position.xy + menu_target_offset ) * menu_target_size_inverse.xy ) - float2( 0.5, 0.5 ) ) * float2( 2.0, -2.0 ); // float4( ( input.position.xy * menu_target_size_inverse.xy ) + menu_target_offset, 0.0, 1.0 );
	output.position_view.z = 0.0;
	output.position_view.w = 1.0;
	output.position = input.position;
	output.color = input.color;
}
