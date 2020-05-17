#pragma once

#include "cheonsa_scene_types.h"
#include "cheonsa_scene_tree_octo.h"
#include "cheonsa_audio_interface.h"
#include "cheonsa_physics_scene.h"
#include "cheonsa_video_interface.h"
#include "cheonsa_video_renderer_interface.h"
#include "cheonsa_core_linked_list.h"

namespace cheonsa
{

	// a 3d scene, or world.
	//
	// manages scene state for renderables, audio, and physics simulation.
	//
	// primarily manages the scene state for gpu renderables.
	// secondarily interfaces with a couple other systems that also need to maintain their own internal scene representations (somewhat redundant):
	//     audio
	//     physics
	// the engine's user interface manager acts as an interface between the scene and the user.
	//
	// the scene's contents will probably be managed closely by the game.
	//
	// it's spatially optimized a little bit for general case (open world), it uses octo trees to hold scene components and scene lights, which allows for quick look up based on proximity.
	//
	// update on scene objects has to be managed by the game, and it should occur in the following order:
	//     game calls update_animations() on all its animated model scene components.
	//     game calls update() on the physics scene contained in the scene.
	//     game calls update_simulated_bone_logics() on all its animated model scene components.
	//     game calls get_scene_camera() to update the camera's properties. the camera's transform will be fed to the audio scene listener when update_audio() is called.
	//     game calls update_audio() on the scene to tell the sound components in the scene to add their sources to the audio scene if they are in range of the camera.
	// in order to render the scene, it must be associated with the engine's user interface manager.
	// or you can render it manually to an off-screen target using the engine's video renderer interface.
	class scene_c
	{
		friend class scene_component_c;
		friend class scene_component_sound_c;
		friend class scene_component_model_c;
		friend class scene_component_light_c;
		friend class scene_component_light_probe_c;
		friend class scene_light_c;
		friend class video_renderer_interface_c;
		friend class user_interface_c;

	private:
		// if this scene is set as the scene in the engine's user interface manager, then this pointer will be set to that user interface manager.
		// there is only one user interface per game client, and at most only one scene can be associated with it at a time.
		user_interface_c * _user_interface;
		void_c _set_user_interface( user_interface_c * user_interface ); // only the user interface should call this.

		// the color that the canvas will be cleared to before rendering to it.
		vector32x4_c _clear_color;

		// the point of view that this scene will be experienced from when it visuals are rendered and its audio is mixed.
		scene_camera_c _scene_camera;

		// all root scene nodes.
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
		audio_scene_c * _audio_scene;

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

		// sky setup.
		// any number of models to render "behind" everything else.
		// the game should position these relative to the world origin, as the world origin is the point of view that is used when the renderer renders the sky models.
		core_list_c< scene_component_model_c * > _sky_model_list; // models that represent planets or suns that orbit around the observer. these are rendered with depth testing enabled (less than or equal to) and with (for opaque) depth write enabled or (for transparent) depth write disabled.
		//core_list_c< scene_light_c * > sky_body_lights; // one or two directional lights (at this time they must be directional type) which may also be shadow casters. these should be managed by the game to always point from the sun towards the camera.

		// 16 colors that all pixel shaders can read from for a given frame, which makes them good for holding sky/environment related colors.
		// most likely only the sky models will have shaders that use these values, since light probes can handle how indirect light from the sky models affects models on the "ground". totally up to you how you want to use these values though.
		vector32x4_c _color_array[ video_renderer_interface_c::scene_colors_count ];

		// light probe quality and baking.
		boolean_c _automatic_light_probe_invalidation_enabled; // set to true to disable automatic light probe invalidation scene-wide. this is useful if you are adding or removing a bunch objects to or from the scene at once.
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
		~scene_c(); // does not delete any scene objects in the scene. it is the game's responsibility to manage life time of scene objects. will assert if there are still any scene objects associated with the scene when the destructor is called.

		vector32x4_c const & get_clear_color() const;
		void_c set_clear_color( vector32x4_c const & value );
		void_c reset_clear_color();

		scene_camera_c & get_camera(); // each scene has one built in camera that the game can set the properties on directly.

