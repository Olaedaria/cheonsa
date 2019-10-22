#include "base.hlsl"

#define shadow_sample_bias 0.00005
#define shadow_sample_count 9
#define shadow_sample_size 0.5

#define pb_roughness 0.5
#define pb_metallic 0.0

float make_glare( float to_surface_dot, float roughness )
{
	return lerp( pow( 1.0 - abs( to_surface_dot ), 3.0 ), 1.0, roughness );
}

void ps_main( in vertex_mesh_view_c input, out float4 output : SV_Target0 )
{
	#if defined( masked )
	clip( material_texture_0.Sample( sample_linear_wrap, input.texture_a.xy, 0 ).a - 0.5 );
	#endif

	output = 0;

	// boiler plate
	float4 surface_albedo = material_texture_0.Sample( sample_linear_wrap, input.texture_a.xy ) * material_colors[ 0 ];
	float3 surface_normal = normalize( input.normal );
	float3 surface_position = input.position;
	float3 surface_position_biased = surface_position + surface_normal * 0.03;
	float3 eye_to_surface = input.position;
	float  eye_to_surface_distance = length( eye_to_surface );
	float3 eye_to_surface_direction = ( input.position ) / eye_to_surface_distance;
	float  eye_to_surface_dot = -dot( eye_to_surface_direction, surface_normal );
	float  eye_to_surface_glare = make_glare( eye_to_surface_dot, pb_roughness );

	//float EdgeFactor = pow( 1.0f - abs( dot( eye_to_surface_direction, surface_normal ) ), ( 1.0f - RoughnessFactor + 1.0f ) * 3.0f );
	//float EdgeFactor = pow( saturate( 1.0 - -dot( eye_to_surface_direction, surface_normal ) ), specular_power );

	//float2 ScreenNoise = NoiseTexture.Sample( PointWrap, input.PosCamera.xy * ( 1.0 / 64.0 ) + SceneRandomNumber * 5.0 );

	// diffues alpha transparency.
	output.a = surface_albedo.a;

	float3 surface_albedo_metallic = lerp( float3( 1.0, 1.0, 1.0 ), surface_albedo.rgb, pb_metallic );

	// ambient light contribution from light probe.
	float3 component_diffuse = surface_albedo.rgb * sample_ambient_light( surface_position, surface_normal, pb_roughness );
	float3 component_specular = sample_ambient_light( surface_position, reflect( eye_to_surface_direction, surface_normal ), pb_roughness ) * surface_albedo_metallic;
	output.rgb = lerp( component_diffuse, component_specular, eye_to_surface_glare );

	// direct light contribution from light sources.
	float3 light_to_surface_direction;
	float  light_direct_factor;
	float  light_to_surface_distance;
	float  light_attenuation_factor;
	float  light_shadow_factor;
	float  light_to_surface_dot;
	float  light_to_surface_glare;
	float3 light_to_surface_energy;
	for ( int i = 0; i < object_lights_count; i++ )
	{
		int light_index = object_light_indices[ i / 4 ][ i % 4 ];
		if ( light_index < 0 )
		{
			break;
		}

		light_c light = load_light( light_index );
		light_attenuation_factor = 1.0;
		light_shadow_factor = 1.0;

		if ( light.parameters.x == light_type_e_direction )
		{
			if ( light.parameters.z > 0 )
			{
				light_shadow_factor = sample_shadow_for_direction_light( light.parameters.y, surface_position_biased, shadow_sample_bias, shadow_sample_count, shadow_sample_size );
				if ( light_shadow_factor <= 0 )
				{
					continue;
				}
			}
			light_to_surface_direction = normalize( light.direction_angle.xyz );
			light_to_surface_dot = -dot( light_to_surface_direction, surface_normal );
			//light_to_surface_glare = saturate( -dot( normalize( light_to_surface_direction + eye_to_surface_direction ), surface_normal ) );
			//light_shadow_factor = min( saturate( light_to_surface_dot ), light_shadow_factor );
		}
		else if ( light.parameters.x == light_type_e_point )
		{
			light_to_surface_distance = distance( input.position, light.position_range.xyz );
			if ( light_to_surface_distance > light.position_range.w )
			{
				continue;
			}
			light_attenuation_factor = 1.0 - ( light_to_surface_distance / light.position_range.w );
			light_attenuation_factor = light_attenuation_factor * light_attenuation_factor;
			if ( light.parameters.z > 0 )
			{
				light_shadow_factor = sample_shadow_for_point_light( light.parameters.y, surface_position_biased, shadow_sample_bias * 2, shadow_sample_count, shadow_sample_size );
				if ( light_shadow_factor <= 0 )
				{
					continue;
				}
			}
			light_to_surface_direction = normalize( input.position.xyz - light.position_range.xyz );
			light_to_surface_dot = -dot( light_to_surface_direction, surface_normal );
			//light_to_surface_glare = saturate( -dot( normalize( light_to_surface_direction + eye_to_surface_direction ), surface_normal ) );
			//light_shadow_factor = min( saturate( light_to_surface_dot ), light_shadow_factor );
		}
		else if ( light.parameters.x == light_type_e_cone )
		{
			light_to_surface_distance = distance( input.position, light.position_range.xyz );
			if ( light_to_surface_distance > light.position_range.w )
			{
				continue;
			}
			light_to_surface_direction = normalize( input.position.xyz - light.position_range.xyz );
			light_to_surface_dot = -dot( light_to_surface_direction, surface_normal );
			if ( light_to_surface_dot < light.direction_angle.w )
			{
				continue;
			}
			light_attenuation_factor = 1.0 - ( light_to_surface_distance / light.position_range.w );
			light_attenuation_factor = light_attenuation_factor * light_attenuation_factor;
			if ( light.parameters.z > 0 )
			{
				light_shadow_factor = sample_shadow_for_spot_light( light.parameters.y, surface_position_biased, shadow_sample_bias, shadow_sample_count, shadow_sample_size );
				if ( light_shadow_factor <= 0 )
				{
					continue;
				}
			}
			//light_to_surface_glare = saturate( -dot( normalize( light_to_surface_direction + eye_to_surface_direction ), surface_normal ) );
			//light_shadow_factor = min( saturate( light_to_surface_dot ), light_shadow_factor );
		}

		light_to_surface_glare = make_glare( light_to_surface_dot, pb_roughness );
		light_to_surface_energy = light.energy_unused.rgb * light_attenuation_factor * light_shadow_factor;

		component_diffuse = surface_albedo.rgb * light_to_surface_energy * ( 1.0 - light_to_surface_glare );
		component_specular = surface_albedo_metallic * light_to_surface_energy * ( light_to_surface_glare * eye_to_surface_glare );
		output.rgb += component_diffuse + component_specular;
	}
}
