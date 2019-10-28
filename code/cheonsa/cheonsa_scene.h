#pragma once

#include "cheonsa_scene_types.h"
#include "cheonsa_scene_tree_octo.h"
#include "cheonsa_audio2.h"
#include "cheonsa_physics_scene.h"
#include "cheonsa_video_interface.h"
#include "cheonsa_video_renderer_interface.h"
#include "cheonsa_core_linked_list.h"

namespace cheonsa
{

	/*
	// imagine an infinite square grid on the xy plane of the scene, where the center grid cell is centered at the world origin, where each grid line is _cell_stride meters away from the lines next to it.
	// imagine each grid cell on this grid either being empty, or being populated by a scene_cell_c.
	// the level designer is responsible for enabling or disabling scene_cell_c's in each imaginary grid cell.
	// the scene holds a flat list of these scene_cell_c's, each one positioned at a different grid cell.
	// each scene_cell_c has a center point that is placed in somewhere the absolute scene space, at an increment of however wide and long each cell is.
	// each scene_cell_c contains a collection of scene_object_c's, and each one of those is positioned relative to the scene_cell_c's center point.
	// this is how we somewhat solve the problem of floating point imprecision (at least on the x and y axes, but not the z axis).
	// as the player character object moves through the world, they cross cell boundaries.
	// as scene objects update in the scene, if their positions every cross into the boundary of a different cell than the one they were in before, then they are removed from their old cell and inserted into the new cell. their relative cell position changes, but their absolute position remains similar.
	// each scene_cell_c has its own physics scene, with simulation occurring around that physics scene's zero origin (which is virtually at the cell's absolute_center).
	// when a static object like an environment mesh has it's center point in one cell, but its bounds intersecting with the borders of one or more neighboring cells, then a physics collision component is spawned in the physics scene of each affected cell. this uses a little extra memory, but it allows the level designer to place objects like trees or rocks on the borders of the cells.
	// when a dynamic object has bounds that intersect with more than one cell, then we do the same thing that we do with the static mesh, we create redundant physics body components in each affected cell's physics scene, but we average the result of all of the 1-4 simulations for that object, and then apply that back to the scene object. i'm not sure how broken this will be in practice, but i'm going to try it.
	// when a physics object intersects with border of one or more neighboring cells, then we temporarily spawn other potentially interacting physics objects from the neighboring cells into the current cell's physics scene, and the physics simulation results from the current cell takes priority over those temporary objects.
	// when a physics object fully crosses over into another cell, 
	class scene_cell_c
	{
	public:
		scene_c * _scene; // the scene that owns this cell.
		sint32_c _coordinates[3]; // xy grid cell coordinates.
		vector32x3_c _absolute_center; // this should not suffer from precision issues because we are using large increments of 100 meters.
		box32x3_c _absolute_bounds;
		physics_scene_c _physics_scene; // the physics scene for the cell.
		scene_object_c * _object_list_first;
		scene_object_c * _object_list_last;

	public:
		scene_cell_c();
		void_c insert_object_at_end( scene_object_c * object );
		void_c remove_object( scene_object_c * object );

	};
	*/

	// pixel perfect hit detection.
	// works with renderer to render geometry in the line of the pick ray.
	// sorts results by their depth, from near to far.
	class scene_pick_c
	{
	public:
		float32_c depth; // the depth of the pick, or -1 if it was not hit.
		scene_component_c * component; // the geometry component that we are testing for pixel perfect hit detection against, it will be either a scene_component_model_c or a scene_component_sprite_c. use run-time type checking to determine what type it actually is.
	public:
		scene_pick_c();
		static float64_c quick_sort_function( scene_pick_c const & pick );
	};

