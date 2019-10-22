#include "base.hlsl"

void ps_main( in vertex_mesh_view_c input, out float output : SV_Target0 )
{
	#if defined( masked )
	clip( material_texture_0.Sample( sample_linear_wrap, input.texture_a.xy, 0 ).a - 0.5 );
	#endif
	output = object_colors[ 0 ].x; // outline color index will be in object_colors[ 0 ].x.
}
