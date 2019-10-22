#include "base.hlsl"

void vs_main( in vertex_menu_c input, out vertex_menu_view_c output )
{
	float4 position_2d_multiplied_by_basis = input.position.xyxy * menu_basis.xzyw;
	float2 position_2d = float2( position_2d_multiplied_by_basis.x + position_2d_multiplied_by_basis.y, position_2d_multiplied_by_basis.z + position_2d_multiplied_by_basis.w ) + menu_origin;
	//float2 position_2d = float2( ( MenuBasis.x * Input.Position.x ) + ( MenuBasis.z * Input.Position.y ), ( MenuBasis.y * Input.Position.x ) + ( MenuBasis.w * Input.Position.y ) ) + MenuOrigin;
	float4 position_3d = mul( float4( position_2d, 0.0, 1.0 ), menu_world_transform );
	output.position_view = mul( position_3d, camera_view_projection_transform );
	output.position = position_3d;
	output.texture_a = input.texture_a;
	output.color = input.color;
	output.parameters = input.parameters;
}
