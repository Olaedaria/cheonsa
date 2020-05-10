#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_physics_ridgid_body.h"

namespace cheonsa
{

	// add this component to a scene object to enable rigid body physics simulation for that object.
	// in order to initialize the body for use, you must provide the physics shapes as instances that you created on the heap with new.
	// you must have initialized these shapes already as any non-compound type of shape.
	class scene_component_ridgid_body_c
		: public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_body; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	private:
		// if there is only one shape, then it will be in this list.
		// if there is more than one shape, then the last shape will be created as the compound shape to encapsulate all the other shapes.
		core_list_c< physics_shape_c * > _shape_list;
		// if there is only one shape in the _shape_list and it's frame is identiy, then this will point to that shape.
		// otherwise this will be a new compound shape that contains all of the shapes in the _shape_list.
		physics_shape_c * _master_shape;

		physics_rigid_body_c * _ridgid_body;
		transform3d_c _frame; // the physics rigid body may have a transform that is offset from the scene object by this relative amount. this transform also defines the center of mass of the rigid body. it should not be scaled.
		transform3d_c _frame_inverse;

		boolean_c _is_initialized; // just keeps track of if this has been initialized, so that we don't initialize it more than once.

		boolean_c _is_muted; // used to prevent getting into a feedback loop.

		// bullet physics interoperability function.
		// copies the world space transform from the _scene_object to the _physics_ridgid_body.
		// for rigid bodies this is called once per initialization.
		// for kinematic rigid bodies this is called once per physics simulation update frame.
		static void_c _physics_copy_world_space_transform_from_game_to_physics( void_c * instance, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis );
		// bullet physics interoperability function.
		// copies the world space transform from the _physics_ridgid_body to the _scene_object.
		// for rigid bodies, this is called once per game engine update frame.
		static void_c _physics_copy_world_space_transform_from_physics_to_game( void_c * instance, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_bais );

		virtual void_c _handle_before_removed_from_scene() override;
		virtual void_c _handle_after_added_to_scene() override;

		virtual void_c _handle_on_world_space_transform_changed( transform3d_c const & old_world_space_transform, transform3d_c const & new_world_space_transform ) override;

		scene_component_ridgid_body_c();

	public:
		virtual ~scene_component_ridgid_body_c() override;

		static scene_component_ridgid_body_c * make_new_instance();

		void_c remove_all_shapes(); // removes all shapes from this body.
		void_c add_shape( physics_shape_c * shape ); // adds a shape to this body.
		physics_shape_c const * get_shape_at_index( sint32_c index ) const;
		physics_shape_c * get_shape_at_index( sint32_c index ); // you may use function only when the ridgid body is in an uninitialized shape. you can use this to modify (uninitialzie and initialize) existing shape instances that you added before.

		// physics_shape_list should contain a list of one or more physics_shape_c instances that you have allocated on the heap.
		// when you call this function, you are giving ownership of those shapes to this physics body, so you should not try to delete them afterwards.
		// if you want a single shape, put just a single shape in the list.
		// if you want a compound shape, put more than one shape in the list.
		// the initializer will create a compound shape if needed to hold the shape or shapes that you provide.
		// a compound shape is created if:
		//     more than one shape is provided.
		//     or the one shape that is provided has a frame that is not identity.
		void_c initialize( transform3d_c const & frame, float64_c mass, uint32_c layer, uint32_c layer_mask, boolean_c kinematic );
		void_c uninitialize();

		//boolean_c get_kinematic() const;
		//void_c set_kinematic( boolean_c value );

	};

}
