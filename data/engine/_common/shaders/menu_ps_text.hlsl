#include "base.hlsl"

#define glyph_atlas_texel_size_inverse ( 1.0 / 4096.0 )

static float2 sample_offsets[8] = {
	float2( -1.0, 0.0 ),
	float2( -0.7071, -0.7071 ),
	float2( 0.0, -1.0 ),
	float2( 0.7071, -0.7071 ),
	float2( 1.0, 0.0 ),
	float2( 0.7071, 0.7071 ),
	float2( 0.0, 1.0 ),
	float2( -0.7071, 0.7071 )
};

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
	//output.rgb = adjust_color_saturation( output.rgb, menu_saturation );
	output = float4( 1.0, 1.0, 1.0, a ) * input.color * menu_color;
}