		core_list_c< scene_component_model_c * > & get_sky_model_list(); // gets a reference to the list of sky models that the game can edit directly. the scene components must be associated with scene objects (in order to give the models spatial properties), but the scene objects must not actually be added to any scene.

		vector32x4_c const & get_color( sint32_c index ) const; // the scene has 16 colors that can be set by the game and which the shaders can be programmed to read.
		void_c set_color( sint32_c index, vector32x4_c const & value ); // the scene has 16 colors that can be set by the game and which the shaders can be programmed to read.

		void_c update_audio( float32_c time_step ); // takes the properties of the camera to update the properties of the listener in the audio scene.

		void_c add_object( scene_object_c * object ); // adds a scene object to this scene.
		void_c remove_object( scene_object_c * object ); // removes a scene object from this scene.

		void_c add_light( scene_light_c * light ); // adds a light to this scene.
		void_c remove_light( scene_light_c * light ); // removes a light from this scene.

		void_c _gather_pickables_along_segment( segment64_c const & pick_segment, core_list_c< scene_component_model_c * > & models_of_interest, core_list_c< scene_component_sprite_c * > & sprites_of_interest ); // internal use by pixel picker, performs broad phase ray intersection test against each model's bounding box and stores the results in the internal list. the input lists need would be assembled already by the renderer that calls this function, and they would contain pointers to objects in this scene.
		void_c pixel_pick_during_next_render( vector32x2_c const & screen_coordinates ); // queues up a pixel perfect pick operation to occur during the next render. screen_coordinates are in pixels with the top left corner as the origin.
		scene_component_c * get_pixel_pick_result_component() const;
		scene_object_c * get_pixel_pick_result_object( scene_object_c * after_object ) const; // returns the pick result from the last time this scene was rendered. the pixel perfect pick internally has access to a list of hits sorted by distance from near to far that intersect with the ray. if after_object is set, then returns the next result in that list after after_object (used to cycle to the next selectable object if the user clicks on the same spot that intersects with multiple objects and over different frames). otherwise, it returns the first entry in that list.

		void_c suspend_automatic_light_probe_invalidation(); // lets you batch add/remove/edit a bunch of scene objects/components without invalidating light probes after each operation.
		void_c resume_automatic_light_probe_invalidation();

		void_c invalidate_light_probes_every_where();
		void_c invalidate_light_probes_with_light( scene_light_c const * light, transform3d_c const * old_world_space_transform );
		void_c invalidate_light_probes_with_box( box64x3_c const & bounding_box, transform3d_c const & bounding_box_transform );
		void_c invalidate_light_probes_with_before_and_after_box( box64x3_c const & bounding_box, transform3d_c const & before_bounding_box_transform, transform3d_c & after_bounding_box_transform );
		void_c invalidate_light_probes_with_sphere( sphere64_c const & sphere );
		void_c invalidate_light_probes_with_before_and_after_sphere( sphere64_c const & before_sphere, sphere64_c const & after_sphere );
		scene_component_light_probe_c * find_light_probe_with_point( vector64x3_c const & position );

		//boolean_c physics_ray_cast( vector64x3_c & result, segment64_c const segment ); // casts a ray into the physics scene, returns true if the ray hits anything, false if otherwise. result will hold the closest point of intersection.

		user_interface_c * get_user_interface() const; // gets the user interface that this scene is associated with, if any. this will only be set if you call engine.get_user_interface()->set_scene(). the engine has only one user interface and only one scene can be associated with it at a time.

		audio_scene_c * get_audio_scene(); // gets a pointer to the audio scene interface that manages mixing of sounds for this scene. this will never return nullptr.

		physics_scene_c * get_physics_scene(); // gets a pointer to the physics scene that manages physics simulation for this scene. this will never return nullptr.

		core_linked_list_c< scene_object_c * > const & get_scene_object_list() const; // gets a reference to the list that holds all the scene objects in this scene.

		scene_tree_octo_c< scene_component_c * > const & get_scene_component_tree() const; // gets a reference to the tree that holds all of the scene components in this scene.

		void_c * get_user_pointer();
		void_c set_user_pointer( void_c * value );

	};

}
