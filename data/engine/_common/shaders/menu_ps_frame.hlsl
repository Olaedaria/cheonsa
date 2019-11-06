#include "base.hlsl"

void ps_main( in vertex_menu_view_c input, out float4 output : SV_Target0 )
{
	if ( get_menu_clip( input.position.xy ) == 0.0 )
	{
		discard;
	}

	output = material_texture_0.Sample( sample_linear_clamp, input.texture_a.xy ) * input.color * menu_color;
	//output.rgb = adjust_color_saturation( output.rgb, menu_saturation );
}
