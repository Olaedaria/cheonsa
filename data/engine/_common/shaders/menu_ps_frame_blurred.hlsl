#include "base.hlsl"

void ps_main( in vertex_menu_view_c input, out float4 output : SV_Target0 )
{
	if ( get_menu_clip( input.position.xy ) == 0.0 )
	{
		discard;
	}

	output = material_texture_0.Sample( sample_linear_clamp, input.texture_a.xy ) * input.color * menu_draw_color * menu_color;

	float3 blurred = target_color_quarter_blurred.Sample( sample_linear_clamp, input.position.xy * menu_actual_target_size_inverse.xy ).rgb;
	output.rgb *= 0.9;//lerp( blurred, float3( 1.0, 1.0, 1.0 ), 0.5 );
	output.rgb += blurred;
}
