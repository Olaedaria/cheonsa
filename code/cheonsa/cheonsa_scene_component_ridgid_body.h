#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_physics_ridgid_body.h"

namespace cheonsa
{

	// provides a ridgid body object, for physics simulation.
	class scene_component_ridgid_body_c
		: public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_ridgid_body; }
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
		virtual void_c _handle_on_world_space_transform_modified( transform3d_c const & old_world_space_transform, scene_component_c * initiator ) override;

		scene_component_ridgid_body_c();

	public:
		virtual ~scene_component_ridgid_body_c() override;

		static scene_component_ridgid_body_c * make_new_instance();

		void_c remove_all_shapes(); // removes all shapes from this body.
		void_c add_shape( physics_shape_c * shape ); // adds a shape to this body.
		physics_shape_c const * get_shape_at_index( sint32_c index ) const;
		physics_shape_c * get_shape_at_index( sint32_c index ); // you may use function only when the ridgid body is in an uninitialized shape. you can use this to modify (uninitialzie and initialize) existing shape instances that you added before.

		// when you call this function, this initializes the collision object with the shapes that you added with add_shape().
		// adds the collision object to the physics simulation scene if able.
		void_c initialize( transform3d_c const & frame, float64_c mass, uint32_c layer, uint32_c layer_mask, boolean_c kinematic );

		// removes the ridgid body from the physics simulation scene if needed.
		// releases the collision object.
		void_c uninitialize();

		// you can set physics properties on the ridgid body with this.
		// don't set world space transform properties from here though, do so from the scene object.
		// will return nullptr of the ridgid body is not initialized.
		physics_rigid_body_c * get_ridgid_body() const;

	};

}