	// a 3d scene, or world.
	// it's spatially optimized a little bit, it uses octo trees to hold scene components and scene lights, which allows for quick look up based on proximity.
	// update on scene objecst has to be managed by the game, and it should occur in the following order:
	//   game calls update_animations() on all its animated model scene components.
	//   game calls update() on the physics scene contained in the scene.
	//   game calls update_simulated_bone_logics() on all its animated model scene components.
	//   game calls get_scene_camera() to update the camera's properties. the camera's transform will be fed to the audio scene listener when update_audio() is called.
	//   game calls update_audio() on the scene to tell the sound components in the scene to add their sources to the audio scene if they are in range of the camera.
	class scene_c
	{
	private:
		friend class scene_component_c;
		friend class scene_component_sound_c;
		friend class scene_component_model_c;
		friend class scene_component_light_c;
		friend class scene_component_light_probe_c;
		friend class scene_light_c;
		friend class video_renderer_interface_c;

		// if this scene is set on a user interface, then this is that user interface.
		// there is only one user interface per game client, and at most only one scene can be associated with it at a time.
		user_interface_c * _user_interface;

		// the point of view that this scene will be experienced from when it visuals are rendered and its audio is mixed.
		scene_camera_c _scene_camera;

		// all root level scene objects in the scene in a doubly linked list.
		core_linked_list_c< scene_object_c * > _scene_object_list;

		// octo tree that holds all components.
		scene_tree_octo_c< scene_component_c * > _scene_component_tree;

		// flat list that holds just the global effect (direction) lights in the scene.
		core_list_c< scene_light_c * > _global_lights_list;
		// octo tree that holds just the local effect (point, cone) lights in the scene.
		// this is a little rendundant with _scene_component_tree, but we do it this way because it lets model instances create internal light instances without spawning additional scene objects and light components.
		scene_tree_octo_c< scene_light_c * > _local_lights_tree;

		// counters.
		uint32_c _render_frame; // used by the renderer to keep track of which objects (namely for model skinning and sprite vertex generation) have been processed for a given render frame so that they are not processed more than once per render frame. starts counting at 1, and wraps around to 1.
		float64_c _time_counter; // used by renderer as input to some shaders.
		float32_c _random_number; // used by renderer as input to some shaders.

		// corresponding audio scene.
		audio2_scene_c * _audio_scene;
		audio2_scene_listener_c * _audio_scene_listener;

		// corresponding physics scene.
		// uses bullet compiled to use double precision float, hopefully its enough for open world.
		physics_scene_c * _physics_scene;

		// renderables.
		//core_list_c< scene_component_menu_control_c * > _scene_component_menu_control_list; // all of the menu control components in this scene in a flat list.
		//core_linked_list_c< scene_component_light_c * > _light_list; // all of the light components in this scene in a flat list.
		core_linked_list_c< scene_component_light_probe_c * > _light_probe_list; // all of the light probe components in this scene in a flat list.
		scene_component_light_probe_c * _last_baked_light_probe; // used by the renderer to keep track of the last light probe that was baked, for incremental baking over multiple frames.

		// audibles.
		core_linked_list_c< scene_component_sound_c * > _sound_list; // flat list for now, could be spatially partitioned later.

	public:
		// sky setup.
		// any number of models to render "behind" everything else.
		// if these models contain directional lights, then they will act as the sky lights.
		// the game should position these relative to the world origin, as the world origin is the point of view that is used when the renderer renders the sky models.
		core_list_c< scene_component_model_c * > sky_models;
		//core_list_c< scene_light_c * > sky_lights; // one or two directional lights (at this time they must be directional type) which may also be shadow casters. these should be managed by the game to point from the point of view of the sun or moon or whatever.

		// 16 colors that all pixel shaders can read from for a given frame, which makes them good for holding sky/environment related colors.
		// most likely only the sky models will have shaders that use these values, since light probes can handle how indirect light from the sky models affects models on the "ground". totally up to you how you want to use these values though.
		vector32x4_c colors[ video_renderer_interface_c::scene_colors_count ];

	private:
		// light probe quality and baking.
		boolean_c _suspend_automatic_light_probe_invalidation; // set to true to disable automatic light probe invalidation scene-wide. this is useful if you are adding or removing a bunch objects to or from the scene at once.
		float32_c _light_probe_clip_near; // distance to near clip plane when rendering geometry into the light probe cube map.
		float32_c _light_probe_clip_far; // determines the size of the axis aligned bounding box of light probes, which determines when to invalidate them, and what their draw distance is when they are baked.

