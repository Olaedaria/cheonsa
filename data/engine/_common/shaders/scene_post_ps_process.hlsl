#include "base.hlsl"

/**
Basic FXAA implementation based on the code on geeks3d.com with the
modification that the texture2DLod stuff was removed since it's
unsupported by WebGL.
--
From:
https://github.com/mitsuhiko/webgl-meincraft
Copyright (c) 2011 by Armin Ronacher.
Some rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.
* The names of the contributors may not be used to endorse or
promote products derived from this software without specific
prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FXAA_REDUCE_MIN
#define FXAA_REDUCE_MIN   (1.0 / 128.0)
#endif
#ifndef FXAA_REDUCE_MUL
#define FXAA_REDUCE_MUL   (1.0 / 8.0)
#endif
#ifndef FXAA_SPAN_MAX
#define FXAA_SPAN_MAX     8.0
#endif

//optimized version for mobile, where dependent
//texture reads can be a bottleneck
float4 fxaa( float2 lookup )
{
	float4 color;
	float3 rgbNW = target_color.Sample( sample_linear_clamp, lookup + float2( -canvas_actual_size_inverse.x, -canvas_actual_size_inverse.y ) ).rgb;
	float3 rgbNE = target_color.Sample( sample_linear_clamp, lookup + float2(  canvas_actual_size_inverse.x, -canvas_actual_size_inverse.y ) ).rgb;
	float3 rgbSW = target_color.Sample( sample_linear_clamp, lookup + float2( -canvas_actual_size_inverse.x,  canvas_actual_size_inverse.y ) ).rgb;
	float3 rgbSE = target_color.Sample( sample_linear_clamp, lookup + float2(  canvas_actual_size_inverse.x,  canvas_actual_size_inverse.y ) ).rgb;
	float4 texColor = target_color.Sample( sample_linear_clamp, lookup );

	float3 luma = float3( 0.299, 0.587, 0.114 );
	float lumaNW = dot( rgbNW, luma );
	float lumaNE = dot( rgbNE, luma );
	float lumaSW = dot( rgbSW, luma );
	float lumaSE = dot( rgbSE, luma );
	float lumaM = dot( texColor.rgb, luma );
	float lumaMin = min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) );
	float lumaMax = max( lumaM, max( max( lumaNW, lumaNE ), max( lumaSW, lumaSE ) ) );

	float2 dir;
	dir.x = -( ( lumaNW + lumaNE ) - ( lumaSW + lumaSE ) );
	dir.y = ( ( lumaNW + lumaSW ) - ( lumaNE + lumaSE ) );

	float dirReduce = max( ( lumaNW + lumaNE + lumaSW + lumaSE ) * ( 0.25 * FXAA_REDUCE_MUL ), FXAA_REDUCE_MIN );

	float rcpDirMin = 1.0 / ( min( abs( dir.x ), abs( dir.y ) ) + dirReduce );
	dir = min( float2( FXAA_SPAN_MAX, FXAA_SPAN_MAX ), max( float2( -FXAA_SPAN_MAX, -FXAA_SPAN_MAX ), dir * rcpDirMin ) ) * canvas_actual_size_inverse;

	float3 rgbA = 0.5 * (
		target_color.Sample( sample_linear_clamp, lookup + dir * ( 1.0 / 3.0 - 0.5 ) ).rgb +
		target_color.Sample( sample_linear_clamp, lookup + dir * ( 2.0 / 3.0 - 0.5 ) ).rgb );
	float3 rgbB = rgbA * 0.5 + 0.25 * (
		target_color.Sample( sample_linear_clamp, lookup + dir * -0.5 ).rgb +
		target_color.Sample( sample_linear_clamp, lookup + dir * 0.5 ).rgb );

	float lumaB = dot( rgbB, luma );
	if ( ( lumaB < lumaMin ) || ( lumaB > lumaMax ) )
	{
		color = float4( rgbA, texColor.a );
	}
	else
	{
		color = float4( rgbB, texColor.a );
	}
	return color;
}

void ps_main( in vertex_point_view_c input, out float4 output : SV_Target0 )
{
	float2 lookup = ( input.position.xy * float2( 0.5, -0.5 ) + float2( 0.5, 0.5 ) ) * canvas_apparent_to_actual_ratio;
	//output = fxaa( lookup );
	output = target_color.Sample( sample_point_clamp, lookup );
	//output.rgb += target_color_half_blurred.Sample( sample_linear_clamp, lookup ).rgb;
	//output.rgb += target_color_quarter_blurred.Sample( sample_linear_clamp, lookup ).rgb;
	output.rgb += target_color_eighth_blurred.Sample( sample_linear_clamp, lookup ).rgb;
}
