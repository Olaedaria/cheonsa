#pragma once

#include "cheonsa_scene_component.h"
#include "cheonsa_physics_body.h"

namespace cheonsa
{

	// a body is a physically simulated rigid body.
	// add this component to a scene object to enable rigid body physics simulation for that object.
	// a physics body that is active will simulate as usual, and will actively read (in edit mode) or write (in play mode) the scene object's scene transform directly.
	// a physics body that is passive will simulate as usual, but will not actively read or write the scene object's scene transform, and so it is the responsiblity of the game logic to do what it wants with the simulated body position|rotation|transform.
	//
	// initially, the physics body is in an uninitialized state.
	// initialization procedure has a bunch of steps:
	//     1. add the scene component to the scene object (so that the scale of the scene object can be accessed when initialize() is called).
	//     2. call add_physics_shape() one or more times to add physics shapes to the body.
	//         2.1. call any of the initialize_*() functions on each of the physics shapes that were added.
	//     3. call initialize().
	// the physics body can now be added to a physics scene.
	//
	// physics shape properties can't really be modified easily.
	// if you want to modify shapes at run time, you have to:
	//     1. call uninitialize() on the physics body.
	//     2. change shapes (uninitialize and (re)initialize (with potentially different properties) the shape instances that you want to reuse, remove the shape instances that you don't want to reuse, add new shape instances and initialize them).
	//     3. call initialize() on the physics body.
	//
	// keep in mind that physics shapes need to be reinitialzied every time the scene object's scale changes.
	// this is a limitation with the physics engine, because object scale is not a property that the physics engine is programmed to be able to work with.
	// physics shapes always have an implicit unit scale, so when cheonsa scene objects have a non-unit scale, those physics shapes need to be initialized with dimensions that reflect the non-unit scale.
	class scene_component_body_c
		: public scene_component_c
	{
	public:
		static inline uint8_c get_type_code_static() { return scene_component_type_e_body; }
		virtual inline uint8_c get_type_code() const override { return get_type_code_static(); }

	protected:
		physics_body_c _physics_body;
		core_list_c< physics_shape_c * > _physics_shape_list; // one or more physics shapes that make up the shape of this physics body.
		physics_shape_c * _physics_shape; // compound shape made up of all the shapes in _physics_shape_list, even if there's only one shape (so that we can take into account local space transform of the shape). this might incur a performance hit, i hope it's not too bad.
		boolean_c _is_initialized;
		boolean_c _is_enabled; // used in conjunction with _is_added_to_phyics_scene to determine when _physics_body needs to be added to or removed from the physics scene.
		boolean_c _is_added_to_phyics_scene; // keeps track of if the _physics_body has been added to the physics scene or not.
		//boolean_c _is_active; // if true, then this body component will read/write the scene object's scene transform directly. if false, then this component is passive and the physics body will continue to simulate but it is the responsibility then of the game logic to make use of the simualted values how it wants.
		boolean_c _ignore_property_modified; // lets us move the scene object and physics object without and ignore the feedback loop of the physics engine notifying us of the change.

		// bullet physics interoperability function.
		// copies the world space transform from the _scene_object to the _physics_body.
		// for rigid bodies this is called once per initialization.
		// for kinematic rigid bodies this is called once per physics simulation update frame.
		static void_c _copy_world_space_transform_from_game_to_physics( void_c * instance, vector64x3_c & world_space_position, matrix32x3x3_c & world_space_basis );
		// bullet physics interoperability function.
		// copies the world space transform from the _physics_body to the _scene_object.
		// for rigid bodies, this is called once per game engine update frame.
		static void_c _copy_world_space_transform_from_physics_to_game( void_c * instance, vector64x3_c const & world_space_position, matrix32x3x3_c const & world_space_bais );

		virtual void_c _handle_after_added_to_scene() override;
		virtual void_c _handle_before_removed_from_scene() override;

		virtual void_c _handle_after_property_modified( scene_object_property_e property ) override;

	public:
		scene_component_body_c( boolean_c is_active = true );
		virtual ~scene_component_body_c() override;

		boolean_c get_enabled() const; // gets the enabled state of this physics body. an enabled physics body is added to the physics scene. a disabled physics body is removed from the physics scene.
		void_c set_enabled( boolean_c value ); // sets the enabled state of this physics body. an enabled physics body is added to the physics scene. a disabled physics body is removed from the physics scene.

		sint32_c get_physics_shape_count() const; // gets the number of physics shapes.
		physics_shape_c * get_physics_shape( sint32_c index ) const; // gets an existing physics shape.
		physics_shape_c * add_physics_shape(); // adds a new uninitialized physics shape.
		void_c remove_physics_shape( physics_shape_c * value ); // removes and deletes the given physics shape.
		void_c remove_physics_shape( sint32_c index ); // removes and deletes the given physics shape.
		void_c remove_all_physics_shapes(); // removes and deletes all physics shapes.

		physics_body_c const & get_physics_body() const;
		physics_body_c & get_physics_body();

		// call this after adding and initializing one or more shapes with add_physics_shape().
		// locks the contents of the physics shape list and initializes the physics body with the shapes defined therein.
		// once the physics body is initialized then it can be added to a physics scene.
		void_c initialize( float64_c mass, uint32_c layer, uint32_c layer_mask, boolean_c kinematic );

		// releases the physics body.
		// unlocks the contents of the physics shape list.
		// you should remove the physics body from the physics scene before doing this.
		void_c uninitialize();

		// convenience function to uninitialize and initialize _physics_shape with all the same properties except scale (because in the physics engine scale is more of a fixed value rather than a variable one).
		// this is called automatically by cheonsa when the scene object's scale changes.
		void_c reinitialize();

	};

}
