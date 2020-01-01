#include "base.hlsl"

void ps_main( in vertex_menu_view_c input, out float4 output : SV_Target0 )
{
	if ( get_menu_clip( input.position.xy ) == 0.0 )
	{
		discard;
	}

	float a = glyph_atlas_texture.Sample( sample_linear_clamp, input.texture_a.xyz ).r;
	float weight = input.parameters.y;
	float softness = input.parameters.z;
	a = smoothstep( 0.5 - softness - weight, 0.5 + softness - weight, a );
	output = float4( menu_draw_shared_colors[ 1 ].rgb, a ) * input.color * menu_draw_color * menu_color;
	//output.rgb = adjust_color_saturation( output.rgb, menu_saturation );
}
