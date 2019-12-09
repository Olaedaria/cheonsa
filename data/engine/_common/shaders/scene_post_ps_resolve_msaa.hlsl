#include "base.hlsl"

// not used at this time but keeping around for reference in case i decide to add msaa support again in the future.
// it's just annoying to support msaa because it creates a lot of situations that need special handling.
void ps_main( in vertex_point_view_c input, out float4 output : SV_Target0 )
{
	int2 lookup = ( int2 )( ( input.position.xy * float2( 0.5, -0.5 ) + float2( 0.5, 0.5 ) ) * canvas_actual_size );
	#if defined( msaa_count )
	resolved_normal_depth = 0;
	resolved_color = 0;
	for ( int i = 0; i < msaa_count; i++ )
	{
		resolved_normal_depth += target_normal_and_depth_msaa.Load( lookup, i );
		resolved_color += target_color_msaa.Load( lookup, i );
	}
	resolved_normal_depth /= (float)msaa_count;
	resolved_color /= (float)msaa_count;
	#else
	resolved_normal_depth = target_normal_and_depth.Load( lookup, 0 );
	resolved_color = target_color.Load( lookup, 0 )
	#endif
}
