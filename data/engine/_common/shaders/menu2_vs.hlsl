#include "base.hlsl"

void vs_main( in vertex_menu_c input, out vertex_menu_view_c output )
{
	float4 position_2d_multiplied_by_basis = input.position.xyxy * menu_basis.xzyw;
	float2 position_2d = float2( position_2d_multiplied_by_basis.x + position_2d_multiplied_by_basis.y, position_2d_multiplied_by_basis.z + position_2d_multiplied_by_basis.w ) + menu_origin;
	output.position_view.xy = ( ( ( position_2d.xy + menu_target_offset ) * menu_target_size_inverse.xy ) - float2( 0.5, 0.5 ) ) * float2( 2.0, -2.0 );
	output.position_view.z = 0.0;
	output.position_view.w = 1.0;
	output.position = float3( position_2d, 0.0 );
	output.texture_a = input.texture_a;
	output.color = input.color;
	output.parameters = input.parameters;
}
