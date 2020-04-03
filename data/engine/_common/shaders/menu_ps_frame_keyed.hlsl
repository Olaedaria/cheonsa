#include "base.hlsl"

void ps_main( in vertex_menu_view_c input, out float4 output : SV_Target0 )
{
	if ( get_menu_clip( input.position.xy ) == 0.0 )
	{
		discard;
	}

	float4 values = material_texture_0.Sample( sample_linear_clamp, input.texture_a.xy );
	output = ( values.r * menu_draw_shared_colors[ 0 ] ) + ( values.g * menu_draw_shared_colors[ 1 ] ) + ( values.b * menu_draw_shared_colors[ 2 ] ) * ( input.color * menu_draw_color * menu_color );
	output.a = values.a;
	//output.rgb = adjust_color_saturation( output.rgb, menu_saturation );
}