		// pixel perfect picking.
		boolean_c _pick_on_next_render; // set to true to enable pixel perfect picking during the next render.
		vector32x2_c _pick_canvas_coordinates; // pick the object under these screen space pixel coordinates.
		core_list_c< scene_pick_c > _pick_list_original; // original list of objects that intersected with the broad phase ray intersection test.
		core_list_c< scene_pick_c > _pick_list_final; // final result of pixel perfect pick, sorted by depth from near to far, used by video_renderer_interface_c.
		video_depth_stencil_c * _target_pick_depth; // for screen space object picking.
		video_texture_c * _target_pick_depth_readable; // CPU readable copy of _target_pick_depth.

		void_c * _user_pointer;

	public:
		scene_c();
		virtual ~scene_c();

		scene_camera_c & get_scene_camera();

		void_c update_audio( float32_c time_step );

		virtual void_c add_object( scene_object_c * object ); // adds a scene object to this scene.
		virtual void_c remove_object( scene_object_c * object ); // removes a scene object from this scene.

		void_c add_light( scene_light_c * light ); // adds a light to this scene.
		void_c remove_light( scene_light_c * light ); // removes a light from this scene.

		void_c _gather_pickables_along_segment( segment64_c const & pick_segment, core_list_c< scene_component_model_c * > & models_of_interest, core_list_c< scene_component_sprite_c * > & sprites_of_interest ); // internal use by pixel picker, performs broad phase ray intersection test against each model's bounding box and stores the results in the internal list. the input lists need would be assembled already by the renderer that calls this function, and they would contain pointers to objects in this scene.
		void_c pixel_pick_during_next_render( vector32x2_c const & screen_coordinates ); // queues up a pixel perfect pick operation to occur during the next render. screen_coordinates are in pixels with the top left corner as the origin.
		scene_component_c * get_pixel_pick_result_component() const;
		scene_object_c * get_pixel_pick_result_object( scene_object_c * after_object ) const; // returns the pick result from the last time this scene was rendered. the pixel perfect pick internally has access to a list of hits sorted by distance from near to far that intersect with the ray. if after_object is set, then returns the next result in that list after after_object (used to cycle to the next selectable object if the user clicks on the same spot that intersects with multiple objects and over different frames). otherwise, it returns the first entry in that list.

		void_c suspend_automatic_light_probe_invalidation(); // lets you batch add/remove/edit a bunch of scene objects/components without invalidating light probes after each operation.
		void_c resume_automatic_light_probe_invalidation();

		void_c invalidate_light_probes_every_where();
		void_c invalidate_light_probes_with_light( scene_light_c * light );
		void_c invalidate_light_probes_with_box( box64x3_c const & bounding_box, space_transform_c const & bounding_box_transform );
		void_c invalidate_light_probes_with_before_and_after_box( box64x3_c const & bounding_box, space_transform_c const & before_bounding_box_transform, space_transform_c & after_bounding_box_transform );
		void_c invalidate_light_probes_with_sphere( sphere64_c const & sphere );
		void_c invalidate_light_probes_with_before_and_after_sphere( sphere64_c const & before_sphere, sphere64_c const & after_sphere );
		scene_component_light_probe_c * find_light_probe_with_point( vector64x3_c const & position );

		//boolean_c physics_ray_cast( vector64x3_c & result, segment64_c const segment ); // casts a ray into the physics scene, returns true if the ray hits anything, false if otherwise. result will hold the closest point of intersection.

		user_interface_c * get_user_interface() const;

		audio2_scene_c * get_audio_scene();

		physics_scene_c * get_physics_scene();

		core_linked_list_c< scene_object_c * > const & get_scene_object_list() const;

		scene_tree_octo_c< scene_component_c * > const & get_scene_component_tree() const;

		void_c * get_user_pointer();
		void_c set_user_pointer( void_c * value );

	};

}
