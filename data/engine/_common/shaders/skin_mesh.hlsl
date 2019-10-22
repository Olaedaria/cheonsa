#include "base.hlsl"

void vs_main( in vertex_mesh_skin_c input, out vertex_mesh_c output )
{
	if ( input.bone_weights.x != 0 )
	{
		float4x4 bone_transform = load_bone_transform( input.bone_weights, input.bone_indices );
		output.position = mul( float4( input.position, 1.0 ), bone_transform ).xyz;
		output.normal = normalize( mul( input.normal, (float3x3)bone_transform ) );
		output.normal_u = normalize( mul( input.normal_u, (float3x3)bone_transform ) );
		output.normal_v = normalize( mul( input.normal_v, (float3x3)bone_transform ) );
	}
	else
	{
		output.position = input.position;
		output.normal = input.normal;
		output.normal_u = input.normal_u;
		output.normal_v = input.normal_v;
	}
	output.texture_a = input.texture_a;
}
