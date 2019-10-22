#include "cheonsa_physics_point_spring_strand.h"
#include "cheonsa_ops.h"
#include "cheonsa_engine.h"

namespace cheonsa
{

	physics_point_spring_strand_c::point_c::point_c()
		: world_position_start()
		, world_position()
		, velocity()
		, radius( 0.0f )
	{
	}

	physics_point_spring_strand_c::spring_c::spring_c()
		: local_transform()
		, world_target_transform()
		, world_transform()
		, linear_tension( 0.0f )
		, angular_tension( 0.0f )
		, length_original( 1.0f )
	{
	}

	physics_point_spring_strand_c::physics_point_spring_strand_c()
		: world_transform()
		, gravity( 0.0f )
		, wind( 0.0f )
		, damping( 0.0f )
		, point_list()
		, spring_list()
		//, body_list()
	{
	}

	physics_point_spring_strand_c::~physics_point_spring_strand_c()
	{
	}

	void_c physics_point_spring_strand_c::update( space_transform_c & scene_transform_in, float32_c time_step, vector64x3_c gravity, vector64x3_c wind )
	{
		assert( point_list.get_length() && spring_list.get_length() );
		assert( spring_list.get_length() + 1 == point_list.get_length() );

		world_transform.basis = scene_transform_in.get_scaled_basis();
		world_transform.position = scene_transform_in.position;
		//normalized( world_transform );

		point_list[ 0 ].world_position = world_transform.position;
		for ( sint32_c i = 1; i < point_list.get_length(); i++ )
		{
			point_c * tail = & point_list[ i ];
			tail->world_position_start = tail->world_position;
			tail->world_position += tail->velocity;
			tail->world_position += gravity * ( gravity * time_step );
		}

		for ( sint32_c i = 0; i < spring_list.get_length(); i++ )
		{
			spring_c * spring = & spring_list[ i ];
			point_c * head = & point_list[ i ];
			point_c * tail = & point_list[ i + 1 ];

			// linear relax constraint
			vector64x3_c head_to_tail = ops::make_vector64x3_normalized( tail->world_position - head->world_position );
			vector64x3_c tail_position_target = head->world_position + ( head_to_tail * spring->length_original );
			vector64x3_c to_target_delta = ( tail_position_target - tail->world_position ) * ( spring->linear_tension );
			if ( i > 0 )
			{
				to_target_delta *= 0.5f;
				tail->world_position += to_target_delta;
				head->world_position -= to_target_delta;
			}
			else
			{
				tail->world_position += to_target_delta;
			}

			//// adjust for collisions with bodies
			//for ( sint32_c j = 0; j < body_list.get_count(); j++ )
			//{
			//	physics_body_c * body = body_list[ i ];
			//		
			//	if ( tail->radius > 0.0f )
			//	{
			//		float32_c distance = length( tail->position - body->position );
			//		float32_c minimum_distance = tail->radius + body->radius;
			//		if ( distance < minimum_distance )
			//		{
			//			// displace tail towards the exterior of the intersected body in a direction that is tangental to the axis of the spring
			//			//vector32x3_c SpringDirection = tail->position - head->position;
			//			//float32_c SpringLength = SpringDirection.length();
			//			//tail->position = get_normal( tail->position - body.position ) * minimum_distance;
			//			tail->position = get_normal( tail->position - body->position ) * minimum_distance;
			//		}
			//	}
			//}

			// angular relax constraint
			if ( i > 0 )
			{
				spring->world_target_transform = spring->local_transform * spring_list[ i - 1 ].world_transform;
			}
			else
			{
				spring->world_transform = world_transform;
			}
			spring->world_transform.basis.c = ops::make_vector32x3_normalized( spring->world_target_transform.basis.c );

			head_to_tail = tail->world_position - head->world_position;
			float64_c head_to_tail_length = static_cast< float32_c >( ops::make_float64_length( head_to_tail ) );
			assert( head_to_tail_length > constants< float64_c >::division_near_zero() );
			head_to_tail /= head_to_tail_length;
			float64_c to_target_delta_angle = ops::math_arc_cosine( ops::make_float64_dot_product( vector64x3_c( spring->world_target_transform.basis.c ), head_to_tail ) );
			if ( to_target_delta_angle > constants< float64_c >::angle_near_zero() )
			{
				to_target_delta = ( ops::interpolate_linear( head_to_tail, vector64x3_c( spring->world_target_transform.basis.c ), ( spring->angular_tension ) ) - head_to_tail ) * ( head_to_tail_length );
				if ( i > 0 )
				{
					to_target_delta *= 0.5;
					tail->world_position += to_target_delta;
					head->world_position -= to_target_delta;
				}
				else
				{
					tail->world_position += to_target_delta;
				}
			}
		}

		point_list[ 0 ].velocity = point_list[ 0 ].world_position - point_list[ 0 ].world_position_start;
		for ( sint32_c i = 0; i < spring_list.get_length(); i++ )
		{
			spring_c * spring = & spring_list[ i ];
			point_c * head = & point_list[ i ];
			point_c * tail = & point_list[ i + 1 ];

			tail->velocity = tail->world_position - tail->world_position_start;
			tail->velocity -= tail->velocity * damping;

			if ( i > 0 )
			{
				spring->world_target_transform = spring->local_transform * spring_list[ i - 1 ].world_transform;
			}
			else
			{
				spring->world_target_transform = world_transform;
			}
			spring->world_target_transform.basis.c = ops::make_vector32x3_normalized( spring->world_target_transform.basis.c );

			vector64x3_c head_to_tail = tail->world_position - head->world_position;
			float64_c head_to_tail_length = ops::make_float64_length( head_to_tail );
			//float32_c length_percent = head_to_tail_length / spring->length_original;
			assert( head_to_tail_length > constants< float64_c >::division_near_zero() );
			head_to_tail /= head_to_tail_length;
			spring->world_transform = spring->world_target_transform;
			spring->world_transform.basis = ops::make_matrix32x3x3_basis_socket_rotated_c( spring->world_transform.basis, vector32x3_c( head_to_tail ) );
			spring->world_transform.position = head->world_position;

			//global_engine_instance.interfaces.video_renderer_interface->add_debug_line( true, head->world_position, tail->world_position, vector32x4_c( 1.0f, 1.0f, 1.0f, 1.0f ) );
			//global_engine_instance.interfaces.video_renderer_interface->add_debug_axes( true, spring->world_transform, 0.5f );
			//global_engine_instance.interfaces.video_renderer_interface->add_debug_axes( true, spring->world_target_transform, 0.25f );
		}
	}

}
