#include "base.hlsl"

void ps_main( in vertex_menu_view_c input, out float4 output : SV_Target0 )
{
	if ( get_menu_clip( input.position.xy ) == 0.0 )
	{
		discard;
	}

	output = input.color * menu_draw_color * menu_color;
	output.rgb = pow( output.rgb, 0.5 ); // makes secondary color (yellow, cyan, magenta) bands wider, to make them appear to be of equal width as primary color (red, green, blue) bands.
}
