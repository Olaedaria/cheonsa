#pragma once

#include "cheonsa__types.h"
#include "cheonsa_core_list.h"

namespace cheonsa
{

	// used for simulating hair and clothes on characters.
	// it is its own thing, it does not interact with bullet physics.
	// the code that uses it needs to initialize it by "hand" in order to work.
	// so you need to understand how it expects to be set up.
	// basically, set scene_transform, gravity, wind, daming, point_list, and spring_list. scene_transform will be set again each update(). the gravity, wind, and damping properties can be changed at any time too.
	// point_list count is equal to spring_list count + 1, and each neighboring pair of points in the list are interpreted to make up the head and tail for each corresponding spring in the spring_list.
	// spring_list is the number of springs or bones in the strand. springs are analogous to bones.
	// each spring needs to be initialized with its own local_transform, linear_tension, angular_tension, and length_original.
	// local_transform for each segment can be static, or it can be taken from the bone animation. this transform will drive the target_scene_transform for which the simulation will try to orient the segment towards.
	class physics_point_spring_strand_c
	{
	public:
		struct point_c
		{
			vector64x3_c world_position_start; // scene space position at the start of the simulation update, used to figure out the velocity of the point at the end of the simulation update.
			vector64x3_c world_position; // scene space position.
			vector64x3_c velocity; // velocity of the point, as calculated by difference in position between the start and end of the simulation update.
			float64_c radius; // radius of the point, used to perform collision detection against the body_list.
			point_c();
		};

		class spring_c
		{
		public:
			transform3d_simple_c local_transform;
			transform3d_simple_c world_target_transform;
			transform3d_simple_c world_transform;
			float64_c linear_tension; // strength of forces to apply to the spring to bring its length to its target.
			float64_c angular_tension; // strength of forces to apply to the spring to bring its orientation to its target.
			float64_c length_original; // base length of the spring, which is the length that the spring will try to maintain.
			spring_c();
		};

	public:
		transform3d_simple_c world_transform;
		float64_c gravity;
		float64_c wind;
		float64_c damping;
		core_list_c< point_c > point_list;
		core_list_c< spring_c > spring_list;
		//core_list_c< physics_rigid_body_c * > * body_list; // pointer to a list of bodies to collide against. this is usually owned by the model.

	public:
		physics_point_spring_strand_c();
		~physics_point_spring_strand_c();

		void_c update( transform3d_c & scene_transform, float32_c time_step, vector64x3_c gravity, vector64x3_c wind );

	};

}
