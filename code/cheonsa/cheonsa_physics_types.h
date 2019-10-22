#pragma once

#include "cheonsa_math_types.h"
#include "cheonsa_core_list.h"

// the global namespace is a lost cause when it comes to trying to keep it clean.
// bullet lives in the global namespace, but i don't want to include any bullet headers from our headers.
class btVector3;
class btQuaternion;
class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btConstraintSolver;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btIDebugDraw;
class btCollisionShape;
class btCollisionObject;
class btRigidBody;
class btScaledBvhTriangleMeshShape;
class btTypedConstraint;
class btTriangleIndexVertexArray;
class btMotionState;
class btMatrix3x3;

namespace cheonsa
{

	class physics_scene_c;
	class physics_shape_c;
	class physics_body_c;
	class physics_constraint_c;

	enum physics_shape_type_e
	{
		physics_shape_type_e_none,
		physics_shape_type_e_sphere,
		physics_shape_type_e_box,
		physics_shape_type_e_capsule,
		physics_shape_type_e_cylinder,
		physics_shape_type_e_cone,
		physics_shape_type_e_convex_hull,
		physics_shape_type_e_triangle_mesh,
		physics_shape_type_e_compound,
	};

	enum physics_constraint_type_e
	{
		physics_constraint_type_e_none,
		physics_constraint_type_e_fixed, // welds two objects together at a given frame.
		physics_constraint_type_e_point, // connects two objects together at a given point, and both objects are free to rotate around the point.
		physics_constraint_type_e_hinge, // connects two ojbects together at a given frame, and both objects are free to rotate around the frame's x axis (with optional limits).
		physics_constraint_type_e_cone, // connects two objects together at a given frame, and both objects are free to rotate (swing) around the frame's y and z axes, and twist around the remaining axis.
		physics_constraint_type_e_generic, // connects two objects together at a given frame, and both objects are free to rotate and translate around each of the frame's three axes (with optional limits for angular and linear motion).
	};

	enum physics_layer_e
	{
		physics_layer_e_static = 0x01, // for static environments objects, collides with static, dynamic, character, and camera.
		physics_layer_e_static_no_clip = 0x02, // for regular objects, collides with static, dynamic, and character. out of phase with camera.
		physics_layer_e_dynamic = 0x04, // for regular objects, collides with static, dynamic, and camera. out of phase with character.
		physics_layer_e_dynamic_no_clip = 0x08, // for regular objects, collides with static and dynamic. out of phase with character and camera.
		physics_layer_e_character = 0x10, // for character bounds, collides with static, dynamic, and camera. out of phase with character.
		physics_layer_e_character_no_clip = 0x20, // for character bounds, collides with static and dynamic. out of phase with character and camera.
		physics_layer_e_camera = 0x40, // for player camera, collides with all layers except layers marked as no_clip. out of phase with no_clip and camera.
	};

	// given layer as physics_layer_e, this determines what other layers can be collided with.
	inline uint16_c physics_get_layer_mask_for_layer( uint16_c layer )
	{
		if ( layer == physics_layer_e_static )
		{
			return physics_layer_e_static | physics_layer_e_static_no_clip | physics_layer_e_dynamic | physics_layer_e_dynamic_no_clip | physics_layer_e_character | physics_layer_e_character_no_clip | physics_layer_e_camera;
		}
		else if ( layer == physics_layer_e_static_no_clip )
		{
			return physics_layer_e_static | physics_layer_e_static_no_clip | physics_layer_e_dynamic | physics_layer_e_dynamic_no_clip | physics_layer_e_character | physics_layer_e_character_no_clip;
		}
		else if ( layer == physics_layer_e_dynamic )
		{
			return physics_layer_e_static | physics_layer_e_static_no_clip | physics_layer_e_dynamic | physics_layer_e_dynamic_no_clip | physics_layer_e_camera;
		}
		else if ( layer == physics_layer_e_dynamic_no_clip )
		{
			return physics_layer_e_static | physics_layer_e_static_no_clip | physics_layer_e_dynamic | physics_layer_e_dynamic_no_clip;
		}
		else if ( layer == physics_layer_e_character )
		{
			return physics_layer_e_static | physics_layer_e_static_no_clip | physics_layer_e_camera;
		}
		else if ( layer == physics_layer_e_character_no_clip )
		{
			return physics_layer_e_static | physics_layer_e_static_no_clip;
		}
		else if ( layer == physics_layer_e_camera )
		{
			return physics_layer_e_static | physics_layer_e_dynamic | physics_layer_e_character;
		}
		return 0;
	}

	class physics_sweep_test_result_c
	{
	public:
		class hit_c
		{
		public:
			physics_body_c * object;
			float64_c fraction;
			vector64x3_c position;
			vector64x3_c normal;
			inline hit_c()
				: object( nullptr )
				, fraction( 0.0f )
				, position( 0.0f, 0.0f, 0.0f )
				, normal( 0.0f, 0.0f, 0.0f )
			{
			}
		};
		core_list_c< hit_c > hit_list;
	};

	void_c physics_make_btTransform_from_space_transform( space_transform_c const & in_transform, btMatrix3x3 & out_basis, btVector3 & out_origin ); // creates out_basis (unscaled) and out_origin from in_transform.
	void_c physics_make_space_transform_from_btTransform( btMatrix3x3 const & in_basis, btVector3 const & in_position, space_transform_c & out_transform );

}
